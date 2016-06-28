
add_library(Qt5::QM3uPlaylistPlugin MODULE IMPORTED)

_populate_Multimedia_plugin_properties(QM3uPlaylistPlugin RELEASE "playlistformats/qtmultimedia_m3u.dll")

list(APPEND Qt5Multimedia_PLUGINS Qt5::QM3uPlaylistPlugin)
