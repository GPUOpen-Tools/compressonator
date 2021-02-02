set(PLUGIN_COMMON_SRC
    atiformats.cpp
    cexr.cpp
    cmdline.cpp
    cmp_fileio.cpp
    misc.cpp
    modeldata.cpp
    mipstoqimage.cpp
    pluginmanager.cpp
    query_timer.cpp
    ssim.cpp
    tc_plugininternal.cpp
    textureio.cpp
    userinterface.cpp
    utilfuncs.cpp
)

set(PLUGIN_COMMON_H
    atiformats.h
    cexr.h
    cmdline.h
    cmp_fileio.h
    common_kerneldef.h
    crc32.h
    hpc_compress.h
    misc.h
    modeldata.h
    mipstoqimage.h
    namespacealias.h
    pluginbase.h
    plugininterface.h
    pluginmanager.h
    query_timer.h
    ssim.h
    stb_image.h
    tc_pluginapi.h
    tc_plugininternal.h
    testreport.h
    texture.h
    textureio.h
    userinterface.h
    utilfuncs.h
    vectypes.h
)

add_library(CMP_Common STATIC ${PLUGIN_COMMON_SRC} ${PLUGIN_COMMON_H})

target_include_directories(CMP_Common PRIVATE
  .
  ${PROJECT_SOURCE_DIR}/cmp_framework
  ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
  ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf
  ${PROJECT_SOURCE_DIR}/cmp_core/shaders
  ${PROJECT_SOURCE_DIR}/cmp_core/source
  ${PROJECT_SOURCE_DIR}/applications/_plugins/cmesh
  ${PROJECT_SOURCE_DIR}/applications/_plugins/cmesh/mesh_optimizer
  ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
  ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math
  ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/rapidxml
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
  ${OpenCV_INCLUDE_DIRS}
  ${OpenEXR_INCLUDE_DIRS}
  ${draco_INCLUDE_DIRS}
  )

target_link_libraries(CMP_Common Qt5::Widgets)

set_target_properties(CMP_Common PROPERTIES FOLDER "Libs")

add_subdirectory(gltf)

