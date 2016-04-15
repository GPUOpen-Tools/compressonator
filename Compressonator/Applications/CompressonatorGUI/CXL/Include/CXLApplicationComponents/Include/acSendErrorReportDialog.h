//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acSendErrorReportDialog.h 
/// 
//================================================================================== 

//------------------------------ acSendErrorReportDialog.h ------------------------------

#ifndef __GDSENDERRORREPORTDIALOG
#define __GDSENDERRORREPORTDIALOG

// Warnings:
#include <CXLBaseTools/Include/gtIgnoreCompilerWarnings.h>

// Qt:
#include <QtWidgets>

// Forward decelerations:
class osCallStack;
class acQHTMLWindow;
class acTextCtrl;

// Infra:
#include <CXLAPIClasses/Include/Events/apIEventsObserver.h>
#include <CXLAPIClasses/Include/Events/apExceptionEvent.h>

// Local:
#include <CXLApplicationComponents/Include/acApplicationComponentsDLLBuild.h>

// ----------------------------------------------------------------------------------
// Class Name:           acSendErrorReportDialog : public acDialog
// General Description:
// Author:               Avi Shapira
// Creation Date:        9/11/2003
// Revision:             Name changed to acSendErrorReportDialog (27/09/15)
// ----------------------------------------------------------------------------------
class AC_API acSendErrorReportDialog : public QDialog, public apIEventsObserver
{
    Q_OBJECT

public:
    acSendErrorReportDialog(QWidget* pParent, const QString& productName, const QIcon& icon);

    virtual ~acSendErrorReportDialog();

    void registerForRecievingDebuggedProcessEvents();
    void onUnhandledException(osExceptionCode& exceptionCode, void* pExceptionContext, bool allowDifferentSystemPath);
    void onMemoryAllocationFailure(const osCallStack& allocCallStack, bool allowDifferentSystemPath);
    void displayErrorReportDialog(osExceptionReason exceptionReason, const osCallStack& callStack, const QString& additionalInformation, bool displayOnlySpyRelatedErrorReports = true, bool allowDifferentSystemPath = false);

protected:

    // Overrides apIEventsObserver:
    virtual void onEvent(const apEvent& eve, bool& vetoEvent);
    virtual const wchar_t* eventObserverName() const { return L"SendErrorReportDialog"; };

protected slots:

    void onAnchorClicked(const QUrl& link);
    void onSendErrorReportClick();
    void onDontSendErrorReportClick();
    void onPrivacyPolicyClick();

private:

    void updateCodeXLVersionAsString();
    bool updateGeneralStrings();
    void setDialogLayout();
    bool isCrashInSpyTerminationFunc(QString& callStackString);
    void getBugReportDetails(QString& bugReportDetails);
    void buildHTMLMainText(QString& mainStringAsHTML);

private:

    QTextEdit* m_pAdditionalInformation;
    acQHTMLWindow* m_pMainText;
    QLineEdit* m_pEmailAddress;
    QPushButton* m_pSendButton;
    QPushButton* m_pDontSendButton;
    QPushButton* m_pPrivacyButton;

    QString m_callStackString;
    QString m_errorReportTitle;
    QString m_CodeXLVersionString;
    QString m_osDescriptionString;
    QString m_productDescriptionString;
    QString m_additionalInformation;
    QString m_productName;

    // Contains true if this class instance is registered for receiving debugged process events:
    bool m_isRegisterForRecievingDebuggedProcessEvents;

};


#endif  // __GDSENDERRORREPORTDIALOG
