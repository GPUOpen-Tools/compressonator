
add_library(DirectXTex)

file(GLOB_RECURSE HEADERS

    directxtex-submodule/*.h
    directxtex-submodule/*.inl
)

file(GLOB_RECURSE SOURCES

    directxtex-submodule/*.cpp
)

file(GLOB_RECURSE SHADERS

    directxtex-submodule/Shaders/*.hlsl
    directxtex-submodule/Shaders/*.inc
)

target_sources(DirectXTex PRIVATE

    ${HEADERS}
    ${SOURCES}
)

target_include_directories(DirectXTex PUBLIC

    directxtex-submodule
)

set_target_properties(DirectXTex PROPERTIES FOLDER "External")