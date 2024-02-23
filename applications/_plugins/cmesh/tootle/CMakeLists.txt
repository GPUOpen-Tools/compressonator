link_directories(
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Release
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Bin/Desktop_2019/x64/Debug
    )

add_library(Mesh_Tootle SHARED)

target_sources(Mesh_Tootle PRIVATE
    mesh_tootle.cpp
    mesh_tootle.h
    option.h
    timer.cpp
    timer.h
    ${PROJECT_SOURCE_DIR}/applications/_libs/CMP_Mesh/tootlelib.cpp
    ${PROJECT_SOURCE_DIR}/applications/_libs/CMP_Mesh/tootlelib.h
    ${PROJECT_SOURCE_DIR}/applications/_libs/CMP_Mesh/tootlepch.h
    ${PROJECT_SOURCE_DIR}/applications/_libs/CMP_Mesh/tootleraytracer.cpp
    ${PROJECT_SOURCE_DIR}/applications/_libs/CMP_Mesh/tootleraytracer.h
    ${PROJECT_SOURCE_DIR}/applications/_libs/CMP_Mesh/overdraw.cpp
)

target_include_directories(Mesh_Tootle
               PRIVATE
               ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
               ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
               ${PROJECT_SOURCE_DIR}/applications/_plugins/common
               ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_mesh
               ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_mesh/jrt
               ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math
               ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
               ${PROJECT_SOURCE_DIR}/applications/_plugins/c3dmodel_loaders/obj
               ${PROJECT_SOURCE_DIR}/../common/lib/ext/apitrace/dxsdk/Include
               )

target_link_libraries(Mesh_Tootle 
    PRIVATE
    CMP_Compressonator
    CMP_Framework
    CMP_Math
    CMP_Mesh
)

target_compile_definitions(Mesh_Tootle PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(Mesh_Tootle PROPERTIES 
        FOLDER ${PROJECT_FOLDER_SDK_PLUGIN_DYNAMIC}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
        )
