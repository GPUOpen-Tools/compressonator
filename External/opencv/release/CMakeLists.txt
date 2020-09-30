message(STATUS "(3:R)======================== opencv release" )

include(${CMAKE_CURRENT_LIST_DIR}/../opencv-clone-targets.cmake)
opencv_clone_targets("Release")
set(OpenCV_RELEASE_LIBRARIES ${OPENCV_RELEASE_LIBRARIES} PARENT_SCOPE)
set(OpenV_RELEASE_INCLUDE_DIRS ${OPENCV_RELEASE_INCLUDE_DIRS} PARENT_SCOPE)
