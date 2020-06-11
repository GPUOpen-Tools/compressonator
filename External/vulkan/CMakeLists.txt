# -------------------------------------------------------------------------
#
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/vulkan-submodule ${CMAKE_CURRENT_BINARY_DIR}/submodule)

unset(VULKAN_SDK)
unset(Vulkan_LIBRARIES)
unset(Vulkan_INCLUDE_DIRS)
unset(Vulkan_LIBRARY_DIRS)

# -------------------------------------------------------------------------
# Vulkan

add_library(ExtVulkan INTERFACE)

if (CMP_HOST_APPLE)

    get_filename_component(VULKAN_SDK ${COMPRESSONATOR_PROJECT_DIR}/External/vulkan/submodule/Apple/MoltenVK ABSOLUTE)
    set(ENV{VULKAN_SDK} ${VULKAN_SDK})

    set(MOLTEN_VK_PATHS

        ${VULKAN_SDK}/macOS/framework
        #${VULKAN_SDK}/macOS/static
        #${VULKAN_SDK}/macOS/dynamic
    )

    find_library(
        MOLTEN_VK_FRAMEWORK

        NAMES MoltenVK
        PATHS ${MOLTEN_VK_PATHS}
        NO_DEFAULT_PATH
    )

    find_library(IO_KIT_FRAMEWORK		IOKit)
    find_library(IO_SURFACE_FRAMEWORK	IOSurface)
    find_library(METAL_FRAMEWORK		Metal)
    find_library(QUARTZ_CORE_FRAMEWORK	QuartzCore)

    set(
        Vulkan_LIBRARIES

        ${IO_KIT_FRAMEWORK}
        ${IO_SURFACE_FRAMEWORK}
        ${METAL_FRAMEWORK}
        ${MOLTEN_VK_FRAMEWORK}
        ${QUARTZ_CORE_FRAMEWORK}
    )

    find_path(MVK_INCLUDE_DIR
        NAMES MoltenVK/mvk_vulkan.h
        PATHS ${VULKAN_SDK}/include
    )

    set(
        Vulkan_INCLUDE_DIRS

        ${MVK_INCLUDE_DIR}
        ${MOLTEN_VK_FRAMEWORK}
    )

elseif(CMP_HOST_LINUX)

    get_filename_component(VULKAN_SDK ${COMPRESSONATOR_PROJECT_DIR}/External/vulkan/submodule/Linux/x86_64 ABSOLUTE)
    set(ENV{VULKAN_SDK}	${VULKAN_SDK})

    find_package(Vulkan REQUIRED)

elseif(CMP_HOST_WINDOWS)

    get_filename_component(VULKAN_SDK ${COMPRESSONATOR_PROJECT_DIR}/External/vulkan/submodule/Windows ABSOLUTE)
    set(ENV{VULKAN_SDK}	${VULKAN_SDK})

    find_package(Vulkan REQUIRED)

    target_compile_definitions(ExtVulkan INTERFACE VK_USE_PLATFORM_WIN32_KHR)

endif()

target_link_libraries(ExtVulkan INTERFACE ${Vulkan_LIBRARIES})
target_include_directories(ExtVulkan INTERFACE ${Vulkan_INCLUDE_DIRS})