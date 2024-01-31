cmake_minimum_required(VERSION 3.13)

message(STATUS "++++++++++++++++++ External GLFW" )

# Setting default values for GLFW variables so that we build what we need
SET(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
SET(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build the GLFW example programs" FORCE)
SET(GLFW_BUILD_TESTS OFF CACHE BOOL "Build the GLFW test programs" FORCE)
SET(GLFW_BUILD_DOCS OFF CACHE BOOL "Build the GLFW documentation" FORCE)
SET(GLFW_INSTALL OFF CACHE BOOL "Generate installation target" FORCE)

ExternalProject_Add(extern_glfw
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw
    BINARY_DIR ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/build
    INSTALL_COMMAND cmake -E echo "Skipping install step."
)

set_target_properties(extern_glfw PROPERTIES FOLDER ${PROJECT_FOLDER_SDK_EXTERNAL})

add_library(ExtGLFW INTERFACE)

set(GLFW_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/include)

target_include_directories(ExtGLFW INTERFACE ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/include)

target_link_libraries(ExtGLFW INTERFACE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/build/src/$<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>/glfw3.lib
)