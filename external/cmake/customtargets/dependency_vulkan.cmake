# -------------------------------------------------------------------------
# Vulkan
#
# Defines:
# - VULKAN_SDK
# - Vulkan_LIBRARIES
# - Vulkan_INCLUDE_DIRS
# - Vulkan_LIBRARY_DIRS
#

if (NOT DEFINED DEPENDENCY_VULKAN_INCLUDE)
set(DEPENDENCY_VULKAN_INCLUDE ON)

	include(${COMPRESSONATOR_PROJECT_DIR}/external/vulkan/CMakeLists.txt)

	foreach(_lib ${Vulkan_LIBRARIES})
		get_filename_component(_lib_dir ${_lib} DIRECTORY)
		list(APPEND Vulkan_LIBRARY_DIRS ${_lib_dir})
	endforeach()

	list(REMOVE_DUPLICATES Vulkan_LIBRARY_DIRS)

endif()
