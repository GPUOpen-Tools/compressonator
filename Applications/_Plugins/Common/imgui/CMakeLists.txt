# =======================================================================

add_library(Plugin_Common_Imgui INTERFACE)

# This is the current location of these files. They should be moved here
set(LEGACY_LOCATION ../)

target_sources(Plugin_Common_Imgui INTERFACE

    ${LEGACY_LOCATION}/imgui/imconfig.h
    ${LEGACY_LOCATION}/imgui/imgui.cpp
    ${LEGACY_LOCATION}/imgui/imgui.h
    ${LEGACY_LOCATION}/imgui/imgui_demo.cpp
    ${LEGACY_LOCATION}/imgui/imgui_draw.cpp
    ${LEGACY_LOCATION}/imgui/imgui_internal.h
    ${LEGACY_LOCATION}/imgui/stb_rect_pack.h
    ${LEGACY_LOCATION}/imgui/stb_textedit.h
    ${LEGACY_LOCATION}/imgui/stb_truetype.h
)

target_include_directories(Plugin_Common_Imgui INTERFACE
    ./
    ./imgui
    ${LEGACY_LOCATION}
)

#set_target_properties(Plugin_Common_Imgui PROPERTIES FOLDER ${FOLDER_NAME})
