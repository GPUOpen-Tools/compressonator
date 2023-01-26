cmake_minimum_required(VERSION 3.10)

message(STATUS "++++++++++++++++++ BROTLIG")

set(BROTLIG_ROOT_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/brotlig)
#set(BROTLIG_ROOT_PATH ${PROJECT_SOURCE_DIR}/external/brotlig/brotlig)

ExternalProject_Add(extern_brotlig
    SOURCE_DIR ${BROTLIG_ROOT_PATH}
    BINARY_DIR ${BROTLIG_ROOT_PATH}/build
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

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