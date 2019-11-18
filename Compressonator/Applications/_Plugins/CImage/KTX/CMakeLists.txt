set(INCLUDE_DIRS
    "."
    "../../Common"
    "../../../../Header"
    "Lib"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/Boost/boost_1_59_0/"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/glew/1.9.0/include/"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/OpenEXR/openexr-2.2.0/VS2015/x64/include/OpenEXR/"
)

set(SOURCES
    "../../Common/MIPS.cpp"
    "../../Common/TC_PluginInternal.cpp"
    "../../Common/UtilFuncs.cpp"
    "KTX.cpp"
    "StdAfx.cpp"
    "softfloat.cpp"
    "Lib/swap.c"
    "Lib/checkheader.c"
    "Lib/errstr.c"
    "Lib/hashtable.c"
    "Lib/ktxfilestream.c"
    "Lib/ktxmemstream.c"
    "Lib/loader.c"
    "Lib/writer.c"
)

add_library(KTX STATIC ${SOURCES})
target_include_directories(KTX PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(KTX PRIVATE APPLICATION_PLUGIN)
