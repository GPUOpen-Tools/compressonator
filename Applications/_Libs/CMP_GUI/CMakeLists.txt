
add_library(CMP_GUI INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../CompressonatorGUI)

target_sources(CMP_GUI 
	INTERFACE
		${LEGACY_LOCATION}/Common/cvmatandqimage.cpp
		${LEGACY_LOCATION}/Common/cvmatandqimage.h
		${LEGACY_LOCATION}/Components/cpImageLoader.h
		${LEGACY_LOCATION}/Components/cpImageLoader.cpp
)

target_include_directories(CMP_GUI 
	INTERFACE
		./Common
		./Components
		./
		${LEGACY_LOCATION}/Common
		${LEGACY_LOCATION}/Components
		${LEGACY_LOCATION}
)

target_link_libraries(CMP_GUI 
	INTERFACE
		ExtOpenCV
		ExtOpenEXR
		ExtQt5
		ExtQt5Widgets
		Plugin_CmdLine
		Plugin_Common_cExr
		Plugin_TCPluginAPI
)

#set_target_properties(CMP_GUI PROPERTIES FOLDER ${FOLDER_NAME})
