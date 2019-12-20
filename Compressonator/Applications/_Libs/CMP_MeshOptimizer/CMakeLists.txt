set(MESH_OPTIMIZER_SRC
  indexcodec.cpp
  indexgenerator.cpp
  overdrawanalyzer.cpp
  overdrawoptimizer.cpp
  simplifier.cpp
  stripifier.cpp
  vcacheanalyzer.cpp
  vcacheoptimizer.cpp
  vfetchanalyzer.cpp
  vfetchoptimizer.cpp
)

set(MESH_OPTIMIZER_H
  meshoptimizer.h
)

add_library(CMP_MeshOptimizer STATIC ${MESH_OPTIMIZER_H} ${MESH_OPTIMIZER_SRC})
