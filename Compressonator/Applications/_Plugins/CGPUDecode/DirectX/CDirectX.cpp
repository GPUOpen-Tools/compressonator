
#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "Compressonator.h"
#include "CDirectX.h"

CMIPS *CDirectX_CMips;

#define BUILD_AS_PLUGIN_DLL

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_CDirectX)
SET_PLUGIN_TYPE("GPUDECODE")
SET_PLUGIN_NAME("DIRECTX")
#else
void *make_Plugin_GPUDecode_Vulkan() { return new Plugin_CVulkan; }
#endif

#pragma comment(lib,"advapi32.lib")        // for RegCloseKey and other Reg calls ...

Plugin_CDirectX::Plugin_CDirectX()
{
}

Plugin_CDirectX::~Plugin_CDirectX()
{
}


int Plugin_CDirectX::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
    pPluginVersion->guid                    = g_GUID_DIRECTX;
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}


int Plugin_CDirectX::TC_Init(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback)
{
    m_pGPUDecode = (TextureControl *)new GPU_DirectX(Width, Height, callback);
    if (m_pGPUDecode == NULL)
        return -1;
    return 0;
}

CMP_ERROR Plugin_CDirectX::TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture)
{
    CMP_ERROR result = CMP_OK;
    if (m_pGPUDecode)
        result = m_pGPUDecode->Decompress(pSourceTexture, pDestTexture);
    return result;
}

int Plugin_CDirectX::TC_Close()
{
    if (m_pGPUDecode)
    {
        delete m_pGPUDecode;
        m_pGPUDecode = NULL;
    }
    return 0;
}


