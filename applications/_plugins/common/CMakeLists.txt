
set (PLUGIN_COMMON_SRC "")
set (PLUGIN_COMMON_H   "")

if (OPTION_CMP_QT)
    set(PLUGIN_COMMON_SRC_QT
        mipstoqimage.cpp
        )
    set(PLUGIN_COMMON_INC_QT
        mipstoqimage.h
        )
else()
    set(PLUGIN_COMMON_SRC_QT)
    set(PLUGIN_COMMON_INC_QT)
endif()


list(APPEND PLUGIN_COMMON_SRC
    atiformats.cpp
    cmdline.cpp
    cmp_fileio.cpp
    misc.cpp
    modeldata.cpp
    pluginmanager.cpp
    query_timer.cpp
    ssim.cpp
    tc_plugininternal.cpp
    textureio.cpp
    userinterface.cpp
    utilfuncs.cpp
)

list(APPEND PLUGIN_COMMON_H
    atiformats.h
    cmdline.h
    cmp_fileio.h
    common_kerneldef.h
    crc32.h
    hpc_compress.h
    misc.h
    modeldata.h
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

if (OPTION_BUILD_EXR)
    list(APPEND PLUGIN_COMMON_SRC cexr.cpp)
    list(APPEND PLUGIN_COMMON_H   cexr.h)
endif()

add_library(CMP_Common STATIC 
    ${PLUGIN_COMMON_SRC} 
    ${PLUGIN_COMMON_SRC_QT}
    ${PLUGIN_COMMON_H}
    ${PLUGIN_COMMON_INC_QT}
    )

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

if (OPTION_CMP_QT)
    target_link_libraries(CMP_Common Qt5::Widgets)
endif()

set_target_properties(CMP_Common PROPERTIES FOLDER "Libs")

