
add_library(Qt5::QAxWidgetPlugin MODULE IMPORTED)

_populate_Designer_plugin_properties(QAxWidgetPlugin RELEASE "designer/qaxwidget.dll")

list(APPEND Qt5Designer_PLUGINS Qt5::QAxWidgetPlugin)
