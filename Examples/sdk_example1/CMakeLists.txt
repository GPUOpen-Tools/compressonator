add_executable(SDKExample1)

target_sources(SDK_Example1
    PRIVATE
        SDK_Example1.cpp
)

target_link_libraries(SDK_Example1
    PRIVATE
        DDS_Helper
)