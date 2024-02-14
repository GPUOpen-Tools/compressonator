# -----------------------------------
# Create external libraries
# -----------------------------------
add_library(ExtDirectX INTERFACE)

target_link_libraries(ExtDirectX INTERFACE
    d3d12.lib
    dxgi.lib
    d3dcompiler.lib
)

if (OPTION_BUILD_BROTLIG)
    # TODO: Change this back before the release
    set(BROTLIG_ROOT_PATH ${COMPRESSONATOR_ROOT_PATH}/compressonator_internal/external/brotlig)
    #set(BROTLIG_ROOT_PATH ${COMPRESSONATOR_ROOT_PATH}/../common/lib/ext/brotlig)

    ExternalProject_Add(extern_brotlig
        SOURCE_DIR ${BROTLIG_ROOT_PATH}
        BINARY_DIR ${BROTLIG_ROOT_PATH}/build
        INSTALL_COMMAND cmake -E echo "Skipping install step."
    )

    set_target_properties(extern_brotlig PROPERTIES FOLDER ${PROJECT_FOLDER_SDK_EXTERNAL})

    add_library(ExtBrotlig INTERFACE)

    target_include_directories(ExtBrotlig INTERFACE
        ${BROTLIG_ROOT_PATH}/inc/
        ${BROTLIG_ROOT_PATH}/external/
        ${BROTLIG_ROOT_PATH}/external/brotli/c/include/
    )

    target_link_libraries(ExtBrotlig INTERFACE
        ${BROTLIG_ROOT_PATH}/build/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/brotlig.lib
        ${BROTLIG_ROOT_PATH}/build/external/brotli/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/brotli.lib
    )

    add_dependencies(ExtBrotlig extern_brotlig)
endif()