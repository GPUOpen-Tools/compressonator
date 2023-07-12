
# Host platform detection
set(CMP_HOST_APPLE OFF)
set(CMP_HOST_LINUX OFF)
set(CMP_HOST_WINDOWS OFF)

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin")
    set(CMP_HOST_APPLE ON)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(CMP_HOST_LINUX ON)
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(CMP_HOST_WINDOWS ON)
else()
    message(FATAL_ERROR "Unknown platform")
endif()


# Helper function for setting persistent CMake options
macro(cmp_option OPTION DESCRIPTION DEFAULT REQUIREMENT)
    set(requirement ${ARGV})
    list(REMOVE_AT requirement 0 1 2)

    if (${requirement})
        option(${OPTION} ${DESCRIPTION} ${DEFAULT})
    elseif ($CACHE{${OPTION}})
        message(WARNING "cache = $CACHE{${OPTION}} Turning off ${OPTION} because it requires ${requirement}")
        set(${OPTION} OFF)
    endif()

    if (${${OPTION}})
        add_compile_definitions(${OPTION}=1)
        message(STATUS "[ON] ${OPTION}")
    else()
        add_compile_definitions(${OPTION}=0)
        message(STATUS "[__] ${OPTION}")
    endif()
endmacro()

# Helper function to gather transitive (potential shared/dynamic) dependencies of a target
function(_get_transitive_dylibs _TARGET)

    # Clear variable for reuse
    if (NOT DEFINED _transitive_search_started)
        set(_transitive_search_started ON)
        unset(TRANSITIVE_DYLIBS)
    endif()

    # INTERFACE_LIBRARY targets can only have INTERFACE_LINK_LIBRARIES properties
    get_target_property(target_type ${_TARGET} TYPE)
    if(target_type STREQUAL "INTERFACE_LIBRARY")
        get_target_property(DEPENDENCIES ${_TARGET} INTERFACE_LINK_LIBRARIES)
    else()
        get_target_property(LINK_LIBRARIES ${_TARGET} LINK_LIBRARIES)
        get_target_property(INTERFACE_LINK_LIBRARIES ${_TARGET} INTERFACE_LINK_LIBRARIES)
        set(DEPENDENCIES ${LINK_LIBRARIES} ${INTERFACE_LINK_LIBRARIES})
    endif()
 
    # Collect transitive dependencies from target
    foreach(_DEP ${DEPENDENCIES})
        # Check to see if the dependency is a dynamic library
        string(REGEX MATCH ".*\\.(so|dll|dylib)(\\.[a-zA-Z0-9\\.]*)?$" dylib ${_DEP})
        if (NOT (dylib STREQUAL ""))
            list(APPEND TRANSITIVE_DYLIBS ${dylib})
        elseif(TARGET ${_DEP})
            # # Only traverse targets we have not visited before
            get_target_property(dep_type ${_DEP} TYPE)
            if(dep_type STREQUAL "INTERFACE_LIBRARY")
                # Marks the library for removal but prevents traversing it again
                set(DEP_FILE "${_DEP}.interface")
            else()
                set(DEP_FILE $<TARGET_FILE:${_DEP}>)
            endif()
            list(FIND TRANSITIVE_DYLIBS ${DEP_FILE} DEP_IN_LIST)
            if (DEP_IN_LIST EQUAL -1)
                # Add the target to the list
                list(APPEND TRANSITIVE_DYLIBS ${DEP_FILE})
                # Get the targets dependencies
                _get_transitive_dylibs(${_DEP})
            endif()
        endif()
    endforeach()

    # Prune and publish list
    list(REMOVE_DUPLICATES TRANSITIVE_DYLIBS)
    set(TRANSITIVE_DYLIBS ${TRANSITIVE_DYLIBS} PARENT_SCOPE)
endfunction()

# Helper function to gather transitive (potential shared/dynamic) dependencies of a target
function(get_transitive_dylibs _TARGET)

    _get_transitive_dylibs(${_TARGET})

    # Prune and publish list
    list(FILTER TRANSITIVE_DYLIBS EXCLUDE REGEX "\\.interface$")
    set(TRANSITIVE_DYLIBS ${TRANSITIVE_DYLIBS} PARENT_SCOPE)
endfunction()

# Helper function used to copy all dynamic libraries to an output directory
function(executable_post_build_dylibs TARGET_NAME OUTPUT_DIR)

    # yields TRANSITIVE_DYLIBS
    get_transitive_dylibs(${TARGET_NAME})

    # Don't add the custom target if no dynamic libraries were found
    list(LENGTH TRANSITIVE_DYLIBS _NUM_DYLIBS)
    if(_NUM_DYLIBS GREATER 0)
        add_custom_command(
            TARGET ${TARGET_NAME}
            PRE_BUILD
            COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/cmake/PostBuildCopy.cmake "\"${TRANSITIVE_DYLIBS}\""
                    ${OUTPUT_DIR}
        )
    endif()
endfunction()

macro(cmp_copy_to_output executable source dest)
    add_custom_command(TARGET ${executable} PRE_BUILD
        # COMMENT "Copying ${source} -> ${dest}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
                ${source}
                ${dest}
    )
endmacro()

macro(cmp_execute_process COMMAND WORKING_DIR)

    set(CMD_MSG "")
    foreach(CMD ${${COMMAND}})
        set(CMD_MSG "${CMD_MSG}${CMD} ")
    endforeach()

    message(STATUS "Executing process: ${CMD_MSG}")
    execute_process(
        COMMAND ${${COMMAND}}
        WORKING_DIRECTORY ${WORKING_DIR}
        ERROR_VARIABLE _stderr
        OUTPUT_VARIABLE _stdout
        RESULT_VARIABLE _exit_code ENCODING UTF-8
    )

    if(NOT _exit_code EQUAL 0)
        message(STATUS ${_stdout})
        if(EXISTS ${VULKAN_OUTPUT_DIR})
            file(REMOVE_RECURSE ${VULKAN_OUTPUT_DIR})
        endif()
        message(FATAL_ERROR "Process exited with non 0 exit code: ${_exit_code}\n${_stderr}")
    endif()

endmacro()