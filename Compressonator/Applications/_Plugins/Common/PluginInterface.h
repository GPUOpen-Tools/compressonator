//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// \file PluginInterface.h
/// \version 2.50
/// \brief Declares the interface to the AMDCompress library.
//
//=====================================================================

#ifndef _PLUGININTERFACE_H
#define _PLUGININTERFACE_H

#include "PluginBase.h"
#include "Texture.h"
#include "PluginManager.h"
#include "MIPS.h"

typedef CMP_DWORD_PTR TC_HANDLE;  ///< Generic Texture API handle
typedef TC_HANDLE HFILETYPE;  ///< Handle to a FileType.
typedef TC_HANDLE HCODEC;     ///< Handle to a Codec.
typedef TC_HANDLE HMIPPER;    ///< Handle to a Mipper.
typedef TC_HANDLE HVIEWTYPE;  ///< \internal Handle to a ViewType.
typedef TC_HANDLE HVIEW;      ///< \internal Handle to a View.

#ifndef _WIN32
typedef CMP_DWORD WNDPROC;
#endif

class PluginInterface_Image : PluginBase
{
public:
    PluginInterface_Image(){}
    virtual ~PluginInterface_Image(){}
    virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
    virtual int TC_PluginSetSharedIO(void* Shared)=0;
    
    virtual int TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet) = 0;
    virtual int TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet) = 0;
    virtual int TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture) = 0;
    virtual int TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture) = 0;
};

class PluginInterface_Codec : PluginBase
{
public:
    PluginInterface_Codec(){}
    virtual ~PluginInterface_Codec(){}
    virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
};

// These type of plugins are used to Analyze images
class PluginInterface_Analysis : PluginBase
{
public:
        PluginInterface_Analysis(){}
        virtual ~PluginInterface_Analysis(){}
        virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
        virtual int TC_ImageDiff(const char *in1, const char *in2, const char *out, char *resultsFile, void *pluginManager, void **cmipImages, CMP_Feedback_Proc pFeedbackProc = NULL) { (void)in1, (void)in2, (void)out, (void)resultsFile; (void)pluginManager; (void*)cmipImages; (void)pFeedbackProc; return 0; };
        virtual int TC_PSNR_MSE(const char *in1, const char *in2, char *resultsFile, void *pluginManager, CMP_Feedback_Proc pFeedbackProc = NULL) { (void)in1, (void)in2, (void)resultsFile; (void)pluginManager; (void)pFeedbackProc; return 0; };
        virtual int TC_SSIM(const char *in1, const char *in2, char *resultsFile, void *pluginManager, CMP_Feedback_Proc pFeedbackProc = NULL) { (void)in1, (void)in2, (void)resultsFile; (void)pluginManager; (void)pFeedbackProc; return 0; };
};


// These type of plugins are used to Generate or transform images
class PluginInterface_Filters : PluginBase
{
public:
        PluginInterface_Filters(){}
        virtual ~PluginInterface_Filters(){}
        virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)=0;
        virtual int TC_GenerateMIPLevels(MipSet *pMipSet, int nMinSize)=0;
};


// These type of plugins are used for Compute
class PluginInterface_Compute : PluginBase
{
public:
    PluginInterface_Compute() {}
    virtual ~PluginInterface_Compute() {}
    virtual int     TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual int     TC_Init(CMP_Texture *srcTexture, void  *kernel_options) = 0;
    virtual char    *TC_ComputeSourceFile() = 0;
};

// These type of plugins are used to Decompress Images using GPU
class PluginInterface_Compute2 : PluginBase
{
public:
    PluginInterface_Compute2() {}
    virtual ~PluginInterface_Compute2() {}
    virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual int TC_Init(CMP_Texture *srcTexture, void  *kernel_options) = 0;
    virtual CMP_ERROR TC_Compress(char *source_file, void  *kernel_options, CMP_Texture &SrcTexture, CMP_Texture &destTexture) = 0;
    virtual void TC_SetComputeOptions(void *options) = 0;
    virtual char *TC_ComputeSourceFile() = 0;
    virtual int TC_Close() = 0;
};


// These type of plugins are used to Decompress Images using GPU
class PluginInterface_GPUDecode : PluginBase
{
public:
    PluginInterface_GPUDecode() {}
    virtual ~PluginInterface_GPUDecode() {}
    virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual int TC_Init(CMP_DWORD Width, CMP_DWORD Height, WNDPROC callback) = 0;
    virtual CMP_ERROR TC_Decompress(const CMP_Texture* pSourceTexture, CMP_Texture* pDestTexture) = 0;
    virtual int TC_Close() = 0;
};


class PluginInterface_3DModel : PluginBase
{
public:
    PluginInterface_3DModel() {}
    virtual ~PluginInterface_3DModel() {}
    virtual int  TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual int  TC_PluginSetSharedIO(void* Shared) = 0;
    virtual void *CreateView(void *ModelData, CMP_LONG Width, CMP_LONG Height, void *HWND, void *pluginManager, void *msghandler,CMP_Feedback_Proc pFeedbackProc = NULL) = 0;
    virtual void *ShowView(void *data) = 0;
    virtual void CloseView()    = 0;
    virtual bool OnRenderView()   = 0;
    virtual void OnReSizeView(CMP_LONG w, CMP_LONG h) = 0;
    virtual void processMSG(void *message) = 0;
};


class PluginInterface_3DModel_Loader : PluginBase
{
public:
    PluginInterface_3DModel_Loader() {}
    virtual ~PluginInterface_3DModel_Loader() {}
    virtual int  TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual int  TC_PluginSetSharedIO(void* Shared) = 0;
    virtual int  LoadModelData(const char* pszFilename, const char* pszFilename2, void *pluginManager, void *msghandler, CMP_Feedback_Proc pFeedbackProc) = 0;
    virtual int  SaveModelData(const char* pdstFilename, void* meshData) = 0;
    virtual void *GetModelData() = 0;
};

class PluginInterface_Mesh : PluginBase
{
public:
    PluginInterface_Mesh() {}
    virtual ~PluginInterface_Mesh() {}
    virtual int  TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual int  TC_PluginSetSharedIO(void* Shared) = 0;

    virtual int Init() = 0;
    virtual void* ProcessMesh(void* data = NULL, void* setting = NULL, void* statsOut = NULL, CMP_Feedback_Proc pFeedbackProc = NULL) = 0;
    virtual int CleanUp() = 0;
};

class PluginInterface_WindowViews : PluginBase
{
public:
    PluginInterface_WindowViews() {}
    virtual ~PluginInterface_WindowViews() {}
    virtual int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion) = 0;
    virtual void  ShowWindow(bool Show) = 0;
    virtual void *CreateView(void *window) = 0;
    virtual void  DeleteView()  = 0;
};


// Feature driven classes based on Base Plugin interface
typedef PluginInterface_Codec*      (*PLUGIN_FACTORYFUNC_CODEC)();
typedef PluginInterface_Image*      (*PLUGIN_FACTORYFUNC_IMAGE)();
typedef PluginInterface_Analysis*   (*PLUGIN_FACTORYFUNC_ANALYSIS)();
typedef PluginInterface_Filters*    (*PLUGIN_FACTORYFUNC_FILTERS)();
typedef PluginInterface_Compute*    (*PLUGIN_FACTORYFUNC_COMPUTE)();
typedef PluginInterface_Compute2*   (*PLUGIN_FACTORYFUNC_COMPUTE2)();
typedef PluginInterface_GPUDecode*  (*PLUGIN_FACTORYFUNC_GPUDECODE)();
typedef PluginInterface_3DModel*    (*PLUGIN_FACTORYFUNC_3DMODEL)();
typedef PluginInterface_3DModel_Loader*    (*PLUGIN_FACTORYFUNC_3DMODEL_LOADER)();
typedef PluginInterface_Mesh*       (*PLUGIN_FACTORYFUNC_MESH)();
typedef PluginInterface_WindowViews*      (*PLUGIN_FACTORYFUNC_WINDOWVIEWS)();

#endif
