link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(framework_example2)

target_sources(framework_example2
    PRIVATE
        Framework_Example2.cpp
)

target_include_directories(framework_example2 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/CMP_Framework/                    # Source based path
    ${COMPRESSONATOR_ROOT}/SDK/include/                     # Installer based path
)

target_link_libraries(framework_example2
    PRIVATE
        CMP_Framework
)

set_target_properties(framework_example2 PROPERTIES FOLDER ${FOLDER_NAME})

