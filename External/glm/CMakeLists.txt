message(STATUS "++++++++++++++++++ External-GLM" )

# -------------------------------------------------------------------------
#
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../../../Common/Lib/Ext/glm/  ${CMAKE_CURRENT_BINARY_DIR}/../../../Common/Lib/Ext/glm/)

# -------------------------------------------------------------------------
# GLM

add_library(ExtGLM INTERFACE)

target_include_directories(ExtGLEW INTERFACE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
)

target_link_libraries(ExtGLM INTERFACE
    glm::glm
)
