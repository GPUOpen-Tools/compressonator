add_executable(core_example1)

target_sources(core_example1
    PRIVATE
        CoreExample.cpp
)

target_link_libraries(core_example1
    PRIVATE
        CMP_Core
)

set_target_properties(core_example1 PROPERTIES FOLDER ${FOLDER_NAME})

