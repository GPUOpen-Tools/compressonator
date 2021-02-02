# add_library(CMP_MeshOptimizer SHARED)
# 
# target_sources(CMP_MeshOptimizer PRIVATE
#     meshoptimizer.h
#     indexcodec.cpp
#     indexgenerator.cpp
#     overdrawanalyzer.cpp
#     overdrawoptimizer.cpp
#     simplifier.cpp
#     stripifier.cpp
#     vcacheanalyzer.cpp
#     vcacheoptimizer.cpp
#     vfetchanalyzer.cpp
#     vfetchoptimizer.cpp
# )
# 
# target_compile_definitions(CMP_MeshOptimizer PRIVATE BUILD_AS_PLUGIN_DLL=1)
# 
# set_target_properties(CMP_MeshOptimizer PROPERTIES 
#         FOLDER "PluginDynamic/Mesh_Compression"
#         RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
#         RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
#         )

add_library(CMP_MeshOptimizer)

target_sources(CMP_MeshOptimizer PUBLIC
    indexcodec.cpp
    indexgenerator.cpp
    meshoptimizer.h
    overdrawanalyzer.cpp
    overdrawoptimizer.cpp
    simplifier.cpp
    stripifier.cpp
    vcacheanalyzer.cpp
    vcacheoptimizer.cpp
    vfetchanalyzer.cpp
    vfetchoptimizer.cpp
)

target_include_directories(CMP_MeshOptimizer PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common

)

set_target_properties(CMP_MeshOptimizer PROPERTIES FOLDER "Libs")


