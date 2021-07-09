add_executable(core_example1)

target_sources(core_example1
    PRIVATE
        coreexample.cpp
)

target_include_directories(core_example1 PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_core/source
    )

target_link_libraries(core_example1
    PRIVATE
        CMP_Core
)

set_target_properties(core_example1 PROPERTIES 
    FOLDER ${FOLDER_NAME}
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
    )

