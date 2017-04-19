//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include "cpImageAnalysis.h"

extern bool ProgressCallback(float fProgress, DWORD_PTR pUser1, DWORD_PTR pUser2);

C_AnalysisData ::~C_AnalysisData()
{
}

bool C_AnalysisData::SourceAndDestFileExtMatch(const char *fsource, const char *fdest)
{
    string fsource_extension = boost::filesystem::extension(fsource);
    boost::algorithm::to_lower(fsource_extension);

    string fdest_extension = boost::filesystem::extension(fdest);
    boost::algorithm::to_lower(fdest_extension);

    return (fsource_extension.compare(fdest_extension) == 0);
}

CMipImages* C_AnalysisData::GenerateDiffImage(const char *fsource, const char *fdest)
{
    int testpassed = 0;
    string src_ext = "";
    string des_ext = "";
    src_ext = boost::filesystem::extension(fsource);
    des_ext = boost::filesystem::extension(fdest);

    if (strcmp(src_ext.c_str(), "")==0 || strcmp(des_ext.c_str(), "")==0)
    {
        printf("Error: Source or compressed files cannot be found \n");
        return NULL;
    }

    string results_file = "";
    string fdiff = "";
    boost::algorithm::to_lower(src_ext);
    boost::algorithm::to_lower(des_ext);

    boost::filesystem::path file_path = boost::filesystem::path(fdest);

    //if (!SourceAndDestFileExtMatch(src_ext.c_str(), des_ext.c_str()))
    //{
    //    printf("Error: Both source and destination file types must be the same\n");
    //    return "";
    //}
    Plugin_Canalysis *Plugin_Analysis;
    fdiff = file_path.string();
    int lastindex = fdiff.find_last_of(".");
    fdiff = fdiff.substr(0, lastindex);
    fdiff.append("_diff.bmp");

    results_file = file_path.string();
    int index = results_file.find_last_of(".");
    results_file = results_file.substr(0, index);
    results_file.append("_analysis.xml");

    Plugin_Analysis = reinterpret_cast<Plugin_Canalysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        testpassed = Plugin_Analysis->TC_ImageDiff(fsource, fdest, fdiff.c_str(), (char*)results_file.c_str(), &g_pluginManager, (void**)&diffCMipImages, &ProgressCallback);
        
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

int C_AnalysisData::GeneratePSNRMSEAnalysis(const char *fsource, const char *fdest)
{
    int testpassed = 0;
    string src_ext = "";
    string des_ext = "";
    src_ext = boost::filesystem::extension(fsource);
    des_ext = boost::filesystem::extension(fdest);

    if (strcmp(src_ext.c_str(), "") == 0 || strcmp(des_ext.c_str(), "") == 0)
    {
        printf("Error: Source or compressed files cannot be found \n");
        return NULL;
    }

    string results_file = "";
    boost::algorithm::to_lower(src_ext);
    boost::algorithm::to_lower(des_ext);

    boost::filesystem::path file_path = boost::filesystem::path(fdest);

    //if (!SourceAndDestFileExtMatch(src_ext.c_str(), des_ext.c_str()))
    //{
    //    printf("Error: Both source and destination file types must be the same\n");
    //    return "";
    //}
    Plugin_Canalysis *Plugin_Analysis;

    results_file = file_path.string();
    int index = results_file.find_last_of(".");
    results_file = results_file.substr(0, index);
    results_file.append("_analysis.xml");

    Plugin_Analysis = reinterpret_cast<Plugin_Canalysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        testpassed = Plugin_Analysis->TC_PSNR_MSE(fsource, fdest, (char*)results_file.c_str(), &g_pluginManager, &ProgressCallback);

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

int C_AnalysisData::GenerateSSIMAnalysis(const char *fsource, const char *fdest)
{
    int testpassed = 0;
    string src_ext = "";
    string des_ext = "";
    src_ext = boost::filesystem::extension(fsource);
    des_ext = boost::filesystem::extension(fdest);

    if (strcmp(src_ext.c_str(), "") == 0 || strcmp(des_ext.c_str(), "") == 0)
    {
        printf("Error: Source or compressed files cannot be found \n");
        return NULL;
    }

    string results_file = "";

    boost::algorithm::to_lower(src_ext);
    boost::algorithm::to_lower(des_ext);

    boost::filesystem::path file_path = boost::filesystem::path(fdest);

    //if (!SourceAndDestFileExtMatch(src_ext.c_str(), des_ext.c_str()))
    //{
    //    printf("Error: Both source and destination file types must be the same\n");
    //    return "";
    //}
    Plugin_Canalysis *Plugin_Analysis;

    results_file = file_path.string();
    int index = results_file.find_last_of(".");
    results_file = results_file.substr(0, index);
    results_file.append("_analysis.xml");

    Plugin_Analysis = reinterpret_cast<Plugin_Canalysis*>(g_pluginManager.GetPlugin("IMAGE", "ANALYSIS"));
    if (Plugin_Analysis)
    {
        testpassed = Plugin_Analysis->TC_SSIM(fsource, fdest, (char*)results_file.c_str(), &g_pluginManager, &ProgressCallback);

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