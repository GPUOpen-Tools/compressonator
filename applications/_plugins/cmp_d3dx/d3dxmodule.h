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

#include "string"
#include "d3dx9.h"
#include "module.h"

typedef struct {
    const TCHAR*    szModuleName;
    const TCHAR*    szDescription;
    bool            bFound;
} D3DXModule;

extern D3DXModule g_aD3DXModules[];
extern const DWORD g_dwD3DXModules;

typedef HRESULT(WINAPI FAR* D3DXCreateBufferProc)(DWORD NumBytes, LPD3DXBUFFER *ppBuffer);
typedef HRESULT(WINAPI FAR* D3DXCompileShaderProc)(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
        LPD3DXINCLUDE pInclude, LPCSTR pFunctionName, LPCSTR pProfile,
        DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs,
        LPD3DXCONSTANTTABLE* ppConstantTable);
typedef HRESULT(WINAPI FAR* D3DXAssembleShaderProc)(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines,
        LPD3DXINCLUDE pInclude, DWORD Flags, LPD3DXBUFFER* ppShader,
        LPD3DXBUFFER* ppErrorMsgs);
typedef HRESULT(WINAPI FAR* D3DXDisassembleShaderProc)(CONST DWORD* pShader, BOOL EnableColorCode, LPCSTR pComments,
        LPD3DXBUFFER* ppDisassembly);
typedef HRESULT(WINAPI FAR* D3DXFilterTextureProc)(LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY* pPalette,
        UINT SrcLevel, DWORD Filter);

class CD3DXModule : public CModule {
  public:
    CD3DXModule();
    CD3DXModule(LPCTSTR lpModuleName);
    virtual ~CD3DXModule();

    virtual bool LoadModule(LPCTSTR lpModuleName);

    HRESULT D3DXCreateBuffer(DWORD NumBytes, LPD3DXBUFFER *ppBuffer);

    HRESULT D3DXCompileShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude,
                              LPCSTR pFunctionName, LPCSTR pProfile, DWORD Flags, LPD3DXBUFFER* ppShader,
                              LPD3DXBUFFER* ppErrorMsgs, LPD3DXCONSTANTTABLE* ppConstantTable);
    HRESULT D3DXAssembleShader(LPCSTR pSrcData, UINT SrcDataLen, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude,
                               DWORD Flags, LPD3DXBUFFER* ppShader, LPD3DXBUFFER* ppErrorMsgs);
    HRESULT D3DXDisassembleShader(CONST DWORD* pShader, BOOL EnableColorCode, LPCSTR pComments, LPD3DXBUFFER* ppDisassembly);

    HRESULT D3DXFilterTexture(LPDIRECT3DBASETEXTURE9 pBaseTexture, CONST PALETTEENTRY* pPalette, UINT SrcLevel, DWORD Filter);

  protected:
    virtual void UnloadModule();

    D3DXCreateBufferProc m_pCreateBuffer;
    D3DXCompileShaderProc m_pCompileShader;
    D3DXAssembleShaderProc m_pAssembleShader;
    D3DXDisassembleShaderProc m_pDisassembleShader;
    D3DXFilterTextureProc m_pFilterTexture;
};


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

DWORD LoadD3DX(CD3DXModule& D3DX, std::string& strD3DXModule);

#ifdef __cplusplus
}
#endif // __cplusplus
