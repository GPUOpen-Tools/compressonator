
add_library(Plugin_CImage_KTX)

add_dependencies(Plugin_CImage_KTX extern_ktx)

target_sources(Plugin_CImage_KTX PRIVATE
    KTX.cpp
    cKTX.h
    softfloat.cpp
    softfloat.h
)

target_include_directories(Plugin_CImage_KTX PRIVATE
    ./
    ${VULKAN_SDK_PATH}/include/
)

target_link_libraries(Plugin_CImage_KTX PRIVATE

    CompressonatorLIB
    CMP_FrameworkLIB
    GPU_Decode
    Plugin_TCPluginAPI
    ExtGLEW
    ExtOpenGL 
    ExtKTX
)

if (CMP_HOST_APPLE)
    target_compile_definitions(Plugin_CImage_KTX PUBLIC KTX_USE_GETPROC=1)
endif()

set_target_properties(Plugin_CImage_KTX PROPERTIES FOLDER ${FOLDER_NAME})
