add_library(dds_helper STATIC)

target_sources(dds_helper
    PRIVATE
    dds_helpers.cpp
    dds_helpers.h
)

target_include_directories(dds_helper PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/            # source based path
    ${PROJECT_SOURCE_DIR}/examples/common/                  # source based path
    ${COMPRESSONATOR_ROOT}/sdk/include/                     # installer based path
)

target_link_libraries(dds_helper
    PUBLIC
    CMP_Compressonator
)

set_target_properties(dds_helper PROPERTIES FOLDER ${FOLDER_NAME})


