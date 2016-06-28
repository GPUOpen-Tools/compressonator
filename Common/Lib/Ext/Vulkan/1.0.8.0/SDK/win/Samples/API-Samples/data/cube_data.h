/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

//--------------------------------------------------------------------------------------
// Mesh and VertexFormat Data
//--------------------------------------------------------------------------------------
struct Vertex {
    float posX, posY, posZ, posW; // Position data
    float r, g, b, a;             // Color
};

struct VertexUV {
    float posX, posY, posZ, posW; // Position data
    float u, v;                   // texture u,v
};

#define XYZ1(_x_, _y_, _z_) (_x_), (_y_), (_z_), 1.f
#define UV(_u_, _v_) (_u_), (_v_)

static const Vertex g_vbData[] = {
    {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},

    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 1.f)},

    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},

    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f)},

    {XYZ1(1, 1, 1), XYZ1(1.f, 1.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(0.f, 1.f, 0.f)},

    {XYZ1(1, -1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 0.f, 0.f)},
};

static const Vertex g_vb_solid_face_colors_Data[] = {
    {XYZ1(-1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, -1, -1), XYZ1(1.f, 0.f, 0.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 0.f)},

    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(-1, 1, 1), XYZ1(0.f, 1.f, 0.f)},
    {XYZ1(1, 1, 1), XYZ1(0.f, 1.f, 0.f)},

    {XYZ1(1, 1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(1, -1, 1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(0.f, 0.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 0.f, 1.f)},

    {XYZ1(-1, 1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, -1, 1), XYZ1(1.f, 1.f, 0.f)},
    {XYZ1(-1, -1, -1), XYZ1(1.f, 1.f, 0.f)},

    {XYZ1(1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(1, 1, -1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, 1), XYZ1(1.f, 0.f, 1.f)},
    {XYZ1(-1, 1, -1), XYZ1(1.f, 0.f, 1.f)},

    {XYZ1(1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, 1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
    {XYZ1(-1, -1, -1), XYZ1(0.f, 1.f, 1.f)},
};

static const VertexUV g_vb_texture_Data[] = {
    {XYZ1(-1, -1, -1), UV(0.f, 0.f)},
    {XYZ1(-1, 1, 1), UV(1.f, 1.f)},
    {XYZ1(-1, -1, 1), UV(1.f, 0.f)},
    {XYZ1(-1, 1, 1), UV(1.f, 1.f)},
    {XYZ1(-1, -1, -1), UV(0.f, 0.f)},
    {XYZ1(-1, 1, -1), UV(0.f, 1.f)},

    {XYZ1(-1, -1, -1), UV(1.f, 0.f)},
    {XYZ1(1, -1, -1), UV(0.f, 0.f)},
    {XYZ1(1, 1, -1), UV(0.f, 1.f)},
    {XYZ1(-1, -1, -1), UV(1.f, 0.f)},
    {XYZ1(1, 1, -1), UV(0.f, 1.f)},
    {XYZ1(-1, 1, -1), UV(1.f, 1.f)},

    {XYZ1(-1, -1, -1), UV(1.f, 1.f)},
    {XYZ1(1, -1, 1), UV(0.f, 0.f)},
    {XYZ1(1, -1, -1), UV(1.f, 0.f)},
    {XYZ1(-1, -1, -1), UV(1.f, 1.f)},
    {XYZ1(-1, -1, 1), UV(0.f, 1.f)},
    {XYZ1(1, -1, 1), UV(0.f, 0.f)},

    {XYZ1(-1, 1, -1), UV(1.f, 1.f)},
    {XYZ1(1, 1, 1), UV(0.f, 0.f)},
    {XYZ1(-1, 1, 1), UV(0.f, 1.f)},
    {XYZ1(-1, 1, -1), UV(1.f, 1.f)},
    {XYZ1(1, 1, -1), UV(1.f, 0.f)},
    {XYZ1(1, 1, 1), UV(0.f, 0.f)},

    {XYZ1(1, 1, -1), UV(1.f, 1.f)},
    {XYZ1(1, -1, 1), UV(0.f, 0.f)},
    {XYZ1(1, 1, 1), UV(0.f, 1.f)},
    {XYZ1(1, -1, 1), UV(0.f, 0.f)},
    {XYZ1(1, 1, -1), UV(1.f, 1.f)},
    {XYZ1(1, -1, -1), UV(1.f, 0.f)},

    {XYZ1(-1, 1, 1), UV(0.f, 1.f)},
    {XYZ1(1, 1, 1), UV(1.f, 1.f)},
    {XYZ1(-1, -1, 1), UV(0.f, 0.f)},
    {XYZ1(-1, -1, 1), UV(0.f, 0.f)},
    {XYZ1(1, 1, 1), UV(1.f, 1.f)},
    {XYZ1(1, -1, 1), UV(1.f, 0.f)},
};
