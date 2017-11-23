#ifndef _PLUGIN_IMAGE_BMP_H
#define _PLUGIN_IMAGE_BMP_H

#include "stdafx.h"
#include "PluginInterface.h"
#include "glTF_DX12DeviceEx.h"


#ifdef _WIN32
// {2505D1C0-D0F3-4E57-BCED-8358689D3FCC}
static const GUID g_GUID = { 0x2505d1c0, 0xd0f3, 0x4e57,{ 0xbc, 0xed, 0x83, 0x58, 0x68, 0x9d, 0x3f, 0xcc } };

#else
static const GUID g_GUID = { 0 };
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

CMIPS *DX12_CMips = NULL;

class Plugin_glTF_DX12_EX : public PluginInterface_3DModel
{
public: 
        Plugin_glTF_DX12_EX();
        virtual ~Plugin_glTF_DX12_EX();
        int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        int TC_PluginSetSharedIO(void *Shared);

        int  CreateView(const char* pszFilename,  CMP_LONG Width, CMP_LONG Height, void *userHWND, void *pluginManager, void *msghandler, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc);
        bool RenderView();
        void CloseView();
        void processMSG(void *message);
        void ReSizeView(CMP_LONG w, CMP_LONG h);

        int  LoadModel(const char* pszFilename, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc = NULL);

private:
        glTF_DX12DeviceEx *m_glTF_DX12DeviceEx;
        GLTFCommon       m_gltfLoader[2];
        HWND             m_hwnd;
};

extern void *make_Plugin_glTF_DX12_EX();


#endif