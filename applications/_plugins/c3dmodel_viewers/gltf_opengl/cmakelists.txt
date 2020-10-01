
add_library(Plugin_C3DModel_Viewers_glTF_OpenGL)


file(GLOB_RECURSE SRCS

    *.cpp
    *.h
)

target_sources(Plugin_C3DModel_Viewers_glTF_OpenGL PRIVATE

    ${SRCS}
)

target_include_directories(Plugin_C3DModel_Viewers_glTF_OpenGL PUBLIC

    ./
)

target_link_libraries(Plugin_C3DModel_Viewers_glTF_OpenGL PRIVATE

    CompressonatorLIB
    CMP_Math

    ExtOpenGL
    ExtQt5OpenGL

    Plugin_GLTF
    Plugin_UserInterface
    Plugin_Common_Imgui
    Plugin_Common_QtImgui_OpenGL
    Plugin_TCPluginAPI
    Plugin_Model_Viewer
    Plugin_CmdLine
)

set_target_properties(Plugin_C3DModel_Viewers_glTF_OpenGL PROPERTIES 
    AUTOMOC ON
    FOLDER ${FOLDER_NAME}
    )
