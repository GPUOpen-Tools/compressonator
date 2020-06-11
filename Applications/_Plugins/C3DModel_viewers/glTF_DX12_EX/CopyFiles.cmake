
macro(gltf_dx12_copy_files EXECUTABLE_NAME PLUGINS_PATH)

    set(CURRENT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/Applications/_Plugins/C3DModel_viewers/glTF_DX12_EX)

    file(GLOB_RECURSE SHADERS ${CURRENT_SOURCE_DIR}/DX12Util/shaders/*)
    foreach(shader ${SHADERS})
        file(RELATIVE_PATH asset ${CURRENT_SOURCE_DIR}/DX12Util ${shader})
        cmp_copy_to_output(${EXECUTABLE_NAME} ${shader} ${PLUGINS_PATH}/${asset})
    endforeach()

    file(GLOB_RECURSE MEDIA ${CURRENT_SOURCE_DIR}/media/*)
    foreach(media ${MEDIA})
        file(RELATIVE_PATH asset ${CURRENT_SOURCE_DIR} ${media})
        cmp_copy_to_output(${EXECUTABLE_NAME} ${media} ${PLUGINS_PATH}/${asset})
    endforeach()

endmacro()
