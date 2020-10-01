
add_library(Plugin_CImage INTERFACE)

add_subdirectory(ASTC)
add_subdirectory(DDS)
add_subdirectory(EXR)
add_subdirectory(KTX)
add_subdirectory(TGA)

target_link_libraries(Plugin_CImage INTERFACE

    Plugin_CImage_ASTC
    Plugin_CImage_DDS
    Plugin_CImage_EXR
    Plugin_CImage_KTX
    Plugin_CImage_TGA
)

#set_target_properties(Plugin_CImage PROPERTIES FOLDER ${FOLDER_NAME})
