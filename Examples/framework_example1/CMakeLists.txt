
add_executable(Framework_Example1)

target_sources(Framework_Example1
    PRIVATE
        Framework_Example1.cpp
)

target_link_libraries(Framework_Example1
    PRIVATE
        CMP_Framework 
)
