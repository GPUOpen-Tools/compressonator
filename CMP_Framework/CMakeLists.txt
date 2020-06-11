
add_library(CMP_FrameworkLIB INTERFACE)

target_sources(CMP_FrameworkLIB INTERFACE

    CMP_Framework.h
)

target_include_directories(CMP_FrameworkLIB INTERFACE
    ./
)

add_library(CMP_Framework)

target_link_libraries(CMP_Framework PRIVATE

    CMP_Common
    CMP_FrameworkLIB
    CompressonatorLIB
    Plugin_PluginManager
)
