# Externals CMakeLists.txt

message(STATUS "++++++++++++++++++OpenEXR Custom TargetCalled" )

set(OPENEXR_VERSION 5754217)
get_filename_component(OPENEXR_DIR ${COMPRESSONATOR_PROJECT_DIR}/external/openexr ABSOLUTE)
set(OPENEXR_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/openexr-${OPENEXR_VERSION})
file(MAKE_DIRECTORY ${OPENEXR_BINARY_DIR})

# ====================================================
#                   I L M B A S E
# ====================================================

file(GLOB_RECURSE ILMBASE_SRC

    ${OPENEXR_DIR}/IlmBase/*.h
    ${OPENEXR_DIR}/IlmBase/*.cpp
)

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(ILMBASE_LIBS    
            ${DEPENDENCIES_INSTALL_DIR}/lib/Half-2_4_d.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/Iex-2_4_d.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/IexMath-2_4_d.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/IlmThread-2_4_d.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/Imath-2_4_d.lib
        )
    else()
        set(ILMBASE_LIBS
            ${DEPENDENCIES_INSTALL_DIR}/lib/Half-2_4.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/Iex-2_4.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/IexMath-2_4.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/IlmThread-2_4.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/Imath-2_4.lib
        )
    endif()
else()
    set(ILMBASE_LIBS

        ${DEPENDENCIES_INSTALL_DIR}/lib/libHalf-2_4.a
        ${DEPENDENCIES_INSTALL_DIR}/lib/libIex-2_4.a
        ${DEPENDENCIES_INSTALL_DIR}/lib/libIexMath-2_4.a
        ${DEPENDENCIES_INSTALL_DIR}/lib/libIlmThread-2_4.a
        ${DEPENDENCIES_INSTALL_DIR}/lib/libImath-2_4.a
    )
endif()


set(ILMBASE_DEPS

    ${ILMBASE_SRC}
    zlib
)


# ====================================================
#                   O P E N E X R
# ====================================================

file(GLOB_RECURSE OPENEXR_SRC

    ${OPENEXR_DIR}/OpenEXR/*.h
    ${OPENEXR_DIR}/OpenEXR/*.cpp
)

set(OPENEXR_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)

if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OPENEXR_LIBS
            ${DEPENDENCIES_INSTALL_DIR}/lib/IlmImf-2_4_d.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/IlmImfUtil-2_4_d.lib
        )
    else()
        set(OPENEXR_LIBS
            ${DEPENDENCIES_INSTALL_DIR}/lib/IlmImf-2_4.lib
            ${DEPENDENCIES_INSTALL_DIR}/lib/IlmImfUtil-2_4.lib
        )
    endif()
else()
    set(OPENEXR_LIBS

        ${DEPENDENCIES_INSTALL_DIR}/lib/libIlmImf-2_4.a
        ${DEPENDENCIES_INSTALL_DIR}/lib/libIlmImfUtil-2_4.a
    )
endif()

# ====================================================

set(OPENEXR_DEPENDS

    ${ILMBASE_DEPS}
    ${ILMBASE_SRC}
    ${OPENEXR_SRC}
)

set(OPENEXR_OUTPUT

    ${ILMBASE_LIBS}
    ${OPENEXR_LIBS}
)

add_custom_command(OUTPUT ${OPENEXR_OUTPUT}
    PRE_BUILD

    COMMENT "Compiling OpenEXR"

    DEPENDS ${OPENEXR_DEPENDS}

    WORKING_DIRECTORY ${OPENEXR_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} ${OPENEXR_DIR}
        -DBoost_NO_BOOST_CMAKE=ON
        -DBUILD_SHARED_LIBS=${BUILD_SHARED_LIBS}
        -DBUILD_TESTING=OFF
        -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX="${DEPENDENCIES_INSTALL_DIR}"
        -DCMAKE_MSVC_RUNTIME_LIBRARY="${CMAKE_MSVC_RUNTIME_LIBRARY}"
        -DCMAKE_SYSTEM_PREFIX="${DEPENDENCIES_INSTALL_DIR}"
        -DPYILMBASE_ENABLE=OFF
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS}
    COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
)

add_custom_target(openexr

    DEPENDS ${OPENEXR_OUTPUT}
)

# ====================================================

add_library(ExtOpenEXR INTERFACE)

add_dependencies(ExtOpenEXR
    openexr
)

target_link_libraries(ExtOpenEXR INTERFACE
    ${ILMBASE_LIBS}
    ${OPENEXR_LIBS}
)

target_include_directories(ExtOpenEXR INTERFACE
    ${OPENEXR_INCLUDE_DIR}/OpenEXR
)

