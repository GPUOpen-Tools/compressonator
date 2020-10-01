//=====================================================================
// Copyright 2008 (c), ATI Technologies Inc. All rights reserved.
// Copyright 2020 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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

#pragma once

#include "d3dx10.h"
#include "Module.h"

typedef struct {
    const TCHAR*    szModuleName;
    const TCHAR*    szDescription;
    bool            bFound;
} D3DCompilerModule;

extern D3DCompilerModule g_aD3DCompilerModules[];
extern const DWORD g_dwD3DCompilerModules;

typedef BOOL(WINAPI FAR* D3DXCheckVersionProc)(UINT D3DSdkVersion, UINT D3DX10SdkVersion);

class CD3DCompilerModule : public CModule {
  public:
    CD3DCompilerModule();
    CD3DCompilerModule(LPCTSTR lpModuleName);
    virtual ~CD3DCompilerModule();

    virtual bool LoadModule(LPCTSTR lpModuleName);

  protected:
    virtual void UnloadModule();

    D3DXCheckVersionProc m_pD3DXCheckVersion;
};


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DWORD LoadD3DCompiler(CD3DCompilerModule& D3DX10, CString& strD3DCompilerModule);

#ifdef __cplusplus
}
#endif // __cplusplus
