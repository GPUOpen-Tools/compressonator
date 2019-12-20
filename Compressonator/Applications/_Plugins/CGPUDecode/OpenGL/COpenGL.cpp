
#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "Compressonator.h"
#include "COpenGL.h"


CMIPS *COpenGL_CMips;

#define BUILD_AS_PLUGIN_DLL

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_COpenGL)
SET_PLUGIN_TYPE("GPUDECODE")
SET_PLUGIN_NAME("OPENGL")
#else
void *make_Plugin_GPUDecode_OpenGL() { return new Plugin_COpenGL; }
#endif

#pragma comment(lib,"advapi32.lib")        // for RegCloseKey and other Reg calls ...

Plugin_COpenGL::Plugin_COpenGL()
{
    m_pGPUDecode = NULL;
}

Plugin_COpenGL::~Plugin_COpenGL()
{
    if (m_pGPUDecode)
            delete m_pGPUDecode;
}


int Plugin_COpenGL::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{
    pPluginVersion->guid                    = g_GUID_OPENGL;
    pPluginVersion->dwAPIVersionMajor       = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor       = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_COpenGL::TC_Init(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback)
{
    m_pGPUDecode = (TextureControl *)new GPU_OpenGL(Width, Height, callback);
    if (m_pGPUDecode == NULL)
        return -1;
    return 0;
}

CMP_ERROR Plugin_COpenGL::TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture)
{
    CMP_ERROR result = CMP_OK;
    if (m_pGPUDecode)
        result = m_pGPUDecode->Decompress(pSourceTexture, pDestTexture);
    return result;
}

int Plugin_COpenGL::TC_Close()
{
    if (m_pGPUDecode)
    {
        delete m_pGPUDecode;
        m_pGPUDecode = NULL;
    }
    return 0;
}


