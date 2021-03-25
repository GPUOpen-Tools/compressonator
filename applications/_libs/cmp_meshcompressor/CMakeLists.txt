cmake_minimum_required(VERSION 3.10)

# Feature removed as of v4.2 to be replaced by new mesh compressor code
# add_library(CMP_MeshCompressor STATIC "")
# 
# file(GLOB_RECURSE DracoLib
#      "./draco/src/draco/*.h"
#      "./draco/src/draco/attributes/*.h"
#      "./draco/src/draco/attributes/*.cc"
#      "./draco/src/draco/compression/*.h"
#      "./draco/src/draco/compression/*.cc"
#      "./draco/src/draco/core/*.h"
#      "./draco/src/draco/core/*.cc"
#      "./draco/src/draco/io/*.h"
#      "./draco/src/draco/io/*.cc"
#      "./draco/src/draco/mesh/*.h"
#      "./draco/src/draco/mesh/*.cc"
#      "./draco/src/draco/metadata/*.h"
#      "./draco/src/draco/metadata/*.cc"
#      "./draco/src/draco/point_cloud/*.h"
#      "./draco/src/draco/point_cloud/*.cc"
#      )
# 
# target_sources(CMP_MeshCompressor
#                PRIVATE
#                ${DracoLib}
#                )
# 
# target_include_directories(CMP_MeshCompressor
#                            PRIVATE
#                            ${PROJECT_SOURCE_DIR}/applications/_libs/cmp_meshcompressor/draco/src
#                            )
# 
# set_target_properties(CMP_MeshCompressor PROPERTIES FOLDER "Libs")
