link_directories(
    ${CMAKE_BINARY_DIR}/lib         # build from source lib path
    )

add_executable(cmp_cas)

file(GLOB_RECURSE RESOURCES
    resources/*
)

target_sources(cmp_cas
    PRIVATE
        ${RESOURCES}
        cmp_cas.cpp
)

target_include_directories(cmp_cas PUBLIC
    ./
    ${PROJECT_SOURCE_DIR}/cmp_framework
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib/            # source based path
    ${PROJECT_SOURCE_DIR}/cmp_core/shaders
    ${PROJECT_SOURCE_DIR}/cmp_core/source
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common  # stb_image
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common/gltf  # stb_image_write
)

target_link_libraries(cmp_cas
    PRIVATE
    CMP_Framework
    CMP_Compressonator
)



set_target_properties(cmp_cas
     PROPERTIES 
     FOLDER "Applications"
     OUTPUT_NAME "cmp_cas"
     VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$(Configuration)"
    )

