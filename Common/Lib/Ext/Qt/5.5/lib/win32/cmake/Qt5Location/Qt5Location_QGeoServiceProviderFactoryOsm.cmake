
add_library(Qt5::QGeoServiceProviderFactoryOsm MODULE IMPORTED)

_populate_Location_plugin_properties(QGeoServiceProviderFactoryOsm RELEASE "geoservices/qtgeoservices_osm.dll")

list(APPEND Qt5Location_PLUGINS Qt5::QGeoServiceProviderFactoryOsm)
