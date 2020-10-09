# Externals CMakeLists.txt

# ====================================================
#                     O P E N C V
# ====================================================
set(OPENCV_VERSION 4.1.2)
set(OPENCV_VERSION_SUFFIX 412)
get_filename_component(OPENCV_CMP_DIR ${COMPRESSONATOR_PROJECT_DIR}/external/opencv ABSOLUTE)
get_filename_component(OPENCV_DIR ${OPENCV_CMP_DIR}/opencv-submodule ABSOLUTE)

# OpenCV has a weird install layout on windows. So we need to carefully split every 
# configuration into unique directories
get_filename_component(OPENCV_INSTALL_DIR ${DEPENDENCIES_INSTALL_DIR} ABSOLUTE)
if (WIN32)
    set(OPENCV_INSTALL_DIR "${OPENCV_INSTALL_DIR}/$<IF:$<CONFIG:Debug>,opencv-debug,opencv-release>")
endif()

set(OPENCV_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/opencv-${OPENCV_VERSION})
file(MAKE_DIRECTORY ${OPENCV_BINARY_DIR})

set(OPENCV_MODULES

    androidcamera
    calib3d
    contrib
    core
    dynamicuda
    features2d
    flann
    gpu
    highgui
    imgproc
    java
    legacy
    ml
    nonfree
    objdetect
    ocl
    photo
    python
    stitching
    superres
    ts
    video
    videostab
    viz
    world
)

foreach(module ${OPENCV_MODULES})

    file(GLOB_RECURSE _OPENCV_SRC

        ${OPENCV_DIR}/modules/${module}/src/*.cpp
        ${OPENCV_DIR}/modules/${module}/src/*.h
    )

    list(APPEND OPENCV_SRC ${_OPENCV_SRC})

endforeach()

set(OPENCV_INCLUDE_DIR ${DEPENDENCIES_INSTALL_DIR}/include)
string(TOLOWER ${CMAKE_BUILD_TYPE} OPENCV_BUILD_TYPE)
set(OPENCV_DEPENDS

    ${OPENCV_SRC}
    libtiff
    openexr
)


if (WIN32)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(OPENCV_BUILD_TYPE_SUFFIX "d")
    endif()
    set(OPENCV_OUTPUT ${DEPENDENCIES_INSTALL_DIR}/opencv-${OPENCV_BUILD_TYPE}/x64/vc16/lib/opencv_core${OPENCV_VERSION_SUFFIX}${OPENCV_BUILD_TYPE_SUFFIX}.lib)
else()
    list(APPEND OPENCV_DEPENDS
        libpng
    )
    set(OPENCV_OUTPUT

        ${DEPENDENCIES_INSTALL_DIR}/lib/libopencv_ts.a
    )
    set(OPENCV_CXX_FLAGS
        -Wno-tautological-pointer-compare
        -Wno-pointer-bool-conversion
        -Wno-error=address
    )
    set(OPENCV_EXTRA_FLAGS 
        -DCMAKE_OSX_DEPLOYMENT_TARGET=10.9
        -DCMAKE_CXX_FLAGS="${OPENCV_CXX_FLAGS}"
    )
endif()

add_custom_command(OUTPUT ${OPENCV_OUTPUT}
    PRE_BUILD

    COMMENT "Compiling OpenCV"

    DEPENDS ${OPENCV_DEPENDS}

    WORKING_DIRECTORY ${OPENCV_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} ${OPENCV_CMP_DIR}
        -DBUILD_DOCS:BOOL=OFF 
        -DBUILD_EXAMPLES:BOOL=OFF
        -DBUILD_PERF_TESTS:BOOL=OFF
        -DBUILD_SHARED_LIBS:BOOL=${BUILD_SHARED_LIBS}
        -DBUILD_TESTS:BOOL=OFF 
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
        -DCMAKE_CXX_STANDARD:STRING=${CMAKE_CXX_STANDARD}
        -DCMAKE_INSTALL_PREFIX:STRING=${OPENCV_INSTALL_DIR}
        -DCMAKE_MSVC_RUNTIME_LIBRARY:STRING=${CMAKE_MSVC_RUNTIME_LIBRARY}
        -DCMAKE_SYSTEM_PREFIX:STRING=${OPENCV_INSTALL_DIR}
        -DINSTALL_C_EXAMPLES:BOOL=OFF
        -DINSTALL_PYTHON_EXAMPLES:BOOL=OFF
        -DINSTALL_TESTS:BOOL=OFF
        -DWITH_OPENEXR:BOOL=OFF
        ${OPENCV_EXTRA_FLAGS}
    COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --parallel ${NUM_PARALLEL_JOBS}
    COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
)


add_custom_target(opencv

    DEPENDS ${OPENCV_OUTPUT}
)
