link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(sdk_example3)

add_dependencies(sdk_example3 dds_helper)

target_sources(sdk_example3
    PRIVATE
        sdk_example3.cpp
)

target_include_directories(sdk_example3 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/            # Source based path
    ${PROJECT_SOURCE_DIR}/examples/common/                  # Source based path
    ${COMPRESSONATOR_ROOT}/sdk/include/                     # Installer based path
)

target_link_libraries(sdk_example3
    PRIVATE
        dds_helper
        CMP_Compressonator
)

set_target_properties(sdk_example3 PROPERTIES 
    FOLDER ${FOLDER_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
    )
