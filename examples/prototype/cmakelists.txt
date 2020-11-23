link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(cmp_prototype)

target_sources(cmp_prototype
    PRIVATE
        cmp_prototype.cpp
)

target_include_directories(cmp_prototype PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_framework
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders
    ${PROJECT_SOURCE_DIR}/cmp_core/source
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
    ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_math
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # stb_image
    ${PROJECT_SOURCE_DIR}/external/glad/include/
    ${PROJECT_SOURCE_DIR}/external/glad/include/glad
)

target_link_libraries(cmp_prototype
    PRIVATE
    ExtGLM 
    ExtGLFW
    CMP_Framework
    CMP_Compressonator
    d3d11.lib
)

set_target_properties(cmp_prototype 
    PROPERTIES FOLDER ${FOLDER_NAME}
    )

