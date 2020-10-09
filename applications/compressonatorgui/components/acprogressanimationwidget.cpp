//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acProgressAnimationWidget.cpp
///
//==================================================================================

#include "acprogressanimationwidget.h"
#include <QtWidgets>

#define AC_PROGRESS_ANIM_COEFF 30
#define AC_PROGRESS_ANIM_INC 30
#define AC_PROGRESS_ANIM_CIRCLE_WIDTH 10
#define AC_PROGRESS_ANIM_INNER_RADIUS_COEF 0.16f
#define AC_PROGRESS_ANIM_OUTER_RADIUS_COEF 0.5f
#define AC_PROGRESS_ANIM_NUM_TONES 12
#define AC_SOURCE_CODE_EDITOR_MARGIN_BG_COLOR QColor(240, 240, 240, 255)

acProgressAnimationWidget::acProgressAnimationWidget(QWidget* parent)
    : QWidget(parent), m_color(Qt::black), m_angle(0), m_animationDelay(0), m_animationDelayAccumulator(0), m_isAnimationStarted(false) {
    setFocusPolicy(Qt::NoFocus);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void acProgressAnimationWidget::SetColor(const QColor& color) {
    m_color = color;
    update();
}

void acProgressAnimationWidget::SetAnimationDelay(int delay) {
    m_animationDelay = delay;
    m_animationDelayAccumulator = 0;
}

void acProgressAnimationWidget::StartAnimation() {
    m_angle = 0;
    m_isAnimationStarted = true;
    m_animationDelayAccumulator = 0;
}

void acProgressAnimationWidget::StopAnimation() {
    m_isAnimationStarted = false;
    update();
    m_animationDelayAccumulator = 0;
}

bool acProgressAnimationWidget::IsAnimationStarted()const {
    return m_isAnimationStarted;
}

void acProgressAnimationWidget::paintEvent(QPaintEvent* /*event*/) {
    if (m_isAnimationStarted) {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);
        PaintGradientCircle(p);
    }
}
void acProgressAnimationWidget::Increment(unsigned int value) {
    if (!m_isAnimationStarted) {
        StartAnimation();
    }

    unsigned int delayedValue = value;

    if (m_animationDelay) {
        m_animationDelayAccumulator += value;

        if (m_animationDelayAccumulator > m_animationDelay) {
            delayedValue = (m_animationDelayAccumulator / m_animationDelay);
            m_animationDelayAccumulator = (m_animationDelayAccumulator % m_animationDelay);
        } else {
            delayedValue = 0;
        }
    }

    m_angle = (m_angle + delayedValue) % 360;
    update();
}

void acProgressAnimationWidget::SetProgressValue(unsigned int value) {
    if (!m_isAnimationStarted) {
        StartAnimation();
    }

    if (value > 100) {
        m_angle = 100;
    } else {
        m_angle = value;
    }

    update();
}

void acProgressAnimationWidget::PaintLineCircle(QPainter& painter) {
    int width = qMin(this->width(), this->height());
    int innerRadius = (width - 1) * AC_PROGRESS_ANIM_INNER_RADIUS_COEF;
    int outerRadius = (width - 1) * AC_PROGRESS_ANIM_OUTER_RADIUS_COEF;

    int barLen = outerRadius - innerRadius;
    int barWidth = (width > 32) ? barLen * .23 : barLen * .35;
    int barRadius = barWidth / 2;

    for (int i = 0; i < AC_PROGRESS_ANIM_NUM_TONES; i++) {
        QColor color = m_color;
        color.setAlphaF(1.0f - ((float)i / (float)AC_PROGRESS_ANIM_NUM_TONES));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.save();
        painter.translate(rect().center());
        painter.rotate(m_angle - i * AC_PROGRESS_ANIM_COEFF);
        painter.drawRoundedRect(-barWidth * 0.5, -(innerRadius + barLen), barWidth, barLen, barRadius, barRadius);
        painter.restore();
    }
}

void acProgressAnimationWidget::PaintDotsCircle(QPainter& painter) {
    int width = qMin(this->width(), this->height());

    int innerRadius = (width - 1) * AC_PROGRESS_ANIM_INNER_RADIUS_COEF;
    int outerRadius = (width - 1) * AC_PROGRESS_ANIM_OUTER_RADIUS_COEF;

    int barHeight = outerRadius - innerRadius;

    for (int i = 0; i < AC_PROGRESS_ANIM_NUM_TONES; i++) {
        QColor color = m_color;
        color.setAlphaF(1.0f - ((float)i / (float)AC_PROGRESS_ANIM_NUM_TONES));
        painter.setPen(Qt::NoPen);
        painter.setBrush(color);
        painter.save();
        painter.translate(rect().center());
        painter.rotate(m_angle - i * AC_PROGRESS_ANIM_COEFF);
        painter.drawEllipse(15, 15, barHeight * 0.5, barHeight * 0.5);
        painter.restore();
    }
}

QColor acQAMD_GRAY3_COLOUR(199, 200, 202, 0xff);                    // #C7C8CA
void acProgressAnimationWidget::PaintGradientCircle(QPainter& painter) {
    int width = AC_PROGRESS_ANIM_CIRCLE_WIDTH;

    QRect drawingRect;
    drawingRect.setX(rect().x() + width);
    drawingRect.setY(rect().y() + width);
    drawingRect.setWidth(rect().width() - width * 2);
    drawingRect.setHeight(rect().height() - width * 2);

    QConicalGradient gradient;
    gradient.setCenter(drawingRect.center());
    gradient.setAngle(90);
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, acQAMD_GRAY3_COLOUR);

    int arcLengthApproximation = width + width / 3;
    QPen pen(QBrush(gradient), width);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    int progressInDegrees = (double)(m_angle * 360) / 100;
    painter.drawArc(drawingRect, 88 * 16 - arcLengthApproximation, -progressInDegrees * 16);

}

void acProgressAnimationWidget::PaintCircle(QPainter& painter) {
    int progress = m_angle;
    int progressInDegrees = (double)(progress * 360) / 300;

    int barWidth = AC_PROGRESS_ANIM_CIRCLE_WIDTH;

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(QPen(m_color, barWidth, Qt::SolidLine, Qt::RoundCap));
    painter.drawArc(barWidth / 2, barWidth / 2, this->width() - barWidth, this->height() - barWidth, 90 * 16, progressInDegrees * -16);
}

void acProgressAnimationWidget::paintDoubleCircle(QPainter& painter) {
    int m_width = AC_PROGRESS_ANIM_CIRCLE_WIDTH;
    int doubleAngle = m_angle * 2;

    QRect drawingRect;
    drawingRect.setX(rect().x() + m_width);
    drawingRect.setY(rect().y() + m_width);
    drawingRect.setWidth(rect().width() - m_width * 2);
    drawingRect.setHeight(rect().height() - m_width * 2);

    QConicalGradient gradient;
    gradient.setCenter(drawingRect.center());
    gradient.setAngle(90);
    gradient.setColorAt(0, m_color);
    gradient.setColorAt(1, QColor(199, 200, 202, 0xff));

    int arcLengthApproximation = m_width + m_width / 3;
    QPen pen(QBrush(gradient), m_width);
    pen.setCapStyle(Qt::RoundCap);
    painter.setPen(pen);
    painter.drawArc(drawingRect, 90 * 15 - arcLengthApproximation, -doubleAngle * 17);

    if (doubleAngle > 360) {
        QPen penEraser(AC_SOURCE_CODE_EDITOR_MARGIN_BG_COLOR, m_width + 1);
        penEraser.setCapStyle(Qt::FlatCap);
        painter.setPen(penEraser);
        painter.drawArc(drawingRect, 90 * 16 - arcLengthApproximation, -(doubleAngle - 360) * 16);
    }
}
