cmake_minimum_required(VERSION 3.10)

add_library(CMP_MeshCompressor STATIC "")

file(GLOB_RECURSE DracoLib
     "./Draco/src/draco/*.h"
     "./Draco/src/draco/attributes/*.h"
     "./Draco/src/draco/attributes/*.cc"
     "./Draco/src/draco/compression/*.h"
     "./Draco/src/draco/compression/*.cc"
     "./Draco/src/draco/core/*.h"
     "./Draco/src/draco/core/*.cc"
     "./Draco/src/draco/io/*.h"
     "./Draco/src/draco/io/*.cc"
     "./Draco/src/draco/mesh/*.h"
     "./Draco/src/draco/mesh/*.cc"
     "./Draco/src/draco/metadata/*.h"
     "./Draco/src/draco/metadata/*.cc"
     "./Draco/src/draco/point_cloud/*.h"
     "./Draco/src/draco/point_cloud/*.cc"
     )
target_sources(CMP_MeshCompressor
               PRIVATE
               ${DracoLib}
               )

target_include_directories(CMP_MeshCompressor
                           PRIVATE
                           ./Draco/src/
                           )
