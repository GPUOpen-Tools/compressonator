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

#include "d3dxmodule.h"

TCHAR szLatestD3DX9[MAX_PATH] = "";

D3DXModule g_aD3DXModules[] = {
    {(""), szLatestD3DX9, false},
    {("D3DX9_41.DLL"), ("D3DX9_41 (March 09 SDK)"), false},
    {("D3DX9_40.DLL"), ("D3DX9_40 (November 08 SDK)"), false},
    {("D3DX9_39.DLL"), ("D3DX9_39 (August 08 SDK)"), false},
    {("D3DX9_38.DLL"), ("D3DX9_38 (June 08 SDK)"), false},
    {("D3DX9_37.DLL"), ("D3DX9_37 (March 08 SDK)"), false},
    {("D3DX9_36.DLL"), ("D3DX9_36 (November 07 SDK)"), false},
    {("D3DX9_35.DLL"), ("D3DX9_35 (August 07 SDK)"), false},
    {("D3DX9_34.DLL"), ("D3DX9_34 (June 07 SDK)"), false},
    {("D3DX9_33.DLL"), ("D3DX9_33 (April 07 SDK)"), false},
    {("D3DX9_32.DLL"), ("D3DX9_32 (December 06 SDK)"), false},
    {("D3DX9_31.DLL"), ("D3DX9_31 (October 06 SDK)"), false},
    {("D3DX9_30.DLL"), ("D3DX9_30 (April 06 SDK)"), false},
    {("D3DX9_29.DLL"), ("D3DX9_29 (February 06 SDK)"), false},
    {("D3DX9_28.DLL"), ("D3DX9_28 (December 05 SDK)"), false},
    {("D3DX9_27.DLL"), ("D3DX9_27 (August 05 SDK)"), false},
    {("D3DX9_26.DLL"), ("D3DX9_26 (June 05 SDK)"), false},
    {("D3DX9_25.DLL"), ("D3DX9_25 (April 05 SDK)"), false},
    {("D3DX9_24.DLL"), ("D3DX9_24 (February 05 SDK)"), false},
};
const DWORD g_dwD3DXModules = (sizeof(g_aD3DXModules) / sizeof(g_aD3DXModules[0]));
DWORD g_dwNewestD3DXModule = 0;

CD3DXModule::CD3DXModule(void) : CModule() {
    m_pCreateBuffer = NULL;
    m_pCompileShader = NULL;
    m_pAssembleShader = NULL;
    m_pDisassembleShader = NULL;
    m_pFilterTexture = NULL;
}

CD3DXModule::CD3DXModule(LPCTSTR lpModuleName) {
    LoadModule(lpModuleName);
}

CD3DXModule::~CD3DXModule(void) {
    UnloadModule();
}

bool CD3DXModule::LoadModule(LPCTSTR lpModuleName) {
    if(__super::LoadModule(lpModuleName)) {
        m_pCreateBuffer = (D3DXCreateBufferProc) GetProcAddress("D3DXCreateBuffer");
        m_pCompileShader = (D3DXCompileShaderProc) GetProcAddress("D3DXCompileShader");
        m_pAssembleShader = (D3DXAssembleShaderProc) GetProcAddress("D3DXAssembleShader");
        m_pDisassembleShader = (D3DXDisassembleShaderProc) GetProcAddress("D3DXDisassembleShader");
        m_pFilterTexture = (D3DXFilterTextureProc) GetProcAddress("D3DXFilterTexture");
        if(!m_pCompileShader || !m_pAssembleShader || !m_pDisassembleShader || !m_pFilterTexture) {
            UnloadModule();
            return false;
        } else
            return true;
    } else
        return false;
}

void CD3DXModule::UnloadModule() {
    __super::UnloadModule();
    m_pCreateBuffer = NULL;
    m_pCompileShader = NULL;
    m_pAssembleShader = NULL;
    m_pDisassembleShader = NULL;
    m_pFilterTexture = NULL;
}

HRESULT CD3DXModule::D3DXCreateBuffer(DWORD NumBytes, LPD3DXBUFFER *ppBuffer) {
    if(m_pCreateBuffer)
        return m_pCreateBuffer(NumBytes, ppBuffer);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DXModule::D3DXCompileShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
                                       LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile,
                                       DWORD Flags, LPD3DXBUFFER* ppShader,  LPD3DXBUFFER* ppErrorMsgs,
                                       LPD3DXCONSTANTTABLE* ppConstantTable) {
    if(m_pCompileShader)
        return m_pCompileShader(pSrcData, SrcDataLen, pDefines, pInclude, pFunctionName, pProfile, Flags,
                                ppShader,  ppErrorMsgs, ppConstantTable);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DXModule::D3DXAssembleShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
                                        LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXBUFFER* ppShader,
                                        LPD3DXBUFFER* ppErrorMsgs) {
    if(m_pAssembleShader)
        return m_pAssembleShader(pSrcData, SrcDataLen, pDefines, pInclude, Flags, ppShader, ppErrorMsgs);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DXModule::D3DXDisassembleShader(CONST DWORD* pShader, BOOL EnableColorCode, LPCSTR pComments,
        LPD3DXBUFFER* ppDisassembly) {
    if(m_pDisassembleShader)
        return m_pDisassembleShader(pShader, EnableColorCode, pComments, ppDisassembly);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DXModule::D3DXFilterTexture(LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY* pPalette, UINT SrcLevel, DWORD Filter) {
    if(m_pFilterTexture)
        return m_pFilterTexture(pBaseTexture, pPalette, SrcLevel, Filter);
    else
        return D3DERR_INVALIDCALL;
}

DWORD LoadD3DX(CD3DXModule& D3DX, std::string& strD3DXModule) {
    g_aD3DXModules[0].bFound = false;
    for(DWORD i = 1; i < g_dwD3DXModules; i++)
        g_aD3DXModules[0].bFound |= g_aD3DXModules[i].bFound = D3DX.LoadModule(g_aD3DXModules[i].szModuleName);

    if(!g_aD3DXModules[0].bFound)
        return LOAD_FAILED;

    for(g_dwNewestD3DXModule = 1; g_dwNewestD3DXModule < g_dwD3DXModules; g_dwNewestD3DXModule++)
        if(g_aD3DXModules[g_dwNewestD3DXModule].bFound) {
            sprintf_s(szLatestD3DX9, MAX_PATH,"Use latest D3DX9 - %s", g_aD3DXModules[g_dwNewestD3DXModule].szDescription);
            break;
        }

    if(!strD3DXModule.length() == 0  && D3DX.LoadModule(strD3DXModule.c_str())) {
        for(DWORD dwD3DXModule = 0; dwD3DXModule < g_dwD3DXModules; dwD3DXModule++) {
            // need to do this with no case sens
            if(strD3DXModule.compare(g_aD3DXModules[dwD3DXModule].szModuleName) == 0)
                return dwD3DXModule;
        }
    } else {
        strD3DXModule = "";
        if(D3DX.LoadModule(g_aD3DXModules[g_dwNewestD3DXModule].szModuleName))
            return 0;
        else
            return LOAD_FAILED;
    }

    return LOAD_FAILED;
}
