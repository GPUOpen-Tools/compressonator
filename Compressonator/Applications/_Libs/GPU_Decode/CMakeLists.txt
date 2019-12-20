set(GPU_DECODE_H
  GPU_DecodeBase.cpp
  GPU_Decode.cpp
)

set(GPU_DECODE_SRC
  GPU_DecodeBase.h
  GPU_Decode.h
)

add_library(GpuDecode STATIC ${GPU_DECODE_H} ${GPU_DECODE_SRC})

target_include_directories(GpuDecode PRIVATE
  ../../../CMP_CompressonatorLib
)
