//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acSoftwareUpdaterWindow.h 
/// 
//================================================================================== 

#ifndef __ACSOFTWAREUPDATERWINDOW_H
#define __ACSOFTWAREUPDATERWINDOW_H

// Infra:
#include <AMDTBaseTools/Include/gtString.h>

// Qt:
#include <QtWidgets>
#include <QDate>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <QThread>
#include <QtXml/qdom.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkProxyFactory>
#include <QtWebKitWidgets/QWebView>

// Local:
#include <AMDTApplicationComponents/Include/acApplicationComponentsDLLBuild.h>
#include <AMDTOSWrappers/Include/osFilePath.h>
#include <AMDTApplicationComponents/Include/acIcons.h>

class acSoftwareUpdaterWindow;

class AC_API acSoftwareUpdaterThread : public QThread
{
    Q_OBJECT

public:
    enum acUpdaterThreadOperationType
    {
        AF_DOWNLOAD,
        AF_GET_LATEST_VERSION_INFO
    };

    acSoftwareUpdaterThread(acUpdaterThreadOperationType operationType, acSoftwareUpdaterWindow* pDialog);
    virtual ~acSoftwareUpdaterThread();

    virtual void run();

private:
    acUpdaterThreadOperationType m_threadOperationType;
    acSoftwareUpdaterWindow* m_pDialog;
};


/// Class to contain application specific informations.
class AC_API acSoftwareUpdateInfo
{
public:
    int m_iProgramVersionBuild;         ///< Contains application Build version.
    int m_iProgramVersionMajor;         ///< Contains application Major version.
    int m_iProgramVersionMinor;         ///< Contains application Minor version.
    int m_iReleaseDay;                  ///< Contains application release day.
    int m_iReleaseMonth;                ///< Contains application release month.
    int m_iReleaseYear;                 ///< Contains application release year.
    QString m_strProgramFile;          ///< Contains application URL path.
    QString m_strProgramFile2;         ///< Contains application second URL path.
    QString m_strProgramFile3;         ///< Contains application third URL path.
    QString m_versionDescriptionURL;///< Contains version description URL
    QString m_strProgramName;          ///< Contains application name.

    /// Default constructor.
    acSoftwareUpdateInfo()
    {
        m_iProgramVersionBuild = -1;
        m_iProgramVersionMajor = -1;
        m_iProgramVersionMinor = -1;
        m_iReleaseDay = -1;
        m_iReleaseMonth = -1;
        m_iReleaseYear = -1;
        m_strProgramFile.clear();
        m_strProgramFile2.clear();
        m_strProgramFile3.clear();
        m_versionDescriptionURL.clear();
        m_strProgramName.clear();
    }

    /// Compares two Program versions. Currently ignores the date components
    /// \param rightElement Application info to compare version.
    /// \return true if left version is lesser than right.
    bool operator <(const acSoftwareUpdateInfo& rightElement)
    {
        if (this->m_iProgramVersionMajor < rightElement.m_iProgramVersionMajor)
        {
            return true;
        }
        else if (this->m_iProgramVersionMajor == rightElement.m_iProgramVersionMajor &&
                 this->m_iProgramVersionMinor < rightElement.m_iProgramVersionMinor)
        {
            return true;
        }
        else if (this->m_iProgramVersionMajor == rightElement.m_iProgramVersionMajor &&
                 this->m_iProgramVersionMinor == rightElement.m_iProgramVersionMinor &&
                 this->m_iProgramVersionBuild < rightElement.m_iProgramVersionBuild)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /// Compares two Program versions. Currently ignores the date components
    /// \param rightElement Application info to compare.
    /// \return true if left version is grater than right.
    bool operator >(const acSoftwareUpdateInfo& rightElement)
    {
        if (this->m_iProgramVersionMajor > rightElement.m_iProgramVersionMajor)
        {
            return true;
        }
        else if (this->m_iProgramVersionMajor == rightElement.m_iProgramVersionMajor &&
                 this->m_iProgramVersionMinor > rightElement.m_iProgramVersionMinor)
        {
            return true;
        }
        else if (this->m_iProgramVersionMajor == rightElement.m_iProgramVersionMajor &&
                 this->m_iProgramVersionMinor == rightElement.m_iProgramVersionMinor &&
                 this->m_iProgramVersionBuild > rightElement.m_iProgramVersionBuild)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};




/// -----------------------------------------------------------------------------------------------
/// \class Name: acSoftwareUpdaterWindow : public QDialog
/// \brief Description:  The GUI for the updater object
/// -----------------------------------------------------------------------------------------------
class AC_API acSoftwareUpdaterWindow : public QDialog
{
    Q_OBJECT

public:
    /// constructor
    acSoftwareUpdaterWindow(const QString& productName, const acIconId& productIconId);


    /// Destructor
    ~acSoftwareUpdaterWindow();

    // Display the updater dialog:
    void displayDialog(bool forceDialogDisplay = true);

    void performAutoCheckForUpdate();

    // Initialize the current version details:
    void initVersionDetails(const osFilePath& userConfigFilePath, const gtString& iconImagePath);

private:
    void postInit();

    void setDialogLayout();

    /// To check\uncheck OnStartupCheckBox.
    /// \param flag To check or uncheck the CheckBox.
    /// \return True on success and false on failure.
    bool setOnStartUpCheckBox(bool flag);

    /// To set frequency for check update.
    /// \param days Will indicate the frequency
    /// \return True if it gets success else false.
    void setCheckUpdateFrequencyComboBox(int days);

    /// To set product informations
    /// \param prodVersion Will contain new product version
    /// \param versionDescriptionURL the url with the version description
    void setVersionDescriptionURL(const QString& prodVersion, const QString& versionDescriptionURL);

    /// Will set current status in the label.
    /// \param message Will have message string
    void setStatusLabel(const QString& message = "", int progress = -1);

    /// Will load user profile from local config XML file.
    /// \return true if loading is successfully else false.
    bool loadUserProfile();

    /// Will save program information and user profile information.
    void saveUserProfile();

    /// Will go and check for latest update.
    /// \param forceCheck flag to force the update
    /// \return true if check is successful.
    bool checkForUpdate();

    /// Will write into a file in byte formate
    /// \param fileName URL file path.
    /// \param data byte data that needs to write into the file.
    /// \return True if execution gets successful.
    bool writeByteFile(const QString& fileName, QIODevice* data);

    /// To add a DOM tree under specified node.
    /// \param doc Dom document root
    /// \param node Dom node in which new node has to be get attached.
    /// \param tag New element should have same tag.
    /// \param value Value to the new node.
    /// \return created new sub tree root.
    QDomElement addXMLElement(QDomDocument& doc, QDomNode& node, const QString& tag, const QString& value = QString::null);

    /// Will check if network is connected.
    /// \return True if there is successful network connection else false.
    bool isNetworkConnected();

signals:

    /// Gets emitted when updater needs to download latest application version
    void download();

    /// Called to get latest application version information from server.
    bool getLatestVersionInfo();


private:

    /// To enable/disable skip build button.
    void updateButtonsState();

    /// Connects the necessary slots to signals:
    void  connectSlots();

    /// Gets emitted when download is completed.
    void launchInstalledFile(const QString& filePath);

    /// Display an error message in the HTML window
    /// \param title the error title
    /// \param message the error content
    void updateWindowStatusLabels(const QString& title, const QString& message, const QString& status = "", const QString& header = "");


private slots:

    /// Called on change in the state of "Check for update on startup" CheckBox.
    void checkUpdateOnStartupStateChanged(int state);

    /// Called when "Install" button gets clicked:
    void onInstallClick();

    /// Called when "download Tarball" button gets clicked:
    void OnDownloadTarball();

    /// Called when "download RPM" button gets clicked:
    void OnDownloadRPM();

    /// Called when "download debian package" button gets clicked:
    void OnDownloadDebian();

    /// Called on "Cancel" button click:
    void onCancelClick();

    /// Called on "Close" button click.
    void onCloseClick();

    /// Will get called to update the change in frequency selection.
    void onUpdateCheckDaysChanged(const QString& text);

    /// Will get called once the version URL load had started:
    void onVersionLoadStart();

    /// Will get called once the version URL load had finished:
    void onVersionLoadFinish(bool status);

    /// Called when application package download is finished.
    void onSetupDownloadComplete(QNetworkReply* reply);

    /// Called when application package download is finished.
    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

    /// Called to download and parse latest version available on server.
    void onDownloadXMLFileFinish(QNetworkReply* reply);

    /// Called to get latest version information from server once proxy setting is done.
    bool onLazyGetLatestVersionInfo();

    /// Called to get latest application version information from server.
    bool onGetLatestVersionInfo();

    /// Will set available build as skipped build.
    /// \param to set or reset available build as skipped build.
    void onSkipBuildChanged(int state);

    /// Gets emitted to notify about update required or not.
    void setNewAvailableVersionDetails(bool updateNeeded);

    /// Displays the folder selection dialog.
    void onDestinationFolderButtonClick();

protected:

    enum acUpdaterDownloadType
    {
        AC_DOWNLOAD_EXE,
        AC_DOWNLOAD_RPM,
        AC_DOWNLOAD_TAR,
        AC_DOWNLOAD_DEB
    };

    // Overrides QDialog:
    virtual void closeEvent(QCloseEvent* pEvent);
    virtual void showEvent(QShowEvent* pEvent);

    /// Will find if need to check update.
    /// \return true if need to check update else false.
    bool isCheckForUpdateNeeded();

    /// Will download and install the package from specified URL.
    /// \return true if the download had succeeded
    bool downloadAndInstall();

    /// Will extract the content of the web XML config file to the data in this class:
    /// \return true if the parse of the file had succeeded
    bool extractXMLContentFromNetworkReply(QNetworkReply* pReply);

    /// Get the path to the downloaded version file, according to the download type:
    /// \return QString containing the download file path
    QString FindDownloadPath() const ;

    QString ShowFolderSelectionDialog(const QString& dialogCaption, QString& defaultFolder);
private:

    QString m_title;
    QString m_productName;
    gtString m_imagesPath;

    // Dialog GUI components:
    QCheckBox* m_pCheckOnStartUpCheckBox; ///< CheckBox to enable\disable check for update on startup option.
    QCheckBox* m_pSkipBuildCheckBox;    ///< CheckBox to skip this build
    QComboBox* m_pChkUpdateFrequencyComboBox;  ///< ComboBox to select update check frequency.
    QPushButton* m_pInstallButton;    ///< PushButton to start download.
    QPushButton* m_pCancelButton;    ///< PushButton to cancel the installation
    QPushButton* m_pCloseButton;    ///< PushButton to close the updater window
    QLabel* m_pUpdateHeaderLabel;       ///< Label for the dialog header
    QWebView* m_pVersionDetailsWebView; ///< TextEdit to show new update information.
    QLabel* m_pStatusLabel;           ///< Status bar
    QProgressBar* m_pProgressBar;       ///< Status bar
    QLabel* m_pDestinationFolderLabel;    ///< Label for the destination folder text box.
    QLineEdit* m_pDestinationFolderLineEdit;    ///< Label for the destination folder text box.
    QToolButton* m_pDestinationFolderButton;    ///< Button for displaying the destination folder dialog.

    acSoftwareUpdateInfo* m_pCurrentInfo;            ///< Contains current application version informations.
    acSoftwareUpdateInfo* m_pLatestVersionInfo;      ///< Contain latest application version available.


    int m_errorCode;                    ///< Will keep record of any recent error code passed by Updater backend.

    int  m_iSkippedBuild;               ///< Contains skipped build version
    bool m_shouldSkipThisBuild;              ///< Indicates if updater has skipped the current build.
    QString m_strDownloadPath;      ///< Contain download file path.

    bool m_isAutoCheckEnabled;           ///< Indicates if Auto check is enabled
    QString m_installedProdVersion;     ///< Installed product version
    QString m_latestProdVersion;        ///< Latest available product version
    int m_iNextCheckScheduleInDay;          ///< Contain next check schedule in terms of no. of days.
    QDate m_lastCheckDate;                  ///< Contains Last checked Date.

    QString m_strUserProfile;           ///< Contains local user XML file path
    QString m_strVersionInfoURL;        ///< Conatin server XML file URL

    bool m_isNewerVersionAvailable;          ///< Indicate if newer version is available
    bool m_isProcessingUpdate;                  ///< Indicate update request is processing or over
    bool m_isVersionDescriptionDisplayed;                  ///< Indicate update request is processing or over

    bool m_isUpdateNeeded;             ///< Indicate if dialog pop up is required.

    QUrl m_setUpUrl;                        ///< Contains package URL path.
    QNetworkAccessManager* m_pNetworkManager;     ///< Network access manager to download latest XML file from server.
    QNetworkAccessManager m_downloadManager;///< Network access manager to download latest package from server.
    QNetworkReply* m_pNetworkReply;                  ///< Network reply

    acSoftwareUpdaterThread* m_pUpdaterThreadForLatestInfo;   ///< Thread to fetch latest info from server
    acSoftwareUpdaterThread* m_pUpdaterThreadToDownloadPackage;///< Thread to download package from server

    qint64 m_bytesReceived;
    qint64 m_bytesTotal;

    bool m_isDownloading;
    bool m_isCheckingForNewUpdate;
    bool m_isForcingUpdateCheck;
    bool m_isDialogRaised;

    acUpdaterDownloadType m_downloadFileType;

    acIconId m_ProductIconId;
};



#endif //__ACSOFTWAREUPDATERWINDOW_H

