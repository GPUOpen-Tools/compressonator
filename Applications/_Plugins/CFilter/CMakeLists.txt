
add_library(Plugin_CFilter)

target_sources(Plugin_CFilter PRIVATE

    BoxFilter.cpp
    BoxFilter.h
)

target_include_directories(Plugin_CFilter PUBLIC
    ./
)

target_link_libraries(Plugin_CFilter PRIVATE

	CMP_FileIO
	CMP_MeshCompressor
	CMP_MeshOptimizer
	CompressonatorLIB

	Plugin_CmdLine
	Plugin_PluginManager
	Plugin_TCPluginAPI

	ExtOpenEXR
)

set_target_properties(Plugin_CFilter PROPERTIES FOLDER ${FOLDER_NAME})
