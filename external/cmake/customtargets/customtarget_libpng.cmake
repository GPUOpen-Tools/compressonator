# Externals CMakeLists.txt

# ====================================================
#                    L I B P N G
# ====================================================
set(LIBPNG_VERSION 1.6.37)
get_filename_component(LIBPNG_DIR ${COMPRESSONATOR_PROJECT_DIR}/external/libpng/libpng-submodule ABSOLUTE)

set(LIBPNG_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libpng-${LIBPNG_VERSION})
file(MAKE_DIRECTORY ${LIBPNG_BINARY_DIR})


file(GLOB_RECURSE LIBPNG_SRC

    ${LIBPNG_DIR}/*.h
    ${LIBPNG_DIR}/*.c
)

set(LIBPNG_DEPS

    ${LIBPNG_SRC}
    zlib
)

set(LIBPNG_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)
if (WIN32)
    set(LIBPNG_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/libpng16.lib)
    file(WRITE ${LIBPNG_BINARY_DIR}/cmake-configure.bat
    
"\"C:\\Program Files\\Git\\bin\\bash.exe\" -c ^
\"'${CMAKE_COMMAND}' '${LIBPNG_DIR}' -DCMAKE_INSTALL_PREFIX='${DEPENDENCIES_INSTALL_DIR}' -DCMAKE_SYSTEM_PREFIX='${DEPENDENCIES_INSTALL_DIR}'\"

\"C:\\Program Files\\Git\\bin\\bash.exe\" -c ^
\"'${CMAKE_COMMAND}' --build . --config Release\"

\"C:\\Program Files\\Git\\bin\\bash.exe\" -c ^
\"'${CMAKE_COMMAND}'  --install .\"
"
    )
    set(LIBPNG_CMAKE_GENERATE_COMMAND
        ${LIBPNG_BINARY_DIR}/cmake-configure.bat 
    )

    add_custom_command(OUTPUT ${LIBPNG_LIBS}
        PRE_BUILD

        COMMENT "Compiling libpng"

        DEPENDS ${LIBPNG_DEPS}

        WORKING_DIRECTORY ${LIBPNG_BINARY_DIR}
        COMMAND ${LIBPNG_CMAKE_GENERATE_COMMAND}
    )
else()
    set(LIBPNG_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/libpng.a)

    add_custom_command(OUTPUT ${LIBPNG_LIBS}
        PRE_BUILD

        COMMENT "Compiling libpng"

        DEPENDS ${LIBPNG_DEPS}

        WORKING_DIRECTORY ${LIBPNG_BINARY_DIR}
        COMMAND ${CMAKE_COMMAND} ${LIBPNG_DIR} 
            -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
            -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
            -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}
            -DCMAKE_MSVC_RUNTIME_LIBRARY="${CMAKE_MSVC_RUNTIME_LIBRARY}"
            -DCMAKE_SYSTEM_PREFIX=${DEPENDENCIES_INSTALL_DIR}
        COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS}
        COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
    )
endif()

add_custom_target(libpng

    DEPENDS ${LIBPNG_LIBS}
)
