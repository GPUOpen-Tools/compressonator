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

#include "D3DX10Module.h"

TCHAR szLatestD3DX10[256] = "";

D3DX10Module g_aD3DX10Modules[] = {
    {(""), szLatestD3DX10, false},
    {("D3DX10_41.DLL"),    ("D3DX10_41 (March 09 SDK)"), false},
    {("D3DX10_40.DLL"),    ("D3DX10_40 (November 08 SDK)"), false},
    {("D3DX10_39.DLL"),    ("D3DX10_39 (August 08 SDK)"), false},
    {("D3DX10_38.DLL"),    ("D3DX10_38 (June 08 SDK)"), false},
    {("D3DX10_37.DLL"),    ("D3DX10_37 (March 08 SDK)"), false},
    {("D3DX10_36.DLL"),    ("D3DX10_36 (November 07 SDK)"), false},
    {("D3DX10_35.DLL"),    ("D3DX10_35 (August 07 SDK)"), false},
    {("D3DX10_34.DLL"),    ("D3DX10_34 (June 07 SDK)"), false},
    {("D3DX10_33.DLL"),    ("D3DX10_33 (April 07 SDK)"), false},
};
const DWORD g_dwD3DX10Modules = (sizeof(g_aD3DX10Modules) / sizeof(g_aD3DX10Modules[0]));
DWORD g_dwNewestD3DX10Module = 0;

CD3DX10Module::CD3DX10Module(void) : CModule() {
    m_pD3DXCheckVersion = NULL;
    m_pD3DX10CompileFromMemory = NULL;
    m_pD3DX10DisassembleShader = NULL;
    m_pD3DX10ReflectShader = NULL;
    m_pCompileModule = NULL;
}

CD3DX10Module::CD3DX10Module(LPCTSTR lpModuleName) {
    LoadModule(lpModuleName);
}

CD3DX10Module::~CD3DX10Module(void) {
    UnloadModule();

    if ( m_pCompileModule != NULL ) {
        delete m_pCompileModule;
    }
}

bool CD3DX10Module::LoadModule(LPCTSTR lpModuleName) {
    // it is complicated to support ReflectShader call since DX SDK August 08,
    // for now we will disable it.

    if(__super::LoadModule(lpModuleName)) {
        m_pD3DXCheckVersion = (D3DXCheckVersionProc) GetProcAddress("D3DXCheckVersion");
        m_pD3DX10CompileFromMemory = (D3DX10CompileFromMemoryProc) GetProcAddress("D3DX10CompileFromMemory");
        m_pD3DX10DisassembleShader = (D3DX10DisassembleShaderProc) GetProcAddress("D3DX10DisassembleShader");
//        m_pD3DX10ReflectShader = (D3DX10ReflectShaderProc) GetProcAddress("D3DX10ReflectShader");

        if ( !m_pD3DX10CompileFromMemory || !m_pD3DX10DisassembleShader /*|| !m_pD3DX10ReflectShader */) {
            LPCSTR lpCompilerModuleName = _T("");

            if ( lstrcmpi( lpModuleName, _T("D3DX10_41.DLL") ) == 0 ) {
                lpCompilerModuleName = _T("D3DCompiler_41.dll");
            } else if ( lstrcmpi( lpModuleName, _T("D3DX10_40.DLL") ) == 0 ) {
                lpCompilerModuleName = _T("D3DCompiler_40.dll");
            }

            if ( lstrcmpi( lpModuleName, _T("") ) != 0 ) {
                if ( m_pCompileModule == NULL ) {
                    m_pCompileModule = new CModule( lpCompilerModuleName );
                }

                if ( !m_pD3DX10DisassembleShader ) {
                    m_pD3DX10DisassembleShader = (D3DX10DisassembleShaderProc) m_pCompileModule->GetProcAddress("D3DDisassemble");
                }

//            if ( !m_pD3DX10ReflectShader )
//            {
//                m_pD3DX10ReflectShader = (D3DX10ReflectShaderProc) m_pCompileModule->GetProcAddress("D3DReflect");
//            }
            }
        }

        if(/*!m_pD3DXCheckVersion || */!m_pD3DX10CompileFromMemory || !m_pD3DX10DisassembleShader /*|| !m_pD3DX10ReflectShader*/) {
            UnloadModule();
            return false;
        } else
            return true;
    } else
        return false;
}

void CD3DX10Module::UnloadModule() {
    __super::UnloadModule();
    m_pD3DXCheckVersion = NULL;
    m_pD3DX10CompileFromMemory = NULL;
    m_pD3DX10DisassembleShader = NULL;
    m_pD3DX10ReflectShader = NULL;

    if ( m_pCompileModule != NULL ) {
        delete m_pCompileModule;
        m_pCompileModule = NULL;
    }
}

HRESULT CD3DX10Module::D3DXCheckVersion(UINT D3DSdkVersion, UINT D3DX10SdkVersion) {
    if(m_pD3DXCheckVersion)
        return m_pD3DXCheckVersion(D3DSdkVersion, D3DX10SdkVersion);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DX10Module::D3DX10CompileFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
        LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult) {
    if(m_pD3DX10CompileFromMemory)
        return m_pD3DX10CompileFromMemory(pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags1, Flags2, pPump, ppShader, ppErrorMsgs, pHResult);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DX10Module::D3DX10DisassembleShader(CONST void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, LPCSTR pComments, ID3D10Blob **ppDisassembly) {
    if(m_pD3DX10DisassembleShader)
        return m_pD3DX10DisassembleShader(pShader, BytecodeLength, EnableColorCode, pComments, ppDisassembly);
    else
        return D3DERR_INVALIDCALL;
}

HRESULT CD3DX10Module::D3DX10ReflectShader(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3DSHADERREFLECTION **ppReflector) {
    if(m_pD3DX10ReflectShader)
        return m_pD3DX10ReflectShader(pShaderBytecode, BytecodeLength, ppReflector);
    else
        return D3DERR_INVALIDCALL;
}


DWORD LoadD3DX10(CD3DX10Module& D3DX10, CString& strD3DX10Module) {
    g_aD3DX10Modules[0].bFound = false;
    for(DWORD i = 1; i < g_dwD3DX10Modules; i++)
        g_aD3DX10Modules[0].bFound |= g_aD3DX10Modules[i].bFound = D3DX10.LoadModule(g_aD3DX10Modules[i].szModuleName);

    if(!g_aD3DX10Modules[0].bFound)
        return LOAD_FAILED;

    for(g_dwNewestD3DX10Module = 1; g_dwNewestD3DX10Module < g_dwD3DX10Modules; g_dwNewestD3DX10Module++)
        if(g_aD3DX10Modules[g_dwNewestD3DX10Module].bFound) {
            _stprintf(szLatestD3DX10, _T("Use latest D3DX10 - %s"), g_aD3DX10Modules[g_dwNewestD3DX10Module].szDescription);
            break;
        }

    if(!strD3DX10Module.IsEmpty() && D3DX10.LoadModule(strD3DX10Module)) {
        for(DWORD dwD3DXModule = 0; dwD3DXModule < g_dwD3DX10Modules; dwD3DXModule++)
            if(strD3DX10Module.CompareNoCase(g_aD3DX10Modules[dwD3DXModule].szModuleName) == 0)
                return dwD3DXModule;
    } else {
        strD3DX10Module = "";
        if(D3DX10.LoadModule(g_aD3DX10Modules[g_dwNewestD3DX10Module].szModuleName))
            return 0;
        else
            return LOAD_FAILED;
    }

    return LOAD_FAILED;
}
