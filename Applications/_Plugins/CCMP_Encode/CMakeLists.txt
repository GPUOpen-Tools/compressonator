
add_library(Plugin_CCMP_Encode)

target_sources(Plugin_CCMP_Encode PRIVATE

    HPC/CCPU_HPC.cpp
    HPC/CCPU_HPC.h
    HPC/cmp_hpc.cpp
    HPC/cmp_hpc.h
    HPC/Compute_CPU_HPC.cpp
    HPC/Compute_CPU_HPC.h
)

target_include_directories(Plugin_CCMP_Encode PUBLIC
    ./
    ./HPC
)

target_link_libraries(Plugin_CCMP_Encode PRIVATE

    CMP_Common
	CompressonatorLIB

	Plugin_TCPluginAPI
    Plugin_Common_QueryPerformance
)

set_target_properties(Plugin_CCMP_Encode PROPERTIES FOLDER ${FOLDER_NAME})
