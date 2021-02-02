message(STATUS "(3:D)======================== opencv debug" )

include(${CMAKE_CURRENT_LIST_DIR}/../opencv-clone-targets.cmake)
opencv_clone_targets("Debug")
set(OpenCV_DEBUG_LIBRARIES ${OPENCV_DEBUG_LIBRARIES} PARENT_SCOPE)
set(OpenCV_DEBUG_INCLUDE_DIRS ${OPENCV_DEBUG_INCLUDE_DIRS} PARENT_SCOPE)
