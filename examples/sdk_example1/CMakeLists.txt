link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
)

add_executable(sdk_example1)

add_dependencies(sdk_example1 dds_helper)

target_sources(sdk_example1
    PRIVATE
    sdk_example1.cpp
)

target_include_directories(sdk_example1 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/            # source based path
    ${PROJECT_SOURCE_DIR}/examples/common/                  # source based path
    ${COMPRESSONATOR_ROOT}/sdk/include/                     # installer based path
)


target_link_libraries(sdk_example1
    PRIVATE
    dds_helper
    CMP_Compressonator
)

set_target_properties(sdk_example1 PROPERTIES 
    FOLDER ${FOLDER_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
)
