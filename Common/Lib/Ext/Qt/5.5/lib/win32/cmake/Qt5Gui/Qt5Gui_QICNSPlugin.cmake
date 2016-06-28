
add_library(Qt5::QICNSPlugin MODULE IMPORTED)

_populate_Gui_plugin_properties(QICNSPlugin RELEASE "imageformats/qicns.dll")

list(APPEND Qt5Gui_PLUGINS Qt5::QICNSPlugin)
