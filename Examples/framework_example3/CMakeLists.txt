add_executable(FrameworkExample3)

target_sources(Framework_Example3
    PRIVATE
        Framework_Example3.cpp
)

target_link_libraries(Framework_Example1
    PRIVATE
        CMP_Framework 
)