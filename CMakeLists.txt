cmake_minimum_required (VERSION 3.15)
project(Compressonator C CXX)

add_definitions(-DDISABLE_TESTCODE)

add_subdirectory(Compressonator)
add_subdirectory(Compressonator/Applications/_Plugins/CImage/DDS)
add_subdirectory(Compressonator/Applications/_Plugins/CImage/KTX)
