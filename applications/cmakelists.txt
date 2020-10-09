
set(FOLDER_NAME Applications)

add_subdirectory(compressonatorcli)

#enable when code is clean for Linux support
if (OPTION_BUILD_GUI)
add_subdirectory(compressonatorgui)
endif()
