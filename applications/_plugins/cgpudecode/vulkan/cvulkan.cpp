
#include <stdio.h>
#include <stdlib.h>
#include "tc_pluginapi.h"
#include "tc_plugininternal.h"
#include "compressonator.h"
#include "cvulkan.h"

CMIPS *CVulkan_CMips;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_CVulkan)
SET_PLUGIN_TYPE("GPUDECODE")
SET_PLUGIN_NAME("VULKAN")
#else
void *make_Plugin_GPUDecode_Vulkan() {
    return new Plugin_CVulkan;
}
#endif

#pragma comment(lib,"advapi32.lib")        // for RegCloseKey and other Reg calls ...

Plugin_CVulkan::Plugin_CVulkan() {
}

Plugin_CVulkan::~Plugin_CVulkan() {
}


int Plugin_CVulkan::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) {
    pPluginVersion->guid                    = g_GUID_VULKAN;
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}


int Plugin_CVulkan::TC_Init(std::uint32_t Width, std::uint32_t Height, WNDPROC callback) {
    m_pGPUDecode = (TextureControl *)new GPU_Vulkan(Width, Height, callback);
    if (m_pGPUDecode == NULL)
        return -1;
    return 0;
}

CMP_ERROR Plugin_CVulkan::TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture) {
    CMP_ERROR result = CMP_OK;
    if (m_pGPUDecode) {
        result = m_pGPUDecode->Decompress(pSourceTexture, pDestTexture);
    }

    return result;
}

int Plugin_CVulkan::TC_Close() {
    if (m_pGPUDecode) {
        delete m_pGPUDecode;
        m_pGPUDecode = NULL;
    }
    return 0;
}


