
add_library(Qt5::QWebViewPlugin MODULE IMPORTED)

_populate_Designer_plugin_properties(QWebViewPlugin RELEASE "designer/qwebview.dll")

list(APPEND Qt5Designer_PLUGINS Qt5::QWebViewPlugin)
