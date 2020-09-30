message(STATUS "(2)======================== ExtOpenCV " )



set(OpenCV_STATIC (NOT BUILD_SHARED_LIBS))

add_library(ExtOpenCV INTERFACE)

if (CMP_HOST_WINDOWS)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/debug ${CMAKE_CURRENT_BINARY_DIR}/opencv-debug)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/release ${CMAKE_CURRENT_BINARY_DIR}/opencv-release)
    
    target_link_libraries(ExtOpenCV INTERFACE
        $<IF:$<CONFIG:Debug>,${OpenCV_DEBUG_LIBRARIES},${OpenCV_RELEASE_LIBRARIES}>
    )
    target_include_directories(ExtOpenCV INTERFACE
        $<IF:$<CONFIG:Debug>,${OpenCV_DEBUG_INCLUDE_DIRS},${OpenCV_RELEASE_INCLUDE_DIRS}>
    )
else()
    find_package(OpenCV REQUIRED QUIET
        PATHS ${CMAKE_FIND_ROOT_PATH}
    )
    target_link_libraries(ExtOpenCV INTERFACE ${OpenCV_LIBRARIES})
    target_include_directories(ExtOpenCV INTERFACE ${OpenCV_INCLUDE_DIRS})
endif()