cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 17)
project(ImFileDialogExample)

# source code
set(SOURCES
	example.cpp
	ImFileDialog.cpp

	libs/imgui/imgui.cpp
	libs/imgui/imgui_widgets.cpp
	libs/imgui/imgui_draw.cpp
	libs/imgui/examples/imgui_impl_opengl3.cpp
	libs/imgui/examples/imgui_impl_sdl.cpp
)

# cmake toolchain
if(CMAKE_TOOLCHAIN_FILE)
    include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

# pkg
if (UNIX)
  find_package(PkgConfig REQUIRED)
endif()

# sdl2
if(WIN32 OR APPLE OR NOT ${USE_PKG_CHECK_SDL})
  find_package(SDL2 REQUIRED)
else()
  pkg_check_modules(SDL2 REQUIRED sdl2)
endif()

# opengl
set(OpenGL_GL_PREFERENCE GLVND CACHE STRING "Linux only: if GLVND, use the vendor-neutral GL libraries (default). If LEGACY, use the legacy ones (might be necessary to have working optirun/primusrun)")
set_property(CACHE OpenGL_GL_PREFERENCE PROPERTY STRINGS GLVND LEGACY)
find_package(OpenGL REQUIRED)

# glew
find_package(GLEW REQUIRED)

# create executable
add_executable(ImFileDialogExample ${SOURCES})

# properties
set_target_properties(ImFileDialogExample PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED YES
)

# include directories
target_include_directories(ImFileDialogExample PRIVATE ${SDL2_INCLUDE_DIRS} ${GLEW_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIRS})
target_include_directories(ImFileDialogExample PRIVATE libs/)

# link libraries
target_link_libraries(ImFileDialogExample ${OPENGL_LIBRARIES})

if(WIN32)
	# link specific win32 libraries
	target_link_libraries(ImFileDialogExample GLEW::GLEW SDL2::SDL2 SDL2::SDL2main)

elseif(UNIX AND NOT APPLE)
	# link linux libraries
	target_link_libraries(ImFileDialogExample ${GLEW_LIBRARIES} ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS} stdc++fs pthread)
elseif(APPLE)
	target_link_libraries(ImFileDialogExample GLEW::GLEW ${SDL2_LIBRARIES} ${CMAKE_DL_LIBS})
endif()