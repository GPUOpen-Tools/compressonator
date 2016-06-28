
add_library(Qt5::QGeoServiceProviderFactoryNokia MODULE IMPORTED)

_populate_Location_plugin_properties(QGeoServiceProviderFactoryNokia RELEASE "geoservices/qtgeoservices_nokia.dll")

list(APPEND Qt5Location_PLUGINS Qt5::QGeoServiceProviderFactoryNokia)
