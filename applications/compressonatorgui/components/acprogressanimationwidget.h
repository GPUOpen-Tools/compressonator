//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acProgressAnimationWidget.h
///
//==================================================================================

#ifndef ACPROGRESSANIMATIONWIDGET_H
#define ACPROGRESSANIMATIONWIDGET_H


#include <QWidget>
#include <QColor>
#include <QPainter>

class QTimerEvent;

// ----------------------------------------------------------------------------------
// Class Name:          acProgressAnimationWidget
// General Description: QT progress widget which displays busy animation
//
//
// Author:              Naama Zur
// Creation Date:
// ----------------------------------------------------------------------------------
class acProgressAnimationWidget : public QWidget {
    Q_OBJECT
  public:
    acProgressAnimationWidget(QWidget* parent = 0);

    /// Sets the color of the widget
    void SetColor(const QColor& color);

    /// Sets the delay between animation steps in milliseconds
    void SetAnimationDelay(int delay);

  public slots:
    /// Starts the spin animation
    void StartAnimation();

    /// Stops the spin animation
    void StopAnimation();

    /// Obtains the animation status
    bool IsAnimationStarted()const;

    /// Progresses the spin animation
    void Increment(unsigned int value = 1);
    /// Progresses the spin animation
    void SetProgressValue(unsigned int value);

  protected:
    virtual void paintEvent(QPaintEvent* event);

    /// Progress animation styles
    void PaintLineCircle(QPainter& p);
    void PaintDotsCircle(QPainter& p);
    void PaintGradientCircle(QPainter& p);
    void PaintCircle(QPainter& p);
    void paintDoubleCircle(QPainter& p);


  private:
    QColor m_color;
    unsigned int m_angle;
    unsigned int m_animationDelay;
    unsigned int m_animationDelayAccumulator;
    bool m_isAnimationStarted;
};

#endif // ACPROGRESSANIMATIONWIDGET_H
