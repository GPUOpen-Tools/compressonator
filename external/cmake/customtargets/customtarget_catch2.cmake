# Externals CMakeLists.txt

# ====================================================
#                     C A T C H 2
# ====================================================
set(CATCH2_VERSION 0e9bae1c)
get_filename_component(CATCH2_DIR ${COMPRESSONATOR_PROJECT_DIR}/External/catch2/catch2-submodule ABSOLUTE)

set(CATCH2_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/catch2-${CATCH2_VERSION})
file(MAKE_DIRECTORY ${CATCH2_BINARY_DIR})


file(GLOB_RECURSE CATCH2_SRC

    ${CATCH2_DIR}/*.cpp
    ${CATCH2_DIR}/*.h
    ${CATCH2_DIR}/*.hpp
)

set(CATCH2_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)

set(CATCH2_OUTPUTS ${DEPENDENCIES_INSTALL_DIR}/include/catch2/catch.hpp) 

add_custom_command(OUTPUT ${CATCH2_OUTPUTS}
    PRE_BUILD

    COMMENT "Compiling Catch2"

    DEPENDS ${CATCH2_SRC}

    WORKING_DIRECTORY ${CATCH2_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} ${CATCH2_DIR} 
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DCMAKE_BUILD_TYPE:STRING="${CMAKE_BUILD_TYPE}"
        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX:STRING=${DEPENDENCIES_INSTALL_DIR}
        -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING="${CMAKE_MSVC_RUNTIME_LIBRARY}"
        -DBUILD_TESTING:BOOL=OFF
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS}
    COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
)

add_custom_target(catch2

    DEPENDS ${CATCH2_OUTPUTS}
)
