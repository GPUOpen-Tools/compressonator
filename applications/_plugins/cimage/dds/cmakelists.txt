
add_library(Plugin_CImage_DDS)

target_sources(Plugin_CImage_DDS PRIVATE

    DDS.cpp
    DDS.h
    DDS_DX10.cpp
    DDS_DX10.h
    DDS_File.cpp
    DDS_File.h
    DDS_Helpers.cpp
    DDS_Helpers.h
)

target_link_libraries(Plugin_CImage_DDS PRIVATE

    CompressonatorLIB

    Plugin_TCPluginAPI

    ExtOpenEXR
)

set_target_properties(Plugin_CImage_DDS PROPERTIES FOLDER ${FOLDER_NAME})
