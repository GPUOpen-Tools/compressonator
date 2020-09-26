# DependencyInfo.cmake

if (NOT DEFINED DEPENDENCIES_INSTALL_DIR)
    get_filename_component(DEPENDENCIES_INSTALL_DIR ${CMAKE_CURRENT_LIST_DIR}/../build-out ABSOLUTE)
endif()

get_filename_component(DEPENDENCIES_INSTALL_DIR ${DEPENDENCIES_INSTALL_DIR}/${CMAKE_SYSTEM_NAME} ABSOLUTE)

if (WIN32)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>$<$<BOOL:BUILD_SHARED_LIBS>:DLL>")
    if (BUILD_SHARED_LIBS)
        get_filename_component(DEPENDENCIES_INSTALL_DIR ${DEPENDENCIES_INSTALL_DIR}/MD ABSOLUTE)
    else()
        get_filename_component(DEPENDENCIES_INSTALL_DIR ${DEPENDENCIES_INSTALL_DIR}/MT ABSOLUTE)
    endif()
elseif (NOT APPLE)
    if (EXISTS /etc/os-release)
        file(STRINGS /etc/os-release ID_FIELD
            REGEX "^ID=.*$"
        )
        string(REPLACE "ID=" "" OS_NAME_STR ${ID_FIELD})
        string(REPLACE "\"" "" OS_NAME ${OS_NAME_STR})
        get_filename_component(DEPENDENCIES_INSTALL_DIR ${DEPENDENCIES_INSTALL_DIR}/${OS_NAME} ABSOLUTE)
    endif()
endif()