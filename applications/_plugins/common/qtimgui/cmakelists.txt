add_library(CMP_Imgui)

file(GLOB_RECURSE IMGUI_SRCS
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/imgui/*.h
)

file(GLOB_RECURSE QTIMGUI_SRCS
)

target_sources(CMP_Imgui PRIVATE
    ${IMGUI_SRCS}
    )

target_include_directories(CMP_Imgui
    PRIVATE
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    )

target_link_libraries(CMP_Imgui 
    PRIVATE
    ExtQt5
)

set_target_properties(CMP_Imgui PROPERTIES 
    FOLDER "Libs"
    )
# =======================================================================

