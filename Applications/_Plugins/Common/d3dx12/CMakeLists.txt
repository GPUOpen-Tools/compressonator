# =======================================================================
# See https://github.com/microsoft/DirectX-Graphics-Samples/tree/master/Libraries/D3DX12
#

add_library(Plugin_Common_D3DX12 INTERFACE)

target_sources(Plugin_Common_D3DX12 INTERFACE

    d3dx12.h
)

target_include_directories(Plugin_Common_D3DX12 INTERFACE
    ./
)

#set_target_properties(Plugin_Common_D3DX12 PROPERTIES FOLDER ${FOLDER_NAME})
