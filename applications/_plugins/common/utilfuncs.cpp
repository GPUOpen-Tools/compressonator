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
// UtilFuncs.cpp : Source file for utility functions
//
 
#include "utilfuncs.h"

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif // !ASSERT

#ifdef _WIN32
// Windows Header Files:
#include <shobjidl.h>
#include <shlwapi.h>
#include <tchar.h>
#include <windows.h>
#pragma warning(disable : 4201)  //nameless struct/union
#endif

void SwizzleBytes(void *src, unsigned long numBytes) {
    unsigned char tmp[8]; // large enough to hold a double
    unsigned long max, i;

    ASSERT(src);
    if ((numBytes == 0) || (numBytes == 1) || (numBytes > 8))
        return;

    char *pSrc = (char *)src;
    max = numBytes - 1;
    for (i = 0; i < numBytes; i++)
        tmp[(max - i)] = pSrc[i];

    for (i = 0; i < numBytes; i++)
        pSrc[i] = tmp[i];
}

float HalfToFloat(uint16_t h) {
    union FP32 {
        uint32_t u;
        float f;
        struct {
            unsigned Mantissa : 23;
            unsigned Exponent : 8;
            unsigned Sign : 1;
        };
    };

    const FP32 magic = {(254 - 15) << 23};
    const FP32 was_infnan = {(127 + 16) << 23};

    FP32 o;
    o.u = (h & 0x7fff) << 13; // exponent/mantissa bits
    o.f *= magic.f;           // exponent adjust
    if (o.f >= was_infnan.f)  // check Inf/NaN
        o.u |= 255 << 23;
    o.u |= (h & 0x8000) << 16; // sign bit
    return o.f;
}

void getFileNameExt(const char *FilePathName, char *fnameExt, int maxbuffsize) {
#ifdef _WIN32
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char ext[_MAX_EXT];
    char fname[_MAX_FNAME];
    _splitpath_s(FilePathName, drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    snprintf(fnameExt, maxbuffsize, "%s%s", fname, ext);
#else
    std::string fname = CMP_GetFileNameAndExt(FilePathName);
    snprintf(fnameExt, maxbuffsize, "%s", fname.c_str());
#endif
}

#ifdef _WIN32

typedef struct {
    TCHAR *pszName;
    HWND hWnd;
} FTLWData;

#define MAX_WINDOW_TEXT 256

//BOOL CALLBACK FTLWProc(HWND hwnd, LPARAM lParam)
//{
//    if(hwnd && lParam)
//    {
//        FTLWData* pFTLWData = reinterpret_cast<FTLWData*>(lParam);
//
//        TCHAR szWindowText[MAX_WINDOW_TEXT];
//        if(GetWindowText(hwnd, szWindowText, MAX_WINDOW_TEXT))
//        {
//            if(_tcsncmp(szWindowText, pFTLWData->pszName, _tcslen(pFTLWData->pszName)) == 0)
//            {
//                pFTLWData->hWnd = hwnd;
//                return FALSE;
//            }
//        }
//    }
//    return TRUE;
//}

//HWND FindTopLevelWindow(TCHAR* pszName)
//{
//    if(pszName == NULL || *pszName == NULL)
//        return NULL;
//
//    FTLWData ftlwData;
//    ftlwData.pszName = pszName;
//    ftlwData.hWnd = NULL;
//    EnumWindows(FTLWProc, reinterpret_cast<LPARAM>(&ftlwData));
//
//    return ftlwData.hWnd;
//}


bool writeObjFileState(std::string filename, std::string state) {
    std::ofstream file;
    try {
        file.open(filename, std::ios::app);
        file << state << std::endl;
        file.close();
    } catch (...) {
        return false;
    }

    return true;
}

std::string readObjFileState(std::string filename) {
    std::ifstream objfile(filename, std::ios::in);

    if (objfile) {
        //get file size
        objfile.seekg(0, objfile.end);
        std::streamoff length = objfile.tellg();
        objfile.seekg(0, objfile.beg);

        // char c = '\0';
        std::string lastLine = "";
        //loop from the end of the file, skip endline and endfile characters
        //last line suppose consist of 14 characters for cmpcopy of obj
        for (int i = (int)length - 2; i > 0; i -= (int)CMP_STATENUM) {
            objfile.seekg(i);
            //read last line
            std::getline(objfile, lastLine);
            if (lastLine == CMP_COPY) {
                return CMP_COPY;
            }
            if (lastLine == CMP_PROCESSED) {
                return CMP_PROCESSED;
            }
            // break out of the loop if last line is passed (14 characters + 2 (EOF and EOL))
            if (i < length - CMP_STATENUM - 2) {
                return CMP_ORIGINAL;
            }
        }
    }

    return CMP_FILE_ERROR;
}

#endif

#ifdef __AFX_H__
CString GetDirectory(const CString &strPath) {
    int nIndex = strPath.ReverseFind('\\');

    if (nIndex <= 0) // No extension
        return CString(_T(""));
    else {
        CString strDirectory(strPath.Left(nIndex + 1));
        return strDirectory;
    }
}

CString GetFilename(const CString &strPath) {
    int nIndex = strPath.ReverseFind('\\');

    if (nIndex == -1) // No extension
        return CString(_T(""));
    else {
        CString strFilename(strPath.Mid(nIndex + 1));
        return strFilename;
    }
}

CString GetExtension(const CString &strFilename) {
    int nIndex = strFilename.ReverseFind('.');

    if (nIndex == -1) // No extension
        return CString(_T(""));
    else {
        CString strExt(strFilename.Mid(nIndex + 1));
        strExt.MakeLower();
        return strExt;
    }
}

CString RemoveExtension(const CString &strFilename) {
    int nIndex = strFilename.ReverseFind('.');

    if (nIndex == -1) // No extension
        return strFilename;
    else
        return strFilename.Left(nIndex);
}

bool FileExists(const CString &strFilename) {
    if (strFilename.IsEmpty())
        return FALSE;

    // SHGetFileInfo fails when forward slashes are used so replace them with back slashes
    CString strFixedFilename = strFilename;
    strFixedFilename.Replace('/', '\\');

    SHFILEINFO shFileInfo;
    if (SHGetFileInfo(strFixedFilename, 0, &shFileInfo, sizeof(shFileInfo), SHGFI_ATTRIBUTES))
        if (!(shFileInfo.dwAttributes & SFGAO_FOLDER))
            return TRUE;
    return FALSE;
}

bool DirectoryExists(const CString &strDirectory) {
    if (strDirectory.IsEmpty())
        return FALSE;

    // SHGetFileInfo fails when forward slashes are used so replace them with back slashes
    CString strFixedDirectory = strDirectory;
    strFixedDirectory.Replace('/', '\\');

    SHFILEINFO shFileInfo;
    if (SHGetFileInfo(strFixedDirectory, 0, &shFileInfo, sizeof(shFileInfo), SHGFI_ATTRIBUTES))
        if (shFileInfo.dwAttributes & SFGAO_FOLDER)
            return TRUE;
    return FALSE;
}

BOOL IsFullPath(const CString &strPath) {
    return DirectoryExists(GetDirectory(strPath));
}

static const char *f_pszInvalidNames[] = {
    _T("CON"),
    _T("PRN"),
    _T("AUX"),
    _T("CLOCK$"),
    _T("NUL"),
    _T("COM0"),
    _T("COM1"),
    _T("COM2"),
    _T("COM3"),
    _T("COM4"),
    _T("COM5"),
    _T("COM6"),
    _T("COM7"),
    _T("COM8"),
    _T("COM9"),
    _T("LPT0"),
    _T("LPT1"),
    _T("LPT2"),
    _T("LPT3"),
    _T("LPT4"),
    _T("LPT5"),
    _T("LPT6"),
    _T("LPT7"),
    _T("LPT8"),
    _T("LPT9"),
};

static const DWORD f_dwInvalidNames = sizeof(f_pszInvalidNames) / sizeof(f_pszInvalidNames[0]);

BOOL IsValidName(const CString &strName) {
    for (DWORD i = 0; i < f_dwInvalidNames; i++)
        if (strName.CompareNoCase(f_pszInvalidNames[i]) == 0)
            return FALSE;
    return TRUE;
}

BOOL IsValidSubPath(CString strPath) {
    int nToken = max(strPath.ReverseFind('\\'), strPath.ReverseFind('/'));

    if (nToken == strPath.GetLength())
        return IsValidSubPath(strPath.Left(nToken - 1));

    if (nToken > 0) {
        if (!IsValidName(strPath.Mid(nToken + 1)))
            return FALSE;

        return IsValidSubPath(strPath.Left(nToken));
    }

    return TRUE;
}

BOOL IsValidPath(const CString &strPath) {
    if (!PathSearchAndQualify(strPath, NULL, 0))
        return FALSE;

    if (strPath.FindOneOf(_T("\"|<>*?")) != -1)
        return FALSE;

    if (strPath.Find(':', 2) != -1)
        return FALSE;

    return IsValidSubPath(strPath);
}

BOOL CreateDirectory(const CString &strDirectory) {
    int nIndex = strDirectory.ReverseFind('\\');
    if (nIndex == -1)
        nIndex = strDirectory.ReverseFind('/');

    if (nIndex > 2) {
        CString strSubDirectory = strDirectory.Mid(0, nIndex);
        CreateDirectory(strSubDirectory);
    }

    if (CreateDirectory(strDirectory, NULL))
        return TRUE;
    else if (GetLastError() == ERROR_ALREADY_EXISTS)
        return TRUE;
    else
        return FALSE;
}

const TCHAR *GetCurrentFilter(const TCHAR *szFilter, int nIndex) {
    ASSERT(szFilter);
    ASSERT(szFilter[0]);
    ASSERT(nIndex >= 0);

    while (szFilter && szFilter[0] && --nIndex) {
        szFilter += _tcslen(szFilter) + 1;
        szFilter += _tcslen(szFilter) + 1;
    }

    if (nIndex == 0)
        return szFilter;

    return NULL;
}

BOOL GetFilterListFromOFN(CStringList &strFilterList, const OPENFILENAME &ofn) {
    ASSERT(ofn.lpstrFilter);
    ASSERT(ofn.lpstrFilter[0]);
    ASSERT(ofn.nFilterIndex > 0);

    if (ofn.lpstrFilter && ofn.lpstrFilter[0] && ofn.nFilterIndex > 0) {
        TCHAR *pszFilter = const_cast<TCHAR *>(ofn.lpstrFilter);
        int nIndex = ofn.nFilterIndex;
        while (pszFilter && pszFilter[0] && --nIndex) {
            pszFilter += _tcslen(pszFilter) + 1;
            pszFilter += _tcslen(pszFilter) + 1;
        }
        if (pszFilter) {
            pszFilter += _tcslen(pszFilter) + 1;
            TCHAR *pszToken = NULL;
            pszFilter = _tcstok_s(pszFilter, _T(";"), &pszToken);
            while (pszFilter) {
                strFilterList.AddTail(pszFilter);
                pszFilter = _tcstok_s(NULL, _T(";"), &pszToken);
            }

            return TRUE;
        }
    }
    return FALSE;
}

void GetComboText(const CComboBox &combo, CString &rString) {
    if (combo.GetCurSel() != -1)
        combo.GetLBText(combo.GetCurSel(), rString);
    else
        combo.GetWindowText(rString);
}

#ifdef _WIN32
int GetUsableTitleWidth(HWND hWnd) {
    if (hWnd == NULL)
        return 0;

    CRect rect;
    ::GetWindowRect(hWnd, rect);

    WINDOWINFO wi;
    wi.cbSize = sizeof(wi);
    GetWindowInfo(hWnd, &wi);

    int nWidth = rect.Width() - (wi.cxWindowBorders * 2);

    DWORD dwButtonSize = GetSystemMetrics(SM_CXSIZE) + 2;
    if (!(wi.dwExStyle & WS_EX_TOOLWINDOW))
        nWidth -= dwButtonSize;

    if (wi.dwStyle & WS_SYSMENU)
        nWidth -= dwButtonSize;

    if (wi.dwStyle & WS_MAXIMIZEBOX)
        nWidth -= dwButtonSize;

    if (wi.dwStyle & WS_MINIMIZEBOX)
        nWidth -= dwButtonSize;

    return nWidth;
}
#endif
#endif // __AFX_H__
