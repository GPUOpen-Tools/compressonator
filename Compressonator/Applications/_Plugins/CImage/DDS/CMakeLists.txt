set(INCLUDE_DIRS
    "."
    "../../Common/"
    "../../../../Header/"
)

set(SOURCES
    "DDS.cpp"
    "DDS_DX10.cpp"
    "DDS_File.cpp"
    "DDS_Helpers.cpp"
    "stdafx.cpp"
    "../../Common/MIPS.cpp"
    "../../Common/TC_PluginInternal.cpp"
    "../../Common/UtilFuncs.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/BC.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/BC4BC5.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/BC6HBC7.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexCompress.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexConvert.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexD3D11.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexImage.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexMipmaps.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexResize.cpp"
    "${PROJECT_SOURCE_DIR}/Common/Lib/Ext/DirectXTex/v132/DirectXTexUtil.cpp"
)

add_library(DDS STATIC ${SOURCES})
target_include_directories(DDS PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(DDS PRIVATE APPLICATION_PLUGIN)
