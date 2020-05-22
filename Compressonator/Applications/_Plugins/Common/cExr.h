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

#ifndef cEXR_HEADER
#define cEXR_HEADER

#include "namespaceAlias.h"

#pragma warning(push)
#pragma warning(disable : 4100)
#pragma warning(disable : 4800)

#include "ImfArray.h"
#include "ImfRgba.h"
#include <ImathBox.h>
#include <ImfArray.h>
#include <ImfRgbaFile.h>
#pragma warning(pop)

#include <string.h>

#include "Common.h"
#include "Compressonator.h"

using namespace IMF;
using namespace IMATH;

#include "CMP_FileIO.h"

#pragma warning(disable : 4201)
typedef unsigned int uint;

class Exr
{
public:
	Exr(){};
	~Exr(){};

	static void fileinfo(const string inf, int &width, int &height);
	static void readRgba(const string inf, Array2D<Rgba> &pix, int &w, int &h);
	static void writeRgba(const string outf, const Array2D<Rgba> &pix, int w, int h);
};

extern void Rgba2Texture(Array2D<Rgba> &pixels, CMP_HALFSHORT *data, int w, int h);
extern void Texture2Rgba(CMP_HALFSHORT *data, Array2D<Rgba> &pixels, int w, int h, CMP_FORMAT isDeCompressed);
extern float half_conv_float(unsigned short in);

#endif