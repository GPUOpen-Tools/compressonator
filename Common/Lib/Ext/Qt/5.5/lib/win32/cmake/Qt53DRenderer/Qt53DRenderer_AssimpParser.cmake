
add_library(Qt5::AssimpParser MODULE IMPORTED)

_populate_3DRenderer_plugin_properties(AssimpParser RELEASE "sceneparsers/assimpsceneparser.dll")

list(APPEND Qt53DRenderer_PLUGINS Qt5::AssimpParser)
