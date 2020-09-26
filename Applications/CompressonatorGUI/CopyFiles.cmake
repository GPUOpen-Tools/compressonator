
macro(cmp_gui_copy_to_output source dest)
    cmp_copy_to_output(CompressonatorGUI-bin ${source} ${dest})
endmacro()

macro(cmp_gui_update_rpath target rpath)
    if (CMP_HOST_LINUX)
        add_custom_command(TARGET CompressonatorGUI-bin POST_BUILD
            # COMMENT "Copying ${source} -> ${dest}"
            VERBATIM
            COMMAND patchelf --set-rpath "${rpath}" ${target}
        )
    endif()
endmacro()

if (CMP_HOST_APPLE)
    # Following: https://developer.apple.com/library/archive/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html#//apple_ref/doc/uid/10000123i-CH101-SW1
    set(ASSETS_PATH $<TARGET_BUNDLE_CONTENT_DIR:CompressonatorGUI-bin>/Resources)
    set(DYLIBS_PATH $<TARGET_BUNDLE_CONTENT_DIR:CompressonatorGUI-bin>/Frameworks)
    set(PLUGINS_PATH $<TARGET_BUNDLE_CONTENT_DIR:CompressonatorGUI-bin>/PlugIns)
else()
    set(ASSETS_PATH $<TARGET_FILE_DIR:CompressonatorGUI-bin>)
    set(DYLIBS_PATH $<TARGET_FILE_DIR:CompressonatorGUI-bin>)
    set(PLUGINS_PATH $<TARGET_FILE_DIR:CompressonatorGUI-bin>/plugins)
endif()

cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/RunTime/BC7_Compression.cprj ${ASSETS_PATH}/Projects/BC7_Compression.cprj)
cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/RunTime/BC6H_Compression.cprj ${ASSETS_PATH}/Projects/BC6H_Compression.cprj)

cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/RunTime/images/Balls.exr ${ASSETS_PATH}/images/Balls.exr)
cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/RunTime/images/ruby.bmp ${ASSETS_PATH}/images/ruby.bmp)

file(GLOB_RECURSE GUI_ASSETS WelcomePage/*)
foreach(gui_asset ${GUI_ASSETS})
    file(RELATIVE_PATH asset ${CMAKE_CURRENT_LIST_DIR} ${gui_asset})
    cmp_gui_copy_to_output(${gui_asset} ${ASSETS_PATH}/${asset})
endforeach()

file(GLOB_RECURSE RESOURCES Resources/*)
foreach(resource ${RESOURCES})
    file(RELATIVE_PATH asset ${CMAKE_CURRENT_LIST_DIR}/Resources ${resource})
    cmp_gui_copy_to_output(${resource} ${ASSETS_PATH}/${asset})
endforeach()

cmp_gui_copy_to_output(${CMAKE_CURRENT_LIST_DIR}/qt.conf ${ASSETS_PATH}/qt.conf)

cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/Applications/_Plugins/CGPUDecode/Vulkan/VK_ComputeShader/texture.vert.spv ${ASSETS_PATH}/texture.vert.spv)
cmp_gui_copy_to_output(${PROJECT_SOURCE_DIR}/Applications/_Plugins/CGPUDecode/Vulkan/VK_ComputeShader/texture.frag.spv ${ASSETS_PATH}/texture.frag.spv)

# TODO Plugins/Compute
# TODO Plugins/Media
# TODO plugins/shaders

get_property(QT_LIB_DIR GLOBAL PROPERTY QT_LIB_DIR)

if (CMP_HOST_WINDOWS)
    cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/platforms/qwindows$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/platforms/qwindows$<$<CONFIG:Debug>:d>.dll)

    cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/imageformats/qtga$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/imagefomrats/qtga$<$<CONFIG:Debug>:d>.dll)
    cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/imageformats/qtiff$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/imagefomrats/qtiff$<$<CONFIG:Debug>:d>.dll)
    cmp_gui_copy_to_output(${QT_LIB_DIR}/plugins/imageformats/qjpeg$<$<CONFIG:Debug>:d>.dll ${PLUGINS_PATH}/imagefomrats/qjpeg$<$<CONFIG:Debug>:d>.dll)

    cmp_gui_copy_to_output(${QT_LIB_DIR}/bin/QtWebEngineProcess$<$<CONFIG:Debug>:d>.exe ${DYLIBS_PATH}/QtWebEngineProcess$<$<CONFIG:Debug>:d>.exe)
else ()
    # Copy the platform libraries into the bundle
    file(GLOB_RECURSE QT_PLATFORM_LIBS ${QT_LIB_DIR}/plugins/platforms/*)
    foreach(rsc ${QT_PLATFORM_LIBS})
        file(RELATIVE_PATH asset ${QT_LIB_DIR}/plugins ${rsc})
        cmp_gui_copy_to_output(${rsc} ${PLUGINS_PATH}/${asset})
        cmp_gui_update_rpath(${PLUGINS_PATH}/${asset} "\$ORIGIN/../../")
    endforeach()

    file(GLOB_RECURSE QT_IMAGEFORMAT_PLUGINS ${QT_LIB_DIR}/plugins/imageformats/*)
    foreach(rsc ${QT_IMAGEFORMAT_PLUGINS})
        file(RELATIVE_PATH asset ${QT_LIB_DIR}/plugins ${rsc})
        cmp_gui_copy_to_output(${rsc} ${PLUGINS_PATH}/${asset})
        cmp_gui_update_rpath(${PLUGINS_PATH}/${asset} "\$ORIGIN/../../")
    endforeach()

    if (CMP_HOST_LINUX)
        cmp_gui_copy_to_output(${QT_LIB_DIR}/libexec/QtWebEngineProcess ${DYLIBS_PATH}/QtWebEngineProcess)
        cmp_gui_update_rpath(${DYLIBS_PATH}/QtWebEngineProcess "\$ORIGIN")
        cmp_gui_copy_to_output(${QT_LIB_DIR}/lib/libQt5XcbQpa.so.5 ${DYLIBS_PATH}/libQt5XcbQpa.so.5)
        cmp_gui_copy_to_output(${QT_LIB_DIR}/lib/libQt5DBus.so.5 ${DYLIBS_PATH}/libQt5DBus.so.5)
    endif()
endif()

if (NOT CMP_HOST_APPLE)
    cmp_gui_copy_to_output(${QT_LIB_DIR}/translations/qtwebengine_locales/en-US.pak ${ASSETS_PATH}/translations/qtwebengine_locales/en-US.pak)
endif()

cmp_gui_copy_to_output(${QT_LIB_DIR}/translations/qtwebengine_en.qm ${ASSETS_PATH}/translations/qtwebengine_en.qm)

file(GLOB_RECURSE QT_RESOURCES ${QT_LIB_DIR}/resources/*)
foreach(rsc ${QT_RESOURCES})
    file(RELATIVE_PATH asset ${QT_LIB_DIR} ${rsc})
    cmp_gui_copy_to_output(${rsc} ${ASSETS_PATH}/${asset})
endforeach()

include(${PROJECT_SOURCE_DIR}/Applications/_Plugins/C3DModel_viewers/glTF_DX12_EX/CopyFiles.cmake)
gltf_dx12_copy_files(CompressonatorGUI-bin ${PLUGINS_PATH})

include(${PROJECT_SOURCE_DIR}/Applications/_Plugins/C3DModel_viewers/Vulkan/CopyFiles.cmake)
gltf_vulkan_copy_files(CompressonatorGUI-bin ${PLUGINS_PATH})

executable_post_build_dylibs(CompressonatorGUI-bin ${DYLIBS_PATH})
