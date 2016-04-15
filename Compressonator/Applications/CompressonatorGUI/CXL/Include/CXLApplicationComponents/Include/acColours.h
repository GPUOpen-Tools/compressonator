//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file acColours.h 
/// 
//================================================================================== 

//------------------------------ acColours.h ------------------------------

#ifndef __ACCOLOURS_H
#define __ACCOLOURS_H
// Qt:
#include <QtWidgets>

// Local:
#include <CXLApplicationComponents/Include/acApplicationComponentsDLLBuild.h>
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #pragma warning( push )
    #pragma warning( disable : 4091)
#endif
// Graphic Remedy:
// AC_API extern QColor acQGREMEDY_ORANGE_COLOUR;       // Graphic remedy Orange color.

// AMD Nov 2012 branding:
// AC_API extern QColor acQAMD_GREEN_COLOUR;               // AMD Green color
// AC_API extern QColor acQAMD_RED_COLOUR;                 // AMD Red color
// AC_API extern QColor acQAMD_ORANGE_COLOUR;              // AMD Orange accent color
// AC_API extern QColor acQAMD_PURPLE_COLOUR;              // AMD Purple accent color
// AC_API extern QColor acQAMD_GRAY_COLOUR;                // AMD Gray accent color
// AC_API extern QColor acQAMD_AQUA_COLOUR;                // AMD Aqua accent color
// AC_API extern QColor acQAMD_YELLOW_COLOUR;              // AMD Yellow accent color

// AMD Sep 2013 branding:
AC_API extern QColor acQAMD_RED_PRIMARY_COLOUR;             // AMD Red primary color
AC_API extern QColor acQAMD_PURPLE_PRIMARY_COLOUR;          // AMD Purple primary color
AC_API extern QColor acQAMD_CYAN_PRIMARY_COLOUR;            // AMD Cyan primary color
AC_API extern QColor acQAMD_GREEN_PRIMARY_COLOUR;           // AMD Green primary color
AC_API extern QColor acQAMD_ORANGE_PRIMARY_COLOUR;          // AMD Orange primary color
AC_API extern QColor acQAMD_RED_OVERLAP_COLOUR;             // AMD Red overlap color
AC_API extern QColor acQAMD_PURPLE_OVERLAP_COLOUR;          // AMD Purple overlap color
AC_API extern QColor acQAMD_CYAN_OVERLAP_COLOUR;            // AMD Cyan overlap color
AC_API extern QColor acQAMD_GREEN_OVERLAP_COLOUR;           // AMD Green overlap color
AC_API extern QColor acQAMD_ORANGE_OVERLAP_COLOUR;          // AMD Orange overlap color
AC_API extern QColor acQAMD_GRAY1_COLOUR;                   // AMD Brand gray color 1 (darkest)
AC_API extern QColor acQAMD_GRAY2_COLOUR;                   // AMD Brand gray color 2 (middle)
AC_API extern QColor acQAMD_GRAY3_COLOUR;                   // AMD Brand gray color 3 (lightest)
AC_API extern QColor acQAMD_GRAY_LIGHT_COLOUR;              // White-gray of the same hue as the AMD gray colors
AC_API extern QColor acQAMD_CYAN_SELECTION_BKG_COLOUR;      // Light-blue

AC_API enum acAMDColorHue
{
    AC_AMD_RED,
    AC_AMD_PURPLE,
    AC_AMD_CYAN,
    AC_AMD_GREEN,
    AC_AMD_ORANGE,
    AC_AMD_GRAY,
};

AC_API const QColor& acGetAMDColorScaleColor(acAMDColorHue hue, unsigned int indexInSeries);

AC_API enum acCodeXLColorHue
{
    AC_CODEXL_BLUE,
    AC_CODEXL_MAGENTA,
};

AC_API const QColor& acGetCodeXLColorScaleColor(acCodeXLColorHue hue, unsigned int indexInSeries);

// Warning colors:
AC_API const QColor& acGetWarningScaleColor(int percentage);
AC_API extern QColor acQGREEN_WARNING_COLOUR;           // A color used to highlight no warning
AC_API extern QColor acQYELLOW_WARNING_COLOUR;          // A color used to highlight mild warnings
AC_API extern QColor acQORANGE_WARNING_COLOUR;          // A color used to highlight medium warnings
AC_API extern QColor acQRED_WARNING_COLOUR;             // A color used to highlight severe warnings

// Other colours:
AC_API extern QColor acQLIST_HIGHLIGHT_COLOUR;          // A color used to highlight Qt list items
AC_API extern QColor acQLIST_EDITABLE_ITEM_COLOR;       // A color used for editable items in Qt list
AC_API extern QColor acQGREY_TEXT_COLOUR;               // Grey text color
AC_API extern QColor acQRAW_FILE_NOT_IN_SCOPE_COLOR;
AC_API extern QColor acQRAW_FILE_ABOVE_RANGE_COLOR;
AC_API extern QColor acQRAW_FILE_BELOW_RANGE_COLOR;
AC_API extern QColor acQRAW_FILE_TOP_RANGE_COLOR;
AC_API extern QColor acGRAY_BG_COLOR;
AC_API extern QColor acYELLOW_INFO_COLOUR;              // A color used to highlight labels
AC_API extern QColor acRED_NUMBER_COLOUR;               // A color used to paint a red number in tables
AC_API extern QColor acDARK_GREEN;                      // dark Green used in navigation ribbons
AC_API extern QColor acLIGHT_GREEN;                     // light Green used in navigation ribbons
AC_API extern QColor acDARK_PURPLE;                     // dark purple used in navigation ribbons
AC_API extern QColor acLIGHT_PURPLE;                    // light purple used in navigation ribbons

AC_API extern QColor acPATH_SELECTED_COLOR; // Path Indicator selected color
AC_API extern QColor acPATH_HOVER_COLOR; // Path Indicator hover color

// changes the color to its negative color
AC_API void acReadableNegativeColor(QColor& color);

// Blends two colors:
AC_API void acBlendInto(QColor& base, const QColor& top);
AC_API void acBlendIntoWithAlpha(QColor& base, const QColor& top, int topA);
#if AMDT_BUILD_TARGET == AMDT_WINDOWS_OS
    #pragma warning( pop )
#endif
#endif //__ACCOLOURS_H

