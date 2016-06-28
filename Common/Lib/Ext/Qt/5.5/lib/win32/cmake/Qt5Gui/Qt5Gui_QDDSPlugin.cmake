
add_library(Qt5::QDDSPlugin MODULE IMPORTED)

_populate_Gui_plugin_properties(QDDSPlugin RELEASE "imageformats/qdds.dll")

list(APPEND Qt5Gui_PLUGINS Qt5::QDDSPlugin)
