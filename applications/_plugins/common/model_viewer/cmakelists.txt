# =======================================================================

add_library(Plugin_Common_Model_Viewer INTERFACE)

target_sources(Plugin_Common_Model_Viewer INTERFACE

    modeltexture.cpp
    modeltexture.h
)

target_include_directories(Plugin_Common_Model_Viewer INTERFACE
    ./
)

target_link_libraries(Plugin_Common_Model_Viewer INTERFACE

    CompressonatorLIB

    Plugin_PluginManager

    Plugin_Common_UtilFuncs
    Plugin_GLTF
)

#set_target_properties(Plugin_Common_Model_Viewer PROPERTIES FOLDER ${FOLDER_NAME})
