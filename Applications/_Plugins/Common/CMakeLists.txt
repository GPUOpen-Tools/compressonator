set(PLUGIN_COMMON_SRC
  ATIFormats.cpp
  cExr.cpp
  cmdline.cpp
  CMP_FileIO.cpp
  #Misc.cpp # Windows API time functions and types
  ModelData.cpp
  PluginManager.cpp
  #PSNR.cpp # Old OpenVR API, unused
  query_timer.cpp
  SSIM.cpp
  TC_PluginInternal.cpp
  #Texture.cpp # Windows API
  TextureIO.cpp
  UserInterface.cpp
  UtilFuncs.cpp
)

set(PLUGIN_COMMON_H
  ATIFormats.h
  cExr.h
  cmdline.h
  CMP_FileIO.h
  Common_KernelDef.h
  crc32.h
  HPC_Compress.h
  Misc.h
  ModelData.h
  namespaceAlias.h
  PluginBase.h
  PluginInterface.h
  PluginManager.h
  #PSNR.h
  query_timer.h
  SSIM.h
  stb_image.h
  TC_PluginAPI.h
  TC_PluginInternal.h
  TestReport.h
  Texture.h
  TextureIO.h
  UserInterface.h
  UtilFuncs.h
  vectypes.h
)

add_library(PluginCommon STATIC ${PLUGIN_COMMON_SRC} ${PLUGIN_COMMON_H})

target_include_directories(PluginCommon PRIVATE
  .
  ${OpenCV_INCLUDE_DIRS}
  ${OpenEXR_INCLUDE_DIRS}
  ${draco_INCLUDE_DIRS}
  ../../../CMP_Framework
  ../../../CMP_Framework/Common/half
  ../../../CMP_CompressonatorLib
  gltf/
  ../../../CMP_Core/shaders
  ../../../CMP_Core/source
  ../CMesh/mesh_optimizer
  ../../_Libs/GPU_Decode
  ../../_Libs/CMP_Math
  ../../_Libs/CMP_MeshOptimizer)

target_link_libraries(PluginCommon Qt5::Widgets)

add_subdirectory(gltf)
