cmake_minimum_required(VERSION 3.13)

message(STATUS "++++++++++++++++++ External-GLEW" )

# if(POLICY CMP0091)
#   cmake_policy(SET CMP0091 NEW)
# endif()

#project(CompressonatorGLEW)
add_library(ExtGLEW INTERFACE)


set(ExtGLEW_BIN_PATH ${PROJECT_SOURCE_DIR}/../common/lib/ext/glew/1.9.0/bin/x64)
#set_property(GLOBAL PROPERTY ExtGLEW_BIN_PATH "${ExtGLEW_BIN_PATH}")
#list(APPEND CMAKE_FIND_ROOT_PATH ${ExtGLEW_BIN_PATH})

target_link_libraries(ExtGLEW INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/glew/1.9.0/lib/x64/glew32.lib)
target_include_directories(ExtGLEW INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/glew/1.9.0/include
)
