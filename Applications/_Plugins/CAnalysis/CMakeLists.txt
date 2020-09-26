

add_library(Plugin_CAnalysis)

target_sources(Plugin_CAnalysis PRIVATE

    Analysis/CAnalysis.cpp
    Analysis/CAnalysis.h
)

target_include_directories(Plugin_CAnalysis PUBLIC

    ./Analysis
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
		ExtRapidXML
	
	PUBLIC
		Plugin_Common_TestReport
)

set_target_properties(Plugin_CAnalysis PROPERTIES FOLDER ${FOLDER_NAME})
