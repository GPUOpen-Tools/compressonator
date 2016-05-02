//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acFunctions.h 
/// 
//================================================================================== 

//------------------------------ acFunctions.h ------------------------------

#ifndef __ACFUNCTIONS
#define __ACFUNCTIONS

// Qt:
#include <qtIgnoreCompilerWarnings.h>
#include <QColor>
#include <QString>
#include <QTableWidget>
#include <QtWidgets>

// Forward declarations:
class QWebView;

// Infra:
#include <AMDTBaseTools/Include/gtString.h>
#include <AMDTBaseTools/Include/gtASCIIString.h>
#include <AMDTOSWrappers/Include/osFilePath.h>

// Local:
#include <AMDTApplicationComponents/Include/acApplicationComponentsDLLBuild.h>



#define AC_HOURS_POSTFIX " hrs"
#define AC_MINUTES_POSTFIX " min"
#define AC_SECONDS_POSTFIX " sec"
#define AC_MSEC_POSTFIX  "ms"
#define AC_MICROSEC_POSTFIX  "\xc2\xb5s"
#define AC_NSEC_POSTFIX  "ns"
#define AC_MSEC_STR_TIME_STRUCTURE "%02d:%02d:%02d.%03d"
#define AC_NANOSEC_STR_TIME_STRUCTURE "%02d:%02d:%03d.%03d"
#define AC_MICROSEC_STR_TIME_STRUCTURE "%d.%03d"


/// The class is used to add line breaks to a single line string
/// Each line
class acLineBreaker
{
public:

    /// Call this function when you want to convert a single line string (org), to a multiple lines string(dest)
    /// Line is built to contain TOOLTIP_MAX_CHARS_IN_LINE
    /// \param org the single line string
    /// \param dest[out] the multiple lines string
    static void AddLineBreaksToTooltipString(const QString& org, QString& dest);

private:

    /// Private constructor
    acLineBreaker() {}

    enum TypeOfEndPosChar
    {
        PUNCTUATION,
        WHITESPACE,
        OTHER
    };

    static void FindPosForLineBreak(const QString& org, unsigned int readIndex, unsigned int& endOfLineIndex, TypeOfEndPosChar& endPosCharType);
};


AC_API QColor acGetSystemDefaultBackgroundColor();
AC_API gtString acGetSystemDefaultBackgroundColorAsHexString();
AC_API QString acGetSystemDefaultBackgroundColorAsHexQString();
AC_API gtString acQColorAsHexString(const QColor& color);
AC_API QString acQColorAsHexQString(const QColor& color);

/// QString <==> GTString
void AC_API acWideQStringToGTString(const QString& org, gtString& dst);

/// QString <==> GTString
void AC_API acQStringToOSFilePath(const QString& org, osFilePath& dst);

/// Converts a gtString to a QString
/// \param inputStr the gtString instance to convert
/// \return the converted QString
QString AC_API acGTStringToQString(const gtString& inputStr);

/// Converts a gtASCIIString to a QString
/// \param inputStr the gtASCIIString instance to convert
/// \return the converted QString
QString AC_API acGTASCIIStringToQString(const gtASCIIString& inputStr);

/// Converts a QString to a gtString
/// \param inputStr the QString instance to convert
/// \return the converted gtString
gtString AC_API acQStringToGTString(const QString& inputStr);

/// Converts a QString to a WChar array
/// \param inputStr the QString instance to convert
/// \param pOutputStr the wchar_t after conversion
void AC_API acQStringToWCharArray(const QString& inputStr, wchar_t*& pOutputStr);

/// Convert a long number to a string
gtString AC_API  acDurationAsString(unsigned long duration);

/// Build formatted tooltip string:
/// \param title - the title
/// \param description - description of item
/// \param tooltip[out] - the formatted tooltip
AC_API void acBuildFormattedTooltip(const QString& title, const QString& description, QString& tooltip);

/// Build formatted tooltip string:
/// \param title - the title
/// \param description - description of item
/// \param tooltip[out] - the formatted tooltip
AC_API void acWrapAndBuildFormattedTooltip(const QString& title, const QString& description, QString& tooltip);

/// Build formatted string:
/// \param title - the title
/// \param description - description of item
/// \param str[out] - the formatted string
AC_API void acBuildFormattedStringByLength(const QString& title, const QString& description, QString& str);

/// Export the requested table widget to a CSV file
/// \param outputFilePath the CSV output file path
/// \param pTableWidget the table widget for export
/// \return true if the export succeeded
AC_API bool acExportTableViewToCSV(const QString& outputFilePath, const QTableWidget* pTableWidget);

/// Export the requested table widget to a CSV file
/// \param outputFilePath the CSV output file path
/// \param pWebView a web view on which a page is displayed. The function assumes that the HTML text contain a single table, and this single table will be exported to CSV
/// \return true if the export succeeded
AC_API bool acExportHTMLTableToCSV(const QString& outputFilePath, const QWebView* pWebView);

/// Extracts the file extension from a QString representing a file path:
/// \filePath the requested file path
/// \isExtensionStructureValid make sure that only single dot is used in the file path (E.g. "filename....ext" is not valid)
/// \return the file extension or an empty string is there isn't. The function returns the last extension,
/// if there are multiple. E.g. filename.ext1.ext2 will return ext2
AC_API QString acGetFileExtension(const QString& filePath, bool& isExtensionStructureValid);

/// Get the global position (screen) from a local position (client) relative to a specific widget.
/// Not using the Qt QWidget mapToGlobal in Windows in VS since in VS the main window is not Qt window and the mechanism is not working correctly
/// and we need to revert to low level win32 API
/// \param pWidget the relative local widget
/// \param localPosition the local position in the widget
/// \return the global position
AC_API QPoint acMapToGlobal(QWidget* pWidget, const QPoint& localPoint);

// Platform-specific functions:
#if ((AMDT_BUILD_TARGET == AMDT_LINUX_OS) && (AMDT_LINUX_VARIANT == AMDT_GENERIC_LINUX_VARIANT))
    // Generic Linux variants:
    AC_API void acValidateGTKThemeIcons();
#endif // AMDT_BUILD_TARGET


// Get version info:
AC_API bool acGetVersionDetails(int& buildVersion, int& majorVersion, int& minorVersion, int& year, int& month, int& day);


/// Translates milliseconds into time format including msec if last parameter is true
/// Zero part is not displayed, i.e. 00:34.332 will be displayed as 34.332
/// \param msec - milliseconds
/// \param bForActiveRange is true if for active range
/// \param bUseWords
/// \param millisecondsOnly - show only milliseconds (do not converst to seconds, minutes..)
/// \returns the time as string
AC_API QString NanosecToTimeString(double msec, bool bForActiveRange = false, bool bUseWords = false, bool millisecondsOnly = true);

/// Translates milliseconds into time format including msec if last parameter is true
/// Zero part is not displayed, i.e. 00:34.332 will be displayed as 34.332
/// \param msec - milliseconds
/// \param shouldShowSmallestUnits is true if the smallest digits should be displayed
/// \param shouldUseWords
/// \returns the time as string
AC_API QString MsecToTimeString(double msec, bool shouldShowSmallestUnits = false, bool shouldUseWords = false);

/// Translates microseconds into time format including msec if last parameter is true
/// Zero part is not displayed, i.e. 00:34.332 will be displayed as 34.332
/// \param microsec - microseconds
/// \param bForActiveRange is true if for active range
/// \param bUseWords
/// \returns the time as string
AC_API QString MicroSecToTimeString(double microsec, bool bForActiveRange = false, bool bUseWords = false);

AC_API QString NanosecToTimeStringFormatted(double nanoseconds, bool shouldShowSmallestUnits = false, bool shouldUseWords = false, bool millisecondsOnly = true);

/// Return true iff the child is a direct or an indirect child of the parent
/// \param pChild the child object
/// \param pParent the possible parent object
/// \return true iff pChild is a direct or an indirect child of pParent
AC_API bool acIsChildOf(QObject* pChild, QObject* pParent);

#if AMDT_BUILD_TARGET == AMDT_LINUX_OS
    AC_API QIcon acGetPushButtonIcon(QPushButton* pPushButton, const gtString& iconPath);
    AC_API QIcon acGetToolButtonIcon(QToolButton* pToolButton, const gtString& iconPath);
    AC_API void acPrepareDialog(QFileDialog& dialog, const gtString& imagesPath);
#endif


#endif  // __ACFUNCTIONS
