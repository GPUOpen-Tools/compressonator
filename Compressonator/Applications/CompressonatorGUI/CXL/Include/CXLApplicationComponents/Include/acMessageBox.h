//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acMessageBox.h 
/// 
//================================================================================== 

//------------------------------ acMessageBox.h ------------------------------
#ifndef __ACMESSAGEBOX_H
#define __ACMESSAGEBOX_H

// Qt:
#include <QtWidgets>

// Forward declaration:
class QWidget;

// Infra:
#include <CXLBaseTools/Include/AMDTDefinitions.h>

// Local:
#include <CXLApplicationComponents/Include/acApplicationComponentsDLLBuild.h>

class AC_API acMessageBox
{
public:

    static acMessageBox& instance();
    virtual ~acMessageBox();
    static void setApplicationIconPixmap(QPixmap* pPixmap) {m_spApplicationIconPixmap = pPixmap;};
    void setParentWidget(QWidget* pParent) {m_pParent = pParent;};
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    bool setParentHwnd(HWND parentHwnd);
#endif

    QMessageBox::StandardButton critical(const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    QMessageBox::StandardButton information(const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    QMessageBox::StandardButton question(const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);
    QMessageBox::StandardButton warning(const QString& title, const QString& text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton);

    QMessageBox::StandardButton showNewMessageBox(QWidget* parent, QMessageBox::Icon icon, const QString& title, const QString& text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton);

    int doModal(QDialog* pMessageBox);

    void useHideDialogButton()          { m_hideDialogFlag = true; };
    Qt::CheckState hideDialogState()    { return m_spCheckboxButton->checkState(); };

protected:

    // Do not allow the use of my default constructor:
    acMessageBox();
    static acMessageBox* m_spMessageBoxSingleInstance;
    static QPixmap* m_spApplicationIconPixmap;
    static QCheckBox* m_spCheckboxButton;

    // Holds the parent of the message box, should be initialized before any of the calls to the notification functions:
    QWidget* m_pParent;

#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    // handle to the vsp hwnd to handle modality
    HWND m_parentHwnd;
#endif

    bool m_hideDialogFlag;
};

#endif //__ACMESSAGEBOX_H

