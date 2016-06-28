
if (CMAKE_VERSION VERSION_LESS 2.8.3)
    message(FATAL_ERROR "Qt 5 requires at least CMake version 2.8.3")
endif()

get_filename_component(_qt53DRenderer_install_prefix "${CMAKE_CURRENT_LIST_DIR}/../../../" ABSOLUTE)

# For backwards compatibility only. Use Qt53DRenderer_VERSION instead.
set(Qt53DRenderer_VERSION_STRING 5.5.1)

set(Qt53DRenderer_LIBRARIES Qt5::3DRenderer)

macro(_qt5_3DRenderer_check_file_exists file)
    if(NOT EXISTS "${file}" )
        message(FATAL_ERROR "The imported target \"Qt5::3DRenderer\" references the file
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

macro(_populate_3DRenderer_target_properties Configuration LIB_LOCATION IMPLIB_LOCATION)
    set_property(TARGET Qt5::3DRenderer APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

    set(imported_location "${_qt53DRenderer_install_prefix}/bin/${LIB_LOCATION}")
    _qt5_3DRenderer_check_file_exists(${imported_location})
    set_target_properties(Qt5::3DRenderer PROPERTIES
        "INTERFACE_LINK_LIBRARIES" "${_Qt53DRenderer_LIB_DEPENDENCIES}"
        "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        # For backward compatibility with CMake < 2.8.12
        "IMPORTED_LINK_INTERFACE_LIBRARIES_${Configuration}" "${_Qt53DRenderer_LIB_DEPENDENCIES}"
    )

    set(imported_implib "${_qt53DRenderer_install_prefix}/lib/${IMPLIB_LOCATION}")
    _qt5_3DRenderer_check_file_exists(${imported_implib})
    if(NOT "${IMPLIB_LOCATION}" STREQUAL "")
        set_target_properties(Qt5::3DRenderer PROPERTIES
        "IMPORTED_IMPLIB_${Configuration}" ${imported_implib}
        )
    endif()
endmacro()

if (NOT TARGET Qt5::3DRenderer)

    set(_Qt53DRenderer_OWN_INCLUDE_DIRS "${_qt53DRenderer_install_prefix}/include/" "${_qt53DRenderer_install_prefix}/include/Qt3DRenderer")
    set(Qt53DRenderer_PRIVATE_INCLUDE_DIRS "")
    include("${CMAKE_CURRENT_LIST_DIR}/ExtraSourceIncludes.cmake" OPTIONAL)

    foreach(_dir ${_Qt53DRenderer_OWN_INCLUDE_DIRS})
        _qt5_3DRenderer_check_file_exists(${_dir})
    endforeach()

    # Only check existence of private includes if the Private component is
    # specified.
    list(FIND Qt53DRenderer_FIND_COMPONENTS Private _check_private)
    if (NOT _check_private STREQUAL -1)
        foreach(_dir ${Qt53DRenderer_PRIVATE_INCLUDE_DIRS})
            _qt5_3DRenderer_check_file_exists(${_dir})
        endforeach()
    endif()

    set(Qt53DRenderer_INCLUDE_DIRS ${_Qt53DRenderer_OWN_INCLUDE_DIRS})

    set(Qt53DRenderer_DEFINITIONS -DQT_3DRENDERER_LIB)
    set(Qt53DRenderer_COMPILE_DEFINITIONS QT_3DRENDERER_LIB)
    set(_Qt53DRenderer_MODULE_DEPENDENCIES "3DCore;OpenGLExtensions;Gui;Concurrent;Core")


    set(_Qt53DRenderer_FIND_DEPENDENCIES_REQUIRED)
    if (Qt53DRenderer_FIND_REQUIRED)
        set(_Qt53DRenderer_FIND_DEPENDENCIES_REQUIRED REQUIRED)
    endif()
    set(_Qt53DRenderer_FIND_DEPENDENCIES_QUIET)
    if (Qt53DRenderer_FIND_QUIETLY)
        set(_Qt53DRenderer_DEPENDENCIES_FIND_QUIET QUIET)
    endif()
    set(_Qt53DRenderer_FIND_VERSION_EXACT)
    if (Qt53DRenderer_FIND_VERSION_EXACT)
        set(_Qt53DRenderer_FIND_VERSION_EXACT EXACT)
    endif()

    set(Qt53DRenderer_EXECUTABLE_COMPILE_FLAGS "")

    foreach(_module_dep ${_Qt53DRenderer_MODULE_DEPENDENCIES})
        if (NOT Qt5${_module_dep}_FOUND)
            find_package(Qt5${_module_dep}
                5.5.1 ${_Qt53DRenderer_FIND_VERSION_EXACT}
                ${_Qt53DRenderer_DEPENDENCIES_FIND_QUIET}
                ${_Qt53DRenderer_FIND_DEPENDENCIES_REQUIRED}
                PATHS "${CMAKE_CURRENT_LIST_DIR}/.." NO_DEFAULT_PATH
            )
        endif()

        if (NOT Qt5${_module_dep}_FOUND)
            set(Qt53DRenderer_FOUND False)
            return()
        endif()

        list(APPEND Qt53DRenderer_INCLUDE_DIRS "${Qt5${_module_dep}_INCLUDE_DIRS}")
        list(APPEND Qt53DRenderer_PRIVATE_INCLUDE_DIRS "${Qt5${_module_dep}_PRIVATE_INCLUDE_DIRS}")
        list(APPEND Qt53DRenderer_DEFINITIONS ${Qt5${_module_dep}_DEFINITIONS})
        list(APPEND Qt53DRenderer_COMPILE_DEFINITIONS ${Qt5${_module_dep}_COMPILE_DEFINITIONS})
        list(APPEND Qt53DRenderer_EXECUTABLE_COMPILE_FLAGS ${Qt5${_module_dep}_EXECUTABLE_COMPILE_FLAGS})
    endforeach()
    list(REMOVE_DUPLICATES Qt53DRenderer_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt53DRenderer_PRIVATE_INCLUDE_DIRS)
    list(REMOVE_DUPLICATES Qt53DRenderer_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt53DRenderer_COMPILE_DEFINITIONS)
    list(REMOVE_DUPLICATES Qt53DRenderer_EXECUTABLE_COMPILE_FLAGS)

    set(_Qt53DRenderer_LIB_DEPENDENCIES "Qt5::3DCore;Qt5::OpenGLExtensions;Qt5::Gui;Qt5::Concurrent;Qt5::Core")


    add_library(Qt5::3DRenderer SHARED IMPORTED)

    set_property(TARGET Qt5::3DRenderer PROPERTY
      INTERFACE_INCLUDE_DIRECTORIES ${_Qt53DRenderer_OWN_INCLUDE_DIRS})
    set_property(TARGET Qt5::3DRenderer PROPERTY
      INTERFACE_COMPILE_DEFINITIONS QT_3DRENDERER_LIB)

    _populate_3DRenderer_target_properties(RELEASE "Qt53DRenderer.dll" "Qt53DRenderer.lib" )

    if (EXISTS
        "${_qt53DRenderer_install_prefix}/bin/Qt53DRendererd.dll"
      AND EXISTS
        "${_qt53DRenderer_install_prefix}/lib/Qt53DRendererd.lib" )
        _populate_3DRenderer_target_properties(DEBUG "Qt53DRendererd.dll" "Qt53DRendererd.lib" )
    endif()



    file(GLOB pluginTargets "${CMAKE_CURRENT_LIST_DIR}/Qt53DRenderer_*Plugin.cmake")

    macro(_populate_3DRenderer_plugin_properties Plugin Configuration PLUGIN_LOCATION)
        set_property(TARGET Qt5::${Plugin} APPEND PROPERTY IMPORTED_CONFIGURATIONS ${Configuration})

        set(imported_location "${_qt53DRenderer_install_prefix}/plugins/${PLUGIN_LOCATION}")
        _qt5_3DRenderer_check_file_exists(${imported_location})
        set_target_properties(Qt5::${Plugin} PROPERTIES
            "IMPORTED_LOCATION_${Configuration}" ${imported_location}
        )
    endmacro()

    if (pluginTargets)
        foreach(pluginTarget ${pluginTargets})
            include(${pluginTarget})
        endforeach()
    endif()




_qt5_3DRenderer_check_file_exists("${CMAKE_CURRENT_LIST_DIR}/Qt53DRendererConfigVersion.cmake")

endif()
