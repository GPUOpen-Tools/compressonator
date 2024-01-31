//=====================================================================
// Copyright 2016-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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
/// \version 2.20
//
//=====================================================================

#include "cpimageanalysis.h"

#include <cmp_fileio.h>
#include <locale>

extern bool ProgressCallback(float fProgress, CMP_DWORD_PTR pUser1, CMP_DWORD_PTR pUser2);

C_AnalysisData ::~C_AnalysisData()
{
}

static inline void helper_toLower(std::string& str)
{
    for (char& c : str)
        c = tolower(c);
}

bool C_AnalysisData::SourceAndDestFileExtMatch(const char* fsource, const char* fdest)
{
    std::string fsource_extension = CMP_GetJustFileExt(fsource);
    helper_toLower(fsource_extension);

    std::string fdest_extension = CMP_GetJustFileExt(fdest);
    helper_toLower(fdest_extension);

    return (fsource_extension.compare(fdest_extension) == 0);
}

CMipImages* C_AnalysisData::GenerateDiffImage(const char* fsource, const char* fdest)
{
    int         testpassed = 0;
    std::string src_ext    = "";
    std::string des_ext    = "";
    src_ext                = CMP_GetJustFileExt(fsource);
    des_ext                = CMP_GetJustFileExt(fdest);

    if (strcmp(src_ext.c_str(), "") == 0 || strcmp(des_ext.c_str(), "") == 0)
    {
        printf("Error: Source or compressed files cannot be found \n");
        return NULL;
    }

    m_diffFile     = CreateResultsFileName(fsource, fdest, "_diff.bmp");
    m_analysisFile = CreateResultsFileName(fsource, fdest, "_analysis.xml");

    Plugin_Canalysis* Plugin_Analysis;
    Plugin_Analysis = reinterpret_cast<Plugin_Canalysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        testpassed = Plugin_Analysis->TC_ImageDiff(
            fsource, fdest, m_diffFile.c_str(), (char*)m_analysisFile.c_str(), NULL, &g_pluginManager, (void**)&diffCMipImages, &ProgressCallback);

        delete Plugin_Analysis;
        Plugin_Analysis = NULL;
        if (testpassed == -1)
            return NULL;
        else
            return (CMipImages*)diffCMipImages;
    }
    else
    {
        PrintInfo("Plugin Error: image analysis is not loaded (Ouf of Memory)\n");
        return NULL;
    }

    return NULL;
}

std::string C_AnalysisData::CreateResultsFileName(const char* fsource, const char* fdest, const char* type_ext)
{
    std::string results_file;

    std::string file_path    = CMP_GetPath(fdest);
    std::string src_fileName = CMP_GetJustFileName(fsource);
    std::string dst_fileName = CMP_GetJustFileName(fdest);

    results_file = file_path;
    results_file.append("/");
    results_file.append(src_fileName);
    results_file.append("_");
    results_file.append(dst_fileName);
    results_file.append(type_ext);

    return results_file;
}

int C_AnalysisData::GeneratePSNRMSEAnalysis(const char* fsource, const char* fdest)
{
    int         testpassed = 0;
    std::string src_ext    = "";
    std::string des_ext    = "";
    src_ext                = CMP_GetJustFileExt(fsource);
    des_ext                = CMP_GetJustFileExt(fdest);

    if (strcmp(src_ext.c_str(), "") == 0 || strcmp(des_ext.c_str(), "") == 0)
    {
        printf("Error: Source or compressed files cannot be found \n");
        return NULL;
    }

    m_analysisFile = CreateResultsFileName(fsource, fdest, "_analysis.xml");

    Plugin_Canalysis* Plugin_Analysis;
    Plugin_Analysis = reinterpret_cast<Plugin_Canalysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        testpassed = Plugin_Analysis->TC_PSNR_MSE(fsource, fdest, (char*)m_analysisFile.c_str(), &g_pluginManager, &ProgressCallback);

        delete Plugin_Analysis;
        Plugin_Analysis = NULL;
        return testpassed;
    }
    else
    {
        PrintInfo("Plugin Error: image analysis is not loaded (Ouf of Memory)\n");
        return -1;
    }

    return -1;
}

int C_AnalysisData::GenerateSSIMAnalysis(const char* fsource, const char* fdest)
{
    int         testpassed = 0;
    std::string src_ext    = "";
    std::string des_ext    = "";
    src_ext                = CMP_GetJustFileExt(fsource);
    des_ext                = CMP_GetJustFileExt(fdest);

    if (strcmp(src_ext.c_str(), "") == 0 || strcmp(des_ext.c_str(), "") == 0)
    {
        printf("Error: Source or compressed files cannot be found \n");
        return NULL;
    }

    m_analysisFile = CreateResultsFileName(fsource, fdest, "_analysis.xml");

    Plugin_Canalysis* Plugin_Analysis;
    Plugin_Analysis = reinterpret_cast<Plugin_Canalysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        testpassed = Plugin_Analysis->TC_SSIM(fsource, fdest, (char*)m_analysisFile.c_str(), &g_pluginManager, &ProgressCallback);

        delete Plugin_Analysis;
        Plugin_Analysis = NULL;
        return testpassed;
    }
    else
    {
        PrintInfo("Plugin Error: image analysis is not loaded (Ouf of Memory)\n");
        return -1;
    }

    return -1;
}

C_SSIM_Analysis ::~C_SSIM_Analysis()
{
}

C_PSNR_MSE_Analysis ::~C_PSNR_MSE_Analysis()
{
}

C_MSE_PSNR_Analysis ::~C_MSE_PSNR_Analysis()
{
}
