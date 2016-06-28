
add_library(Qt5::QtQuick1Plugin MODULE IMPORTED)

_populate_Declarative_plugin_properties(QtQuick1Plugin RELEASE "qml1tooling/qmldbg_inspector.dll")

list(APPEND Qt5Declarative_PLUGINS Qt5::QtQuick1Plugin)
