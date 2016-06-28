
if (CMAKE_VERSION VERSION_LESS 2.8.3)
    message(FATAL_ERROR "Qt 5 requires at least CMake version 2.8.3")
endif()

get_filename_component(_qt53DCore_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt53DCore_VERSION instead.
set(Qt53DCore_VERSION_STRING 5.5.1)

set(Qt53DCore_LIBRARIES Qt5::3DCore)

macro(_qt5_3DCore_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::3DCore\" references the file
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

macro(_populate_3DCore_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::3DCore APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt53DCore_install_prefix}/bin/${LIB_LOCATION}")
    _qt5_3DCore_check_file_exists(${imported_location})
    set_target_properties(Qt5::3DCore PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt53DCore_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt53DCore_LIB_DEPENDENCIES}"
    )

    set(imported_implib "${_qt53DCore_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_3DCore_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::3DCore PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::3DCore)

    set(_Qt53DCore_OWN_INCLUDE_DIRS "${_qt53DCore_install_prefix}/include/" "${_qt53DCore_install_prefix}/include/Qt3DCore")
    set(Qt53DCore_PRIVATE_INCLUDE_DIRS "")
    include("${CMAKE_CURRENT_LIST_DIR}/ExtraSourceIncludes.cmake" OPTIONAL)

    foreach(_dir ${_Qt53DCore_OWN_INCLUDE_DIRS})
        _qt5_3DCore_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt53DCore_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt53DCore_PRIVATE_INCLUDE_DIRS})
            _qt5_3DCore_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt53DCore_INCLUDE_DIRS ${_Qt53DCore_OWN_INCLUDE_DIRS})

    set(Qt53DCore_DEFINITIONS -DQT_3DCORE_LIB)
    set(Qt53DCore_COMPILE_DEFINITIONS QT_3DCORE_LIB)
    set(_Qt53DCore_MODULE_DEPENDENCIES "Gui;Core")


    set(_Qt53DCore_FIND_DEPENDENCIES_REQUIRED)
    if (Qt53DCore_FIND_REQUIRED)
        set(_Qt53DCore_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt53DCore_FIND_DEPENDENCIES_QUIET)
    if (Qt53DCore_FIND_QUIETLY)
        set(_Qt53DCore_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt53DCore_FIND_VERSION_EXACT)
    if (Qt53DCore_FIND_VERSION_EXACT)
        set(_Qt53DCore_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt53DCore_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt53DCore_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                5.5.1 ${_Qt53DCore_FIND_VERSION_EXACT}
                ${_Qt53DCore_DEPENDENCIES_FIND_QUIET}
                ${_Qt53DCore_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt53DCore_FOUND False)
            return()
        endif()

        list(APPEND Qt53DCore_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt53DCore_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt53DCore_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt53DCore_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt53DCore_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt53DCore_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt53DCore_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt53DCore_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt53DCore_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt53DCore_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt53DCore_LIB_DEPENDENCIES "Qt5::Gui;Qt5::Core")


    add_library(Qt5::3DCore SHARED IMPORTED)

    set_property(TARGET Qt5::3DCore PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt53DCore_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::3DCore PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_3DCORE_LIB)

    _populate_3DCore_target_properties(RELEASE "Qt53DCore.dll" "Qt53DCore.lib" )

    if (EXISTS
        "${_qt53DCore_install_prefix}/bin/Qt53DCored.dll"
      AND EXISTS
        "${_qt53DCore_install_prefix}/lib/Qt53DCored.lib" )
        _populate_3DCore_target_properties(DEBUG "Qt53DCored.dll" "Qt53DCored.lib" )
    endif()



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt53DCore_*Plugin.cmake")

    macro(_populate_3DCore_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt53DCore_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_3DCore_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()




_qt5_3DCore_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt53DCoreConfigVersion.cmake")

endif()
