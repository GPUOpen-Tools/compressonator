#ifndef AMDCOMPRESS_DOC_H
#define AMDCOMPRESS_DOC_H

//=====================================================================
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

/*===================
AMDCompress CLI Tool
=====================*/

/// \mainpage AMDCompressCLI.exe Tool
/// 
///
/// Compressed textures are now a standard part of many real-time 3D applications.
/// The improvements in performance obtained by reducing the memory size and bandwidth requirements of texture maps,
/// and the wide availability of hardware support have resulted in extensive adoption of the industry-standard BCn (DXTn) compression formats.
/// This document describes AMD's comprehensively-featured tool for generating these compressed texture maps and also gives information on how to use it
/// to get the best possible quality results when using texture compression in your application.
///
/// \page Info Info
///
/// AMDCompressCLI.exe is a command line tool that uses AMD Compress library and image IO\n
/// plug-ins for compressing textures and creating mip-map levels.
/// For support or more infomation you can send a request using the \subpage Contact page
/// 
/// \page VersionHistory Version History
/// Version History\n
/// \subpage version21
/// \subpage version20
///
/// \page command_line Commadline Options
/// \subpage CLIOptions \n
/// \subpage codec_options \n
///
/// \page images Supported Image Formats
/// \subpage image_formats
/// The command line tool supports a variety of image formats and compression formats.\n
//  Input or output image formats can be changed by adding into or removing plugins stored in a subdirectory called plugins.
///
/// Texture compression formats are always available in CompressonatorCLI.exe.\n
/// See Formats section for more details on what compression is available for this version of the AMD compress tools.
///
///
/// \page compression_formats Compression Formats
/// The Command line tool supports a wide range of compression formats and image files through it's various image IO plugins. 
///
/// This list describe the most commonly used of these formats, the command line option to specify these formats is -fd 
///
/// \subpage formats_Bitn  Supports conversion of textures between 8bit fixed, 16bit fixed & 32bit float formats\n
/// \subpage formats_ATC   Compression used for handheld devices to save power consumption, memory and bandwidth\n
/// \subpage formats_ASTC  Adaptive Scalar Texture Compression\n
/// \subpage formats_ATI2n Compression format supported by AMD hardware and exposed through DirectX and OpenGL APIs\n 
/// \subpage formats_BCn   Provides options for generating textures in all of the block compression formats\n
/// \subpage formats_DXTn  Provides options for generating textures in all of the basic DXT compression formats\n
/// \subpage formats_ETCn  Compression formats
///
///


/************************************************
*                VERSION HISTORY
*************************************************/

    /// \page version20 V2.0 
    ///  \n
    /// \li    Supports all BCn compression formats.\n
    /// \li    Fast compression modes.\n
    /// \li    Direct access to 4x4 pixel block buffers in BC7 and BC6H compression codecs.\n
    /// \li    Help file documentation and C++ source code examples on how to use the library.\n
    ///\n
    /// \page version21 V2.1 
    ///
    /// SDK Libraries\n
    /// \li    Provided for Microsoft Visual Studio 2015 and 2013\n
    /// \li    Added Adaptive Scalar Texture Compression(ASTC) support for variable block sizes(4x4 to 12x12) and bit rates from 8.0 to 0.89 bits per pixel\n
    ///
    /// Command Line features\n
    /// \li    Added support for TIF and TGA source image formats(to list of previously supported formats DDS, BMP, PNG and EXR)\n
    /// \li    Allows users to add additional codec using Qt v5.5 or third party plugins\n
    /// \li    Added support for Khronos Texture(.KTX) output formats for OpenGL ES 2 and up\n
    /// \li    Added.ASTC output file format\n
    ///\n


/************************************************
*                COMMAND LINE OPTIONS
*************************************************/


    /************************************************
    *                Using Command Line Optons
    *************************************************/

    /// \page CLIOptions Using Command Line Optons
    /// Commadline options are set using the following syntax:  <b>codec_options [value]</b>  
    ///
    /// For example to run BC7 compression on a bit mapped image file called input.bmp and save the result to a DirectDraw Surface file called Result.dds type at the command prompt
    ///
    ///    AMDCompressCLI.exe -fd BC7 -NumThreads 16 Input.bmp Result.dds 
    ///
    /// In the example the first codec_option instructs the application to set the destination format -fd to that of BC7\n
    ///
    /// Next codec_option -NumThreads instructs the application to use 16 threads during compression, each of which will continually compress asynchronously 4x4 pixel image blocks at any given time until the entire image is compressed.\n
    /// 
    /// Finally the application is given the location of the source image to use and the destination file used to save the resulting compression.\n
    ///
    ///
    /// All codec_options starts with a hyphen sign character followed by an option key word that is case sensative, most key words are followed by a value that specifies a setting for the codec to use.\n
    /// For additional details on selecting the compression formats see the help section on \ref compression_formats\n 

    /************************************************
    *                CODEC OPTIONS
    *************************************************/

    /// \page codec_options codec options
    /// Make sure you have first read \ref CLIOptions "Using Command Line Options".
    ///
    ///
    /// The following codec options are defined and are all case sensitive:
    ///
    /// \section codec1 -UseChannelWeighting [value]
    ///
    /// Use channel weightings. With swizzled formats the weighting applies to the data within the specified channel not the channel itself; Value of 1 turns the option 0n, 0 turns the option off
    ///
    /// <b>Colour Weighting</b>\n
    /// These Weight controls allow the user to specify what weighting the compressor will give to the various colour components when it is considering how to compress a block. The default weightings that are supplied are set up to match the relative importance of the colour channels with respect to their contribution to the output colour's luminance, so green has the highest weighting followed by red and blue. Although this generally gives the highest quality of output overall there may be cases where you will want he encoder to have different priorities - for example, if the image you are trying to compress does not contain colour data, and therefore the weightings should not be luminance-biased.
    ///
    /// \section codecWeightR -WeightR [value]
    ///
    /// The weighting of the Red or X Channel; value can be in the range of 0.1 to 1.0
    ///
    /// \section codecWeightG -WeightG [value] 
    ///
    /// The weighting of the Green or Y Channel; value can be in the range of 0.1 to 1.0
    ///
    /// \section codecWeightB -WeightB [value]
    ///
    /// The weighting of the Blue or Z Channel; value can be in the range of 0.1 to 1.0
    ///
    /// \section codecDXT1UseAlpha -DXT1UseAlpha [value]
    ///
    /// Encode single-bit alpha data Only valid when compressing to BC1; Value of 1 turns the option 0n, 0 turns the option off
    ///
    /// \section codecAlphaThreshold -AlphaThreshold [value]
    ///
    /// Adapt weighting on a per-block basis, Value sets the alpha threshold to use when compressing to BC1 with DXT1UseAlpha. Texels with an alpha value less than the threshold are treated as transparent.
    /// Value can be from 0.0 to 1.0
    ///
    /// \section codecBlockRate -BlockRate [value]
    ///
    /// ASTC 2D only - sets block size or bit rate\n
    /// value can be a bit per pixel rate from 0.0 to 9.9 or can be a combination of x and y axes with paired values of 4, 5, 6, 8, 10 or 12 from 4x4 to 12x12\n
    ///
    /// \section codecCompressionSpeed -CompressionSpeed [value]
    ///
    /// The trade-off between compression speed & quality, default is set to fast; this value is ignored for BC6H and BC7 (for BC7 the compression speed depends on Quaility and Performance settings)
    /// Value of 0 (default) sets Highest quality mode, value of 1 sets slightly lower quality but much faster compression mode, value of 2 sets slightly lower quality but much, much faster compression mode 
    ///
    /// \section codecNumThreads -NumThreads [value]
    ///
    /// Number of threads to initialize for BC6H or BC7 encoding. Value can be from 1 to 128, default set to 8;
    ///
    /// \section codecQuality -Quality [value]             
    ///
    /// Quality of encoding for BC7. This value ranges between 0.0 and 1.0
    /// setting quality above 0.00 gives the fastest, lowest quality encoding, 1.0 is the slowest, highest quality encoding.\n
    /// default is set to a low value of 0.05
    ///
    /// \section codecPerformance -Performance [value]             
    ///
    /// Performance of encoding for BC7. Setting performance at 1.0 gives the fastest encoding for a given quality level, lower values will improve quality but increase encoding time.\n
    /// default is set to 1.0
    ///
    /// \section codecColourRestrict -ColourRestrict [value]      
    ///
    /// This setting is a quality tuning setting for BC7 which may be necessary for convenience in some applications; Value of 1 turns the option 0n, 0 turns the option off.\n
    /// default is set to 0
    ///
    /// \section codecAlphaRestrict -AlphaRestrict [value]
    ///
    /// This setting is a quality tuning setting for BC7 which may be necessary for some textures;
    /// if set and the block does not need alpha it instructs the code not to use modes that have combined colour + alpha - this
    /// avoids the possibility that the encoder might choose an alpha other than 1.0 (due to parity) and cause something to
    /// become accidentally slightly transparent (it's possible that when encoding 3-component texture applications will assume that
    /// the 4th component can safely be assumed to be 1.0 all the time; 
    /// Value of 1 turns the option 0n, 0 turns the option off\n
    /// default is set to 0
    ///
    /// \section codecModeMask -ModeMask [value]
    ///
    /// Mode to set BC7 to encode blocks using any of 8 different block modes in order to obtain the highest quality. Default set to 0xCF,
    /// You can combine the bits to test for which modes produce the best image quality.\n 
    ///
    /// \section codecSigned -Signed [value]
    ///
    /// Used for BC6H only, Default BC6H format disables use of a sign bit in the 16-bit floating point channels, with a value set to 1 BC6H format will use a sign bit; Value of 1 turns the option 0n, 0 turns the option off 
    /// 
    /// \section codecnomipmap -nomipmap
    ///
    /// Disables use of mip map generation, if input image is mip mapped it will read the file as normal. 
    ///
    /// \section codecmiplevels -miplevels [value]
    ///
    /// Sets the minimum mip levels to generate on the output file, default is 1. if the level is set greater than 1 and lower than an input files mip levels, the 
    /// output images will be converted to a mipmap set by miplevels. (mipsize overides this option)
    ///
    /// \section codecmipsize -mipsize [value]
    /// The size in pixels used to determine how many mip levels to generate on the output file
    ///
    /// \section codecsilent -silent
    ///
    /// Disables the printing of command line messages
    ///
    /// \section codecperformance -performance
    /// Shows various performance statistics 
    ///
    /// \section codecnoprogress -noprogress
    ///
    /// Disables the printing of command line progress information during compression and decompression
    ///

/*=======================================
            Supported Image Formats
==========================================*/

    /************************************************
    *                IMAGE FORMATS
    *************************************************/

    /// \page image_formats Image Formats
    /// The following formats are currently provided:  
    /// (Most formats are supported directly by AMD Compress CLI application, additional support is provided by DLL plugins)
    ///
    /// \section plugins1 BMP
    ///
    /// Bitmap Image File formats of arbitrary width, height, and resolution.  Input images can be formatted as ARGB (8:8:8:8 bits per channel), RGB (8:8:8 bits per channel), 4 bit per single channel or 1 bit per channel.
    /// The output file is always saved as ARGB (8:8:8:8 bits per channel), format.
    ///
    /// \section plugins2 DDS
    ///
    /// Direct Draw Surface files
    ///
    /// \section plugins3 EXR
    /// High Dynamic Range Imaging format
    /// Supports only RGBA scan line based input files, output files can be RGBA or 16 bit tiled RGB
    ///
    /// \section plugins4 KTX
    ///
    /// Khronos Texture formats for OpenGL ES 2 and up
    ///
    /// \section plugins5 PNG
    ///
    /// Portable Network Graphics files.  
    /// 
    /// \section plugins6 TGA
    ///
    /// Plugin (QTGA.DLL) for TrueVision (TARGA) graphics file format (located in folder plugins - imageformats)
    /// 
    /// \section plugins7 TIFF 
    ///
    /// Plugin (QTIFF.DLL) for Tagged Image File Format(located in folder plugins - imageformats)
    /// 
    /// \section plugins8 UserDefined
    ///
    /// Users can now add new image format DLL to the folder:  plugins - imageformats 
    /// for use with AMD Compress CLI. These DLL's can be obtained from Qt web site by searching for "Qt image plugins"
    /// or similar sites.


/*=======================================
            Compression Formats
==========================================*/

    /************************************************
    *            Bit Conversions
    *************************************************/
    /// \page formats_Bitn Bit Conversions
    /// \section Conversion1 ARGB_16        
    /// A ARGB format with 16-bit fixed channels
    /// \section Conversion2 ARGB_16F       
    /// An ARGB format with 16-bit floating-point channels
    /// \section Conversion3 ARGB_32F       
    /// An ARGB format with 32-bit floating-point channels
    /// \section Conversion4 ARGB_2101010   
    /// An ARGB format with 10-bit fixed channels for colour & a 2-bit fixed channel for alpha
    /// \section Conversion5 ARGB_8888      
    /// An ARGB format with 8-bit fixed channels
    /// \section Conversion6 R_8               
    /// A single component format with 8-bit fixed channels
    /// \section Conversion7 R_16           
    /// A single component format with 16-bit fixed channels
    /// \section Conversion8 R_16F          
    /// A two component format with 32-bit floating-point channels 
    /// \section Conversion9 R_32F          
    /// A single component with 32-bit floating-point channels
    /// \section Conversion10 RG_8           
    /// A two component format with 8-bit fixed channels 
    /// \section Conversion11 RG_16          
    /// A two component format with 16-bit fixed channels 
    /// \section Conversion12 RG_16F         
    /// A two component format with 16-bit floating-point channels 
    /// \section Conversion13 RG_32F         
    /// A two component format with 32-bit floating-point channels 
    /// \section Conversion14 RGB_888        
    /// A RGB format with 8-bit fixed channels 

    /************************************************
    *                ATC
    *************************************************/
    /// \page formats_ATC ATC 
    /// \section format0 ATC          
    /// Compressed RGB format with optional explicit or interpolated alpha 
    ///

    /************************************************
    *                ASTC
    *************************************************/
    /// \page formats_ASTC ASTC 
    /// \section format1 ASTC          
    /// Adaptive Scalable Texture Compression
    ///

    /************************************************
    *                ATI2n
    *************************************************/
    /// \page formats_ATI2n ATI2n 
    /// \section format2 ATI1N          
    /// A single component compression format using the same technique as DXT5 alpha. Four bits per pixel
    ///
    /// \section format3 ATI2N          
    /// A two component compression format using the same technique as DXT5 alpha. Designed for compression object
    ///
    /// \section format4 ATI2N_XY       
    /// A two component compression format using the same technique as DXT5 alpha. The same as ATI2N but with the channels swizzled. Eight bits per pixel
    ///
    /// \section format5 ATI2N_DXT5     
    /// An ATI2N like format using DXT5. Intended for use on GPUs that do not natively support ATI2N. Eight bits per pixel

    /************************************************
    *                BCn
    *************************************************/
    /// \page formats_BCn BCn 
    /// \section format6 BC1 
    /// Stores three-component colour data using a 5:6:5 colour (5 bits red, 6 bits green, 5 bits blue). 
    /// Used for compressing opaque RGB and ARGB textures with 1-bit alpha. As it's alpha support is 1-bit (i.e. fully opaque or fully translucent) it is not suitable for compressing textures with more complex alpha channels. It is the must suitable DXTC format for compressing textures without an alpha channel. 
    ///
    /// \section format7 BC2 
    /// Stores RGB data as a 5:6:5 colour and alpha as a separate 4-bit value.
    /// Used for compressing ARGB textures with 4 bits per pixel for colour data & 4 bits per pixel explicit alpha data. It's use of explicit alpha makes it particularly suitable for textures where the alpha values within a block vary greatly. BC2 is identical to BC3 except for it's use of pre-multiplied alpha and can be ignored. 
    ///
    /// \section format8 BC3 
    /// Stores colour data using 5:6:5 colour and alpha data using one byte.
    /// Uses of interpolated alpha makes it most suitable for use with majority of textures with an alpha channel. 
    ///
    /// \section format9 BC4 
    /// Stores one-component colour data using 8 bits for each colour. 
    /// It is suitable for compressing most single channel textures. 
    ///
    /// \section format10 BC5 
    /// A eight bit per pixel DXTC format compressing two channels of data in an identical form to the alpha channel of BC4 texture. It is suitable for compressing most two channel textures and particularly suited for compressing object space normal maps as the third channel of object space normals can be derived from the first two. 
    ///
    /// \section format11 BC6H 
    /// Stores three component data using 16 bits for each colour.
    /// It is a high-dynamic range compression format but has no support for an alpha channel. AMDCompressCLI.exe support formats that uses a sign or unsigned bit in the 16-bit floating point colour channel values, which can be set with the command line codec_option Signed.
    ///
    /// \section format12 BC7 
    /// Is a high-quality compression format of RGB and RGBA data


    /************************************************
    *                DXTn
    *************************************************/
    /// \page formats_DXTn DXTn 
    /// \section format13 DXT1           
    /// An opaque (or 1-bit alpha) DXTC compressed texture format. Four bits per pixel
    ///
    /// \section format14 DXT3           
    /// A DXTC compressed texture format with explicit alpha. Eight bits per pixel
    ///
    /// \section format15 DXT5           
    /// A DXTC compressed texture format with interpolated alpha. Eight bits per pixel
    ///
    /// \section format16 DXT5_xGBR      
    /// A DXT5 with the red component swizzled into the alpha channel. Eight bits per pixel
    ///
    /// \section format17 DXT5_RxBG      
    /// A swizzled DXT5 format with the green component swizzled into the alpha channel. Eight bits per pixel
    ///
    /// \section format18 DXT5_RBxG      
    /// A swizzled DXT5 format with the green component swizzled into the alpha channel & the blue component swizzled into the green channel. Eight bits per pixel
    ///
    /// \section format19 DXT5_xRBG      
    /// A swizzled DXT5 format with the green component swizzled into the alpha channel & the red component swizzled into the green channel. Eight bits per pixel
    ///
    /// \section format20 DXT5_RGxB      
    /// A swizzled DXT5 format with the blue component swizzled into the alpha channel. Eight bits per pixel
    ///
    /// \section format21 DXT5_xGxR      
    /// A two-component swizzled DXT5 format with the red component swizzled into the alpha channel & the green component in the green channel. Eight bits per pixel


    /************************************************
    *                ETCn
    *************************************************/
    /// \page formats_ETCn ETCn
    /// \section format22 ETC_RGB        
    /// Ericsson Texture Compression - a compressed RGB format.
    ///


#endif

