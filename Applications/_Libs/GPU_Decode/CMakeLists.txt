
add_library(GPU_Decode)

target_sources(GPU_Decode
PRIVATE
    GPU_Decode.cpp
    GPU_Decode.h
    GPU_DecodeBase.cpp
    GPU_DecodeBase.h
)

target_include_directories(GPU_Decode
PUBLIC
    ./
     ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib
     ${PROJECT_SOURCE_DIR}/CMP_Framework/Common
     ${PROJECT_SOURCE_DIR}/CMP_Framework/Common/half
)

target_link_libraries(GPU_Decode
PRIVATE
    CompressonatorLIB
    Plugin_PluginManager
)

set_target_properties(GPU_Decode PROPERTIES FOLDER ${FOLDER_NAME})
