//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acProgressDlg.cpp
///
//==================================================================================

//#include <acDisplay.h>
#include "acprogressdlg.h"
#include "acprogressanimationwidget.h"

#include <QtWidgets>

// Infra:
#include <assert.h>

#define AC_PROGRESSDLG_DEAFULT_HRADER "Loading"
#define AC_PROGRESSDLG_DEAFULT_MSG "Please wait..."

#define AC_PROGRESSBAR_REFRESH_RATE_WITH_CANCEL 50
#define AC_PROGRESSBAR_REFRESH_RATE_NO_CANCEL 200


#define AC_PROGRESSBAR_MIN_WIDTH 520
#define AC_PROGRESSBAR_MIN_HEIGHT 150
#define AC_PROGRESSBAR_MAX_WIDTH 620
#define AC_SOURCE_CODE_EDITOR_MARGIN_BG_COLOR QColor(240, 240, 240, 255)

acProgressDlg::acProgressDlg(QWidget* parent)
    : QDialog(parent, Qt::SubWindow), m_pPercentageLabel(nullptr), m_pHeaderLabel(nullptr), m_pMsgLabel(nullptr), m_pCancelButton(nullptr), m_pProgressAnimationWidget(nullptr), m_showPercentage(false), m_lastTimeMsec(0), m_forceDraw(false), m_mouseDown(false) {
    Init();
}

acProgressDlg::~acProgressDlg() {
    Cleanup();
}

void acProgressDlg::Init() {
    // logical members
    m_minimum = 0;
    m_maximum = 100;
    m_currentProgress = 0;
    m_currentPercentageProgress = 0;
    m_cancellationFlag = false;
    m_showCancelButton = false;

    // GUI
    //setMinimumHeight(acScaleSignedPixelSizeToDisplayDPI(AC_PROGRESSBAR_MIN_HEIGHT));  //150
    //setMinimumWidth(acScaleSignedPixelSizeToDisplayDPI(AC_PROGRESSBAR_MIN_WIDTH));  //520
    //setMaximumWidth(acScaleSignedPixelSizeToDisplayDPI(AC_PROGRESSBAR_MAX_WIDTH));  //620
    setMinimumHeight(150);
    setMinimumWidth(520);
    setMaximumWidth(620);
    setStyleSheet(QString("QDialog{border:1px solid gray; background-color: %1;}").arg(AC_SOURCE_CODE_EDITOR_MARGIN_BG_COLOR.name()));


    m_pPercentageLabel = new QLabel;
    m_pHeaderLabel = new QLabel;
    QFont font;
    font.setPointSize(12);
    font.setBold(true);
    font.setWeight(75);
    m_pHeaderLabel->setFont(font);
    m_pMsgLabel = new QLabel;
    m_pCancelButton = new QPushButton;

    QFont fontNumbers;
    fontNumbers.setPointSize(11);
    fontNumbers.setBold(true);
    m_pPercentageLabel->setFont(fontNumbers);
    m_pPercentageLabel->setVisible(m_showPercentage);
    m_pPercentageLabel->setAlignment(Qt::AlignCenter | Qt::AlignVCenter);
    m_pPercentageLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    m_pHeaderLabel->setText(QApplication::translate("acProgressDlg", AC_PROGRESSDLG_DEAFULT_HRADER, 0));
    m_pHeaderLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pMsgLabel->setText(QApplication::translate("acProgressDlg", AC_PROGRESSDLG_DEAFULT_MSG, 0));
    m_pMsgLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_pMsgLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    m_pCancelButton->setText(QApplication::translate("acProgressDlg", "&Cancel", 0));
    m_pCancelButton->setStyleSheet("QPushButton{ border:1px solid gray; border-radius: 2px; width: 75px; height: 20px;} QPushButton::hover{ border:1px solid gray; border-radius: 2px; background-color: lightgray;}");

    m_pProgressAnimationWidget = new acProgressAnimationWidget(this);
    m_pProgressAnimationWidget->setMinimumSize(QSize(80, 80));
    m_pProgressAnimationWidget->SetColor(QColor(99, 100, 102, 0xff));

    QHBoxLayout* pHLayout = new QHBoxLayout;
    QVBoxLayout* pLeftVLayout = new QVBoxLayout;
    QVBoxLayout* pRightVLayout = new QVBoxLayout;

    pLeftVLayout->addSpacing(15);
    pLeftVLayout->addWidget(m_pProgressAnimationWidget, 0, Qt::AlignCenter);
    pLeftVLayout->addWidget(m_pPercentageLabel, 0, Qt::AlignCenter);
    pLeftVLayout->addSpacing(15);

    pRightVLayout->addSpacing(15);
    pRightVLayout->addWidget(m_pHeaderLabel);
    pRightVLayout->addWidget(m_pMsgLabel, 1, Qt::AlignTop);
    pRightVLayout->addStretch();

    pHLayout->addLayout(pLeftVLayout);
    pHLayout->addSpacing(15);
    pHLayout->addLayout(pRightVLayout);
    pHLayout->addStretch();
    pHLayout->addWidget(m_pCancelButton, 0, Qt::AlignBottom);
    setLayout(pHLayout);


    Qt::WindowFlags flags = windowFlags();
    setWindowFlags(flags | Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);

    m_pCancelButton->setVisible(false);
    m_refreshRateMsec = AC_PROGRESSBAR_REFRESH_RATE_NO_CANCEL;


    QMetaObject::connectSlotsByName(this);
}

void acProgressDlg::Cleanup() {
    if (m_showCancelButton) {
        DisconnectCancelEvents();
    }

    if (m_pProgressAnimationWidget) {
        delete m_pProgressAnimationWidget;
        m_pProgressAnimationWidget = nullptr;
    }
}

void acProgressDlg::Increment(unsigned int numSteps) {
    SetValue(m_currentProgress + numSteps);
}

void acProgressDlg::SetValue(unsigned int value) {
    m_currentProgress = qMin(m_maximum, value);
    int elapsedMsecs = m_refreshTimer.elapsed();

    unsigned int percentageProgress = (m_maximum) ? (int)(100.f * ((float)m_currentProgress / (float)m_maximum)) : 0;

    if (m_forceDraw ||
            (
                (elapsedMsecs >= m_refreshRateMsec) && (m_currentPercentageProgress != percentageProgress)
            )
       ) {
        m_forceDraw = false;
        m_refreshTimer.restart();

        if(m_pPercentageLabel != nullptr && m_pProgressAnimationWidget != nullptr) {
            if (percentageProgress > 0) {
                QString percentage = "  " + (QString::number(percentageProgress)) + "%  ";
                m_pPercentageLabel->setText(QApplication::translate("acProgressDlg", percentage.toStdString().c_str(), 0));
            } else
                m_pPercentageLabel->setText("");

            m_currentPercentageProgress = percentageProgress;

            m_pProgressAnimationWidget->SetProgressValue(percentageProgress);
        }
        repaint();
        qApp->processEvents(); // keep UI responsive
    }
}

unsigned int acProgressDlg::Value()const {
    return m_currentProgress;
}

unsigned int acProgressDlg::RangeMax()const {
    return m_maximum;
}

void acProgressDlg::SetLabelText(const QString& text) {
    if (m_pMsgLabel != nullptr) {
        m_pMsgLabel->setText(text);
    }
}

void acProgressDlg::GetLabelText(QString& text) const {
    if (m_pMsgLabel != nullptr) {
        text = m_pMsgLabel->text();
    } else {
        text.clear();
    }
}

void acProgressDlg::SetHeader(const QString& text) {
    if(m_pHeaderLabel != nullptr) {
        m_pHeaderLabel->setText(text);
    }
}

void acProgressDlg::ShowCancelButton(bool showCancel, funcPtr func) {
    if(m_pCancelButton != nullptr) {
        m_pCancelButton->setVisible(showCancel);

        if (showCancel) {
            m_cancel_callbackfunc = func;
            ConnectCancelEvents();
        } else {
            DisconnectCancelEvents();
            m_cancel_callbackfunc = nullptr;
        }

        m_showCancelButton = showCancel;
    }
    m_refreshRateMsec = (m_showCancelButton ? AC_PROGRESSBAR_REFRESH_RATE_WITH_CANCEL : AC_PROGRESSBAR_REFRESH_RATE_NO_CANCEL);
}

void acProgressDlg::SetRange(unsigned int min, unsigned int max) {
    m_minimum = min;
    m_maximum = max;
    if(m_pPercentageLabel != nullptr) {
        m_pPercentageLabel->setVisible(true);
    }
}

void acProgressDlg::DisconnectCancelEvents() {
    if((m_pCancelButton != nullptr) && (m_pProgressAnimationWidget != nullptr)) {
        QObject::disconnect(m_pCancelButton, SIGNAL(clicked(bool)), m_pProgressAnimationWidget, SLOT(StopAnimation()));
        QObject::disconnect(m_pCancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));
        QObject::disconnect(m_pCancelButton, SIGNAL(released()), this, SLOT(cancel()));
        QObject::disconnect(m_pCancelButton, SIGNAL(clicked(bool)), this, SIGNAL(canceled()));
    }
}

void acProgressDlg::ConnectCancelEvents() {
    if((m_pCancelButton != nullptr) && (m_pProgressAnimationWidget != nullptr)) {
        bool rc = QObject::connect(m_pCancelButton, SIGNAL(clicked(bool)), m_pProgressAnimationWidget, SLOT(StopAnimation()));
        assert(rc);
        rc = QObject::connect(m_pCancelButton, SIGNAL(clicked(bool)), this, SLOT(cancel()));
        assert(rc);
        rc = QObject::connect(m_pCancelButton, SIGNAL(released()), this, SLOT(cancel()));
        assert(rc);
        rc = QObject::connect(m_pCancelButton, SIGNAL(clicked(bool)), this, SIGNAL(canceled()));
        assert(rc);
    }
}

void acProgressDlg::mousePressEvent(QMouseEvent* e) {
    Q_UNUSED(e);
    m_mouseDown = true;
}

void acProgressDlg::mouseMoveEvent(QMouseEvent* pEvent) {
    if (m_mouseDown && (pEvent != nullptr)) {
        QPoint clickPos = pEvent->globalPos();
        move(clickPos);
    }
}

void acProgressDlg::mouseReleaseEvent(QMouseEvent* e) {
    Q_UNUSED(e)
    m_mouseDown = false;
}

void acProgressDlg::cancel() {
    if (m_cancel_callbackfunc != nullptr) {
        m_cancel_callbackfunc();
    }

    m_cancellationFlag = true;
    close();
}

void acProgressDlg::reset() {
    m_currentProgress = m_minimum;
    m_currentPercentageProgress = 0;
    m_cancellationFlag = false;
    show();
}

bool acProgressDlg::WasCanceled() const {
    return m_cancellationFlag;
}

void acProgressDlg::closeEvent(QCloseEvent* e) {
    emit canceled();
    QDialog::closeEvent(e);
}

void acProgressDlg::show() {
    if(m_pProgressAnimationWidget != nullptr) {
        if (m_pProgressAnimationWidget->IsAnimationStarted() == false) {
            m_pProgressAnimationWidget->StartAnimation();
            m_refreshTimer.start();
            m_forceDraw = true;
        }
    }
    QDialog::show();
}

void acProgressDlg::hide() {
    if(m_pProgressAnimationWidget != nullptr) {
        m_pProgressAnimationWidget->StopAnimation();
    }
    QDialog::hide();
}

