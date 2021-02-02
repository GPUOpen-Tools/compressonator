# =======================================================================

set(FOLDER_NAME _Plugins_static)

add_subdirectory(atiformats)
add_subdirectory(c3dmodel_loaders)
add_subdirectory(canalysis)
add_subdirectory(ccmp_encode)
add_subdirectory(ccmp_sdk)
add_subdirectory(cgpudecode)
add_subdirectory(cimage)
add_subdirectory(cmdline)
add_subdirectory(cmesh)
add_subdirectory(common)
add_subdirectory(gltf)
add_subdirectory(half)
add_subdirectory(json)
add_subdirectory(pluginmanager)
add_subdirectory(psnr)
add_subdirectory(tcpluginapi)
add_subdirectory(userinterface)

if (OPTION_BUILD_GUI) 
    add_subdirectory(c3dmodel_viewers)
    add_subdirectory(modelviewer)
endif()


set(FOLDER_NAME _Plugins_dynamic)

add_subdirectory(cmp_gpu)

if (OPTION_CMP_DIRECTX)
add_subdirectory(cfilter)
add_subdirectory(cfilter_fx)
endif()

# =======================================================================
