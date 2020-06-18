//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif
#include "cExr.h"

float half_conv_float(unsigned short in)
{
    union fi32 {
        float f;
        uint i;
    } u;

    uint magic_exp = 113;
    static const unsigned int shift_exp = 0x7c00 << 13; // shift exponent full float mask

    uint half_flo = in;

    u.i = (in & 0x7fff) << 13;  // exponent & mantissa bits
    uint exp = shift_exp & u.i; // exponent from half float
    u.i += (127 - 15) << 23;    // exponent adjust

    //exponent special cases
    if (exp == shift_exp)        //Inf case
        u.i += (128 - 16) << 23; // extra exp adjust
    else if (exp == 0)           // Zero/Denormal case
    {
        u.i += 1 << 23;           // extra exp adjust
        u.f -= (magic_exp << 23); // renormalize
    }

    u.i |= (in & 0x8000) << 16; // sign bit
    return u.f;
}

void Exr::fileinfo(const string inf, int &width, int &height)
{
    RgbaInputFile file(inf.c_str());
    Box2i dw = file.dataWindow();

    width = dw.max.x - dw.min.x + 1;
    height = dw.max.y - dw.min.y + 1;
}

void Exr::readRgba(const string inf, Array2D<Rgba> &pix, int &w, int &h)
{
    RgbaInputFile file(inf.c_str());
    Box2i dw = file.dataWindow();
    w = dw.max.x - dw.min.x + 1;
    h = dw.max.y - dw.min.y + 1;
    pix.resizeErase(h, w);
    file.setFrameBuffer(&pix[0][0] - dw.min.x - dw.min.y * w, 1, w);
    file.readPixels(dw.min.y, dw.max.y);
}

void Exr::writeRgba(const string outf, const Array2D<Rgba> &pix, int w, int h)
{
    RgbaOutputFile file(outf.c_str(), w, h, WRITE_RGBA);
    file.setFrameBuffer(&pix[0][0], 1, w);
    file.writePixels(h);
}

void Rgba2Texture(Array2D<Rgba> &pixels, CMP_HALFSHORT *data, int w, int h)
{
    // Save the Half Data format value into CMP_HALFSHORT bit format for processing later
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
            *data = pixels[y][x].r.bits();
            data++;
            *data = pixels[y][x].g.bits();
            data++;
            *data = pixels[y][x].b.bits();
            data++;
            *data = pixels[y][x].a.bits();
            data++;
        }
    }
}

void Texture2Rgba(CMP_HALFSHORT *data, Array2D<Rgba> &pixels, int w, int h, CMP_FORMAT isDeCompressed)
{

    if (isDeCompressed != CMP_FORMAT_Unknown)
    {
        // Save the Half Data format value into a Float for processing later
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                pixels[y][x].r.setBits(*data);
                data++;
                pixels[y][x].g.setBits(*data);
                data++;
                pixels[y][x].b.setBits(*data);
                data++;
                pixels[y][x].a.setBits(*data);
                data++;
            }
        }
    }
    else
    {
        // Save the Half Data format value into a Float for processing later
        for (int y = 0; y < h; ++y)
        {
            for (int x = 0; x < w; ++x)
            {
                pixels[y][x].r = *data;
                data++;
                pixels[y][x].g = *data;
                data++;
                pixels[y][x].b = *data;
                data++;
                pixels[y][x].a = *data;
                data++;
            }
        }
    }
}