add_library(ExtDirectXTex INTERFACE)

target_include_directories(ExtDirectXTex INTERFACE
    ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex_jun2020b/DirectXTex
)

#message(STATUS "****************** START: Compile DirectXTex [${CMAKE_CXX_COMPILER}]" )
#message(STATUS "****************** START: Compile cl.exe [${CMAKE_CXX_COMPILER}]" )
#message(STATUS "****************** START: Compile msbuild.exe [${CMAKE_VS_MSBUILD_COMMAND}]" )
#message(STATUS "****************** INFO BUILD_TYPE [${CMAKE_BUILD_TYPE}]" ) # This the users cmake command line build type "Release or Debug" for our SDK we need both builds
#message(STATUS "****************** INFO CMAKE_VS_PLATFORM_NAME [${CMAKE_VS_PLATFORM_NAME}]" ) # can use this for x86 or x64 specific builds, note:cmp uses x64 only

# Build Relase platform
execute_process(
    COMMAND ${CMAKE_VS_MSBUILD_COMMAND} 
            ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/DirectXTex_Desktop_2017.vcxproj  /p:Configuration=Release  /p:Platform=${CMAKE_VS_PLATFORM_NAME} 
    )
# Build Debug platform
execute_process(
    COMMAND ${CMAKE_VS_MSBUILD_COMMAND} 
            ${PROJECT_SOURCE_DIR}/../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/DirectXTex_Desktop_2017.vcxproj  /p:Configuration=Debug  /p:Platform=${CMAKE_VS_PLATFORM_NAME} 
    )
#message(STATUS "****************** END: Compile DirectXTex" )

#------------ Code below is not used -----------------------------------------------------
# "$<$<CONFIG:Debug>:Release>$<$<CONFIG:Release>:Debug>"/DirectXTex.lib)

# file(GLOB_RECURSE HEADERS
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/*.h
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/*.inl
# )
# 
# file(GLOB_RECURSE SOURCES
#     directxtex-submodule/*.cpp
# )
# 
# file(GLOB_RECURSE SHADERS
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex/Shaders/*.hlsl
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTexShaders/*.inc
# )
# 
# target_sources(ExtDirectXTex PRIVATE
#     ${HEADERS}
#     ${SOURCES}
# )
# 
# target_include_directories(ExtDirectXTex PUBLIC
#     ${CMAKE_CURRENT_LIST_DIR}/../../../common/lib/ext/directxtex/DirectXTex-jun2020b/DirectXTex
#     #directxtex-submodule
# )
# 
# set_target_properties(ExtDirectXTex PROPERTIES FOLDER "External")