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
#include "mesh_compressor.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <ctime>

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_Mesh_Compressor)
SET_PLUGIN_TYPE("MESH_COMPRESSOR")
SET_PLUGIN_NAME("DRACO")
#else
void* make_Plugin_Mesh_Compressor()
{
    return new class Plugin_Mesh_Compressor;
}
#endif

namespace cmesh_mesh_compress
{
CMIPS* g_CMIPS = nullptr;

#if (LIB_BUILD_MESHCOMPRESSOR == 1)
int EncodeMeshToFile(const draco::Mesh& mesh, const std::string& file, draco::Encoder* encoder)
{
    if (g_CMIPS)
        g_CMIPS->Print("Encode Mesh To File");

    // Encode the geometry.
    draco::EncoderBuffer buffer;
    const draco::Status  status = encoder->EncodeMeshToBuffer(mesh, &buffer);
    if (!status.ok())
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to encode the mesh. %s", status.error_msg());
        return -1;
    }

    // Save the encoded geometry into a file.
    std::ofstream out_file(file, std::ios::binary);
    if (!out_file)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to create the output file.");
        return -1;
    }

    out_file.write(buffer.data(), buffer.size());

    return 0;
}

int EncodePointCloudToFile(const draco::PointCloud& pc, const std::string& file, draco::Encoder* encoder)
{
    if (g_CMIPS)
        g_CMIPS->Print("Encode Point Cloud To File");

    // Encode the geometry.
    draco::EncoderBuffer buffer;

    const draco::Status status = encoder->EncodePointCloudToBuffer(pc, &buffer);
    if (!status.ok())
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to encode the point cloud. %s", status.error_msg());
        return -1;
    }

    // Save the encoded geometry into a file.
    std::ofstream out_file(file, std::ios::binary);
    if (!out_file)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Failed to create the output file.\n");
        return -1;
    }

    out_file.write(buffer.data(), buffer.size());
    return 0;
}
#endif
}  // namespace cmesh_mesh_compress

using namespace cmesh_mesh_compress;

Plugin_Mesh_Compressor::Plugin_Mesh_Compressor()
{
    m_InitOK = false;
}

Plugin_Mesh_Compressor::~Plugin_Mesh_Compressor()
{
}

int Plugin_Mesh_Compressor::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
#ifdef _WIN32
    pPluginVersion->guid = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor    = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor    = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_Mesh_Compressor::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        g_CMIPS              = static_cast<CMIPS*>(Shared);
        g_CMIPS->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

int Plugin_Mesh_Compressor::Init()
{
    if (m_InitOK)
        return 0;
    m_InitOK = true;
    return 0;
}

int Plugin_Mesh_Compressor::CleanUp()
{
    if (m_InitOK)
    {
        m_InitOK = false;
    }
    return 0;
}

void* Plugin_Mesh_Compressor::ProcessMesh(void* data, void* setting, void* statsOut, CMP_Feedback_Proc pFeedbackProc)
{
    if ((!setting) || (!data))
        return NULL;

#if (LIB_BUILD_MESHCOMPRESSOR == 1)
    draco::Mesh*       mesh;
    draco::PointCloud* pc;
    CMP_DracoOptions*  options = (CMP_DracoOptions*)setting;

    if (g_CMIPS)
        g_CMIPS->Print("Mesh Compressor processing ...");

    // Encode the mesh buffer
    if (!options->m_bDecode)
    {
        draco::Encoder encoder;

        // Convert compression level to speed (that 0 = slowest, 10 = fastest).
        const int speed = 10 - options->compression_level;

        // Setup encoder options.
        if (options->pos_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION, options->pos_quantization_bits);
        }

        if (options->tex_coords_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD, options->tex_coords_quantization_bits);
        }

        if (options->normals_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, options->normals_quantization_bits);
        }

        if (options->generic_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC, options->generic_quantization_bits);
        }

        encoder.SetSpeedOptions(speed, speed);

        if (options->m_bLoadedMesh)
        {
            mesh = reinterpret_cast<draco::Mesh*>(data);
            if (EncodeMeshToFile(*mesh, options->output, &encoder) == 0)
                return data;
            else
                return NULL;
        }
        else
        {
            pc = (draco::PointCloud*)data;
            if (EncodePointCloudToFile(*pc, options->output, &encoder) == 0)
                return data;
            else
                return NULL;
        }
    }
    else
    {
        if (g_CMIPS)
            g_CMIPS->Print("Mesh Decoder processing ...");

        draco::DecoderBuffer* buffer;
        buffer = new draco::DecoderBuffer;

        std::vector<char>* inputdata = (std::vector<char>*)data;

        if (inputdata->empty())
        {
            if (g_CMIPS)
                g_CMIPS->Print("Empty input file");
            return NULL;
        }

        buffer->Init(inputdata->data(), inputdata->size());

        draco::Decoder decoder;

        // Decode the input data into a geometry.
        std::unique_ptr<draco::PointCloud>* pc = new std::unique_ptr<draco::PointCloud>;

        draco::Mesh* mesh = nullptr;

        auto type_statusor = draco::Decoder::GetEncodedGeometryType(buffer);
        if (!type_statusor.ok())
        {
            if (g_CMIPS)
                g_CMIPS->Print("Failed to decode the mesh %s", type_statusor.status().error_msg());
            return NULL;
        }

        const draco::EncodedGeometryType geom_type = type_statusor.value();
        if (geom_type == draco::TRIANGULAR_MESH)
        {
            draco::Decoder decoder;
            auto           statusor = decoder.DecodeMeshFromBuffer(buffer);
            if (!statusor.ok())
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed to decode the mesh %s", statusor.status().error_msg());
                return NULL;
            }

            std::unique_ptr<draco::Mesh> in_mesh = std::move(statusor).value();
            if (in_mesh)
            {
                mesh = in_mesh.get();
                //pc = std::move(in_mesh);
            }

            return (void*)mesh;
        }
        else if (geom_type == draco::POINT_CLOUD)
        {
            // Failed to decode it as mesh, so let's try to decode it as a point cloud.
            draco::Decoder decoder;
            auto           statusor = decoder.DecodePointCloudFromBuffer(buffer);
            if (!statusor.ok())
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed to decode the mesh %s", statusor.status().error_msg());
                return NULL;
            }

            *pc = std::move(statusor).value();

            // Decode the mesh buffer
            return (void*)pc;
        }

        // Decode failed
        return NULL;
    }
#else
    // Decode failed: Option not supported
    return NULL;
#endif
}
