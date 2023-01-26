set(GLTF_SRC
    cmp_gltfcommon.cpp
    cmp_gltfhelpers.cpp
    cmp_error.cpp
    tiny_gltf2_utils.cpp
)

set(GLTF_H
    cmp_defines.h
    cmp_gltfcommon.h
    cmp_gltffeatures.h
    cmp_gltfhelpers.h
    cmp_gltfstructures.h
    cmp_error.h
    tiny_gltf2.h
    tiny_gltf2_utils.h
)

add_library(CMP_GUI_Gltf STATIC ${GLTF_H} ${GLTF_SRC})

target_include_directories(CMP_GUI_Gltf PRIVATE
  ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
  ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
  ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common/json
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
)

set_target_properties(CMP_GUI_Gltf PROPERTIES FOLDER "Libs")

