set(GLTF_SRC
  #Error.cpp # Windows API
  GltfCommon.cpp
  glTFHelpers.cpp
  # Misc.cpp # same as Misc.cpp in top level directory. Unsued and Windows API.
  tiny_gltf2_utils.cpp
)

set(GLTF_H
  defines.h
  #Error.h
  GltfCommon.h
  GltfFeatures.h
  glTFHelpers.h
  GltfStructures.h
  # Misc.h
  stb_image.h
  stb_image_write.h
  targetver.h
  tiny_gltf2.h
  tiny_gltf2_utils.h
)

add_library(gltf STATIC ${GLTF_H} ${GLTF_SRC})

target_include_directories(gltf PRIVATE
  ../../../../CMP_CompressonatorLib
  ../../../../CMP_Framework/Common/half
  ${draco_INCLUDE_DIRS}
  ../../../_Libs/CMP_MeshOptimizer
  ../json/
  ../
)
