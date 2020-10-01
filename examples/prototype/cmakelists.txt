link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(cmp_prototype)

target_sources(cmp_prototype
    PRIVATE
        cmp_prototype.cpp
    ${PROJECT_SOURCE_DIR}/External/glad/include/glad/glad.h
    ${PROJECT_SOURCE_DIR}/External/glad/src/glad.c
)
target_include_directories(cmp_prototype PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/CMP_Framework             # compute_base.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/shaders          # common_def.h
    ${PROJECT_SOURCE_DIR}/CMP_Core/source           # cmp_math_vec4.h
    ${PROJECT_SOURCE_DIR}/Applications/_Libs/CMP_Math #cmp_math_common.h
    ${PROJECT_SOURCE_DIR}/Applications/_Plugins/Common  # STB_Image
    ${PROJECT_SOURCE_DIR}/External/glad/include/
    ${PROJECT_SOURCE_DIR}/External/glad/include/glad
)

target_link_libraries(cmp_prototype
    PRIVATE
    ExtGLM 
    ExtGLFW
    CMP_Framework
    CompressonatorLIB
)

set_target_properties(cmp_prototype 
    PROPERTIES FOLDER ${FOLDER_NAME}
    )

