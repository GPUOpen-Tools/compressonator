
add_library(CMP_MeshOptimizer)

target_sources(CMP_MeshOptimizer PRIVATE

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
)

set_target_properties(CMP_MeshOptimizer PROPERTIES FOLDER ${FOLDER_NAME})
