//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
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

// Windows Header Files:
#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include "CAnalysis.h"
#include "PluginManager.h"
#include "TC_PluginInternal.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "Compressonator.h"

// File system
#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp> 

// for XML file processing
#include <boost/property_tree/xml_parser.hpp> 
#include <boost/property_tree/ptree.hpp> 
#include <boost/foreach.hpp> 

using namespace std;

#ifdef _DEBUG
    #define CMP_EXTERNAL_LibExt    "d.lib"
#else
    #define CMP_EXTERNAL_LibExt    ".lib"
#endif

#ifdef USE_OPENCV
#define OpenCV_core_Lib     "opencv_core" CVAUX_STR(CV_VERSION_EPOCH) CVAUX_STR(CV_VERSION_MAJOR) CVAUX_STR(CV_VERSION_MINOR) CMP_EXTERNAL_LibExt
#define OpenCV_highgui_Lib  "opencv_highgui" CVAUX_STR(CV_VERSION_EPOCH) CVAUX_STR(CV_VERSION_MAJOR) CVAUX_STR(CV_VERSION_MINOR) CMP_EXTERNAL_LibExt
#define OpenCV_imgproc_Lib  "opencv_imgproc" CVAUX_STR(CV_VERSION_EPOCH) CVAUX_STR(CV_VERSION_MAJOR) CVAUX_STR(CV_VERSION_MINOR) CMP_EXTERNAL_LibExt

#pragma comment(lib,OpenCV_core_Lib)
#pragma comment(lib,OpenCV_highgui_Lib)
#pragma comment(lib,OpenCV_imgproc_Lib)
#endif


#define Qt5_core_Lib        "Qt5Core" CMP_EXTERNAL_LibExt
#define Qt5_guid_Lib        "Qt5Gui" CMP_EXTERNAL_LibExt
#define Qt5_widgets_Lib     "Qt5Widgets" CMP_EXTERNAL_LibExt

#pragma comment(lib,Qt5_core_Lib)
#pragma comment(lib,Qt5_guid_Lib)
#pragma comment(lib,Qt5_widgets_Lib)


#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_Canalysis)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("ANALYSIS")
#else
void *make_Plugin_CAnalysis() { return new Plugin_Canalysis; }
#endif

#define TEST_TOLERANCE 5 //for 4x4 test block omly

Plugin_Canalysis::Plugin_Canalysis()
{ 
    //default tolerance values
    tolerance_mse     = 1.0100;
    tolerance_psnr    = 0.9900; 
    tolerance_psnrb   = 0.9900;
    tolerance_psnrg   = 0.9900;
    tolerance_psnrr   = 0.9900;
    tolerance_ssim    = 0.9995;
    tolerance_ssimb   = 0.9995; 
    tolerance_ssimg   = 0.9995;
    tolerance_ssimr   = 0.9995;

    m_imageloader     = NULL;
    m_MipSrcImages    = NULL;
    m_MipDestImages   = NULL;
    m_MipDiffImages   = NULL;
    m_RGBAChannels    = 0b1111;

}

Plugin_Canalysis::~Plugin_Canalysis()
{ 
    if (m_MipSrcImages)
        m_imageloader->clearMipImages(&m_MipSrcImages);

    if (m_MipDestImages)
        m_imageloader->clearMipImages(&m_MipDestImages);

    if (m_imageloader)
        delete m_imageloader;
}

int Plugin_Canalysis::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

string f2Str(float data, int prec)
{
    std::string s(16, '\0');
    char pstr[6] = "%.xf";
    pstr[2] = 48+(prec%10); // prec is limited from 0..9
    std::snprintf(&s[0], s.size(), pstr, data);
    return s;
}

void Plugin_Canalysis::write(REPORT_DATA data, char *resultsFile, char option)
{
    using boost::property_tree::ptree;
    ptree pt;
    ptree pt_gold;
    string diffName = "";
    string diffNodeName = "";
    bool nodeExist = false;
    
    if (m_srcFile.size() > 0 && m_destFile.size() > 0)
    {
        boost::filesystem::path src(m_srcFile);
        diffName = src.stem().generic_string();
        diffName.append("_");

        boost::filesystem::path dest(m_destFile);
        diffName.append(dest.stem().generic_string());
        diffNodeName = "ANALYSIS.";
        diffNodeName.append(diffName);
    }

    if ((CMP_FileExists(resultsFile)))
    {

        std::ifstream input(resultsFile);
        read_xml(input, pt);

        // traverse pt
        BOOST_FOREACH(ptree::value_type &v, pt.get_child("ANALYSIS"))
        {
            if (v.first == "DATA")  //gui 
            {
                if (option == 's') //ssim
                {
                    v.second.put("SSIM"         , f2Str(data.SSIM,      4).c_str());
                    v.second.put("SSIM_BLUE"    , f2Str(data.SSIM_Blue, 4).c_str());
                    v.second.put("SSIM_GREEN"   , f2Str(data.SSIM_Green,4).c_str());
                    v.second.put("SSIM_RED"     , f2Str(data.SSIM_Red,  4).c_str());
                    nodeExist = true;
                }
                else if (option == 'p') //psnr
                {
                    v.second.put("MSE"          , f2Str(data.MSE, 1).c_str());
                    v.second.put("PSNR"         , f2Str(data.PSNR, 1).c_str());
                    v.second.put("PSNR_BLUE"    , f2Str(data.PSNR_Blue, 1).c_str());
                    v.second.put("PSNR_GREEN"   , f2Str(data.PSNR_Green, 1).c_str());
                    v.second.put("PSNR_RED"     , f2Str(data.PSNR_Red, 1).c_str());
                    nodeExist = true;
                }
                else 
                {
                    v.second.put("MSE"          , f2Str(data.MSE, 1).c_str());
                    v.second.put("SSIM"         , f2Str(data.SSIM,      4).c_str());
                    v.second.put("SSIM_BLUE"    , f2Str(data.SSIM_Blue, 4).c_str());
                    v.second.put("SSIM_GREEN"   , f2Str(data.SSIM_Green,4).c_str());
                    v.second.put("SSIM_RED"     , f2Str(data.SSIM_Red,  4).c_str());
                    v.second.put("PSNR"         , f2Str(data.PSNR, 1).c_str());
                    v.second.put("PSNR_BLUE"    , f2Str(data.PSNR_Blue, 1).c_str());
                    v.second.put("PSNR_GREEN"   , f2Str(data.PSNR_Green, 1).c_str());
                    v.second.put("PSNR_RED"     , f2Str(data.PSNR_Red, 1).c_str());
                    nodeExist = true;
                    break;
                }
            }
            else if (v.first == diffName) //cmdline, node exist
            {
                v.second.put("MSE"          , f2Str(data.MSE, 1).c_str());
                v.second.put("SSIM"         , f2Str(data.SSIM,4).c_str());
                v.second.put("SSIM_BLUE"    , f2Str(data.SSIM_Blue, 4).c_str());
                v.second.put("SSIM_GREEN"   , f2Str(data.SSIM_Green, 4).c_str());
                v.second.put("SSIM_RED"     , f2Str(data.SSIM_Red, 4).c_str());
                v.second.put("PSNR"         , f2Str(data.PSNR, 1).c_str());
                v.second.put("PSNR_BLUE"    , f2Str(data.PSNR_Blue, 1).c_str());
                v.second.put("PSNR_GREEN"   , f2Str(data.PSNR_Green, 1).c_str());
                v.second.put("PSNR_RED"     , f2Str(data.PSNR_Red, 1).c_str());
                if (data.srcdecodePattern[0]!='\0')
                    v.second.put("FAIL_SRCDECODEPATTERN", data.srcdecodePattern);
                else
                    v.second.erase("FAIL_SRCDECODEPATTERN");
                if (data.destdecodePattern[0] != '\0')
                    v.second.put("FAIL_DESTDECODEPATTERN", data.destdecodePattern);
                else
                    v.second.erase("FAIL_DESTDECODEPATTERN");

                nodeExist = true;
                break;
            }
        }

        if (!nodeExist) //cmdline, node not exist
        {
            ptree & node = pt.add(diffNodeName, "");
            node.add("MSE"          , f2Str(data.MSE, 1).c_str());
            node.add("SSIM"         , f2Str(data.SSIM,4).c_str());
            node.add("SSIM_BLUE"    , f2Str(data.SSIM_Blue, 4).c_str());
            node.add("SSIM_GREEN"   , f2Str(data.SSIM_Green, 4).c_str());
            node.add("SSIM_RED"     , f2Str(data.SSIM_Red, 4).c_str());
            node.add("PSNR"         , f2Str(data.PSNR, 1).c_str());
            node.add("PSNR_BLUE"    , f2Str(data.PSNR_Blue, 1).c_str());
            node.add("PSNR_GREEN"   , f2Str(data.PSNR_Green, 1).c_str());
            node.add("PSNR_RED"     , f2Str(data.PSNR_Red, 1).c_str());
            if (data.srcdecodePattern[0] != '\0')
                node.add("FAIL_SRCDECODEPATTERN", data.srcdecodePattern);
           
            if (data.destdecodePattern[0] != '\0')
                node.add("FAIL_DESTDECODEPATTERN", data.destdecodePattern);
        }
    }
    else //file not exist
    {
        if (option == 's' || option == 'p')  //only gui will have option ssim or psnr
        {
            ptree & node = pt.add("ANALYSIS.DATA", "");
            node.add("MSE"          , f2Str(data.MSE, 1).c_str());
            node.add("SSIM"         , f2Str(data.SSIM,4).c_str());
            node.add("SSIM_BLUE"    , f2Str(data.SSIM_Blue, 4).c_str());
            node.add("SSIM_GREEN"   , f2Str(data.SSIM_Green, 4).c_str());
            node.add("SSIM_RED"     , f2Str(data.SSIM_Red, 4).c_str());
            node.add("PSNR"         , f2Str(data.PSNR, 1).c_str());
            node.add("PSNR_BLUE"    , f2Str(data.PSNR_Blue, 1).c_str());
            node.add("PSNR_GREEN"   , f2Str(data.PSNR_Green, 1).c_str());
            node.add("PSNR_RED"     , f2Str(data.PSNR_Red, 1).c_str());
        }
        else //cmdline
        {
            ptree & node = pt.add(diffNodeName, "");
            node.add("MSE"          , f2Str(data.MSE, 1).c_str());
            node.add("SSIM"         , f2Str(data.SSIM,4).c_str());
            node.add("SSIM_BLUE"    , f2Str(data.SSIM_Blue, 4).c_str());
            node.add("SSIM_GREEN"   , f2Str(data.SSIM_Green, 4).c_str());
            node.add("SSIM_RED"     , f2Str(data.SSIM_Red, 4).c_str());
            node.add("PSNR"         , f2Str(data.PSNR, 1).c_str());
            node.add("PSNR_BLUE"    , f2Str(data.PSNR_Blue, 1).c_str());
            node.add("PSNR_GREEN"   , f2Str(data.PSNR_Green, 1).c_str());
            node.add("PSNR_RED"     , f2Str(data.PSNR_Red, 1).c_str());
            if (data.srcdecodePattern[0] != '\0')
                node.add("FAIL_SRCDECODEPATTERN", data.srcdecodePattern);

            if (data.destdecodePattern[0] != '\0')
                node.add("FAIL_DESTDECODEPATTERN", data.destdecodePattern);
        }
    }

    try {
        std::ofstream output(resultsFile);
        write_xml(output, pt);
    }
    catch (std::exception const&  ex)
    {
        printf("Can't write xml. %s", ex.what());
        return;
    }


    boost::filesystem::path result(resultsFile);
    int lastindex = (int)result.generic_string().find_last_of("/");
    string goldFile = result.generic_string().substr(0, lastindex + 1);
    goldFile.append("golden.xml");

    string toleranceFile = result.generic_string().substr(0, lastindex + 1);
    toleranceFile.append("analysis_tolerance.xml");

    if ((CMP_FileExists(toleranceFile)))
    {
        ptree pt_tolerance;
        try {
            std::ifstream inputtolerance(toleranceFile);
            read_xml(inputtolerance, pt_tolerance);
        }
        catch (std::exception const&  ex)
        {
            printf("Can't read analysis_tolerance.xml. %s", ex.what());
            return;
        }

        BOOST_FOREACH(ptree::value_type &v, pt_tolerance.get_child("ANALYSIS"))
        {
            if (v.first == "TOLERANCE")  
            {
                tolerance_mse = 1.0000 + v.second.get<double>("MSE");
                tolerance_ssim = 1.0000 - v.second.get<double>("SSIM");
                tolerance_ssimb = 1.0000 - v.second.get<double>("SSIM_BLUE");
                tolerance_ssimg = 1.0000 - v.second.get<double>("SSIM_GREEN");
                tolerance_ssimr = 1.0000 - v.second.get<double>("SSIM_RED");
                tolerance_psnr = 1.0000 - v.second.get<double>("PSNR");
                tolerance_psnrb = 1.0000 - v.second.get<double>("PSNR_BLUE");
                tolerance_psnrg = 1.0000 - v.second.get<double>("PSNR_GREEN");
                tolerance_psnrr = 1.0000 - v.second.get<double>("PSNR_RED");
                break;
            }
        }
    }
    //analysis against golden.xml (gold reference) and reporting result
    if ((CMP_FileExists(goldFile)))
    {
        try {
            std::ifstream inputgold(goldFile);
            read_xml(inputgold, pt_gold);

            std::ifstream input(resultsFile);
            read_xml(input, pt);
        }
        catch (std::exception const&  ex)
        {
            printf("Can't read golden.xml. %s", ex.what());
            return;
        }

        int pass = 0;
        int fail = 0;
        int total = 0;
        bool hasFail = false;
        // traverse pt for both golden and test files
        BOOST_FOREACH(ptree::value_type &v_gold, pt_gold.get_child("ANALYSIS"))
        {
            BOOST_FOREACH(ptree::value_type &v, pt.get_child("ANALYSIS"))
            {
                if (boost::iequals(v_gold.first, v.first)) //diff file node exist on both
                {
                    total++;
                    v.second.put("MSE_GOLDREF", v_gold.second.get<double>("MSE"));
                    if (v.second.get<double>("MSE") <= (v_gold.second.get<double>("MSE") *tolerance_mse))
                    {
                        v.second.put("MSE_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("MSE_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("SSIM_GOLDREF", v_gold.second.get<double>("SSIM"));
                    if (v.second.get<double>("SSIM") >= (v_gold.second.get<double>("SSIM") *tolerance_ssim))
                    {
                        v.second.put("SSIM_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("SSIM_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("SSIM_BLUE_GOLDREF", v_gold.second.get<double>("SSIM_BLUE"));
                    if (v.second.get<double>("SSIM_BLUE") >= (v_gold.second.get<double>("SSIM_BLUE") *tolerance_ssimb))
                    {
                        v.second.put("SSIM_BLUE_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("SSIM_BLUE_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("SSIM_GREEN_GOLDREF", v_gold.second.get<double>("SSIM_GREEN"));
                    if (v.second.get<double>("SSIM_GREEN") >= (v_gold.second.get<double>("SSIM_GREEN") *tolerance_ssimg))
                    {
                        v.second.put("SSIM_GREEN_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("SSIM_GREEN_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("SSIM_RED_GOLDREF", v_gold.second.get<double>("SSIM_RED"));
                    if (v.second.get<double>("SSIM_RED") >= (v_gold.second.get<double>("SSIM_RED") *tolerance_ssimr))
                    {
                        v.second.put("SSIM_RED_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("SSIM_RED_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("PSNR_GOLDREF", v_gold.second.get<double>("PSNR"));
                    if (v.second.get<double>("PSNR") >= (v_gold.second.get<double>("PSNR") *tolerance_psnr))
                    {
                        v.second.put("PSNR_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("PSNR_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("PSNR_BLUE_GOLDREF", v_gold.second.get<double>("PSNR_BLUE"));
                    if (v.second.get<double>("PSNR_BLUE") >= (v_gold.second.get<double>("PSNR_BLUE") *tolerance_psnrb))
                    {
                        v.second.put("PSNR_BLUE_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("PSNR_BLUE_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("PSNR_GREEN_GOLDREF", v_gold.second.get<double>("PSNR_GREEN"));
                    if (v.second.get<double>("PSNR_GREEN") >= (v_gold.second.get<double>("PSNR_GREEN") *tolerance_psnrg))
                    {
                        v.second.put("PSNR_GREEN_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("PSNR_GREEN_RESULT", "FAIL");
                        hasFail = true;
                    }

                    v.second.put("PSNR_RED_GOLDREF", v_gold.second.get<double>("PSNR_RED"));
                    if (v.second.get<double>("PSNR_RED") >= (v_gold.second.get<double>("PSNR_RED") *tolerance_psnrr))
                    {
                        v.second.put("PSNR_RED_RESULT", "PASS");
                    }
                    else
                    {
                        v.second.put("PSNR_RED_RESULT", "FAIL");
                        hasFail = true;
                    }

                    //if one of the above statistic fail, the test case (per image) is fail
                    if (hasFail)
                    {
                        hasFail = false;
                        fail++;
                    }
                    else if (!hasFail)
                    {
                        pass++;
                    }
                }
            }
        }

        if (option != 's' && option != 'p')  //only gui will have this option - s= ssim or p= psnr
        {
            ptree & node = pt.put("ANALYSIS.TEST_RESULT", "");
            node.put("PASS", pass);
            node.put("FAIL", fail);
            node.put("TOTAL", total);

            try {
                std::ofstream output(resultsFile);
                write_xml(output, pt);
            }
            catch (std::exception const&  ex)
            {
                printf("Can't write xml. %s", ex.what());
                return;
            }
        }
    }

}

void checkPattern(int* r, int* g, int* b, char *pattern, CMP_FORMAT format)
{
    if (format == CMP_FORMAT_ATI1N) //BC4
    {
        // only test on red channel output
        if (*r <= (0 + TEST_TOLERANCE))
            *pattern = '2';
        else if (*r > 0)
            *pattern = '1';
        else
            *pattern = '8';
    }
    else
    {
        // 1=Red 2=Green 3=R+G 4=Blue 5=R+B 6=G+B 7=R+G+B
        if (*r <= (0 + TEST_TOLERANCE) && *g <= (0 + TEST_TOLERANCE) && *b <= (0 + TEST_TOLERANCE))
            *pattern = '0';
        else if (*r > 0 && *g <= (0 + TEST_TOLERANCE) && *b <= (0 + TEST_TOLERANCE))
            *pattern = '1';
        else if (*r <= (0 + TEST_TOLERANCE) && *g > 0 && *b <= (0 + TEST_TOLERANCE))
            *pattern = '2';
        else if (*r <= (0 + TEST_TOLERANCE) && *g <= (0 + TEST_TOLERANCE) && *b > 0)
            *pattern = '4';
        else if (*r > 0 && *g > 0 && *b <= (0 + TEST_TOLERANCE))
            *pattern = '3';
        else if (*r > 0 && *g <= (0 + TEST_TOLERANCE) && *b > 0)
            *pattern = '5';
        else if (*r <= (0 + TEST_TOLERANCE) && *g > 0 && *b > 0)
            *pattern = '6';
        else if (*r > 0 && *g > 0 && *b > 0)
            *pattern = '7';
        else
            *pattern = '8';
    }

}

void  Plugin_Canalysis::generateBCtestResult(QImage *src, QImage *dest, REPORT_DATA &myReport)
{
    int srcR = 0, srcG = 0, srcB = 0;
    int destR = 0, destG = 0, destB = 0;

    if (m_MipDestImages != NULL)
        m_Compressformat = m_MipDestImages->mipset->m_format;
    char srcPattern[17];
    char destPattern[17];
    memset(srcPattern, 0, 17);
    memset(destPattern, 0, 17);

    //form decode string for src amd dest image
    int index = 0;
    for (int y = 0; y < 4; y++) {
        for (int x = 0; x < 4; x++) {
            QRgb srcPixel = src->pixel(x, y);
            QRgb dstPixel = dest->pixel(x, y);
            srcR = qRed(srcPixel);
            srcG = qGreen(srcPixel);
            srcB = qBlue (srcPixel);

            destR = qRed  (dstPixel);
            destG = qGreen(dstPixel);
            destB = qBlue (dstPixel);

            checkPattern(&srcR, &srcG, &srcB, &srcPattern[index], m_Compressformat);
            checkPattern(&destR, &destG, &destB, &destPattern[index], m_Compressformat);
            index++;
        }
    }

    //compare and if not equal , write to myReport.decodePattern
    if (strcmp(srcPattern, destPattern) != 0)
    {
        strncpy(myReport.srcdecodePattern, srcPattern, 16);
        strncpy(myReport.destdecodePattern, destPattern, 16);
    }
}

#ifdef USE_OPENCV
bool Plugin_Canalysis::psnr(QImage *src, Mat srcimg, QImage *dest, Mat destimg, REPORT_DATA &myReport, CMP_Feedback_Proc pFeedbackProc)
{
    double bMSE = 0, gMSE = 0, rMSE = 0, MSE = 0;
    double MAX = 255.0; // Maximum possible pixel range. For our BMP's, which have 8 bits, it's 255.
    int w = src->width();
    int h = src->height();
    float fProgress = 0.0;

    if (w == 4 && h == 4)
        generateBCtestResult(src, dest, myReport);

    unsigned char dR,dG,dB;
    unsigned char sR,sG,sB;

    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {

            sB = qBlue (src->pixel(x, y));
            sG = qGreen(src->pixel(x, y));
            sR = qRed  (src->pixel(x, y));

            dB = qBlue (dest->pixel(x, y));
            dG = qGreen(dest->pixel(x, y));
            dR = qRed  (dest->pixel(x, y));

            rMSE += pow(sR - dR, 2.0);
            gMSE += pow(sG - dG, 2.0);
            bMSE += pow(sB - dB, 2.0);

        }

        if (pFeedbackProc)
        {
            fProgress = 100.f * (y * w) / (w * h);
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                printf("Analysis canceled!\n");
                return false; //abort
            }
        }
    }


    //------------------- Code needs validation
    // double mse;
    // double psnr;
    // getMSE_PSNR( srcimg, destimg, mse, psnr);
    //-------------------

    bMSE *= (1.0 / (w*h));
    gMSE *= (1.0 / (w*h));
    rMSE *= (1.0 / (w*h));

    myReport.PSNR_Blue  = -1;
    myReport.PSNR_Green = -1;
    myReport.PSNR_Red   = -1;

    switch (m_RGBAChannels)
    {
        case 0b0001:
                    myReport.MSE        = rMSE;
                    if (rMSE != 0)
                        myReport.PSNR_Red = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(rMSE);
                    if (myReport.MSE != 0)
                        myReport.PSNR = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(myReport.MSE);
                    break;
        case 0b0011:
                    myReport.MSE        = (gMSE + rMSE) / 2;
                    if (gMSE != 0)
                        myReport.PSNR_Green = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(gMSE);
                    if (rMSE != 0)
                        myReport.PSNR_Red = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(rMSE);
                    if (myReport.MSE != 0)
                        myReport.PSNR = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(myReport.MSE);
                    break;
        default:
                    myReport.MSE        = (bMSE + gMSE + rMSE) / 3;
                    if (bMSE != 0)
                        myReport.PSNR_Blue = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(bMSE);
                    if (gMSE != 0)
                        myReport.PSNR_Green = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(gMSE);
                    if (rMSE != 0)
                        myReport.PSNR_Red = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(rMSE);
                    if (myReport.MSE != 0)
                        myReport.PSNR = 20 * log10(pow(2.0, 8.0) - 1) - 10 * log10(myReport.MSE);
                    break;
    }

    return (myReport.PSNR != -1);
}
#endif

void Plugin_Canalysis::setActiveChannels()
{
    if (m_MipDestImages)
    {
       if (m_MipDestImages->mipset)
       {
        switch(m_MipDestImages->mipset->m_format)
        {
            case CMP_FORMAT_ATI1N:
            case CMP_FORMAT_BC4: // All channels are used and equal, Red is used as active channel
                                 m_RGBAChannels = 0b0001;  // R
                                 break;
            case CMP_FORMAT_ATI2N_XY:
            case CMP_FORMAT_BC5: // Only Red & Green channels active
                                 m_RGBAChannels = 0b0011;  // ABGR
                                 break;
            default:
                                 m_RGBAChannels = 0b0111;  // BGR , alpha skipped
                                 break;
        }
       }
    }
}


void Plugin_Canalysis::processSSIMResults()
{
   switch (m_RGBAChannels)
   {
       case 0b0001:
           report.data.SSIM_Red    = m_SSIM.val[2];
           report.data.SSIM = report.data.SSIM_Red;
           break;
       case 0b0011:
           report.data.SSIM_Green  = m_SSIM.val[1];
           report.data.SSIM_Red    = m_SSIM.val[2];
           report.data.SSIM = (report.data.SSIM_Green + report.data.SSIM_Red) / 2;
           break;
       default:
           report.data.SSIM_Blue   = m_SSIM.val[0];
           report.data.SSIM_Green  = m_SSIM.val[1];
           report.data.SSIM_Red    = m_SSIM.val[2];
           report.data.SSIM = (report.data.SSIM_Blue + report.data.SSIM_Green + report.data.SSIM_Red) / 3;
           break;
   }
}



int Plugin_Canalysis::TC_ImageDiff(const char * in1, 
                                   const char * in2, 
                                   const char *out, 
                                   char *resultsFile, 
                                   void *usrAnalysisData, 
                                   void *pluginManager, 
                                   void **cmipImages, 
                                   CMP_Feedback_Proc pFeedbackProc)
{
    if (pluginManager == NULL) return -1;

    CMP_ANALYSIS_DATA *analysisData = (CMP_ANALYSIS_DATA *) usrAnalysisData;

    if (m_imageloader == NULL)
        m_imageloader = new CImageLoader(pluginManager);


    QImage* srcImage  = NULL;
    QImage* destImage = NULL;
    QImage* diffImage = NULL;

    if (m_imageloader)
    {

        if (m_MipSrcImages)
            m_imageloader->clearMipImages(&m_MipSrcImages);
        if (m_MipDestImages)
            m_imageloader->clearMipImages(&m_MipDestImages);

        m_MipSrcImages  = m_imageloader->LoadPluginImage(QString::fromUtf8(in1));
        m_MipDestImages = m_imageloader->LoadPluginImage(QString::fromUtf8(in2));
    }

    if (m_MipSrcImages != NULL && m_MipDestImages != NULL)
    {
        setActiveChannels();
        // Analysis is only on top MipLevel and first cubemap face!
        // Need to update the code to handle all faces of cubemaps
        if (m_MipSrcImages->QImage_list[0].count() >0)
        {
            srcImage = m_MipSrcImages->QImage_list[0][0];
        }
        else
        {
            printf("Error: Source Image cannot be loaded\n");
            return -1;
        }

        if (m_MipDestImages->QImage_list[0].count() >0)
        {
            destImage = m_MipDestImages->QImage_list[0][0];
        }
        else
        {
            printf("Error: Dest Image cannot be loaded\n");
            return -1;
        }
    }
    else
    {
        printf("Error: Image(s) cannot be loaded\n");
        return -1;
    }


    if (srcImage != NULL && destImage != NULL)
    {
        m_srcFile = in1;
        m_destFile = in2;

        int w = srcImage->width() == destImage->width() ? srcImage->width() : -1;
        int h = srcImage->height() == destImage->height() ? srcImage->height() : -1;

        if (w == -1 || h == -1)
        {
            printf("Error: Both images must be same size\n");
            return -1;
        }

        if (cmipImages == NULL) //cmdline enable both ssim and psnr
        {

#ifdef USE_OPENCV
           cv::Mat srcimg  = QtOcv::image2Mat(*srcImage);
           cv::Mat destimg = QtOcv::image2Mat(*destImage);
           if (!&srcimg || !&destimg)
           {
               printf("Error: Images fail to allocate for ssim analysis\n");
               return -1;
           }

           bool testpassed = psnr(srcImage, srcimg, destImage, destimg, report.data);
           if (!testpassed)
           {
               printf("Error: Images analysis fail\n");
               return -1;
           }

           m_SSIM = getSSIM(srcimg, destimg, pFeedbackProc);
           processSSIMResults();

           srcimg.release();
           destimg.release();

#endif

           // If we have a report file write to it
           if ((strcmp(resultsFile, "") != 0))
                   write(report.data, resultsFile, 'a');

           if (analysisData)
           {
               analysisData->SSIM       = report.data.SSIM;
               analysisData->SSIM_Red   = report.data.SSIM_Red;
               analysisData->SSIM_Green = report.data.SSIM_Green;
               analysisData->SSIM_Blue  = report.data.SSIM_Blue;
               analysisData->PSNR       = report.data.PSNR;
               analysisData->PSNR_Red   = report.data.PSNR_Red;
               analysisData->PSNR_Green = report.data.PSNR_Green;
               analysisData->PSNR_Blue  = report.data.PSNR_Blue;
               analysisData->MSE        = report.data.MSE;

           }
        } //

        if ((strcmp(out, "") != 0))
        {
            diffImage = new QImage(w, h, QImage::Format_ARGB32);
            QColor src;
            QColor dest;
            QColor diff;
            int r, g, b, a;
            float fProgress = 0.0;

            for (int y = 0; y < h; y++) {
                for (int x = 0; x < w; x++) {
                    src = QColor(srcImage->pixel(x, y));
                    dest = QColor(destImage->pixel(x, y));
                    r = g = b = a = 0;
                    switch (m_RGBAChannels)
                    {
                        case 0b0001:
                            r = qAbs(src.red() - dest.red());
                            break;
                        case 0b0011:
                            r = qAbs(src.red() - dest.red());
                            g = qAbs(src.green() - dest.green());
                            break;
                        default:
                            r = qAbs(src.red() - dest.red());
                            g = qAbs(src.green() - dest.green());
                            b = qAbs(src.blue() - dest.blue());
                            a = qAbs(src.alpha() - dest.alpha());
                            break;
                    }

                    diff.setRed(qMin(r, 255));
                    diff.setGreen(qMin(g, 255));
                    diff.setBlue(qMin(b, 255));
                    diff.setAlpha(qMin(a, 255));
                    diffImage->setPixel(x, y, diff.rgba());
                }

                if (pFeedbackProc)
                {
                    fProgress = 100.f * (y * w) / (w * h);
                    if (pFeedbackProc(fProgress, NULL, NULL))
                    {
                        printf("Analysis canceled!\n");
                        return -1; //abort
                    }
                }
            }
        }
    }
    else
    {
        printf("Error: Image(s) cannot be loaded\n");
        return -1;
    }

    //-------------------
    // Process Image Diff
    //-------------------
    if (diffImage != NULL)
    {
        if ((CMP_FileExists(out)))
        {
            QFile::remove(out);
        }

        bool saved = diffImage->save(out);

        if (saved && cmipImages != NULL) //gui
        {
            delete diffImage;
            if (m_MipDiffImages)
                m_imageloader->clearMipImages(&m_MipDiffImages);

            m_MipDiffImages = m_imageloader->LoadPluginImage(out);
            if (m_MipDiffImages)
            {
                QFile::remove(out);
                *cmipImages = m_MipDiffImages;
                return 0;
            }
            else
            {
                return -1;
            }
        }
        else if (saved && cmipImages == NULL)  //cmdline version pass in null
        {
            delete diffImage;
            if ((strcmp(out, "") == 0))
            {
                QFile::remove(out);
            }
            return 0;
        }
        else if (!saved && cmipImages != NULL) //gui- saved fail due to admin right
        {
            delete diffImage;
            if ((strcmp(out, "") == 0)) {
                return 0;
            }
            else {
                QString appLocalPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
                QString redirectOut = appLocalPath + "/diff.bmp";
                redirectOut.replace("CompressonatorCLI", "Compressonator");
                saved = diffImage->save(redirectOut);
                if (saved)
                {
                    if (m_MipDiffImages)
                        m_imageloader->clearMipImages(&m_MipDiffImages);

                    printf("User does not have admin right to the saved path. Diff image has been redirect saved to %s\n", redirectOut.toStdString().c_str());
                    m_MipDiffImages = m_imageloader->LoadPluginImage(redirectOut);
                    if (m_MipDiffImages)
                    {
                        QFile::remove(redirectOut);
                        *cmipImages = m_MipDiffImages;
                        return 0;
                    }
                    return -1;
                }
                else
                {
                    printf("Error: Image(s) saved failed. Please run the app as admin.\n");
                    return -1;
                }
            }
        }
        else if (!saved && cmipImages == NULL)  //cmdline version pass in null- saved fail due to admin right
        {
            if ((strcmp(out, "") == 0))
            {
                delete diffImage;
                return 0;
            }
            else {
                QString appLocalPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
                QString redirectOut = appLocalPath + "/diff.bmp";
                redirectOut.replace("CompressonatorCLI", "Compressonator");
                saved = diffImage->save(redirectOut);
                delete diffImage;
                if (saved)
                {
                    printf("User does not have admin right to the saved path. Diff image has been redirect saved to %s\n", redirectOut.toStdString().c_str());
                    return 0;
                }
                else
                {
                    printf("Error: Image(s) saved failed. Please run the app as admin.\n");
                    return -1;
                }
            }
            
        }
        else //failed to save image diff
        {
            printf("Error: Image(s) diff saved/write failed. Please try run the app as admin.\n");
            return -1;
        }
    }
    else if ((strcmp(out, "") == 0))
        return 0;
    else
        return -1;
}

int Plugin_Canalysis::TC_PSNR_MSE(const char * in1, const char * in2,  char *resultsFile, void *pluginManager, CMP_Feedback_Proc pFeedbackProc)
{
    if (pluginManager == NULL) return -1;

    if (m_imageloader == NULL)
        m_imageloader = new CImageLoader(pluginManager);

    QImage* srcImage = NULL;
    QImage* destImage = NULL;

    if (m_imageloader)
    {
        m_MipSrcImages  = m_imageloader->LoadPluginImage(QString::fromUtf8(in1));
        m_MipDestImages = m_imageloader->LoadPluginImage(QString::fromUtf8(in2));
    }

    if (m_MipSrcImages != NULL && m_MipDestImages != NULL)
    {
        setActiveChannels();

        if (m_MipSrcImages->QImage_list[0].count() >0)
            srcImage = m_MipSrcImages->QImage_list[0][0];
        else
        {
            printf("Error: Source Image cannot be loaded\n");
            return -1;
        }

        if (m_MipDestImages->QImage_list[0].count() >0)
            destImage = m_MipDestImages->QImage_list[0][0];
        else
        {
            printf("Error: Dest Image cannot be loaded\n");
            return -1;
        }
    }
    else
    {
        printf("Error: Image(s) cannot be loaded\n");
        return -1;
    }


    if (srcImage != NULL && destImage != NULL)
    {
        int w = srcImage->width() == destImage->width() ? srcImage->width() : -1;
        int h = srcImage->height() == destImage->height() ? srcImage->height() : -1;

        if (w == -1 || h == -1)
        {
            printf("Error: Both images must be same size\n");
            return -1;
        }

#ifdef USE_OPENCV
        cv::Mat srcimg  = QtOcv::image2Mat(*srcImage);
        cv::Mat destimg = QtOcv::image2Mat(*destImage);
        if (!&srcimg || !&destimg)
        {
            printf("Error: Images fail to allocate for ssim analysis\n");
            return -1;
        }

        bool testpassed = psnr(srcImage, srcimg, destImage, destimg, report.data);
        if (!testpassed)
        {
            printf("Error: Images analysis fail\n");
            return -1;
        }

        write(report.data, resultsFile,'p');
#endif
        //cout << report;
    }
    else
    {
        printf("Error: Image(s) cannot be loaded\n");
        return -1;
    }

    return 0;
}

int Plugin_Canalysis::TC_SSIM(const char * in1, const char * in2, char *resultsFile, void *pluginManager, CMP_Feedback_Proc pFeedbackProc)
{
    if (pluginManager == NULL) return -1;

    if (m_imageloader == NULL)
        m_imageloader = new CImageLoader(pluginManager);

    QImage* srcImage = NULL;
    QImage* destImage = NULL;

    if (m_imageloader)
    {
        if (m_MipSrcImages)
            m_imageloader->clearMipImages(&m_MipSrcImages);

        if (m_MipDestImages)
            m_imageloader->clearMipImages(&m_MipDestImages);

        m_MipSrcImages  = m_imageloader->LoadPluginImage(QString::fromUtf8(in1));
        m_MipDestImages = m_imageloader->LoadPluginImage(QString::fromUtf8(in2));
    }

    if (m_MipSrcImages != NULL && m_MipDestImages != NULL)
    {
        setActiveChannels();
        if (m_MipSrcImages->QImage_list[0].count() >0)
            srcImage = m_MipSrcImages->QImage_list[0][0];
        else
        {
            printf("Error: Source Image cannot be loaded\n");
            return -1;
        }

        if (m_MipDestImages->QImage_list[0].count() >0)
            destImage = m_MipDestImages->QImage_list[0][0];
        else
        {
            printf("Error: Dest Image cannot be loaded\n");
            return -1;
        }
    }
    else
    {
        printf("Error: Image(s) cannot be loaded\n");
        return -1;
    }


    if (srcImage != NULL && destImage != NULL)
    {
        int w = srcImage->width() == destImage->width() ? srcImage->width() : -1;
        int h = srcImage->height() == destImage->height() ? srcImage->height() : -1;

        if (w == -1 || h == -1)
        {
            printf("Error: Both images must be same size\n");
            return -1;
        }

        report.data.SSIM_Blue   = 0;
        report.data.SSIM_Green  = 0;
        report.data.SSIM_Red    = 0;
        report.data.SSIM        = 0;

#ifdef USE_OPENCV
        cv::Mat srcimg  = QtOcv::image2Mat(*srcImage);
        cv::Mat destimg = QtOcv::image2Mat(*destImage);

        if (!&srcimg || !&destimg)
        {
            printf("Error: Images fail to allocate for ssim analysis\n");
            return -1;
        }

        m_SSIM = getSSIM(srcimg, destimg, pFeedbackProc);
        processSSIMResults();

        srcimg.release();
        destimg.release();

#endif
        write(report.data, resultsFile,'s');

    }
    else
    {
        printf("Error: Image(s) cannot be loaded\n");
        return -1;
    }

    return 0;
}
