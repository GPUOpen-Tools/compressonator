
add_library(ModelViewer_OpenGL SHARED)


file(GLOB_RECURSE SRCS

    *.cpp
    *.h
)

target_sources(ModelViewer_OpenGL PRIVATE
    ${SRCS}
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui/imgui_opengl.h
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui/imgui_openglrenderer.cpp
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui/imgui_openglrenderer.h

    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer/simplifier.cpp
)

target_include_directories(ModelViewer_OpenGL PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/applications/_libs/gpu_decode
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/qtimgui
    ${PROJECT_SOURCE_DIR}/external/imgui
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/imgui 
)

target_link_libraries(ModelViewer_OpenGL PRIVATE
    CMP_Compressonator
    CMP_Framework
    extern_imgui
    CMP_Common              # CModel Data
    ExtOpenGL
    ExtQt5OpenGL
)

target_compile_definitions(ModelViewer_OpenGL PRIVATE BUILD_AS_PLUGIN_DLL=1)

set_target_properties(ModelViewer_OpenGL PROPERTIES 
    AUTOMOC ON
    FOLDER ${PROJECT_FOLDER_APPS_PLUGIN_DYNAMIC}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_BINARY_DIR}/bin/debug/plugins"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/bin/release/plugins"
    )
