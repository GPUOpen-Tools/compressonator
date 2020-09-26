# -------------------------------------------------------------------------
#
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/glm-submodule ${CMAKE_CURRENT_BINARY_DIR}/glm-submodule)

# -------------------------------------------------------------------------
# GLM

add_library(ExtGLM INTERFACE)

target_link_libraries(ExtGLM INTERFACE

    glm::glm
)
