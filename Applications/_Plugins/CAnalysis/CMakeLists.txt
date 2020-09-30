

add_library(Plugin_CAnalysis)

target_sources(Plugin_CAnalysis PRIVATE

    Analysis/CAnalysis.cpp
    Analysis/CAnalysis.h
) 

target_include_directories(Plugin_CAnalysis PUBLIC
    ./Analysis
    ${PROJECT_SOURCE_DIR}/CMP_CompressonatorLib
    ${PROJECT_SOURCE_DIR}/CMP_Framework/Common
    ${PROJECT_SOURCE_DIR}/CMP_Framework/Common/half
)

target_link_libraries(Plugin_CAnalysis 

    PRIVATE
        CMP_GUI
        CompressonatorLIB
        Plugin_TCPluginAPI
        Plugin_PluginManager
        Plugin_Common_ATIFormats
        GPU_Decode
        Plugin_PSNR
        ExtOpenCV
        ExtOpenEXR
        ExtRapidXML

    PUBLIC
        Plugin_Common_TestReport
)

set_target_properties(Plugin_CAnalysis PROPERTIES FOLDER ${FOLDER_NAME})
