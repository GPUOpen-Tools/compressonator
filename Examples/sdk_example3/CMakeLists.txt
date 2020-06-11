add_executable(SDKExample3)

target_sources(SDK_Example3
    PRIVATE
        SDK_Example3.cpp
)

target_link_libraries(SDK_Example3
    PRIVATE
        DDS_Helper
)
