
macro(cmp_gui_copy_to_output source dest)
    cmp_copy_to_output(CompressonatorCLI-bin ${source} ${dest})
endmacro()

set(ASSETS_PATH $<TARGET_FILE_DIR:CompressonatorCLI-bin>)
set(DYLIBS_PATH $<TARGET_FILE_DIR:CompressonatorCLI-bin>)
set(PLUGINS_PATH $<TARGET_FILE_DIR:CompressonatorCLI-bin>/plugins)

cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/Applications/_Plugins/CGPUDecode/Vulkan/VK_ComputeShader/texture.vert.spv ${ASSETS_PATH}/texture.vert.spv)
cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/Applications/_Plugins/CGPUDecode/Vulkan/VK_ComputeShader/texture.frag.spv ${ASSETS_PATH}/texture.frag.spv)

if (OPTION_CMP_QT)
    cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/runtime/qt.conf ${ASSETS_PATH}/qt.conf)

    get_property(QT_LIB_DIR GLOBAL PROPERTY QT_LIB_DIR)

    if (CMP_HOST_WINDOWS)
        cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/platforms/qwindows$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/platforms/qwindows$<$<CONFIG:Debug>:d>.dll)
        cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/imageformats/qtga$<$<CONFIG:Debug>:d>.dll  ${PLUGINS_PATH}/imageformats/qtga$<$<CONFIG:Debug>:d>.dll)
        cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/imageformats/qtiff$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/imageformats/qtiff$<$<CONFIG:Debug>:d>.dll)
        cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/imageformats/qjpeg$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/imageformats/qjpeg$<$<CONFIG:Debug>:d>.dll)
    else ()
        # Copy the platform libraries into the bundle
        file(GLOB_RECURSE QT_PLATFORM_LIBS ${QT_LIB_DIR}/plugins/platforms/*)
        foreach(rsc ${QT_PLATFORM_LIBS})
            file(RELATIVE_PATH asset ${QT_LIB_DIR}/plugins ${rsc})
            cmp_gui_copy_to_output(${rsc} ${PLUGINS_PATH}/${asset})
        endforeach()

        file(GLOB_RECURSE QT_IMAGEFORMAT_PLUGINS ${QT_LIB_DIR}/plugins/imageformats/*)
        foreach(rsc ${QT_IMAGEFORMAT_PLUGINS})
            file(RELATIVE_PATH asset ${QT_LIB_DIR}/plugins ${rsc})
            cmp_gui_copy_to_output(${rsc} ${PLUGINS_PATH}/${asset})
        endforeach()
    endif()
endif()


if (CMP_HOST_WINDOWS)
    #copy glew32.dll
    #get_property(ExtGLEW_BIN_PATH GLOBAL PROPERTY ExtGLEW_BIN_PATH)
    cmp_gui_copy_to_output(${ExtGLEW_BIN_PATH}/glew32.dll ${ASSETS_PATH}/glew32.dll)
    
    # Old OpenCV dll provided by fetch_dependencies
    cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/../common/lib/ext/opencv/2.49/x64/VS2015/bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/opencv_core249$<$<CONFIG:Debug>:d>.dll ${ASSETS_PATH}/opencv_core249$<$<CONFIG:Debug>:d>.dll)
    cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/../common/lib/ext/opencv/2.49/x64/VS2015/bin/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/opencv_imgproc249$<$<CONFIG:Debug>:d>.dll ${ASSETS_PATH}/opencv_imgproc249$<$<CONFIG:Debug>:d>.dll)

    # New OpenCV Support
    if(DEFINED ${OPENCV_DIR})
        cmp_gui_copy_to_output(${OPENCV_DIR}build/x64/vc15/bin/opencv_world420$<$<CONFIG:Debug>:d>.dll ${ASSETS_PATH}/opencv_world420$<$<CONFIG:Debug>:d>.dll)
    endif()

    #KTX2 Features dll
    if (OPTION_BUILD_KTX2)
        cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/../common/lib/ext/ktx/build/$<$<CONFIG:Debug>:debug>$<$<CONFIG:Release>:release>/ktx.dll ${ASSETS_PATH}/ktx.dll)
    else()
        # Use a null dll so that installers can build
        cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/runtime/ktx.dll ${ASSETS_PATH}/ktx.dll)  
    endif()

endif() # end host windows


# GPU Shaders
file(GLOB_RECURSE GPUCOMPUTE_SHADERS ${PROJECT_SOURCE_DIR}/CMP_Core/shaders/*)
#message(STATUS "************** GPUCOMPUTE_SHADERS [${GPUCOMPUTE_SHADERS}]" )
foreach(rsc ${GPUCOMPUTE_SHADERS})
     file(RELATIVE_PATH asset ${PROJECT_SOURCE_DIR}/CMP_Core/shaders ${rsc})
     #message(STATUS "************** COPY SHADERS [${rsc} to ${ASSETS_PATH}/${asset}]" )
     cmp_gui_copy_to_output(${rsc} ${ASSETS_PATH}/plugins/compute/${asset})
 endforeach()

if (OPTION_BUILD_BROTLIG)
    set(BRLG_SHADER_SRC ${BROTLIG_ROOT_PATH}/src/decoder/BrotliGCompute.hlsl)
    message(STATUS "************** BROTLIG SHADER ${BRLG_SHADER_SRC}")
    cmp_gui_copy_to_output(${BRLG_SHADER_SRC} ${ASSETS_PATH}/plugins/compute/)

    set(BRLG_DX_CMP ${BROTLIG_ROOT_PATH}/sample/external/dxc_2021_12_08/bin/x64/dxcompiler.dll)
    cmp_gui_copy_to_output(${BRLG_DX_CMP} ${ASSETS_PATH}/)

    set(BRLG_DX_IL ${BROTLIG_ROOT_PATH}/sample/external/dxc_2021_12_08/bin/x64/dxil.dll)
    cmp_gui_copy_to_output(${BRLG_DX_IL} ${ASSETS_PATH}/)

endif()

executable_post_build_dylibs(CompressonatorCLI-bin ${DYLIBS_PATH})
