add_executable(CoreExample1)

target_sources(CoreExample1
    PRIVATE
        CoreExample.cpp
)

target_link_libraries(CoreExample1
    PRIVATE
        CMP_Core
)
