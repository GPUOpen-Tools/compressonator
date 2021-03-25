
add_library(Plugin_C3DModel_Loaders INTERFACE)

if (CMP_HOST_WINDOWS)

if(LIB_BUILD_MESHCOMPRESSOR)
    add_subdirectory(drc)
endif()
    add_subdirectory(gltf_20)
    add_subdirectory(obj)

if(LIB_BUILD_MESHCOMPRESSOR)
    target_link_libraries(Plugin_C3DModel_Loaders INTERFACE
        Plugin_C3DModel_Loaders_drc
        Plugin_C3DModel_Loaders_glTF_20
        Plugin_C3DModel_Loaders_obj
    )
else()
    target_link_libraries(Plugin_C3DModel_Loaders INTERFACE
        Plugin_C3DModel_Loaders_glTF_20
        Plugin_C3DModel_Loaders_obj
    )
endif()

    target_compile_definitions(Plugin_C3DModel_Loaders INTERFACE PLUGIN_MODEL_LOADERS=1)
endif()
