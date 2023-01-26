add_library(CMP_Imgui)

set(IMGUI_SRCS
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imconfig.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui_demo.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui_draw.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui_internal.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui_tables.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imgui_widgets.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imstb_rectpack.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imstb_textedit.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/imstb_truetype.h

    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx11.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx11.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx12.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx12.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_win32.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_win32.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_vulkan.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_vulkan.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_glfw.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_glfw.cpp

)

target_sources(CMP_Imgui PRIVATE
    ${IMGUI_SRCS}
    )

target_include_directories(CMP_Imgui
    PRIVATE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui
    # these are required for the current backends enabled in CMP
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
    )

target_link_libraries(CMP_Imgui PRIVATE ExtGLFW)

set_target_properties(CMP_Imgui PROPERTIES 
    FOLDER "Libs"
    )

# =======================================================================
