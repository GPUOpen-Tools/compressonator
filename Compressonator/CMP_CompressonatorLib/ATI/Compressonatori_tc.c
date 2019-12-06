//===============================================================================
// Copyright (c) 2007-2016  Advanced Micro Devices, Inc. All rights reserved.
// Copyright (c) 2004-2006 ATI Technologies Inc.
//===============================================================================
//
//  Project        ATI Handheld Graphics - Texture Compression
//
//  Description    This source code is part of  the ATI handheld texture compression library.  
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
//////////////////////////////////////////////////////////////////////////////

#if defined(__ARM__)
#include <string.h>
#else
#include <memory.h>
#endif
#include <stdio.h>
#include "Compressonator_tc.h"

#ifndef BOOL
typedef int BOOL;
#endif

#ifndef TRUE
#define TRUE (1==1)
#endif

#ifndef FALSE
#define FALSE (1==0)
#endif

#ifndef min
#define min(A,B) ((A) < (B) ? (A) : (B))
#endif

#ifndef max
#define max(A,B) ((A) > (B) ? (A) : (B))
#endif

#ifndef HIWORD
#define HIWORD(N32) ((unsigned short)(((unsigned long)(N32) >> 16) & 0x0000FFFF))
#endif

#ifndef LOWORD
#define LOWORD(N32) ((unsigned short)((N32) & 0x0000FFFF))
#endif

// WARNING:  Do not change the luminance calculation!  It needs to match the hardware exactly.
#define LUMINANCE(R, G, B)  ((19*(R) + 38*(G) + 7*(B))>>6)


typedef long MSE_t;
#define MSE_ZERO    0
#define MSE_HIGHEST 255 * 255 * 16

static long
ErrSquared( const Color888_t *pColor1, const Color888_t *pColor2 )
{
    long errorRed, errorGreen, errorBlue;

    errorRed   = ((long)pColor1->red   - pColor2->red);
    errorGreen = ((long)pColor1->green - pColor2->green);
    errorBlue  = ((long)pColor1->blue  - pColor2->blue);

    return ((errorRed   * errorRed  ) + 
            (errorGreen * errorGreen) + 
            (errorBlue  * errorBlue ));
}



#define DIFF(A,B) ((A)>(B) ? (A)-(B) : (B)-(A))

/*
 * Color888To565() - Converts an 8:8:8 color to a 5:6:5 color.  Notice 
 *                   that the code doesn't just throw away the low bits.
 *                   It chooses the 5:6:5 value that will be closest to 
 *                   the original color when expanded back to 8:8:8.  For 
 *                   example, a niave conversion would convert (7,3,7) to 
 *                   (0,0,0) while a smarter algorithm would convert it to 
 *                   (8,4,8) because it will expand back to a color closer  
 *                   to the original.
 */
static unsigned int
Color888To565( const Color888_t *pColor888 )
{
    unsigned int r, g, b;
    unsigned int r_a, g_a, b_a, r_b, g_b, b_b;

    r = pColor888->red;
    g = pColor888->green;
    b = pColor888->blue;

    r_a = (r & 0x000000F8) | (r & 0x000000E0) >> 5;
    g_a = (g & 0x000000FC) | (g & 0x000000C0) >> 6;
    b_a = (b & 0x000000F8) | (b & 0x000000E0) >> 5;

    r_b = r_a ^ 0x00000008;
    g_b = g_a ^ 0x00000004;
    b_b = b_a ^ 0x00000008;

    if ( DIFF(r, r_b) < DIFF(r, r_a) )      r = r_b;
    if ( DIFF(g, g_b) < DIFF(g, g_a) )      g = g_b;
    if ( DIFF(b, b_b) < DIFF(b, b_a) )      b = b_b;

    return ((r >> 3) << 11)  |  ((g >> 2) << 5)  |  (b >> 3);
}



static void 
Color565To888( unsigned int color565, Color888_t *pColor888Ret )
{
    pColor888Ret->red   = (unsigned char)(((color565 & 0x0000F800) >> 8) | ((color565 & 0x0000E000) >> 13));
    pColor888Ret->green = (unsigned char)(((color565 & 0x000007E0) >> 3) | ((color565 & 0x00000600) >> 9));
    pColor888Ret->blue  = (unsigned char)(((color565 & 0x0000001F) << 3) | ((color565 & 0x0000001C) >> 2));
}


/*
 * Color888To1555() - Converts an 8:8:8 color to a 1:5:5:5 color.  The high bit 
 *                    is used to hold "boolBlackTrick"
 */
static unsigned int
Color888To1555( const Color888_t *pColor888, BOOL boolBlackTrick )
{
    unsigned int r, g, b;
    unsigned int r_a, g_a, b_a, r_b, g_b, b_b;
    unsigned int color1555;

    r = pColor888->red;
    g = pColor888->green;
    b = pColor888->blue;

    r_a = (r & 0x000000F8) | (r & 0x000000E0) >> 5;
    g_a = (g & 0x000000F8) | (g & 0x000000e0) >> 5;
    b_a = (b & 0x000000F8) | (b & 0x000000E0) >> 5;

    r_b = r_a ^ 0x00000008;
    g_b = g_a ^ 0x00000008;
    b_b = b_a ^ 0x00000008;

    if ( DIFF(r, r_b) < DIFF(r, r_a) )      r = r_b;
    if ( DIFF(g, g_b) < DIFF(g, g_a) )      g = g_b;
    if ( DIFF(b, b_b) < DIFF(b, b_a) )      b = b_b;

    color1555 = ((r >> 3) << 10)  |  ((g >> 3) << 5)  |  (b >> 3);
    return (boolBlackTrick ? color1555 | 0x00008000 : color1555);
}



/*
 * Color1555To888() - In addition to the conversion, this function returns a boolean
 *                    TRUE or FALSE indicating if the high bit is set.
 */
static BOOL 
Color1555To888( unsigned int color1555, Color888_t *pColor888Ret )
{
    BOOL boolBlackTrick = color1555 & 0x00008000;

    pColor888Ret->red   = (unsigned char)(((color1555 & 0x00007C00) >> 7) | ((color1555 & 0x00007000) >> 12));
    pColor888Ret->green = (unsigned char)(((color1555 & 0x000003E0) >> 2) | ((color1555 & 0x00000380) >> 7));
    pColor888Ret->blue  = (unsigned char)(((color1555 & 0x0000001F) << 3) | ((color1555 & 0x0000001C) >> 2));

    return boolBlackTrick;
}


/*
 * ColorBrightness() - returns the color brightness percentage (0-100).
 *
 * Return values:
 *
 *  < ~14%  means color is very close to black
 *
 *  < ~11%  means color is very close to pitch black
 *
 */
static int 
ColorBrightness( Color888_t *pColor888 )
{
    unsigned int  maximum;
    int           value;

    maximum = max( max( pColor888->red , pColor888->green ), pColor888->blue );
    value = (maximum * 100) / 255;

    return value;
}



/*
 * ColorHueAndSaturation() - Returns the hue of the color in degrees (0-360).
 *                           If saturation is zero, hue is undefined and returned 
 *                           as -1;
 *
 *                           Also returns the saturation of the color as a percentage 
 *                           between 0 and 100.
 *
 * Return values:
 *
 *  saturation < ~5%         means color is a shade of gray between black & white
 *
 *             < ~11%        means color is a shade of gray between black & white 
 *                           but may have a slight hint of color
 *
 */
static void 
ColorHueAndSaturation( Color888_t   *pColor888,
                       int          *pOut_Hue,          // Return value
                       int          *pOut_Saturation    // Return value
                     )
{
    unsigned int  maximum, minimum, deltaMinMax;
    int           hue, saturation;
    unsigned int  red, green, blue;

    red   = pColor888->red;
    green = pColor888->green;
    blue  = pColor888->blue;

    minimum = min( min( red , green ), blue );
    maximum = max( max( red , green ), blue );
    deltaMinMax = maximum - minimum;

    //--------------------------
    // Calculate the saturation
    //--------------------------
    
    saturation = (maximum != 0) ? (deltaMinMax * 100) / maximum : 0;

    //-------------------
    // Calculate the hue
    //-------------------

    if ( saturation == 0) 
        hue = -1;       // Hue is undefined when saturation is zero
    else
    {
        if ( red == maximum )
            hue = (int)((((float)green - blue) / deltaMinMax) * 60);    // Color is between yellow & magenta
        else if ( green == maximum )
            hue = (int)((2 + ((float)blue - red) / deltaMinMax) * 60);  // Color is between cyan and yellow
        else
            hue = (int)((4 + ((float)red - green) / deltaMinMax) * 60); // Color is between magenta and cyan

        hue = (hue < 0) ? hue + 360 : hue;    // Make sure hue isn't negative
    }

    //--------------------
    // Return the results
    //--------------------
    
    if ( pOut_Hue )             *pOut_Hue = hue;
    if ( pOut_Saturation )      *pOut_Saturation = saturation;
}



static BOOL 
HuesNotNear( int hue1, int hue2 )
{
    int  minimum, maximum;
    BOOL boolNotNear;
    int  threshold = 30;            // allowed difference in degrees

    if ( hue1 == -1 )   hue1 = ( hue2 != -1 ) ? hue2 : 0;   // hue is undefined (-1) when saturation is 0
    if ( hue2 == -1 )   hue2 = hue1;                        // hue is undefined (-1) when saturation is 0

    minimum = min( hue1, hue2 );
    maximum = max( hue1, hue2 );

    if ( maximum - minimum > 180 )
    {
        // Wrap the minimum around by 360 degrees 
        // to measure the shorter direction
        maximum = minimum + 360;
        minimum = maximum;
    }

    boolNotNear = (maximum - minimum > threshold);

    return boolNotNear;
}



static BOOL
NearBlack( Color888_t *pColor888 )
{
    return ( ColorBrightness( pColor888 ) < 15 );
}



static void
DeriveMiddleColors( Color888_t *pColorHigh, 
                    Color888_t *pOut_ColorMedHigh,   // Return value
                    Color888_t *pOut_ColorMedLow,    // Return value
                    Color888_t *pColorLow
                  )
{
    pOut_ColorMedHigh->red   = (unsigned char)(((unsigned int)pColorHigh->red   * 5 + (unsigned int)pColorLow->red   * 3) >> 3);
    pOut_ColorMedHigh->green = (unsigned char)(((unsigned int)pColorHigh->green * 5 + (unsigned int)pColorLow->green * 3) >> 3);
    pOut_ColorMedHigh->blue  = (unsigned char)(((unsigned int)pColorHigh->blue  * 5 + (unsigned int)pColorLow->blue  * 3) >> 3);

    pOut_ColorMedLow->red    = (unsigned char)(((unsigned int)pColorHigh->red   * 3 + (unsigned int)pColorLow->red   * 5) >> 3);
    pOut_ColorMedLow->green  = (unsigned char)(((unsigned int)pColorHigh->green * 3 + (unsigned int)pColorLow->green * 5) >> 3);
    pOut_ColorMedLow->blue   = (unsigned char)(((unsigned int)pColorHigh->blue  * 3 + (unsigned int)pColorLow->blue  * 5) >> 3);
}



/*
 * DeriveMedLowFromHighAndMedHighColors():  
 *        This function is used for the special case where we encode the 
 *        high-luminance & medium-high-luminance colors instead of the 
 *        high_luminance & low-luminance colors.  It derives the 
 *        medium-low luminance color.  The low-luminance color is assumed 
 *        to be black.
 *
 * Note:  This is in it's own function for use by the decoder and the 
 *        encoder when checking errors in index selection.  Having this 
 *        code in its own function makes it easy to make sure both 
 *        places use the same exact algorithm.
 *                                          
 */
static void
DeriveMedLowFromHighAndMedHighColors( Color888_t *pColorHigh, 
                                      Color888_t *pColorMedHigh, 
                                      Color888_t *pOut_ColorMedLow,    // Return value
                                      Color888_t *pOut_ColorLow        // Return value
                                    )
{
    // Don't go negative!!!

    pOut_ColorMedLow->red   = (unsigned char)max((signed int)pColorMedHigh->red   - (pColorHigh->red   >> 2), 0);
    pOut_ColorMedLow->green = (unsigned char)max((signed int)pColorMedHigh->green - (pColorHigh->green >> 2), 0);
    pOut_ColorMedLow->blue  = (unsigned char)max((signed int)pColorMedHigh->blue  - (pColorHigh->blue  >> 2), 0);

    pOut_ColorLow->red   = 0;
    pOut_ColorLow->green = 0;
    pOut_ColorLow->blue  = 0;
}



static void
SetDecoderColors( unsigned int colorLow565or1555,       // IN:  Encoded low color
                  unsigned int colorHigh565,            // IN:  Encoded high color
                  Color888_t   (*pDecoderColors888)[4]  // OUT: L, ML, MH, H colors to use
                )
{
    static int      iColorLow     = 0;
    static int      iColorMedLow  = 1;
    static int      iColorMedHigh = 2;
    static int      iColorHigh    = 3;

    //----------------------------------------------------------
    // Set colors based on encoder specified by ATITC SIGNATURE
    //----------------------------------------------------------
    
    BOOL boolBlackTrick = Color1555To888( colorLow565or1555, &(*pDecoderColors888)[ iColorLow ] );
    Color565To888( colorHigh565, &(*pDecoderColors888)[ iColorHigh ] );

    if ( boolBlackTrick )
    {
        (*pDecoderColors888)[ iColorMedHigh ] = (*pDecoderColors888)[ iColorLow ];

        DeriveMedLowFromHighAndMedHighColors( &(*pDecoderColors888)[ iColorHigh ], 
                                              &(*pDecoderColors888)[ iColorMedHigh ], 
                                              &(*pDecoderColors888)[ iColorMedLow ],
                                              &(*pDecoderColors888)[ iColorLow ] );
    }
    else
    {
        DeriveMiddleColors( &(*pDecoderColors888)[ iColorHigh ], 
                            &(*pDecoderColors888)[ iColorMedHigh ], 
                            &(*pDecoderColors888)[ iColorMedLow ],
                            &(*pDecoderColors888)[ iColorLow ] );
    }

}



/*
 * Reconstruct a 4x4 block given two colors and a string of bits that 
 * indicate which of four colors to use to reconstruct each pixel.  Two 
 * additional colors will be derived from the two passed in.
 */
void
atiDecodeRGBBlockATITC( 
                Color888_t      (*pPixelsOut)[4][4],
                unsigned int    bitIndices,
                unsigned int    colorLow565or1555,
                unsigned int    colorHigh565
              )
{
    int             row, col;
    Color888_t      aColor888[ 4 ];
    static int      iColorLow     = 0;
    static int      iColorMedLow  = 1;
    static int      iColorMedHigh = 2;
    static int      iColorHigh    = 3;

    //--------------------
    //  Setup the colors  
    //--------------------

    SetDecoderColors( colorLow565or1555, colorHigh565, &aColor888 );


    //--------------------
    //  Decode the block
    //--------------------

    for ( row = 0;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            (*pPixelsOut)[row][col].red   = aColor888[ bitIndices & 0x00000003 ].red;
            (*pPixelsOut)[row][col].green = aColor888[ bitIndices & 0x00000003 ].green;
            (*pPixelsOut)[row][col].blue  = aColor888[ bitIndices & 0x00000003 ].blue;

            bitIndices >>= 2;
        }
    }
}



/*
 * CalcRegionColors() - This function calculates the representative color for each of four 
 *                      luminance regions.  Each pixel has 2 bits out of the bit indices to 
 *                      indicate which luminance region it belongs to.  The indices are read 
 *                      from LSB to MSB.
 */
static void 
CalcRegionColors( 
                       unsigned int bitIndices,           // IN:  Luma region indices for 4x4 block
                       Color888_t   (*pPixels)[4][4],     // IN:  Color values for 4x4 block
                       Color888_t   *pColorLow888,        // OUT: Ave. color for Low region
                       Color888_t   *pColorMedLow888,     // OUT: Ave. color for MedLow region
                       Color888_t   *pColorMedHigh888,    // OUT: Ave. color for MedHigh region
                       Color888_t   *pColorHigh888,       // OUT: Ave. color for High region
                       int          *pcLow,               // OUT: Num colors in Low region
                       int          *pcMedLow,            // OUT: Num colors in MedLow region
                       int          *pcMedHigh,           // OUT: Num colors in MedHigh region
                       int          *pcHigh               // OUT: Num colors in High region
                )
{
    int             row, col;
    int             rAveHigh,    gAveHigh,    bAveHigh,    cHigh;
    int             rAveMedHigh, gAveMedHigh, bAveMedHigh, cMedHigh;
    int             rAveMedLow,  gAveMedLow,  bAveMedLow,  cMedLow;
    int             rAveLow,     gAveLow,     bAveLow,     cLow;

    //----------------------
    // Zero counts & values
    //----------------------

    rAveHigh    = gAveHigh    = bAveHigh    = cHigh    = 0;
    rAveMedHigh = gAveMedHigh = bAveMedHigh = cMedHigh = 0;
    rAveMedLow  = gAveMedLow  = bAveMedLow  = cMedLow  = 0;
    rAveLow     = gAveLow     = bAveLow     = cLow     = 0;

    //---------------------------------------------------------
    // Sum up the colors of the pixels assigned to each region
    //---------------------------------------------------------

    for ( row = 0;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            switch ( bitIndices & 0x00000003 )
            {
                case 0: rAveLow     += (*pPixels)[row][col].red;
                        gAveLow     += (*pPixels)[row][col].green;
                        bAveLow     += (*pPixels)[row][col].blue;
                        cLow++;
                        break;

                case 1: rAveMedLow  += (*pPixels)[row][col].red;
                        gAveMedLow  += (*pPixels)[row][col].green;
                        bAveMedLow  += (*pPixels)[row][col].blue;
                        cMedLow++;
                        break;

                case 2: rAveMedHigh += (*pPixels)[row][col].red;
                        gAveMedHigh += (*pPixels)[row][col].green;
                        bAveMedHigh += (*pPixels)[row][col].blue;
                        cMedHigh++;
                        break;

                case 3: rAveHigh    += (*pPixels)[row][col].red;
                        gAveHigh    += (*pPixels)[row][col].green;
                        bAveHigh    += (*pPixels)[row][col].blue;
                        cHigh++;
                        break;
            }
            bitIndices >>= 2;
        }
    }

    //--------------------------------------------------------
    // Compute the center of gravity of each luminance region
    //--------------------------------------------------------

    if ( cLow )     { rAveLow     /= cLow;        gAveLow     /= cLow;        bAveLow     /= cLow;     }
    if ( cMedLow )  { rAveMedLow  /= cMedLow;     gAveMedLow  /= cMedLow;     bAveMedLow  /= cMedLow;  }
    if ( cMedHigh ) { rAveMedHigh /= cMedHigh;    gAveMedHigh /= cMedHigh;    bAveMedHigh /= cMedHigh; }
    if ( cHigh )    { rAveHigh    /= cHigh;       gAveHigh    /= cHigh;       bAveHigh    /= cHigh;    }

    //-------------------------------------------------
    // Return the representative color for each region
    //-------------------------------------------------

    pColorLow888->red       = (unsigned char)rAveLow;
    pColorLow888->green     = (unsigned char)gAveLow;
    pColorLow888->blue      = (unsigned char)bAveLow;

    pColorMedLow888->red    = (unsigned char)rAveMedLow;
    pColorMedLow888->green  = (unsigned char)gAveMedLow;
    pColorMedLow888->blue   = (unsigned char)bAveMedLow;

    pColorMedHigh888->red   = (unsigned char)rAveMedHigh;
    pColorMedHigh888->green = (unsigned char)gAveMedHigh;
    pColorMedHigh888->blue  = (unsigned char)bAveMedHigh;

    pColorHigh888->red      = (unsigned char)rAveHigh;
    pColorHigh888->green    = (unsigned char)gAveHigh;
    pColorHigh888->blue     = (unsigned char)bAveHigh;

    //-------------------------------------------
    // Return the count of pixels in each region
    //-------------------------------------------

    *pcLow     = cLow;
    *pcMedLow  = cMedLow;
    *pcMedHigh = cMedHigh;
    *pcHigh    = cHigh;
}



/* 
 * CalcBlockMSE() - This function decodes a block using the specified fields 
 *                  "bitIndices", "colorLow565", and "colorHigh565".  The 
 *                  result is compared against the original block passed in 
 *                  pPixels[][], and the mean squared error is returned.
 *
 */

static MSE_t
CalcBlockMSE( 
              unsigned int bitIndices,          // IN:  Luma region indices for 4x4 block
              unsigned int colorLow565or1555,   // IN:  Low luminance color encoded
              unsigned int colorHigh565,        // IN:  High luminance color encoded
              Color888_t   (*pPixelsOrig)[4][4] // IN:  Color values for original 4x4 block
            )
{
    Color888_t    pixelsDecoded[4][4];
    MSE_t         MSE = MSE_ZERO;
    int           row, col;

    //-------------------------------------------------------------------
    // Decode the pixels using bitIndices, colorLow565, and colorHigh565
    //-------------------------------------------------------------------

    atiDecodeRGBBlockATITC( 
                    &pixelsDecoded,             // place for decoded pixels
                    bitIndices,
                    colorLow565or1555,
                    colorHigh565 );
    
    //--------------------------------------------------------------
    // Compare the reconstructed pixels against the original pixels
    //--------------------------------------------------------------
    
    for ( row = 0;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            MSE += ErrSquared( &(*pPixelsOrig)[row][col], &pixelsDecoded[row][col] );
        }
    }


    return MSE;
}



/*
 * Given a 4x4 block of luma values and 3 luma region 
 * boundaries, set an array of 2-bit indicies to specify 
 * which region each value belongs in.
 */

unsigned int
CollatePixelsIntoRegions( 
      unsigned int  (*pY)[4][4],
      unsigned int  medLowLumaStart,
      unsigned int  medHighLumaStart,
      unsigned int  highLumaStart
    )
{
    int           row, col;
    unsigned int  luma;
    unsigned int  bitMask = 0x00000003;  // Array of 16 2-bit region luma indices
    unsigned int  bitIndices = 0;

    for ( row = 0;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            luma = (*pY)[row][col];

            if ( luma >= highLumaStart )
            {
                // High luminance pixel
                bitIndices |= (bitMask & 0xFFFFFFFF);           // set bits to 11b
            }
            else if ( luma > medHighLumaStart )
            {
                // Medium-high luminance pixel
                bitIndices |= (bitMask & 0xAAAAAAAA);           // set bits to 10b
            }
            else if ( luma > medLowLumaStart )
            {
                // Medium-low luminance pixel
                bitIndices |= (bitMask & 0x55555555);           // set bits to 01b
            }
            else
            {
                // Low luminance pixel
                                                                // set bits to 00b
            }

            bitMask <<= 2;
        }
    }

    return bitIndices;
}


/*
 * ApplyBlackTrickIfHelpful() - indicates if black trick should be used.  Also moves 
 *                              med-low region pixels & color to med-high region if 
 *                              it's empty.
 */
static BOOL
ApplyBlackTrickIfHelpful( 
                           unsigned int *pBitIndices,         // IN/OUT: Luma region indices for 4x4 block
                           Color888_t   *pColorLow888,        // IN/OUT: Ave. color for Low region
                           Color888_t   *pColorMedLow888,     // IN/OUT: Ave. color for MedLow region
                           Color888_t   *pColorMedHigh888,    // IN/OUT: Ave. color for MedHigh region
                           Color888_t   *pColorHigh888,       // IN/OUT: Ave. color for High region
                           int          *pcLow,               // IN/OUT: Num colors in Low region
                           int          *pcMedLow,            // IN/OUT: Num colors in MedLow region
                           int          *pcMedHigh            // IN/OUT: Num colors in MedHigh region
                            )
{
    BOOL            boolUseBlackTrick = FALSE;
    int             hueHigh, hueMedHigh, hueMedLow;
    int             saturationHigh, saturationMedHigh, saturationMedLow;
    unsigned int    bitMask = 0x00000003;       // Mask to indicate luminance (low, med, high) for pixel


    //-----------------------------------------------------------
    // Determine if the black trick can help with this 4x4 block
    //-----------------------------------------------------------

    if ( *pcLow && NearBlack( pColorLow888 ) && ColorBrightness( pColorHigh888 ) > 55 )
    {
        ColorHueAndSaturation( pColorHigh888, &hueHigh, &saturationHigh );

        // Set defaults in case MedHigh or MedLow colors don't exist
        hueMedHigh = hueMedLow = hueHigh;
        saturationMedHigh = saturationMedLow = saturationHigh;

        if ( *pcMedHigh > 0 )
            ColorHueAndSaturation( pColorMedHigh888, &hueMedHigh, &saturationMedHigh );

        if ( *pcMedLow > 0 )
            ColorHueAndSaturation( pColorMedLow888, &hueMedLow, &saturationMedLow );

        if ( HuesNotNear( hueHigh, hueMedHigh )  ||  
             HuesNotNear( hueHigh, hueMedLow  )  || 
             ( *pcMedHigh && ((saturationMedHigh > saturationHigh * 2) || (saturationMedHigh > 70)) ) ||
             ( *pcMedLow  && ((saturationMedLow  > saturationHigh * 2) || (saturationMedLow  > 70)) )
           )
        {
            //-------------------------------
            // We should use the black trick
            //-------------------------------

            boolUseBlackTrick = TRUE;

            //-------------------------------------------------------------------
            // If the medium-high luminance region is empty, move everything 
            // from the medium-low luminance region into the medium-high region.
            // This improves image quality because the medium-high color is 
            // encoded while the medium-low color is derived.
            //-------------------------------------------------------------------

            if ( *pcMedHigh == 0 )
            {
                // Since there are no texels in the medium-high luminance region, use this 
                // slot for the medium-low luminance color.  That way we can get the exact 
                // color we want.

                *pColorMedHigh888 = *pColorMedLow888;
                *pcMedHigh        = *pcMedLow;

                pColorMedLow888->red   = 0;
                pColorMedLow888->green = 0;
                pColorMedLow888->blue  = 0;
                *pcMedLow              = 0;

                // Change all '01b' indices to '10b'
                while ( bitMask )
                {
                    if ( (*pBitIndices & bitMask) == (0x55555555 & bitMask) )
                    {
                        *pBitIndices &= (~bitMask);               // Clear the '01b' index
                        *pBitIndices |= (bitMask & 0xAAAAAAAA);   // Set the '10b' index
                    }
                    bitMask <<= 2;
                }
            }
        }
    }

    return boolUseBlackTrick;
}



static BOOL
CheckPixelRegionAssignment( 
               Color888_t   (*pPixels)[4][4],     // IN:     Original pixels
               unsigned int *pBitIndices,         // IN/OUT: Luma region indices for 4x4 block
               Color888_t   *pColorLow888,        // IN:     low-luminance color
               Color888_t   *pColorHigh888,       // IN:     high-luminance color
               BOOL         boolBlackTrick        // IN:   
              )
{
    int             row, col;
    Color888_t      aColor888[ 4 ];
    Color888_t      colorCurrent;
    static int      iColorLow     = 0;
    static int      iColorMedLow  = 1;
    static int      iColorMedHigh = 2;
    static int      iColorHigh    = 3;
    unsigned int    bitMask = 0x00000003;       // Mask to indicate luminance (low, med, high) for pixel
    unsigned int    index_low;
    MSE_t           err_squared, err_squared_low;
    BOOL            boolSomePixelsMoved = FALSE;
    unsigned int    bitIndices = *pBitIndices;


    //--------------------
    //  Setup the colors  
    //--------------------

    SetDecoderColors( Color888To1555( pColorLow888, boolBlackTrick ), 
                      Color888To565( pColorHigh888 ), 
                      &aColor888 );


    //------------------------------------------------------------------
    // Compare each pixel against the color associated with each region 
    // to make sure that it's placed in the region that results in the 
    // color that's closest to the original.
    //------------------------------------------------------------------

    for ( row = 0;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            colorCurrent.red    = (*pPixels)[row][col].red;
            colorCurrent.green  = (*pPixels)[row][col].green;
            colorCurrent.blue   = (*pPixels)[row][col].blue;

            // --- Calc error w.r.t. low color ---

            err_squared_low = ErrSquared( &colorCurrent, &aColor888[ iColorLow ] );
            index_low = 0x00000000;         // default to low color

            // --- Calc error w.r.t. med-low color ---

            err_squared = ErrSquared( &colorCurrent, &aColor888[ iColorMedLow ] );
            if ( err_squared < err_squared_low )
            {
                err_squared_low = err_squared;
                index_low = 0x55555555;
            }

            // --- Calc error w.r.t. med-low color ---

            err_squared = ErrSquared( &colorCurrent, &aColor888[ iColorMedHigh ] );
            if ( err_squared < err_squared_low )
            {
                err_squared_low = err_squared;
                index_low = 0xAAAAAAAA;
            }

            // --- Calc error w.r.t. high color ---

            err_squared = ErrSquared( &colorCurrent, &aColor888[ iColorHigh ] );
            if ( err_squared < err_squared_low )
            {
                err_squared_low = err_squared;
                index_low = 0xFFFFFFFF;
            }

        
            //------------------------------------------------------------------------------
            // Reset index for pixel if the one we set based on luminance isn't the nearest
            //------------------------------------------------------------------------------

            if ( (bitIndices & bitMask) != (index_low & bitMask) )
            {
                bitIndices &= (~bitMask);
                bitIndices |= (bitMask & index_low);
                boolSomePixelsMoved = TRUE;
            }

            bitMask <<= 2;
        }
    }

    
    //----------------------------------------------------------------------
    // Make sure we still have pixels in the high-luminance & low-luminance 
    // regions because the two colors we encode come from those two regions.
    // Note:  if black trick is in use, we don't require any low-luminance 
    //        region pixels.
    //----------------------------------------------------------------------

    if ( boolSomePixelsMoved )
    {
        int          i, index;
        int          count[4] = {0, 0, 0, 0};
        unsigned int bitIndicesTemp = bitIndices;

        // Count how many pixels are in each region

        for ( i = 0;  i < 16;  i++ )
        {
            index = bitIndicesTemp & 0x00000003;
            count[index]++;
            bitIndicesTemp >>= 2;
        }

        if ( (count[iColorHigh] && count[iColorLow]) || 
             (count[iColorHigh] == 16) ||
             (count[iColorHigh] && boolBlackTrick) )
            *pBitIndices = bitIndices;      // We can apply the changes
        else
            boolSomePixelsMoved = FALSE;
    }


    return boolSomePixelsMoved;
}



/*
 *  (1)
 *  Given a reference to a 4x4 block of RGB pixels, create a 4x4 matrix of 
 *  2-bit values that indicate whether the pixel in the corresponding position 
 *  is in the lower region, medium-low region, medium-high region, or upper region 
 *  of luminance for the block.
 *
 *  Return the 4x4 matrix of bits as an unsigned integer scalar.
 *
 *  (2) 
 *  Calculate the average color for each of the four luminance levels and determine 
 *  two reference colors to be returned to the caller along with a set of two-bit 
 *  indices to these colors.  Each pixel in the block has two of these bits to index 
 *  a color.  Indices 0 & 3 refer to one of the colors returned, while indices 
 *  1 & 2 refer to one of two colors derived from the two returned.
 */
unsigned int
atiEncodeRGBBlockATITC( 
                Color888_t      (*pPixels)[4][4],
                unsigned int    *pColorLow565or1555Ret, // Return value
                unsigned int    *pColorHigh565Ret       // Return value
              )
{
    int             row, col;
    int             cLow, cMedLow, cMedHigh, cHigh = 0;                         // Pixels in each region
    Color888_t      colorLow888, colorMedLow888, colorMedHigh888, colorHigh888; // Color for each region
    unsigned int    medLowLumaStart, medHighLumaStart, highLumaStart;
    unsigned int    medLowLumaStartPrev, highLumaStartPrev;
    unsigned int    luma;
    unsigned int    Y[4][4];                    // 4x4 matrix of luminance for the 4x4 block
    unsigned int    Y_Sorted[17];
    unsigned int    numYSorted;
    unsigned int    i, j;
    unsigned int    bitIndices = 0;
    BOOL            boolBlackTrick;
    unsigned int    colorLow565or1555, colorHigh565;
    MSE_t           blockMSE;
    MSE_t           best_BlockMSE = MSE_HIGHEST;// Begin with highest possible value
    unsigned int    best_BitIndices = 0;
    Color888_t      best_ColorLow888 = {0,0,0}, best_ColorMedLow888 = {0,0,0}, best_ColorMedHigh888 = {0,0,0}, best_ColorHigh888 = {0,0,0};
    BOOL            best_boolBlackTrick = FALSE;


    //-------------------------------------------------------------------------
    // Create a 4x4 block of luma values, and keep a sorted list of the values
    //-------------------------------------------------------------------------

    Y_Sorted[0] = 256;  // Greater than maximum possible value as sentinel
    numYSorted = 0;     // Num values already inserted

    for ( row = 0;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            luma = LUMINANCE( (*pPixels)[row][col].red, 
                              (*pPixels)[row][col].green, 
                              (*pPixels)[row][col].blue );

            // Insert into 4x4 block
            Y[row][col] = luma;

            // Insert into a sorted list
            i = 0;
            while ( luma >= Y_Sorted[i] ) i++;      // Find insertion position

            for ( j = numYSorted+1;  j > i;  j-- )  // Shift elements back to make room
                Y_Sorted[j] = Y_Sorted[j-1];

            Y_Sorted[i] = luma;
            numYSorted++;
        }
    }

    //------------------------------------------
    // Test various luminance region boundaries
    //------------------------------------------

    highLumaStartPrev = 256;                                            // Greater than maximum possible
    for ( i = 15;  i > 0;  i-- )
    {
        highLumaStart = Y_Sorted[i];
        if ( highLumaStart == highLumaStartPrev ) continue;             // Skip duplicate values

        medLowLumaStartPrev = 256;                                      // Greater than maximum possible
        for ( j = 0;  j < i;  j++ )
        {
            medLowLumaStart = Y_Sorted[j];
            if ( medLowLumaStart == medLowLumaStartPrev ) continue;     // Skip duplicate values

            medHighLumaStart = Y_Sorted[(i + j) >> 1];

            //---------------------------------------------------------
            // Create an array of 16 2-bit indices that specify which 
            // luminance region each pixels belongs to
            //---------------------------------------------------------

            bitIndices = CollatePixelsIntoRegions( &Y,               // 4x4 block of luma values
                                                   medLowLumaStart,
                                                   medHighLumaStart,
                                                   highLumaStart );

            //----------------------------------------------------
            // Calculate the representative color for each region
            //----------------------------------------------------

            CalcRegionColors( bitIndices, 
                              pPixels, 
                              &colorLow888, 
                              &colorMedLow888, 
                              &colorMedHigh888, 
                              &colorHigh888,
                              &cLow,
                              &cMedLow,
                              &cMedHigh,
                              &cHigh );

            //--------------------------------------------
            // Determine if we should use the black trick
            //--------------------------------------------

            boolBlackTrick = ApplyBlackTrickIfHelpful( &bitIndices,
                                                       &colorLow888,
                                                       &colorMedLow888,
                                                       &colorMedHigh888,
                                                       &colorHigh888,
                                                       &cLow,
                                                       &cMedLow,
                                                       &cMedHigh );

            //---------------------------------------------------------------
            // Calculate the MSE based on the current luma region boundaries
            //---------------------------------------------------------------

            if ( boolBlackTrick )
            {
                colorLow565or1555  = Color888To1555( &colorMedHigh888, boolBlackTrick );
                colorHigh565       = Color888To565( &colorHigh888 );
            }
            else
            {
                colorLow565or1555  = Color888To1555( &colorLow888, boolBlackTrick );
                colorHigh565       = Color888To565( &colorHigh888 );
            }

            blockMSE = CalcBlockMSE( bitIndices,        // Bit indices to specify luma region per pixel
                                     colorLow565or1555, // Low luma region color
                                     colorHigh565,      // High luma region color
                                     pPixels );           // Original pixel values

            //---------------------------------------------------
            // Keep track of the lowest block mean squared error 
            // and the associated encoding information
            //---------------------------------------------------

            if ( blockMSE < best_BlockMSE )
            {
                best_BlockMSE = blockMSE;

                best_BitIndices      = bitIndices;
                best_ColorLow888     = colorLow888;
                best_ColorMedLow888  = colorMedLow888;
                best_ColorMedHigh888 = colorMedHigh888;
                best_ColorHigh888    = colorHigh888;
                best_boolBlackTrick  = boolBlackTrick;
            }
        }
    }


    //---------------------------------------------------
    // Use the values that produced the lowest block MSE
    //---------------------------------------------------

    bitIndices      = best_BitIndices;
    colorLow888     = best_ColorLow888;
    colorMedLow888  = best_ColorMedLow888;
    colorMedHigh888 = best_ColorMedHigh888;
    colorHigh888    = best_ColorHigh888;
    boolBlackTrick  = best_boolBlackTrick;


    //---------------------------------------------------------------
    // If we're using the black trick, change which colors we encode 
    //---------------------------------------------------------------

    if ( boolBlackTrick )
    {
            colorLow888  = colorMedHigh888;
    }


    //--------------------------------------------------
    // Make sure pixels are assigned to the best region
    //--------------------------------------------------

    if ( CheckPixelRegionAssignment( pPixels, &bitIndices, &colorLow888, &colorHigh888, 
                                     boolBlackTrick ) )
    {
        //------------------------------------------------------
        // Recalculate the representative color for each region
        //------------------------------------------------------

#if 1
        CalcRegionColors( bitIndices, 
                          pPixels, 
                          &colorLow888, 
                          &colorMedLow888, 
                          &colorMedHigh888, 
                          &colorHigh888,
                          &cLow,
                          &cMedLow,
                          &cMedHigh,
                          &cHigh );

        //--------------------------------------------
        // Determine if we should use the black trick
        //--------------------------------------------

        boolBlackTrick = ApplyBlackTrickIfHelpful( &bitIndices,
                                                   &colorLow888,
                                                   &colorMedLow888,
                                                   &colorMedHigh888,
                                                   &colorHigh888,
                                                   &cLow,
                                                   &cMedLow,
                                                   &cMedHigh );

        // WARNING: We have problems if boolBlackTrick == TRUE here but FALSE after the 
        //          call to ApplyBlackTrickIfHelpful().  That's because the MSE up to 
        //          this point was based on the color derivations based on the black trick.

        if ( boolBlackTrick != best_boolBlackTrick )
        {
            // Roll back the recalculation of region colors
            bitIndices      = best_BitIndices;
            colorLow888     = best_ColorLow888;
            colorMedLow888  = best_ColorMedLow888;
            colorMedHigh888 = best_ColorMedHigh888;
            colorHigh888    = best_ColorHigh888;
            boolBlackTrick  = best_boolBlackTrick;
        }

        //---------------------------------------------------------------
        // If we're using the black trick, change which colors we encode 
        //---------------------------------------------------------------

        if ( boolBlackTrick )
        {
                colorLow888  = colorMedHigh888;
        }
#endif
    }


    //---------------------------------
    // Return the two colors to encode
    //---------------------------------

    *pColorLow565or1555Ret  = Color888To1555( &colorLow888, boolBlackTrick );
    *pColorHigh565Ret       = Color888To565( &colorHigh888 );


    //--------------------------------------------------------
    // Return a string of bits that indicate which of the two 
    // (or derivations of the two) encoded colors to use for 
    // each pixel.  
    //--------------------------------------------------------

    //assert( cHigh != 0 );
    return bitIndices;
}



/*
 * atiEncodeAlphaBlockATITCA4() - Source has already been expanded to 8-bit format
 */
void
atiEncodeAlphaBlockATITCA4( unsigned char (*pSrcAlpha)[4][4], unsigned int (*pEncodedAlpha)[2] )
{
    int row, col;
    
    (*pEncodedAlpha)[0] = 0;
    (*pEncodedAlpha)[1] = 0;

    //--------------------------------------------------
    // Fill in 1st 32-bit DWORD with 4-bit alpha values
    //--------------------------------------------------
    for ( row = 0;  row < 2;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            (*pEncodedAlpha)[0] >>= 4;
            (*pEncodedAlpha)[0] |= (unsigned int)((*pSrcAlpha)[row][col] & 0xF0) << 24; // Take just the 4 high bits
        }
    }

    //--------------------------------------------------
    // Fill in 2nd 32-bit DWORD with 4-bit alpha values
    //--------------------------------------------------
    for ( row = 2;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            (*pEncodedAlpha)[1] >>= 4;
            (*pEncodedAlpha)[1] |= (unsigned int)((*pSrcAlpha)[row][col] & 0xF0) << 24; // Take just the 4 high bits
        }
    }

}



/*
 * atiDecodeAlphaBlockATITCA4() - Input is 4 bits of alpha per pixel; output is 8 bits of alpha per pixel
 */
void
atiDecodeAlphaBlockATITCA4( unsigned char (*pAlphaOut)[4][4], 
                  unsigned char *pEncodedData 
                )
{
    unsigned int    encodedAlpha = *((unsigned int*)pEncodedData);
    unsigned int    alpha;
    int             row, col;

    //-----------------------------------------------
    //  Decode the Alpha portion of the 1st two rows
    //-----------------------------------------------

    for ( row = 0;  row < 2;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            alpha = encodedAlpha & 0x0000000F;
            alpha = alpha | (alpha << 4);
            
            (*pAlphaOut)[row][col] = (unsigned char)alpha;

            encodedAlpha >>= 4;
        }
    }

    //------------------------------------------------
    //  Decode the Alpha portion of the last two rows
    //------------------------------------------------

    encodedAlpha = *((unsigned int*)pEncodedData + 1);

    for ( row = 2;  row < 4;  row++ )
    {
        for ( col = 0;  col < 4;  col++ )
        {
            alpha = encodedAlpha & 0x0000000F;
            alpha = alpha | (alpha << 4);
  
            (*pAlphaOut)[row][col] = (unsigned char)alpha;

            encodedAlpha >>= 4;
        }
    }
}

