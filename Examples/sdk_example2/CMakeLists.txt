add_executable(SDKExample2)

target_sources(SDK_Example2
    PRIVATE
        SDK_Example2.cpp
)

target_link_libraries(SDK_Example2
    PRIVATE
        DDS_Helper
)