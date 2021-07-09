link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(framework_example1)

target_sources(framework_example1
    PRIVATE
        framework_example1.cpp
)

target_include_directories(framework_example1 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/
    ${PROJECT_SOURCE_DIR}/cmp_framework/
    ${COMPRESSONATOR_ROOT}/sdk/include/                     # Installer based path
)

target_link_libraries(framework_example1
     PRIVATE
        CMP_Framework
)

set_target_properties(framework_example1 PROPERTIES 
    FOLDER ${FOLDER_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
)
