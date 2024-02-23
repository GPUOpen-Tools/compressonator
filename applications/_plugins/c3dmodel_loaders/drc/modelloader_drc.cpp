//=============================================================================
/// Copyright (c) 2017-2024 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
//=============================================================================

#include "modelloader_drc.h"

#include <time.h>

using namespace draco;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_ModelLoader_drc)
SET_PLUGIN_TYPE("3DMODEL_LOADER")
SET_PLUGIN_NAME("DRC")
#else
void* make_Plugin_ModelLoader_drc()
{
    return new Plugin_ModelLoader_drc;
}
#endif

#ifdef BUILD_AS_PLUGIN_DLL
bool   g_bAbortCompression = false;
CMIPS* g_CMIPS             = nullptr;
#else
extern bool   g_bAbortCompression;
extern CMIPS* g_CMIPS;
#endif

//using namespace ML_drc;

Plugin_ModelLoader_drc::Plugin_ModelLoader_drc()
{
    m_mesh      = nullptr;
    m_ModelData = nullptr;
}

Plugin_ModelLoader_drc::~Plugin_ModelLoader_drc()
{
}

int Plugin_ModelLoader_drc::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
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

int Plugin_ModelLoader_drc::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        g_CMIPS              = static_cast<CMIPS*>(Shared);
        g_CMIPS->m_infolevel = 0x01;  // Turn on print Info
        return 0;
    }
    return 1;
}

void* Plugin_ModelLoader_drc::GetModelData()
{
    if (m_loadModel)
        return (void*)m_ModelData;
    else if (m_loadedMesh)
        return (void*)m_mesh;
    else
        return (void*)m_pc.get();
}

int Plugin_ModelLoader_drc::LoadModelData(const char*       pszFilename,
                                          const char*       pszFilename2,
                                          void*             pluginManager,
                                          void*             inDracoOptions,
                                          CMP_Feedback_Proc pFeedbackProc)
{
    if (!pluginManager)
        return -1;

    int           result        = 0;
    unsigned long loadTime      = 0;
    m_loadedMesh                = true;
    m_loadModel                 = false;
    m_mesh                      = nullptr;
    bool              dracofile = (strcmp(pszFilename, "OBJ") != 0);
    draco::DracoTimer timer;
    CMP_DracoOptions* DracoOptions = (CMP_DracoOptions*)(inDracoOptions);

    if (!dracofile)
    {
        if (!DracoOptions->is_point_cloud)
        {
            if (g_CMIPS)
                g_CMIPS->Print("Reading Mesh From File");

            auto in_mesh = draco::ReadMeshFromFile(DracoOptions->input, DracoOptions->use_metadata);
            if (!in_mesh.ok())
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed loading the input mesh");
                return -1;
            }
            m_loadedMesh = true;
            m_mesh       = in_mesh.value().get();
            m_pc         = std::move(in_mesh).value();
        }
        else
        {
            if (g_CMIPS)
                g_CMIPS->Print("Reading Point Cloud From File");
            auto in_point = draco::ReadPointCloudFromFile(DracoOptions->input);
            if (!in_point.ok())
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed loading the input point cloud.");
                return -1;
            }
            m_pc         = std::move(in_point).value();
            m_loadedMesh = false;
        }

        if (DracoOptions->pos_quantization_bits < 0)
        {
            if (g_CMIPS)
                g_CMIPS->Print("Error: Position attribute cannot be skipped");
            return -1;
        }

        // Delete attributes if needed. This needs to happen before we set any
        // quantization settings.
        if (DracoOptions->tex_coords_quantization_bits < 0)
        {
            if (m_pc->NumNamedAttributes(draco::GeometryAttribute::TEX_COORD) > 0)
            {
                DracoOptions->tex_coords_deleted = true;
            }
            while (m_pc->NumNamedAttributes(draco::GeometryAttribute::TEX_COORD) > 0)
            {
                m_pc->DeleteAttribute(m_pc->GetNamedAttributeId(draco::GeometryAttribute::TEX_COORD, 0));
            }
        }

        if (DracoOptions->normals_quantization_bits < 0)
        {
            if (m_pc->NumNamedAttributes(draco::GeometryAttribute::NORMAL) > 0)
            {
                DracoOptions->normals_deleted = true;
            }
            while (m_pc->NumNamedAttributes(draco::GeometryAttribute::NORMAL) > 0)
            {
                m_pc->DeleteAttribute(m_pc->GetNamedAttributeId(draco::GeometryAttribute::NORMAL, 0));
            }
        }

        if (DracoOptions->generic_quantization_bits < 0)
        {
            if (m_pc->NumNamedAttributes(draco::GeometryAttribute::GENERIC) > 0)
            {
                DracoOptions->generic_deleted = true;
            }
            while (m_pc->NumNamedAttributes(draco::GeometryAttribute::GENERIC) > 0)
            {
                m_pc->DeleteAttribute(m_pc->GetNamedAttributeId(draco::GeometryAttribute::GENERIC, 0));
            }
        }

#ifdef DRACO_ATTRIBUTE_DEDUPLICATION_SUPPORTED
        // If any attribute has been deleted, run deduplication of point indices again
        // as some points can be possibly combined.
        if (DracoOptions->tex_coords_deleted || DracoOptions->normals_deleted || DracoOptions->generic_deleted)
        {
            m_pc->DeduplicatePointIds();
        }
#endif
    }
    /*-------------------------------------------------------
    LOAD  a compressed drc file from disk
    ---------------------------------------------------------*/
    else
    {
        if (g_CMIPS)
            g_CMIPS->Print("Loading compressed drc file");

        std::ifstream input_file(pszFilename, std::ios::binary);

        if (!input_file)
        {
            if (g_CMIPS)
                g_CMIPS->Print("Failed opening the input file");
            return -1;
        }

        // Read the file stream into a buffer.
        std::streampos file_size = 0;

        input_file.seekg(0, std::ios::end);
        file_size = input_file.tellg() - file_size;
        input_file.seekg(0, std::ios::beg);

        std::vector<char> data((unsigned int)file_size);
        input_file.read(data.data(), file_size);

        if (data.empty())
        {
            if (g_CMIPS)
                g_CMIPS->Print("Empty input file");
            return -1;
        }

        // Create a draco decoding buffer. Note that no data is copied in this step.
        draco::DecoderBuffer buffer;
        buffer.Init(data.data(), data.size());

        if (g_CMIPS)
            g_CMIPS->Print("Get Encoded Geometry Type");
        auto type_statusor = draco::Decoder::GetEncodedGeometryType(&buffer);
        if (!type_statusor.ok())
        {
            if (g_CMIPS)
                g_CMIPS->Print("Failed loading the input mesh: %s", type_statusor.status().error_msg());
            return -1;
        }

        const draco::EncodedGeometryType geom_type = type_statusor.value();
        if (geom_type == draco::TRIANGULAR_MESH)
        {
            if (g_CMIPS)
                g_CMIPS->Print("Decode Mesh From Buffer");

            timer.Start();
            draco::Decoder decoder;
            auto           statusor = decoder.DecodeMeshFromBuffer(&buffer);
            if (!statusor.ok())
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed loading the input mesh: %s", statusor.status().error_msg());
                return -1;
            }

            std::unique_ptr<draco::Mesh> in_mesh = std::move(statusor).value();
            timer.Stop();

            if (in_mesh)
            {
                m_mesh = in_mesh.get();
                m_pc   = std::move(in_mesh);
            }
        }
        else if (geom_type == draco::POINT_CLOUD)
        {
            if (g_CMIPS)
                g_CMIPS->Print("Decode Point Cloud From Buffer");
            // Failed to decode it as mesh, so let's try to decode it as a point cloud.
            timer.Start();
            draco::Decoder decoder;
            auto           statusor = decoder.DecodePointCloudFromBuffer(&buffer);
            if (!statusor.ok())
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed loading the input mesh: %s", statusor.status().error_msg());
                return -1;
            }
            m_pc = std::move(statusor).value();
            timer.Stop();
        }

        if (m_pc == nullptr)
        {
            printf("Failed to decode the input file.\n");
            return -1;
        }
    }

    if (m_mesh && m_mesh->num_faces() > 0)
    {
        m_loadedMesh = true;

        if (dracofile)
        {
            draco::ObjEncoder obj_encoder;
            std::string       output;
            //std::string       output = DracoOptions->output;
            //if (output == "")
            //{
            output = std::string(pszFilename) + ".obj";
            //}

            if (!obj_encoder.EncodeToFile(*m_mesh, output))
            {
                if (g_CMIPS)
                    g_CMIPS->Print("Failed to store the decoded mesh as obj.\n");
                return -1;
            }

            clock_t        start              = clock();
            PluginManager* localpluginManager = (PluginManager*)(pluginManager);
            if (localpluginManager)
            {
                PluginInterface_3DModel_Loader* plugin_loader_obj =
                    reinterpret_cast<PluginInterface_3DModel_Loader*>(localpluginManager->GetPlugin("3DMODEL_LOADER", "OBJ"));
                if (plugin_loader_obj)
                {
                    int result = plugin_loader_obj->LoadModelData(output.c_str(), NULL, &pluginManager, NULL, pFeedbackProc);
                    if (result != 0)
                    {
                        throw("Error Loading Model Data");
                    }
                    clock_t elapsed = clock() - start;
                    loadTime        = elapsed / (CLOCKS_PER_SEC / 1000);

                    m_ModelData = (CMODEL_DATA*)plugin_loader_obj->GetModelData();
                    if (!m_ModelData)
                    {
                        if (g_CMIPS)
                            g_CMIPS->Print("Failed loading model data.");
                        return -1;
                    }
                    m_loadModel = true;
                }
            }
            if (g_CMIPS)
                g_CMIPS->Print("(%" PRId64 " ms to decode)\n", timer.GetInMs());
        }
    }
    else
    {
        m_loadedMesh = false;
        m_loadModel  = false;
    }

    if (m_ModelData)
        m_ModelData->m_LoadTime = loadTime + (unsigned long)timer.GetInMs();

    return result;
}

int EncodeMeshToFile(const draco::Mesh& mesh, const std::string& file, draco::Encoder* encoder)
{
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
            g_CMIPS->Print("Failed to create the output file");
        return -1;
    }

    out_file.write(buffer.data(), buffer.size());

    return 0;
}

int EncodePointCloudToFile(const draco::PointCloud& pc, const std::string& file, draco::Encoder* encoder)
{
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
            g_CMIPS->Print("Failed to create the output file");
        return -1;
    }

    out_file.write(buffer.data(), buffer.size());
    return 0;
}

int Plugin_ModelLoader_drc::SaveModelData(const char* pdstFilename, void* meshData)
{
    int ret = -1;

    if (!pdstFilename)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Invalid Filename.");
        return -1;
    }

    if (!meshData)
    {
        if (g_CMIPS)
            g_CMIPS->Print("Invalid Mesh Data.");
        return -1;
    }

    std::string sfullfilename = pdstFilename;
    std::string sfilename     = sfullfilename.substr(sfullfilename.find_last_of(".") + 1);

    if (g_CMIPS)
    {
        std::string sfullfilename = pdstFilename;
        std::string sfilename     = sfullfilename.substr(sfullfilename.find_last_of(".") + 1);

        g_CMIPS->Print("Saving Output File %s...", sfilename.c_str());
    }

    bool encodeMesh = true;

    draco::Encoder encoder;

    if (encodeMesh)
    {
        CMP_DracoOptions options;

        // Convert compression level to speed (that 0 = slowest, 10 = fastest).
        const int speed = 10 - options.compression_level;

        // Setup encoder options.
        if (options.pos_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::POSITION, options.pos_quantization_bits);
        }

        if (options.tex_coords_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::TEX_COORD, options.tex_coords_quantization_bits);
        }

        if (options.normals_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::NORMAL, options.normals_quantization_bits);
        }

        if (options.generic_quantization_bits > 0)
        {
            encoder.SetAttributeQuantization(draco::GeometryAttribute::GENERIC, options.generic_quantization_bits);
        }

        encoder.SetSpeedOptions(speed, speed);

        draco::Mesh* mesh = (draco::Mesh*)meshData;
        if (mesh && mesh->num_faces() > 0)
            ret = EncodeMeshToFile(*mesh, sfullfilename, &encoder);
    }
    else
    {
        draco::PointCloud* pc = (draco::PointCloud*)meshData;
        ret                   = EncodePointCloudToFile(*pc, sfullfilename, &encoder);
    }

    return ret;
}
