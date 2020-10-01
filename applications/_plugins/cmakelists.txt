# =======================================================================

set(FOLDER_NAME _Plugins_static)

add_subdirectory(ATIFormats)
add_subdirectory(C3DModel_Loaders)
add_subdirectory(C3DModel_viewers)
add_subdirectory(CAnalysis)
add_subdirectory(CCMP_Encode)
add_subdirectory(CCMP_SDK)
add_subdirectory(CGPUDecode)
add_subdirectory(CImage)
add_subdirectory(CmdLine)
add_subdirectory(CMesh)
add_subdirectory(Common)
add_subdirectory(GLTF)
add_subdirectory(Half)
add_subdirectory(Json)
add_subdirectory(ModelViewer)
add_subdirectory(PluginManager)
add_subdirectory(PSNR)
add_subdirectory(TCPluginAPI)
add_subdirectory(UserInterface)

set(FOLDER_NAME _Plugins_dynamic)

add_subdirectory(CMP_GPU)
add_subdirectory(CFilter)

# =======================================================================
