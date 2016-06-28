
add_library(Qt5::QTcpServerConnection MODULE IMPORTED)

_populate_Declarative_plugin_properties(QTcpServerConnection RELEASE "qml1tooling/qmldbg_tcp_qtdeclarative.dll")

list(APPEND Qt5Declarative_PLUGINS Qt5::QTcpServerConnection)
