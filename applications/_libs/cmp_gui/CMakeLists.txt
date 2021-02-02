
add_library(CMP_GUI INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../../compressonatorgui)

target_sources(CMP_GUI 
    INTERFACE
        ${LEGACY_LOCATION}/common/cvmatandqimage.cpp
        ${LEGACY_LOCATION}/common/cvmatandqimage.h
        ${LEGACY_LOCATION}/components/cpimageloader.h
        ${LEGACY_LOCATION}/components/cpimageloader.cpp
)

target_include_directories(CMP_GUI 
    INTERFACE
        ./common
        ./components
        ./
        ${LEGACY_LOCATION}/common
        ${LEGACY_LOCATION}/components
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
