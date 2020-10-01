
macro(gltf_vulkan_copy_files EXECUTABLE_NAME PLUGINS_PATH)

    set(CURRENT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Applications/_Plugins/C3DModel_viewers/Vulkan)

    file(GLOB_RECURSE SHADERS ${CURRENT_SOURCE_DIR}/Util/shaders/*)
    
    foreach(shader ${SHADERS})
        file(RELATIVE_PATH asset ${CURRENT_SOURCE_DIR}/Util ${shader})
        cmp_copy_to_output(${EXECUTABLE_NAME} ${shader} ${PLUGINS_PATH}/${asset})
    endforeach()
endmacro()
