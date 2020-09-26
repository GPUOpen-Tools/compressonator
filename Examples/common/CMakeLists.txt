add_library(DDS_Helper STATIC)

target_sources(DDS_Helper
    PRIVATE
        DDS_Helpers.cpp
        DDS_Helpers.h
)

target_link_libraries(DDS_Helper
    PUBLIC
        CompressonatorLIB
)