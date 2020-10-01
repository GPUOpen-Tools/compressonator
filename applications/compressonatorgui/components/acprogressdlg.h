//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acProgressDlg.h
///
//==================================================================================

#ifndef ACPROGRESSDLG_H
#define ACPROGRESSDLG_H

#include <QDialog>
#include <QTime>


class QLabel;
class QPushButton;
class acProgressAnimationWidget;

typedef void(*funcPtr)(); // pointer to function with no args and void return for cancel event for non-QT users


class acProgressDlg : public QDialog {
    Q_OBJECT

  public:
    acProgressDlg(QWidget* parent = 0);
    virtual ~acProgressDlg();

    /// Progresses the progress dlg
    void Increment(unsigned int numSteps = 1);

    /// Sets the value of the progress dlg progress
    void SetValue(unsigned int value);

    /// Sets the text that is displayed in the body of the progress dlg
    void SetLabelText(const QString& text);

    /// Gets the text that is displayed in the body of the progress dlg
    void GetLabelText(QString& text) const;

    /// Sets the header of the progress dlg
    void SetHeader(const QString& text);

    /// Determines whether the progress dlg shows a cancel button
    void ShowCancelButton(bool show, funcPtr func = nullptr);

    /// Sets the range of the progress dlg, default: 0-100
    void SetRange(unsigned int min, unsigned int max);

    /// Obtains the value of the progress dlg progress
    unsigned int Value()const;

    /// Obtains the maximum range value of the progress dlg
    unsigned int RangeMax()const;

    bool WasCanceled() const;
    virtual void closeEvent(QCloseEvent* e);

    virtual void show();
    virtual void hide();

  public Q_SLOTS:
    void cancel();
    void reset();
  Q_SIGNALS:
    void canceled();

  private:
    QLabel* m_pPercentageLabel = nullptr;
    QLabel* m_pHeaderLabel;
    QLabel* m_pMsgLabel;
    QPushButton* m_pCancelButton;
    acProgressAnimationWidget* m_pProgressAnimationWidget;

    unsigned int m_currentProgress;
    unsigned int m_currentPercentageProgress;

    unsigned int m_minimum;
    unsigned int m_maximum;
    bool m_showCancelButton;
    bool m_cancellationFlag;
    bool m_showPercentage;

    funcPtr m_cancel_callbackfunc; // variable of pointer to function

    // mouse handling for modeless mode
  protected:
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
  private:

    // The refresh rate for the progress dialog
    int m_refreshRateMsec;
    int m_lastTimeMsec;
    // Timer for refresh rate for the progress dialog
    QTime m_refreshTimer;
    // Flag for force draw of first draw call
    bool m_forceDraw;
    // Flag for drag operation when mouse is down
    bool m_mouseDown;

    void Init();
    void Cleanup();
    void DisconnectCancelEvents();
    void ConnectCancelEvents();

};

#endif // ACPROGRESSDLG_H
