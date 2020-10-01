
# Parse out the provided arguments
if (CMAKE_ARGC EQUAL 5)
    set(LIBS_LIST ${CMAKE_ARGV3})
    set(APP_OUTPUT ${CMAKE_ARGV4})

    list(LENGTH LIBS_LIST NUM_DYLIBS)
    if (NUM_DYLIBS GREATER 0)

        # Make the directory if it does not exist
        if (NOT EXISTS APP_OUTPUT)
            file(MAKE_DIRECTORY ${APP_OUTPUT})
        endif ()

        foreach (lib ${LIBS_LIST})
            string(REGEX MATCH ".*\\.(so|dll|dylib)(\\.[a-zA-Z0-9\\.]*)?$" dylib ${lib})
            if (NOT (dylib STREQUAL ""))
                execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${lib}" "${APP_OUTPUT}"
                    # COMMAND_ECHO STDOUT
                    ENCODING UTF-8
                    OUTPUT_VARIABLE stdout
                    RESULT_VARIABLE result
                )
                # Error handling
                if (NOT (result EQUAL 0))
                    message(DEBUG "${stdout}")
                    message(FATAL_ERROR "${result}")
                endif()
                get_filename_component(lib_basename "${lib}" NAME)

                # Get list of extensions
                get_filename_component(lib_exts "${lib}" EXT)
                string(REPLACE "." ";" lib_ext_list "${lib_exts}")

                get_filename_component(lib_symlink "${lib}" NAME_WE)

                # Create symlinks for all extensions
                foreach(lib_ext ${lib_ext_list})
                    set(lib_symlink "${lib_symlink}.${lib_ext}")
                    if (lib_symlink STREQUAL "${lib_basename}")
                        continue()
                    endif()
                    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink "${lib_basename}" "${lib_symlink}"
                        # COMMAND_ECHO STDOUT
                        WORKING_DIRECTORY "${APP_OUTPUT}"
                        ENCODING UTF-8
                        OUTPUT_VARIABLE stdout
                        RESULT_VARIABLE result
                    )
                    # Error handling
                    if (NOT (result EQUAL 0))
                        message(DEBUG "${stdout}")
                        message(FATAL_ERROR "${result}")
                    endif()
                endforeach()
            endif()
        endforeach ()

    endif ()
else ()
    message(FATAL_ERROR "Unexpected number of arguments")
endif ()
