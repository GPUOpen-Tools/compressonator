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

#ifdef __D3D10_1SHADER_H__
#define ID3DSHADERREFLECTION ID3D10ShaderReflection1
#else // !__D3D10_1SHADER_H__
#define ID3DSHADERREFLECTION ID3D10ShaderReflection
#endif // !__D3D10_1SHADER_H__
typedef struct {
    const TCHAR*    szModuleName;
    const TCHAR*    szDescription;
    bool            bFound;
} D3DX10Module;

extern D3DX10Module g_aD3DX10Modules[];
extern const DWORD g_dwD3DX10Modules;

typedef BOOL(WINAPI FAR* D3DXCheckVersionProc)(UINT D3DSdkVersion, UINT D3DX10SdkVersion);

typedef HRESULT(WINAPI FAR* D3DX10CompileFromMemoryProc)(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
        LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult);
typedef HRESULT(WINAPI FAR* D3DX10DisassembleShaderProc)(CONST void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, LPCSTR pComments, ID3D10Blob **ppDisassembly);
typedef HRESULT(WINAPI FAR* D3DX10ReflectShaderProc)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3DSHADERREFLECTION **ppReflector);

class CD3DX10Module : public CModule {
  public:
    CD3DX10Module();
    CD3DX10Module(LPCTSTR lpModuleName);
    virtual ~CD3DX10Module();

    virtual bool LoadModule(LPCTSTR lpModuleName);

    HRESULT D3DXCheckVersion(UINT D3DSdkVersion, UINT D3DX10SdkVersion);
    HRESULT D3DX10CompileFromMemory(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, LPD3D10INCLUDE pInclude,
                                    LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags1, UINT Flags2, ID3DX10ThreadPump* pPump, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs, HRESULT* pHResult);
    HRESULT D3DX10DisassembleShader(CONST void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, LPCSTR pComments, ID3D10Blob **ppDisassembly);
    HRESULT D3DX10ReflectShader(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3DSHADERREFLECTION **ppReflector);

  protected:
    virtual void UnloadModule();

    D3DXCheckVersionProc m_pD3DXCheckVersion;
    D3DX10CompileFromMemoryProc m_pD3DX10CompileFromMemory;
    D3DX10DisassembleShaderProc m_pD3DX10DisassembleShader;
    D3DX10ReflectShaderProc m_pD3DX10ReflectShader;

    CModule* m_pCompileModule; // Since DX SDK August 08, we also need to load D3DCompiler_*.dll
};


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DWORD LoadD3DX10(CD3DX10Module& D3DX10, CString& strD3DX10Module);

#ifdef __cplusplus
}
#endif // __cplusplus
