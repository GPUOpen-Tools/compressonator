
if (CMAKE_VERSION VERSION_LESS 2.8.3)
    message(FATAL_ERROR "Qt 5 requires at least CMake version 2.8.3")
endif()

get_filename_component(_qt5AxContainer_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt5AxContainer_VERSION instead.
set(Qt5AxContainer_VERSION_STRING 5.5.1)

set(Qt5AxContainer_LIBRARIES Qt5::AxContainer)

macro(_qt5_AxContainer_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::AxContainer\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
endmacro()

macro(_populate_AxContainer_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::AxContainer APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt5AxContainer_install_prefix}/lib/${LIB_LOCATION}")
    _qt5_AxContainer_check_file_exists(${imported_location})
    set_target_properties(Qt5::AxContainer PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt5AxContainer_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt5AxContainer_LIB_DEPENDENCIES}"
    )

    set(imported_implib "${_qt5AxContainer_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_AxContainer_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::AxContainer PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::AxContainer)

    set(_Qt5AxContainer_OWN_INCLUDE_DIRS "")
    set(Qt5AxContainer_PRIVATE_INCLUDE_DIRS "")

    foreach(_dir ${_Qt5AxContainer_OWN_INCLUDE_DIRS})
        _qt5_AxContainer_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt5AxContainer_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt5AxContainer_PRIVATE_INCLUDE_DIRS})
            _qt5_AxContainer_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt5AxContainer_INCLUDE_DIRS ${_Qt5AxContainer_OWN_INCLUDE_DIRS})

    set(Qt5AxContainer_DEFINITIONS -DQT_AXCONTAINER_LIB)
    set(Qt5AxContainer_COMPILE_DEFINITIONS QT_AXCONTAINER_LIB)
    set(_Qt5AxContainer_MODULE_DEPENDENCIES "AxBase;Widgets;Gui;Core")


    set(_Qt5AxContainer_FIND_DEPENDENCIES_REQUIRED)
    if (Qt5AxContainer_FIND_REQUIRED)
        set(_Qt5AxContainer_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt5AxContainer_FIND_DEPENDENCIES_QUIET)
    if (Qt5AxContainer_FIND_QUIETLY)
        set(_Qt5AxContainer_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt5AxContainer_FIND_VERSION_EXACT)
    if (Qt5AxContainer_FIND_VERSION_EXACT)
        set(_Qt5AxContainer_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt5AxContainer_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt5AxContainer_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                5.5.1 ${_Qt5AxContainer_FIND_VERSION_EXACT}
                ${_Qt5AxContainer_DEPENDENCIES_FIND_QUIET}
                ${_Qt5AxContainer_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt5AxContainer_FOUND False)
            return()
        endif()

        list(APPEND Qt5AxContainer_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt5AxContainer_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt5AxContainer_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt5AxContainer_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt5AxContainer_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt5AxContainer_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5AxContainer_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt5AxContainer_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5AxContainer_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt5AxContainer_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt5AxContainer_LIB_DEPENDENCIES "Qt5::AxBase;Qt5::Widgets;Qt5::Gui;Qt5::Core")


    add_library(Qt5::AxContainer STATIC IMPORTED)
    set_property(TARGET Qt5::AxContainer PROPERTY IMPORTED_LINK_INTERFACE_LANGUAGES CXX)

    set_property(TARGET Qt5::AxContainer PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt5AxContainer_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::AxContainer PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_AXCONTAINER_LIB)

    _populate_AxContainer_target_properties(RELEASE "Qt5AxContainer.lib" "" )

    if (EXISTS "${_qt5AxContainer_install_prefix}/lib/Qt5AxContainerd.lib" )
        _populate_AxContainer_target_properties(DEBUG "Qt5AxContainerd.lib" "" )
    endif()



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt5AxContainer_*Plugin.cmake")

    macro(_populate_AxContainer_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt5AxContainer_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_AxContainer_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()




_qt5_AxContainer_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt5AxContainerConfigVersion.cmake")

endif()
