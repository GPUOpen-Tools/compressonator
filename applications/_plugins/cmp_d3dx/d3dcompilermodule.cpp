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

#include "D3DCompilerModule.h"

TCHAR szLatestD3DX10[256] = _T("");

D3DCompilerModule g_aD3DCompilerModules[] = {
    {_T(""), szLatestD3DX10, false},
    {_T("d3dcompiler_36.DLL"),    _T("D3DCompiler_36 (November 07 SDK)"), false},
    {_T("d3dcompiler_35.DLL"),    _T("D3DCompiler_35 (August 07 SDK)"), false},
    {_T("d3dcompiler_34.DLL"),    _T("D3DCompiler_34 (June 07 SDK)"), false},
    {_T("d3dcompiler_33.DLL"),    _T("D3DCompiler_33 (April 07 SDK)"), false},
};
const DWORD g_dwD3DCompilerModules = (sizeof(g_aD3DCompilerModules) / sizeof(g_aD3DCompilerModules[0]));
DWORD g_dwNewestD3DCompilerModule = 0;

CD3DCompilerModule::CD3DCompilerModule(void) : CModule() {
    m_pD3DXCheckVersion = NULL;
}

CD3DCompilerModule::CD3DCompilerModule(LPCTSTR lpModuleName) {
    LoadModule(lpModuleName);
}

CD3DCompilerModule::~CD3DCompilerModule(void) {
    UnloadModule();
}

bool CD3DCompilerModule::LoadModule(LPCTSTR lpModuleName) {
    if(__super::LoadModule(lpModuleName)) {
        m_pD3DXCheckVersion = (D3DXCheckVersionProc) GetProcAddress("D3DXCheckVersion");
        if(!m_pD3DXCheckVersion) {
            UnloadModule();
            return false;
        } else
            return true;
    } else
        return false;
}

void CD3DCompilerModule::UnloadModule() {
    __super::UnloadModule();
    m_pD3DXCheckVersion = NULL;
}

HRESULT CD3DCompilerModule::D3DXCheckVersion(UINT D3DSdkVersion, UINT D3DX10SdkVersion) {
    if(m_pD3DXCheckVersion)
        return m_pD3DXCheckVersion(D3DSdkVersion, D3DX10SdkVersion);
    else
        return D3DERR_INVALIDCALL;
}

DWORD LoadD3DCompiler(CD3DCompilerModule& D3DX10, CString& strD3DCompilerModule) {
    g_aD3DCompilerModules[0].bFound = false;
    for(DWORD i = 1; i < g_dwD3DCompilerModules; i++)
        g_aD3DCompilerModules[0].bFound |= g_aD3DCompilerModules[i].bFound = D3DX10.LoadModule(g_aD3DCompilerModules[i].szModuleName);

    if(!g_aD3DCompilerModules[0].bFound)
        return LOAD_FAILED;

    for(g_dwNewestD3DCompilerModule = 1; g_dwNewestD3DCompilerModule < g_dwD3DCompilerModules; g_dwNewestD3DCompilerModule++)
        if(g_aD3DCompilerModules[g_dwNewestD3DCompilerModule].bFound) {
            _stprintf(szLatestD3DX10, _T("Use latest D3DX10 - %s"), g_aD3DCompilerModules[g_dwNewestD3DCompilerModule].szDescription);
            break;
        }

    if(!strD3DCompilerModule.IsEmpty() && D3DX10.LoadModule(strD3DCompilerModule)) {
        for(DWORD dwD3DXModule = 0; dwD3DXModule < g_dwD3DCompilerModules; dwD3DXModule++)
            if(strD3DCompilerModule.CompareNoCase(g_aD3DCompilerModules[dwD3DXModule].szModuleName) == 0)
                return dwD3DXModule;
    } else {
        strD3DCompilerModule = "";
        if(D3DX10.LoadModule(g_aD3DCompilerModules[g_dwNewestD3DCompilerModule].szModuleName))
            return 0;
        else
            return LOAD_FAILED;
    }

    return LOAD_FAILED;
}
