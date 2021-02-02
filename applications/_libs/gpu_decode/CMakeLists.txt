set(GPU_DECODE_H
  gpu_decodebase.cpp
  gpu_decode.cpp
)

set(GPU_DECODE_SRC
  gpu_decodebase.h
  gpu_decode.h
)

add_library(CMP_GpuDecode STATIC ${GPU_DECODE_H} ${GPU_DECODE_SRC})

target_include_directories(CMP_GpuDecode PRIVATE
    ${PROJECT_SOURCE_DIR}/applications/_plugins/common
    ${PROJECT_SOURCE_DIR}/cmp_framework/common/half
    ${PROJECT_SOURCE_DIR}/cmp_compressonatorlib
)

set_target_properties(CMP_GpuDecode PROPERTIES FOLDER "Libs")
