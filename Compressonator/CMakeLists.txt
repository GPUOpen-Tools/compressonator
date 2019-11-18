file(GLOB_RECURSE SOURCES "Source/Codec/Block/*.cpp")
file(GLOB_RECURSE SOURCES "Source/Codec/Buffer/*.cpp")
file(GLOB_RECURSE SOURCES "Source/Codec/DXT/*.cpp")
file(GLOB SOURCES "Source/Codec/*.cpp")
file(GLOB_RECURSE SOURCES "Source/Common/*.cpp")
file(GLOB_RECURSE SOURCES "Source/GPU_Decode/*.cpp")
file(GLOB SOURCES "Source/*.cpp")
list(APPEND SOURCES "../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/Half/half.cpp")

set(INCLUDE_DIRS
    "."
    "../Common/Lib/Ext/OpenEXR/ilmbase-2.2.0/Half"
    "Header"
    "Header/Codec"
    "Header/Codec/Block"
    "Header/Codec/Buffer"
    "Header/Codec/DXT"
    "Header/Codec/DXTC"
    "Header/Internal"
)

add_library(Compressonator STATIC ${SOURCES})
target_include_directories(Compressonator PRIVATE ${INCLUDE_DIRS})
