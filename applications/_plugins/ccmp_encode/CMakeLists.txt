
add_library(Plugin_CCMP_Encode)

target_sources(Plugin_CCMP_Encode PRIVATE

    hpc/ccpu_hpc.cpp
    hpc/ccpu_hpc.h
    hpc/cmp_hpc.cpp
    hpc/cmp_hpc.h
    hpc/compute_cpu_hpc.cpp
    hpc/compute_cpu_hpc.h
)

target_include_directories(Plugin_CCMP_Encode PUBLIC
    ./
    ./hpc
)

target_link_libraries(Plugin_CCMP_Encode PRIVATE

    CMP_Common
    CompressonatorLIB

    Plugin_TCPluginAPI
    Plugin_Common_QueryPerformance
)

set_target_properties(Plugin_CCMP_Encode PROPERTIES FOLDER ${FOLDER_NAME})
