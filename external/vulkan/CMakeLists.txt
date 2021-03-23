# -------------------------------------------------------------------------
# Vulkan

message(STATUS "++++++++++++++++++ External-Vulkan")

if (NOT OPTION_CMP_VULKAN)
    message("OPTION_CMP_VULKAN=OFF")
    return()
endif()

add_library(ExtVulkan INTERFACE)

if (NOT DEFINED ENV{VULKAN_SDK})
    message(FATAL_ERROR "A 'VULKAN_SDK' environment variable is required")
endif()

set(VULKAN_SDK_PATH $ENV{VULKAN_SDK})

message(STATUS "++++++++++++++++++ External-VULKAN  ${VULKAN_SDK_PATH}" )


if (CMP_HOST_APPLE)

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

    find_library(IO_KIT_FRAMEWORK       IOKit)
    find_library(IO_SURFACE_FRAMEWORK   IOSurface)
    find_library(METAL_FRAMEWORK        Metal)
    find_library(QUARTZ_CORE_FRAMEWORK  QuartzCore)

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

# More details can be found on this issue: https://vulkan.lunarg.com/issue/view/5ed6eca8e0d2a37f6e19cefc
#    set(RENAME_HACK bash -c "find . -type f -exec sed -i 's,root/sdk-build/${VULKAN_VERSION},${VULKAN_OUTPUT_DIR},' {} \\\;")
#    cmp_execute_process(RENAME_HACK ${VULKAN_OUTPUT_DIR})
#
#    get_filename_component(VULKAN_SDK ${COMPRESSONATOR_PROJECT_DIR}/External/build-out/Linux/vulkan ABSOLUTE)
#    if (NOT EXISTS VULKAN_SDK)
#        file(COPY $ENV{VULKAN_SDK} ${VULKAN_SDK})
#    endif()
#    set(ENV{VULKAN_SDK} ${VULKAN_SDK})
#
#    find_package(Vulkan REQUIRED)

elseif(CMP_HOST_WINDOWS)

    find_package(Vulkan REQUIRED)

    target_compile_definitions(ExtVulkan INTERFACE VK_USE_PLATFORM_WIN32_KHR)

endif()

target_link_libraries(ExtVulkan INTERFACE ${Vulkan_LIBRARIES})
target_include_directories(ExtVulkan INTERFACE 
    ${Vulkan_INCLUDE_DIRS}
    ${Vulkan_INCLUDE_DIRS}/glslang
)

add_library(ExtGlslang INTERFACE)

target_link_libraries(ExtGlslang INTERFACE  
    ${VULKAN_SDK_PATH}/lib/glslang$<$<CONFIG:Debug>:d>.lib
    ${VULKAN_SDK_PATH}/lib/SPIRV$<$<CONFIG:Debug>:d>.lib
    ${VULKAN_SDK_PATH}/lib/HLSL$<$<CONFIG:Debug>:d>.lib
    ${VULKAN_SDK_PATH}/lib/OSDependent$<$<CONFIG:Debug>:d>.lib
    ${VULKAN_SDK_PATH}/lib/OGLCompiler$<$<CONFIG:Debug>:d>.lib
    ${VULKAN_SDK_PATH}/lib/SPIRV-Tools.lib
    ${VULKAN_SDK_PATH}/lib/SPIRV-Tools-opt.lib
    )
