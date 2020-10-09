# Externals CMakeLists.txt

# ====================================================
#                     G L E W
# ====================================================
set(GLEW_VERSION 2.1.0)
get_filename_component(GLEW_DIR ${COMPRESSONATOR_PROJECT_DIR}/external/glew/glew-submodule ABSOLUTE)

set(GLEW_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/glew-${GLEW_VERSION})
file(MAKE_DIRECTORY ${GLEW_BINARY_DIR})

file(GLOB_RECURSE GLEW_SRC

    ${GLEW_DIR}/auto/src/*.c
    ${GLEW_DIR}/auto/src/*.h
)

set(GLEW_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)

if (WIN32)
    set(GLEW_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/libglew32.lib)
else()
    set(GLEW_LIBS ${DEPENDENCIES_INSTALL_DIR}/lib/libGLEW.a)
endif()

add_custom_command(OUTPUT ${GLEW_LIBS}
    PRE_BUILD

    COMMENT "Compiling Glew"

    DEPENDS ${GLEW_SRC}

    WORKING_DIRECTORY ${GLEW_BINARY_DIR}

    COMMAND ${CMAKE_COMMAND} ${GLEW_DIR}/build/cmake 
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX="${DEPENDENCIES_INSTALL_DIR}"
        -DCMAKE_MSVC_RUNTIME_LIBRARY="${CMAKE_MSVC_RUNTIME_LIBRARY}"
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS}
    COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
)

add_custom_target(glew

    DEPENDS ${GLEW_LIBS}
)

add_library(ExtGlew INTERFACE)

add_dependencies(ExtGlew
    glew
)

target_link_libraries(ExtGlew INTERFACE
    ${GLEW_LIBS}
)

target_include_directories(ExtGlew INTERFACE
    ${GLEW_INCLUDE_DIR}
)

