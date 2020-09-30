add_library(dds_helper STATIC)

target_sources(dds_helper
    PRIVATE
        DDS_Helpers.cpp
        DDS_Helpers.h
)

target_include_directories(dds_helper PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib/            # Source based path
    ${PROJECT_SOURCE_DIR}/Examples/common/                  # Source based path
    ${COMPRESSONATOR_ROOT}/SDK/include/                     # Installer based path
)

target_link_libraries(dds_helper
    PUBLIC
        CompressonatorLIB
)

set_target_properties(dds_helper PROPERTIES FOLDER ${FOLDER_NAME})


