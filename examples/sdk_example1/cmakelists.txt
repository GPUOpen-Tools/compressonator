link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(sdk_example1)

add_dependencies(sdk_example1 dds_helper)

target_sources(sdk_example1
    PRIVATE
        SDK_Example1.cpp
)

target_include_directories(sdk_example1 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib/            # Source based path
    ${PROJECT_SOURCE_DIR}/Examples/common/                  # Source based path
    ${COMPRESSONATOR_ROOT}/SDK/include/                     # Installer based path
)


target_link_libraries(sdk_example1
    PRIVATE
        dds_helper
        CompressonatorLIB
)

set_target_properties(sdk_example1 PROPERTIES FOLDER ${FOLDER_NAME})

