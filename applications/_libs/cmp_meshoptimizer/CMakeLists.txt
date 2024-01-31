add_library(extern_meshoptimizer)

target_sources(extern_meshoptimizer PUBLIC
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

target_include_directories(extern_meshoptimizer PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common

)

set_target_properties(extern_meshoptimizer PROPERTIES FOLDER ${PROJECT_FOLDER_SDK_EXTERNAL})


