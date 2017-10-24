#ifndef _PLUGIN_IMAGE_BMP_H
#define _PLUGIN_IMAGE_BMP_H

#include "stdafx.h"
#include "PluginInterface.h"
#include "glTF.h"
#include "glTF_DX12Device.h"


#ifdef _WIN32
// {79436B32-C8E1-45C9-99E3-AAA56B076A05}
static const GUID g_GUID = { 0x79436b32, 0xc8e1, 0x45c9,{ 0x99, 0xe3, 0xaa, 0xa5, 0x6b, 0x7, 0x6a, 0x5 } };
#else
static const GUID g_GUID = { 0 };
#endif

#define TC_PLUGIN_VERSION_MAJOR    1
#define TC_PLUGIN_VERSION_MINOR    0

class Plugin_glTF_DX12 : public PluginInterface_3DModel
{
public: 
        Plugin_glTF_DX12();
        virtual ~Plugin_glTF_DX12();
        int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
        int  CreateView(const char* pszFilename,  CMP_LONG Width, CMP_LONG Height, void *userHWND, void *pluginManager, void *msghandler, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc);
        bool RenderView();
        void CloseView();
        void processMSG(void *message);
        int  LoadModel(const char* pszFilename, const char* pszFilename2, CMP_Feedback_Proc pFeedbackProc = NULL);

private:
        glTF_DX12Device *m_glTF_DX12Device;
        GLTFLoader       m_gltfLoader[2];
        std::string      m_FilePathName;
};

extern void *make_Plugin_glTF_DX12();


#endif