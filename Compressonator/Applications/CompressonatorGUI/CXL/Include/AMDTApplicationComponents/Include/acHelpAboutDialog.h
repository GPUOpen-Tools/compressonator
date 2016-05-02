//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acHelpAboutDialog.h 
/// 
//================================================================================== 

//------------------------------ acHelpAboutDialog.h ------------------------------

#ifndef __ACHELPABOUTDIALOG
#define __ACHELPABOUTDIALOG

// QT:
#include <QDialog>

// Infra:
#include <AMDTBaseTools/Include/gtString.h>
#include <AMDTOSWrappers/Include/osApplication.h>
#include <AMDTApplicationComponents/Include/acIcons.h>

// Local:
#include <AMDTApplicationComponents/Include/acApplicationComponentsDLLBuild.h>

// ----------------------------------------------------------------------------------
// Class Name:          acHelpAboutDialog
// General Description: help dialog screen
//
// Author:              Yoni Rabin
// Creation Date:       9/4/2012
// ----------------------------------------------------------------------------------
class AC_API acHelpAboutDialog : public QDialog
{
    Q_OBJECT
public:

    acHelpAboutDialog(osExecutedApplicationType executionApplicationType, QWidget* pParent);
    void Init(const QString& title, const QString& productName, const osProductVersion& appVersion, const QString& copyRightCaption, const QString& copyRightInformation,
              const acIconId& productIconId, const gtString& versionCaption, const gtString& companyLogoBitmapString, bool addDescriptionString = false);

protected:

    // QT:
    QPixmap* m_pCompanyLogo;
    osExecutedApplicationType m_osExecutedApplicationType;

private:
    QString m_productName;
};

#endif  // __ACHELPABOUTDIALOG
