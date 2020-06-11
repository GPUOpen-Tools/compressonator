# =======================================================================

add_library(Plugin_Common_QtImgui_DX12 INTERFACE)

target_sources(Plugin_Common_QtImgui_DX12 INTERFACE

    ImGUI_DX12.h
    ImGui_DX12Renderer.cpp
    ImGui_DX12Renderer.h
)

target_link_libraries(Plugin_Common_QtImgui_DX12 INTERFACE
    ExtQt5
)

target_include_directories(Plugin_Common_QtImgui_DX12 INTERFACE
    ./
)

#set_target_properties(Plugin_Common_QtImgui PROPERTIES FOLDER ${FOLDER_NAME})

# =======================================================================

add_library(Plugin_Common_QtImgui_OpenGL INTERFACE)

target_sources(Plugin_Common_QtImgui_OpenGL INTERFACE

    ImGUI_OpenGL.h
    ImGui_OpenGLRenderer.cpp
    ImGui_OpenGLRenderer.h
    # demowindow.cpp
    # demowindow.h
)

target_include_directories(Plugin_Common_QtImgui_OpenGL INTERFACE
    ./
)

#set_target_properties(Plugin_Common_QtImgui PROPERTIES FOLDER ${FOLDER_NAME})

# =======================================================================

add_library(Plugin_Common_QtImgui INTERFACE)

target_link_libraries(Plugin_Common_QtImgui INTERFACE
    Plugin_Common_QtImgui_DX12
    Plugin_Common_QtImgui_OpenGL
)

#set_target_properties(Plugin_Common_QtImgui PROPERTIES FOLDER ${FOLDER_NAME})
