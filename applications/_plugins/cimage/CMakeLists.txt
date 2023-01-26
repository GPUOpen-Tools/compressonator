
add_library(Plugin_CImage INTERFACE)

add_subdirectory(astc)
add_subdirectory(dds)
add_subdirectory(exr)
add_subdirectory(ktx)
add_subdirectory(tga)
add_subdirectory(binary)

target_link_libraries(Plugin_CImage INTERFACE

    Plugin_CImage_ASTC
    Plugin_CImage_DDS
    Plugin_CImage_EXR
    Plugin_CImage_KTX
    Plugin_CImage_TGA
    Plugin_CIMAGE_BINARY
)

#set_target_properties(Plugin_CImage PROPERTIES FOLDER ${FOLDER_NAME})
