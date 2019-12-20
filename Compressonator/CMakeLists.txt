cmake_minimum_required(VERSION 3.10)

project(Compressonator)

add_subdirectory(CMP_Framework)
add_subdirectory(CMP_CompressonatorLib)
add_subdirectory(Applications/_Plugins/CImage/ASTC)
add_subdirectory(Applications/_Plugins/CImage/EXR)
add_subdirectory(Applications/_Plugins/CImage/KTX)
add_subdirectory(Applications/_Plugins/CImage/TGA)
add_subdirectory(Applications/_Plugins/CAnalysis/Analysis)
add_subdirectory(Applications/_Libs/CMP_MeshCompressor)
add_subdirectory(Applications/CompressonatorCLI)

add_dependencies(CompressonatorCLI-bin Compressonator CMP_Framework ASTC EXR KTX TGA Analysis CMP_MeshCompressor)