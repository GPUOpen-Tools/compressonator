message(STATUS "++++++++++++++++++ External-GLM" )

# -------------------------------------------------------------------------
#
add_subdirectory(${PROJECT_SOURCE_DIR}/../common/lib/ext/glm ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm)

# -------------------------------------------------------------------------
# GLM

add_library(ExtGLM INTERFACE)

target_include_directories(ExtGLEW INTERFACE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
)

target_link_libraries(ExtGLM INTERFACE
    glm::glm
)
