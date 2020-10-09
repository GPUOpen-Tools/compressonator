# Externals CMakeLists.txt

# ====================================================
#                      Z L I B
# ====================================================
set(ZLIB_VERSION 1.2.10)
get_filename_component(ZLIB_CMP_DIR ${COMPRESSONATOR_PROJECT_DIR}/external/zlib ABSOLUTE)
get_filename_component(ZLIB_DIR ${ZLIB_CMP_DIR}/zlib-submodule ABSOLUTE)

set(ZLIB_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/zlib-${ZLIB_VERSION})
file(MAKE_DIRECTORY ${ZLIB_BINARY_DIR})

file(GLOB_RECURSE ZLIB_SRC

    ${ZLIB_DIR}/*.cpp
)

set(ZLIB_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(ZLIB_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/zlibd.lib)
    else()
        set(ZLIB_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/zlib.lib)
    endif()
else()
    set(ZLIB_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/libz.a) 
endif()

add_custom_command(OUTPUT ${ZLIB_LIBS}
    PRE_BUILD

    COMMENT "Compiling zlib"

    DEPENDS ${ZLIB_SRC}

    WORKING_DIRECTORY ${ZLIB_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} ${ZLIB_CMP_DIR} 
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE:STRING="${CMAKE_BUILD_TYPE}"
        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX:STRING=${DEPENDENCIES_INSTALL_DIR}
        -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING="${CMAKE_MSVC_RUNTIME_LIBRARY}"
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS} --target zlib
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS} --target zlibstatic
    COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
)

add_custom_target(zlib

    DEPENDS ${ZLIB_LIBS}
)
