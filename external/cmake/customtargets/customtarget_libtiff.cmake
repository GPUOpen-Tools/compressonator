# Externals CMakeLists.txt

# ====================================================
#                    L I B T I F F
# ====================================================
set(LIBTIFF_VERSION 4.1.0)
get_filename_component(LIBTIFF_DIR ${COMPRESSONATOR_PROJECT_DIR}/external/libtiff/libtiff-submodule ABSOLUTE)

set(LIBTIFF_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/libtiff-${LIBTIFF_VERSION})
file(MAKE_DIRECTORY ${LIBTIFF_BINARY_DIR})


file(GLOB_RECURSE LIBTIFF_SRC

    ${LIBTIFF_DIR}/*.h
    ${LIBTIFF_DIR}/*.c
)

set(LIBTIFF_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)

if (WIN32)
    set(LIBTIFF_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/tiff.lib)
else()
    set(LIBTIFF_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/libtiff.a)
endif()


add_custom_command(OUTPUT ${LIBTIFF_LIBS}
    PRE_BUILD

    COMMENT "Compiling libtiff"

    DEPENDS ${LIBTIFF_SRC}

    WORKING_DIRECTORY ${LIBTIFF_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} ${LIBTIFF_DIR}
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_INSTALL_DIR}
        -DCMAKE_MSVC_RUNTIME_LIBRARY="${CMAKE_MSVC_RUNTIME_LIBRARY}"
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS}
    COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
)

add_custom_target(libtiff

    DEPENDS ${LIBTIFF_LIBS}
)
