set(GLTF_SRC
    gltfcommon.cpp
    gltfhelpers.cpp
    tiny_gltf2_utils.cpp
    error.cpp
)

set(GLTF_H
    defines.h
    gltfcommon.h
    gltffeatures.h
    gltfhelpers.h
    gltfstructures.h
    stb_image.h
    stb_image_write.h
    tiny_gltf2.h
    tiny_gltf2_utils.h
    error.h
)

add_library(CMP_GUI_Gltf STATIC ${GLTF_H} ${GLTF_SRC})

target_include_directories(CMP_GUI_Gltf PRIVATE
  ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
  ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
  # ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshcompressor/draco/src
  ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshoptimizer
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common
  ${PROJECT_SOURCE_DIR}/applications/_plugins/common/json
  ${PROJECT_SOURCE_DIR}/../common/lib/ext/glm
  ${draco_INCLUDE_DIRS}
)

set_target_properties(CMP_GUI_Gltf PROPERTIES FOLDER "Libs")

