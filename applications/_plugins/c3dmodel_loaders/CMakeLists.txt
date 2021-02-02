
add_library(Plugin_C3DModel_Loaders INTERFACE)

if (CMP_HOST_WINDOWS)
    add_subdirectory(drc)
    add_subdirectory(gltf_20)
    add_subdirectory(obj)

    target_link_libraries(Plugin_C3DModel_Loaders INTERFACE
    
        Plugin_C3DModel_Loaders_drc
        Plugin_C3DModel_Loaders_glTF_20
        Plugin_C3DModel_Loaders_obj
    )

    target_compile_definitions(Plugin_C3DModel_Loaders INTERFACE PLUGIN_MODEL_LOADERS=1)
endif()
