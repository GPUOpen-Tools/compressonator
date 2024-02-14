add_library(extern_imgui)

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

    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_vulkan.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_vulkan.cpp
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_glfw.h
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_glfw.cpp
)

if (CMP_HOST_WINDOWS)
    set(IMGUI_WINDOWS_SRCS
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx11.h
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx11.cpp
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx12.h
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_dx12.cpp
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_win32.h
        ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui/backends/imgui_impl_win32.cpp
    )
else()
    set(IMGUI_WINDOWS_SRCS "")
endif()

target_sources(extern_imgui PRIVATE
    ${IMGUI_SRCS}
    ${IMGUI_WINDOWS_SRCS}
)

target_include_directories(extern_imgui
    PRIVATE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui
    # these are required for the current backends enabled in CMP
    ${GLFW_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}
)

set_target_properties(extern_imgui PROPERTIES 
    FOLDER ${PROJECT_FOLDER_APPS_EXTERNAL}
)

# =======================================================================
