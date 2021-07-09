cmake_minimum_required(VERSION 3.13)

message(STATUS "++++++++++++++++++ External GLFW" )

# if(POLICY CMP0091)
#   cmake_policy(SET CMP0091 NEW)
# endif()

#project(CompressonatorGLFW)
add_library(ExtGLFW INTERFACE)

set(ExtGLFW_BIN_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/glfw-3.3.2.bin.WIN64)
set(GLFW_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/glfw-3.3.2.bin.WIN64/include)

target_link_libraries(ExtGLFW INTERFACE ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/glfw-3.3.2.bin.WIN64/lib-vc2017/glfw3.lib)
target_include_directories(ExtGLFW INTERFACE ${PROJECT_SOURCE_DIR}/../common/lib/ext/glfw/glfw-3.3.2.bin.WIN64/include)
