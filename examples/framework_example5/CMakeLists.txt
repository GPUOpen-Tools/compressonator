link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
)

add_executable(framework_example5)

target_sources(framework_example5
    PRIVATE
    framework_example5.cpp
)

target_include_directories(framework_example5 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/
    ${PROJECT_SOURCE_DIR}/cmp_framework/                    # Source based path
    ${COMPRESSONATOR_ROOT}/sdk/include/                     # Installer based path
)

target_link_libraries(framework_example5
    PRIVATE
    CMP_Framework
)

set_target_properties(framework_example5
    PROPERTIES FOLDER ${FOLDER_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
)

