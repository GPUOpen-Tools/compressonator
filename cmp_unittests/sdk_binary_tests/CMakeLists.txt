cmake_minimum_required(VERSION 3.18)

project(sdk_binary_tests)

set(CMAKE_CONFIGURATION_TYPES
    "Debug"
    "Release"
    CACHE STRING "" FORCE
)

add_compile_definitions($<$<CONFIG:Debug>:_DEBUG>)

# Macro that allows for functions to be called for each variation of a test project with a single call
# This assumes the function to call contains a "target" field
# Any additional arguments needed by the function can be appended at the end of the macro call and they will be passed to the function
macro(call_function_variations_macro function_name project_name)
    cmake_language(CALL ${function_name} ${project_name}_MD ${ARGN})
    cmake_language(CALL ${function_name} ${project_name}_MT ${ARGN})
    cmake_language(CALL ${function_name} ${project_name}_MD_DLL ${ARGN})
    cmake_language(CALL ${function_name} ${project_name}_MT_DLL ${ARGN})
endmacro()

macro(call_link_library_variations project_name library_prefix)
    target_link_libraries(${project_name}_MD PRIVATE ${library_prefix}_MD$<$<CONFIG:Debug>:d>.lib)
    target_link_libraries(${project_name}_MT PRIVATE ${library_prefix}_MT$<$<CONFIG:Debug>:d>.lib)
    target_link_libraries(${project_name}_MD_DLL PRIVATE ${library_prefix}_MD$<$<CONFIG:Debug>:d>_DLL.dll)
    target_link_libraries(${project_name}_MT_DLL PRIVATE ${library_prefix}_MT$<$<CONFIG:Debug>:d>_DLL.dll)
endmacro()

macro(call_compile_options_variations project_name)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        target_compile_options(${project_name}_MT PRIVATE /MT$<$<CONFIG:Debug>:d>)
    endif()
endmacro()

# Variables

set(CORE_TEST_SRCS core_binary_test.cpp)
set(CORE_TEST_INCLUDES ./ ${PROJECT_SOURCE_DIR}/../../cmp_core/include)
set(CORE_TEST_LIBRARY_DIRECTORIES ${PROJECT_SOURCE_DIR}/../../cmp_core/lib/bin/x64/)

set(FRAMEWORK_TEST_SRCS framework_binary_test.cpp)
set(FRAMEWORK_TEST_INCLUDES ./ ${PROJECT_SOURCE_DIR}/../../cmp_framework/include)
set(FRAMEWORK_TEST_LIBRARY_DIRECTORIES ${PROJECT_SOURCE_DIR}/../../cmp_framework/lib/bin/x64/)

set(SDK_TEST_SRCS sdk_binary_test.cpp)
set(SDK_TEST_INCLUDES ./ ${PROJECT_SOURCE_DIR}/../../cmp_sdk/include)
set(SDK_TEST_LIBRARY_DIRECTORIES ${PROJECT_SOURCE_DIR}/../../cmp_sdk/lib/bin/x64/)

# Core Defintion

call_function_variations_macro(add_executable core_binary_test)

call_function_variations_macro(target_sources core_binary_test PRIVATE ${CORE_TEST_SRCS})
call_function_variations_macro(target_include_directories core_binary_test PRIVATE ${CORE_TEST_INCLUDES})
call_function_variations_macro(target_link_directories core_binary_test PRIVATE ${CORE_TEST_LIBRARY_DIRECTORIES})

call_link_library_variations(core_binary_test CMP_Core)

call_compile_options_variations(core_binary_test)

# Framework Defintion

call_function_variations_macro(add_executable framework_binary_test)

call_function_variations_macro(target_sources framework_binary_test PRIVATE ${FRAMEWORK_TEST_SRCS})
call_function_variations_macro(target_include_directories framework_binary_test PRIVATE ${FRAMEWORK_TEST_INCLUDES})
call_function_variations_macro(target_link_directories framework_binary_test PRIVATE ${FRAMEWORK_TEST_LIBRARY_DIRECTORIES})

call_link_library_variations(framework_binary_test CMP_Framework)

call_compile_options_variations(framework_binary_test)

# SDK Defintion

call_function_variations_macro(add_executable sdk_binary_test)

call_function_variations_macro(target_sources sdk_binary_test PRIVATE ${SDK_TEST_SRCS})
call_function_variations_macro(target_include_directories sdk_binary_test PRIVATE ${SDK_TEST_INCLUDES})
call_function_variations_macro(target_link_directories sdk_binary_test PRIVATE ${SDK_TEST_LIBRARY_DIRECTORIES})

call_link_library_variations(sdk_binary_test Compressonator)

call_compile_options_variations(sdk_binary_test)