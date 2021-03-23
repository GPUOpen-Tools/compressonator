# CMakeCustomTargets CMakeLists.txt

message(STATUS "++++++++++++++++++ External-QT " ${QT_PACKAGE_ROOT} )

if (NOT OPTION_CMP_QT)
    message("OPTION_CMP_QT=OFF")
    return()
endif()

#if (NOT DEFINED ENV{QTDIR})
#    message(FATAL_ERROR "A 'QTDIR' environment variable is required")
#endif()

find_package(Qt5 COMPONENTS REQUIRED Core HINTS ${QT_PACKAGE_ROOT})

get_target_property(QtCore_location Qt5::Core LOCATION)

set(QT_LIB_DIR  ${_qt5Core_install_prefix})

# ====================================================
# QT_LIB_DIR
# ====================================================
set_property(GLOBAL PROPERTY QT_LIB_DIR "${QT_LIB_DIR}")
list(APPEND CMAKE_FIND_ROOT_PATH ${QT_LIB_DIR})
set(PATH_SUFFIXES lib plugins/imageformats)

set(QT_FIND_ROOT_PATH
    ${CMAKE_FIND_ROOT_PATH}
    ${QT_LIB_DIR}
)

# ====================================================
#                       Q T 5
# ====================================================

find_library(LIB_PNG png png.a libpng.a libqtpng.a libqtlibpng.a qtlibpng.lib
    PATHS ${QT_FIND_ROOT_PATH}
    PATH_SUFFIXES ${PATH_SUFFIXES}
)

if (${LIB_PNG} STREQUAL "LIB_PNG-NOTFOUND")
    message(FATAL_ERROR "png library not found")
endif()

find_package(Qt5 REQUIRED QUIET
    COMPONENTS Core Gui OpenGL Qml QuickWidgets WebEngineWidgets Widgets Xml
    PATHS ${QT_FIND_ROOT_PATH}
    HINTS ${QT_PACKAGE_ROOT}
)

add_library(ExtQt5 INTERFACE)

target_link_libraries(ExtQt5 
    INTERFACE
        Qt5::Core
        Qt5::Gui
        ${LIB_PNG}
)

if (CMP_HOST_APPLE)
    # find_package(LibLZMA REQUIRED QUIET)
    target_link_libraries(ExtQt5 INTERFACE
        # LibLZMA::LibLZMA
        "-framework Cocoa"
    )
endif()


add_library(ExtQt5OpenGL INTERFACE)
target_link_libraries(ExtQt5OpenGL
    INTERFACE
        Qt5::OpenGL
)

add_library(ExtQt5Widgets INTERFACE)
target_link_libraries(ExtQt5Widgets
    INTERFACE    
        Qt5::Widgets
)

add_library(ExtQt5ForGUI INTERFACE)
target_link_libraries(ExtQt5ForGUI 
    INTERFACE
        ExtQt5
        Qt5::Qml
        Qt5::QuickWidgets
        Qt5::WebEngineWidgets 
        Qt5::Xml
)

if (CMP_HOST_LINUX)

find_library(LIBICUDATA NAMES libicudata.so.56 PATHS ${QT_LIB_DIR}/lib)
if (LIBICUDATA STREQUAL "LIBICUDATA-NOTFOUND")
    message(FATAL_ERROR "libicudata not found in Qt submodule")
endif()
find_library(LIBICUUC NAMES libicuuc.so.56 PATHS ${QT_LIB_DIR}/lib)
if (LIBICUUC STREQUAL "LIBICUUC-NOTFOUND")
    message(FATAL_ERROR "libicuuc not found in Qt submodule")
endif()
find_library(LIBICUI18N NAMES libicui18n.so.56 PATHS ${QT_LIB_DIR}/lib)
if (LIBICUI18N STREQUAL "LIBICUI18N-NOTFOUND")
    message(FATAL_ERROR "libicui18n not found in Qt submodule")
endif()

target_link_libraries(ExtQt5ForGUI 
    INTERFACE
        ${LIBICUDATA}
        ${LIBICUUC}
        ${LIBICUI18N}
)

endif()