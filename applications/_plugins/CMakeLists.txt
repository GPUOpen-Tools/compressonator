# =======================================================================

set(FOLDER_NAME _Plugins_static)

add_subdirectory(c3dmodel_loaders)
add_subdirectory(ccmp_encode)
add_subdirectory(ccmp_sdk)
add_subdirectory(cgpudecode)
add_subdirectory(cimage)
add_subdirectory(cmesh)
add_subdirectory(common)

if (OPTION_BUILD_GUI) 
    add_subdirectory(c3dmodel_viewers)
endif()

set(FOLDER_NAME _Plugins_dynamic)

add_subdirectory(cmp_gpu)

if (OPTION_CMP_DIRECTX)
add_subdirectory(cfilter)
endif()

if (OPTION_BUILD_APPS_CMP_CAS)
add_subdirectory(cfilter_fx)
endif()


# =======================================================================
