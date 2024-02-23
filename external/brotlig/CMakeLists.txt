cmake_minimum_required(VERSION 3.10)

message(STATUS "++++++++++++++++++ BROTLIG")

set(BROTLIG_ROOT_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/brotlig)

# Get dependencies of Brotli-G SDK if they exist
if (EXISTS ${BROTLIG_ROOT_PATH}/fetch_common.bat)
    execute_process(
        COMMAND python ${BROTLIG_ROOT_PATH}/fetch_dependencies.py
        WORKING_DIRECTORY ${BROTLIG_ROOT_PATH}
    )
endif()

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
    ${BROTLIG_ROOT_PATH}/build/external/cmake/customtargets/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/brotli.lib
)

add_dependencies(ExtBrotlig extern_brotlig)