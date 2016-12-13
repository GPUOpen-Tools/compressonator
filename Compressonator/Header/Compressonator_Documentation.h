//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006  ATI Technologies Inc.
//===============================================================================
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
//
//  File Name:   Compressonator_Documentation.h
//  Description: A library to compress/decompress textures
//
//////////////////////////////////////////////////////////////////////////////

/// \mainpage Compressonator lib
///
/// \section intro_sec Introduction
///
/// Compressonator lib is an library for texture compression and is used by the CompressonatorCLI.exe 
/// It is also available to third party developers who wish to incorporate it within their own applications. 
/// It can compress to a wide range of compression formats including ATC, ASTC, ATI2N, BCn (DXT & swizzled DXT) and ETC formats.
/// It also supports conversion of textures between 8bit fixed, 16bit fixed & 32bit float formats.
///
/// \section install_sec Installation
///
/// The Compressonator installer installs the Compress files to the "Program Files" or "Program Files (x86)"
/// directory by default and adds the "COMPRESSONATOR_ROOT" environment variable to point to this. You can
/// copy these files to any other location to suit your build system but it is recommended that you update
/// the "COMPRESSONATOR_ROOT" environment variable & continue to use this to point to the location of Compress tools.
/// 
/// Compressonator includes library files in multiple flavors - for Visual Studio 2013 to Visual Studio 2015, x86 & x64 and for multiple C runtime types. 

//  Enable in documents when DLL files are ready...
//  If the particular combination you require is not included the VC\\DLL versions can be used with any other blend of compiler & C runtime by explicitly loading and linking 
//  the DLL using LoadLibrary & GetProcAddress.

///
/// \section Compatibility_sec Compatibility
/// 
/// AMD Compress tools has been tested with Visual Studio 2013 and 2015 (x86 & x64). 
/// Some versions of the AMD sample applications may require the Microsoft DirectX SDK and DirectXTex Lib to be installed
///
/// \section KnownIssues_sec Known Issues or Support
/// 
/// Decompression of ASTC file formats is limited to 4x4 blocks- Future versions will allow full decompression\n
/// Current ASTC implementation supports only 2D Textures\n
/// For optimal compression ratios, use source images in uncompressed format\n
/// 
/// \section Contact_sec Contact Details
///
/// email: gputools.support@amd.com
/// 
/// website: http://developer.amd.com/tools-and-sdks/graphics-development/
/// 
/// \page VersionHistory Version History
///
/// \section v24 v2.4
/// \li    Improve ASTC performance, Enable 64bit CLI app.
///
/// \section v21 v2.1
/// \li    Supports ASTC destination compression format with variable bit rates or 2D block sizes
///
/// \section v20 v2.0
/// \li    Supports all BCn compression formats.
/// \li    Fast compression modes.
/// \li    Direct access to 4x4 pixel block buffers in BC7 and BC6H compression codecs.
/// \li    Help file documentation and C++ source code examples on how to use the library.
/// 
/// \page using_sec Using Compressonator lib
/// 
/// Compressonator lib is simple to use with a single function #CMP_ConvertTexture supplying the functionality
/// of the library. 
/// 
/// \page feedback_sec Using a callback function
/// 
/// When you supply a CMP_Feedback_Proc pointer to #CMP_ConvertTexture you receive a callback that
/// updates you on compression progress & allows you to cancel the compression. This callback is not
/// guaranteed to be called & is currently only called when converting to a compressed format.
/// 

