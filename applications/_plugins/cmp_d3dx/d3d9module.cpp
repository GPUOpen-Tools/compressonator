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

#include "d3d9module.h"

CD3D9Module::CD3D9Module(void) : CModule() {
    m_pDirect3DCreate9 = NULL;
}

CD3D9Module::~CD3D9Module(void) {
    UnloadModule();
}

bool CD3D9Module::LoadModule() {
    return LoadModule("D3D9.DLL");
}

bool CD3D9Module::LoadModule(LPCTSTR lpModuleName) {
    if(__super::LoadModule(lpModuleName)) {
        m_pDirect3DCreate9 = (Direct3DCreate9Proc) GetProcAddress("Direct3DCreate9");
        if(!m_pDirect3DCreate9) {
            UnloadModule();
            return false;
        } else
            return true;
    } else
        return false;
}

void CD3D9Module::UnloadModule() {
    __super::UnloadModule();
    m_pDirect3DCreate9 = NULL;
}

IDirect3D9* CD3D9Module::Direct3DCreate9(UINT SDKVersion) {
    if(m_pDirect3DCreate9)
        return m_pDirect3DCreate9(SDKVersion);
    else
        return NULL;
}
