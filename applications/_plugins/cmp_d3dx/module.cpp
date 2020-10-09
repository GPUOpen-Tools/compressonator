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


#include <windows.h>
#include "module.h"

CModule::CModule(void) {
    m_hModule = NULL;
}

CModule::CModule(LPCTSTR lpModuleName) {
    LoadModule(lpModuleName);
}

CModule::~CModule(void) {
    UnloadModule();
}

bool CModule::LoadModule(LPCTSTR lpModuleName) {
    UnloadModule();
    return ((m_hModule = LoadLibrary(lpModuleName)) != NULL);
}

void CModule::UnloadModule() {
    if(m_hModule) {
        FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

FARPROC CModule::GetProcAddress(LPCSTR lpProcName) {
    if(m_hModule)
        return ::GetProcAddress(m_hModule, lpProcName);
    else
        return NULL;
}
