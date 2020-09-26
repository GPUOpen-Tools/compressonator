add_executable(Framework_Example2)

target_sources(Framework_Example2
    PRIVATE
        Framework_Example2.cpp
)

target_link_libraries(Framework_Example2
    PRIVATE
        CMP_Framework 
)
