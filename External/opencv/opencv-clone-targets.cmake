# opencv-clone-targets.cmake

# Helper function for cloning a target.
function(_opencv_clone_target TARGET LIB_BUILD_TYPE PROPERTY_LIST)
message(STATUS "(4A)======================== opencv Paths ${CMAKE_CURRENT_LIST_DIR}" )

    string(TOLOWER ${LIB_BUILD_TYPE} build_type_lower)
    string(TOUPPER ${LIB_BUILD_TYPE} build_type_upper)
    set(CMAKE_PROPERTY_LIST ${${PROPERTY_LIST}})
    set(NEW_TARGET ${TARGET}_${build_type_lower})

    if(NOT TARGET ${TARGET})
        return()
    endif()

    if(TARGET ${NEW_TARGET})
        set(OPENCV_TARGET_CLONE ${NEW_TARGET} PARENT_SCOPE)
        return()
    endif()

    get_target_property(TARGET_TYPE ${TARGET} TYPE)
    string(REPLACE "_LIBRARY" "" TARGET_TYPE ${TARGET_TYPE})
    add_library(${NEW_TARGET} ${TARGET_TYPE} IMPORTED GLOBAL)

    if (TARGET_TYPE STREQUAL "INTERFACE")
        list(FILTER CMAKE_PROPERTY_LIST INCLUDE REGEX "^INTERFACE.*")
    endif()

    foreach (prop ${CMAKE_PROPERTY_LIST})
        string(REPLACE "<CONFIG>" "${build_type_upper}" prop ${prop})
        if(prop MATCHES "TYPE" OR prop MATCHES "NAME" OR prop MATCHES "IMPORTED_GLOBAL" OR prop MATCHES "IMPORTED")
            continue()
        endif()
        get_property(propval TARGET ${TARGET} PROPERTY ${prop} SET)
        if (propval)
            get_target_property(propval ${TARGET} ${prop})
            if (propval STREQUAL "")
                continue()
            endif()
            string(FIND "${propval}" "NOT-FOUND" thing REVERSE)
            if (NOT (${thing} EQUAL -1))
                message(FATAL_ERROR "${prop} == ${propval}")
            endif()

            if (prop STREQUAL "INTERFACE_LINK_LIBRARIES")
                foreach(iface_lib ${propval})
                    string(REGEX MATCH "^\\$<LINK_ONLY:.+>$" LINK_ONLY_TARGET ${iface_lib})
                    if (NOT (LINK_ONLY_TARGET STREQUAL ""))
                        string(REPLACE "$<LINK_ONLY:" "" iface_lib ${iface_lib})
                        string(REPLACE ">" "" iface_lib ${iface_lib})
                        _opencv_clone_target(${iface_lib} ${LIB_BUILD_TYPE} CMAKE_PROPERTY_LIST)
                        list(APPEND OPENCV_TARGET_IFACE_LIBS $<LINK_ONLY:${OPENCV_TARGET_CLONE}>)
                    else()
                        _opencv_clone_target(${iface_lib} ${LIB_BUILD_TYPE} CMAKE_PROPERTY_LIST)
                        list(APPEND OPENCV_TARGET_IFACE_LIBS ${OPENCV_TARGET_CLONE})
                    endif()
                endforeach()
                set_target_properties(${NEW_TARGET} PROPERTIES ${prop} "${OPENCV_TARGET_IFACE_LIBS}")
            elseif (prop STREQUAL "INTERFACE_INCLUDE_DIRECTORIES")
                foreach(iface_dir ${propval})
                    string(REGEX REPLACE "opencv-release|opencv-debug" "opencv-${build_type_lower}" iface_dir "${iface_dir}")
                    list(APPEND IFACE_INCLUDE_DIRS ${iface_dir})
                endforeach()
                set_target_properties(${NEW_TARGET} PROPERTIES ${prop} "${IFACE_INCLUDE_DIRS}")
            elseif (prop STREQUAL "LOCATION")
                set_target_properties(${NEW_TARGET} PROPERTIES IMPORTED_LOCATION "${propval}")
                set_target_properties(${NEW_TARGET} PROPERTIES ${prop} "${propval}")
            else()
                set_target_properties(${NEW_TARGET} PROPERTIES ${prop} "${propval}")
            endif()
        endif()
    endforeach()

    set(OPENCV_TARGET_CLONE ${NEW_TARGET} PARENT_SCOPE)
endfunction()


# Helper function for cloning all targets from opencv for a specific build type (debug/release)
function(opencv_clone_targets LIB_BUILD_TYPE)
    string(TOLOWER ${LIB_BUILD_TYPE} build_type_lower)
    string(TOUPPER ${LIB_BUILD_TYPE} build_type_upper)
message(STATUS "(4B)======================== opencv Paths ${CMAKE_CURRENT_LIST_DIR}" )

    # Find OpenCV package
    find_package(OpenCV REQUIRED QUIET
        #PATHS ${DEPENDENCIES_INSTALL_DIR}/opencv-${build_type_lower}
        PATHS ${CMAKE_CURRENT_LIST_DIR}/../opencv2
    )

    #unset(OpenCV_DIR CACHE)

    # Get all propreties that cmake supports
    execute_process(COMMAND cmake --help-property-list OUTPUT_VARIABLE CMAKE_PROPERTY_LIST)

    # Convert command output into a CMake list
    string(REGEX REPLACE ";" "\\\\;" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    string(REGEX REPLACE "\n" ";" CMAKE_PROPERTY_LIST "${CMAKE_PROPERTY_LIST}")
    
    foreach(lib ${OpenCV_LIBRARIES})
        _opencv_clone_target(${lib} ${LIB_BUILD_TYPE} CMAKE_PROPERTY_LIST)
        list(APPEND OPENCV_${build_type_upper}_LIBRARIES ${OPENCV_TARGET_CLONE})
    endforeach()
    set(OPENCV_${build_type_upper}_LIBRARIES ${OPENCV_${build_type_upper}_LIBRARIES} PARENT_SCOPE)

    foreach(lib_dir ${OpenCV_INCLUDE_DIRS})
        string(REGEX REPLACE "opencv-release|opencv-debug" "opencv-${build_type_lower}" lib_dir "${lib_dir}")
        list(APPEND OPENCV_${build_type_upper}_INCLUDE_DIRS ${lib_dir})
    endforeach()
    set(OpenCV_${build_type_upper}_INCLUDE_DIRS ${OPENCV_${build_type_upper}_INCLUDE_DIRS} PARENT_SCOPE)

endfunction()
