
add_library(Qt5::QJpegPlugin MODULE IMPORTED)

_populate_Gui_plugin_properties(QJpegPlugin RELEASE "imageformats/qjpeg.dll")

list(APPEND Qt5Gui_PLUGINS Qt5::QJpegPlugin)
