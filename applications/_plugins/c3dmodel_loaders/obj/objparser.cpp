//=====================================================================
// Copyright 2018-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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
#if defined(_MSC_VER) && !defined(_CRT_SECURE_NO_WARNINGS)
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "objparser.h"

#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

static int fixupIndex(int index, size_t size)
{
    return (index >= 0) ? index - 1 : int(size) + index;
}

static int parseInt(const char* s, const char** end)
{
    // skip whitespace
    while (*s == ' ' || *s == '\t')
        s++;

    // read sign bit
    int sign = (*s == '-');
    s += (*s == '-' || *s == '+');

    unsigned int result = 0;

    for (;;)
    {
        if (unsigned(*s - '0') < 10)
            result = result * 10 + (*s - '0');
        else
            break;

        s++;
    }

    // return end-of-string
    *end = s;

    return sign ? -int(result) : int(result);
}

static float parseFloat(const char* s, const char** end)
{
    static const double digits[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    static const double powers[] = {1e0,   1e+1,  1e+2,  1e+3,  1e+4,  1e+5,  1e+6,  1e+7,  1e+8,  1e+9,  1e+10, 1e+11,
                                    1e+12, 1e+13, 1e+14, 1e+15, 1e+16, 1e+17, 1e+18, 1e+19, 1e+20, 1e+21, 1e+22};

    // skip whitespace
    while (*s == ' ' || *s == '\t')
        s++;

    // read sign
    double sign = (*s == '-') ? -1 : 1;
    s += (*s == '-' || *s == '+');

    // read integer part
    double result = 0;
    int    power  = 0;

    while (unsigned(*s - '0') < 10)
    {
        result = result * 10 + digits[*s - '0'];
        s++;
    }

    // read fractional part
    if (*s == '.')
    {
        s++;

        while (unsigned(*s - '0') < 10)
        {
            result = result * 10 + digits[*s - '0'];
            s++;
            power--;
        }
    }

    // read exponent part
    if ((*s | ' ') == 'e')
    {
        s++;

        // read exponent sign
        int expsign = (*s == '-') ? -1 : 1;
        s += (*s == '-' || *s == '+');

        // read exponent
        int exppower = 0;

        while (unsigned(*s - '0') < 10)
        {
            exppower = exppower * 10 + (*s - '0');
            s++;
        }

        // done!
        power += expsign * exppower;
    }

    // return end-of-string
    *end = s;

    // note: this is precise if result < 9e15
    // for longer inputs we lose a bit of precision here
    if (unsigned(-power) < sizeof(powers) / sizeof(powers[0]))
        return float(sign * result / powers[-power]);
    else if (unsigned(power) < sizeof(powers) / sizeof(powers[0]))
        return float(sign * result * powers[power]);
    else
        return float(sign * result * pow(10.0, power));
}

static const char* parseFace(const char* s, int& vi, int& vti, int& vni)
{
    while (*s == ' ' || *s == '\t')
        s++;

    vi = parseInt(s, &s);

    if (*s != '/')
        return s;
    s++;

    // handle vi//vni indices
    if (*s != '/')
        vti = parseInt(s, &s);

    if (*s != '/')
        return s;
    s++;

    vni = parseInt(s, &s);

    return s;
}

void objParseLine(ObjFile& result, const char* line)
{
    if (line[0] == 'v' && line[1] == ' ')
    {
        const char* s = line + 2;

        float x = parseFloat(s, &s);
        float y = parseFloat(s, &s);
        float z = parseFloat(s, &s);

        result.v.push_back(x);
        result.v.push_back(y);
        result.v.push_back(z);
    }
    else if (line[0] == 'v' && line[1] == 't' && line[2] == ' ')
    {
        const char* s = line + 3;

        float u = parseFloat(s, &s);
        float v = parseFloat(s, &s);
        float w = parseFloat(s, &s);

        result.vt.push_back(u);
        result.vt.push_back(v);
        result.vt.push_back(w);
    }
    else if (line[0] == 'v' && line[1] == 'n' && line[2] == ' ')
    {
        const char* s = line + 3;

        float x = parseFloat(s, &s);
        float y = parseFloat(s, &s);
        float z = parseFloat(s, &s);

        result.vn.push_back(x);
        result.vn.push_back(y);
        result.vn.push_back(z);
    }
    else if (line[0] == 'f' && line[1] == ' ')
    {
        const char* s  = line + 2;
        int         fv = 0;

        size_t v  = result.v.size() / 3;
        size_t vt = result.vt.size() / 3;
        size_t vn = result.vn.size() / 3;

        while (*s)
        {
            int vi = 0, vti = 0, vni = 0;
            s = parseFace(s, vi, vti, vni);

            if (vi == 0)
                break;

            result.f.push_back(fixupIndex(vi, v));
            result.f.push_back(fixupIndex(vti, vt));
            result.f.push_back(fixupIndex(vni, vn));

            fv++;
        }

        result.fv.push_back(char(fv));
    }
}

bool objParseFile(ObjFile& result, const char* path)
{
    FILE* file = fopen(path, "rb");
    if (!file)
        return false;

    char   buffer[65536];
    size_t size = 0;

    while (!feof(file))
    {
        size += fread(buffer + size, 1, sizeof(buffer) - size, file);

        size_t line = 0;

        while (line < size)
        {
            // find the end of current line
            void* eol = memchr(buffer + line, '\n', size - line);
            if (!eol)
                break;

            // zero-terminate for objParseLine
            size_t next = static_cast<char*>(eol) - buffer;

            buffer[next] = 0;

            // process next line
            objParseLine(result, buffer + line);

            line = next + 1;
        }

        // move prefix of the last line in the buffer to the beginning of the buffer for next iteration
        assert(line <= size);

        memmove(buffer, buffer + line, size - line);
        size -= line;
    }

    if (size)
    {
        // process last line
        assert(size < sizeof(buffer));
        buffer[size] = 0;

        objParseLine(result, buffer);
    }

    fclose(file);
    return true;
}

bool objValidate(const ObjFile& result)
{
    size_t total_indices = 0;

    for (size_t face = 0; face < result.fv.size(); ++face)
    {
        int fv = result.fv[face];

        if (fv < 3)
            return false;

        total_indices += fv;
    }

    if (total_indices * 3 != result.f.size())
        return false;

    size_t v  = result.v.size() / 3;
    size_t vt = result.vt.size() / 3;
    size_t vn = result.vn.size() / 3;

    for (size_t i = 0; i < result.f.size(); i += 3)
    {
        int vi  = result.f[i + 0];
        int vti = result.f[i + 1];
        int vni = result.f[i + 2];

        if (vi < 0)
            return false;

        if (vi >= 0 && size_t(vi) >= v)
            return false;

        if (vti >= 0 && size_t(vti) >= vt)
            return false;

        if (vni >= 0 && size_t(vni) >= vn)
            return false;
    }

    return true;
}

void objTriangulate(ObjFile& result)
{
    size_t total_indices = 0;

    for (size_t face = 0; face < result.fv.size(); ++face)
    {
        int fv = result.fv[face];

        assert(fv >= 3);
        total_indices += (fv - 2) * 3;
    }

    std::vector<int> f(total_indices * 3);

    size_t read  = 0;
    size_t write = 0;

    for (size_t face = 0; face < result.fv.size(); ++face)
    {
        int fv = result.fv[face];

        for (int i = 0; i + 2 < fv; ++i)
        {
            f[write + 0] = result.f[read + 0];
            f[write + 1] = result.f[read + 1];
            f[write + 2] = result.f[read + 2];

            f[write + 3] = result.f[read + i * 3 + 3];
            f[write + 4] = result.f[read + i * 3 + 4];
            f[write + 5] = result.f[read + i * 3 + 5];

            f[write + 6] = result.f[read + i * 3 + 6];
            f[write + 7] = result.f[read + i * 3 + 7];
            f[write + 8] = result.f[read + i * 3 + 8];

            write += 9;
        }

        read += fv * 3;
    }

    assert(read == result.f.size());
    assert(write == total_indices * 3);

    result.f.swap(f);
    std::vector<char>(total_indices / 3, 3).swap(result.fv);
}
