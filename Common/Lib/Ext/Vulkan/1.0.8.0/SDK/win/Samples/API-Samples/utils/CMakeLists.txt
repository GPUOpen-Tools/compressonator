file(GLOB UTILS_SOURCE *.cpp)

set(SAMPLES_DATA_DIR ${SAMPLES_DATA_DIR} "${PROJECT_SOURCE_DIR}/API-Samples/data")
include_directories( ${SAMPLES_DATA_DIR} ${GLSLANG_SPIRV_INCLUDE_DIR} ${GLMINC_PREFIX} )

if(WIN32)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_CRT_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES")

    # If MSVC, disable some signed/unsigned mismatch warnings.
    if (MSVC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /wd4267")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4267")
    endif()

else()
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare")
endif()

add_library(${UTILS_NAME} STATIC ${UTILS_SOURCE})

