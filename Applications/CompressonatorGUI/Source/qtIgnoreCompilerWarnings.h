//------------------------------ qtIgnoreCompilerWarnings.h ------------------------------

#ifndef __QTIGNORECOMPILERWARNINGS_H
#define __QTIGNORECOMPILERWARNINGS_H

// Local:

// ----------------------------------------------------------------------------------
// File Name:            qtIgnoreCompilerWarnings
// General Description:
//  This file contains commands that instruct the compiler to ignore warnings
// .due to missing declaration in QT
//  that we thing should be GLOBALLY ignored.
//  This file is based on gtIgnoreCompilerWarnings
//  If you would like to have a single warning ignored for a given code, please
//  use #pragma warning( push ) and #pragma warning( pop ) instead.
//
// Author:               Gilad Yarnitzky
// Creation Date:        7/2/2015
// ----------------------------------------------------------------------------------

// If this is a Microsoft compiler build:
#if GR_CPP_COMPILER == GR_VISUAL_CPP_COMPILER

// foreach(Type T name, Container will generate the following two warnings)
// assignment operator could not be generated
#pragma warning( disable : 4512 )

// conditional expression is constant
#pragma warning( disable : 4127 )

// recursive call has no side effects, deleting
#pragma warning( disable : 4718 )

#pragma warning( disable : 4456 )

#pragma warning( disable : 4996 )

#endif // GR_CPP_COMPILER

#endif //__QTIGNORECOMPILERWARNINGS_H
