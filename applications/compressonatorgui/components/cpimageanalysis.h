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

#ifndef CPIMAGEANALYSIS_H
#define CPIMAGEANALYSIS_H

#include "pluginmanager.h"
#include "canalysis.h"

#include <cpimageloader.h>

#include <QtCore/qobject.h>

class C_AnalysisData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double _MSE MEMBER m_MSE)
    Q_PROPERTY(double _SSIM MEMBER m_SSIM)
    Q_PROPERTY(double _SSIM_Blue MEMBER m_SSIM_Blue)
    Q_PROPERTY(double _SSIM_Green MEMBER m_SSIM_Green)
    Q_PROPERTY(double _SSIM_Red MEMBER m_SSIM_Red)
    Q_PROPERTY(double _PSNR MEMBER m_PSNR)
    Q_PROPERTY(double _PSNR_Blue MEMBER m_PSNR_Blue)
    Q_PROPERTY(double _PSNR_Green MEMBER m_PSNR_Green)
    Q_PROPERTY(double _PSNR_Red MEMBER m_PSNR_Red)

public:
    C_AnalysisData()
    {
        m_MSE          = 0.0;
        m_SSIM         = 0.0;
        m_SSIM_Blue    = 0.0;
        m_SSIM_Green   = 0.0;
        m_SSIM_Red     = 0.0;
        m_PSNR         = 0.0;
        m_PSNR_Blue    = 0.0;
        m_PSNR_Green   = 0.0;
        m_PSNR_Red     = 0.0;
        diffCMipImages = new CMipImages();
        m_analysisFile = "";
        m_diffFile     = "";
    }

    double m_MSE;
    double m_SSIM;
    double m_SSIM_Blue;
    double m_SSIM_Green;
    double m_SSIM_Red;
    double m_PSNR;
    double m_PSNR_Blue;
    double m_PSNR_Green;
    double m_PSNR_Red;

    std::string m_analysisFile;
    std::string m_diffFile;

    ~C_AnalysisData();

    bool        SourceAndDestFileExtMatch(const char* fsource, const char* fdest);
    CMipImages* GenerateDiffImage(const char* fsource, const char* fdest);
    int         GenerateSSIMAnalysis(const char* fsource, const char* fdest);
    int         GeneratePSNRMSEAnalysis(const char* fsource, const char* fdest);
    std::string CreateResultsFileName(const char* fsource, const char* fdest, const char* type_ext);

    CMipImages* diffCMipImages;
};

class C_SSIM_Analysis : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double _SSIM MEMBER m_SSIM)
    Q_PROPERTY(double _SSIM_Blue MEMBER m_SSIM_Blue)
    Q_PROPERTY(double _SSIM_Green MEMBER m_SSIM_Green)
    Q_PROPERTY(double _SSIM_Red MEMBER m_SSIM_Red)

public:
    C_SSIM_Analysis()
    {
        m_SSIM       = 0.0;
        m_SSIM_Blue  = 0.0;
        m_SSIM_Green = 0.0;
        m_SSIM_Red   = 0.0;
    }

    double m_SSIM;
    double m_SSIM_Blue;
    double m_SSIM_Green;
    double m_SSIM_Red;

    ~C_SSIM_Analysis();
};

class C_MSE_PSNR_Analysis : public C_SSIM_Analysis
{
    Q_OBJECT
    Q_PROPERTY(double _MSE MEMBER m_MSE)
    Q_PROPERTY(double _PSNR MEMBER m_PSNR)
    Q_PROPERTY(double _PSNR_Blue MEMBER m_PSNR_Blue)
    Q_PROPERTY(double _PSNR_Green MEMBER m_PSNR_Green)
    Q_PROPERTY(double _PSNR_Red MEMBER m_PSNR_Red)

public:
    C_MSE_PSNR_Analysis()
    {
        m_MSE        = 0.0;
        m_PSNR       = 0.0;
        m_PSNR_Blue  = 0.0;
        m_PSNR_Green = 0.0;
        m_PSNR_Red   = 0.0;
    }

    double m_MSE;
    double m_PSNR;
    double m_PSNR_Blue;
    double m_PSNR_Green;
    double m_PSNR_Red;

    ~C_MSE_PSNR_Analysis();
};

class C_PSNR_MSE_Analysis : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double _MSE MEMBER m_MSE)
    Q_PROPERTY(double _PSNR MEMBER m_PSNR)
    Q_PROPERTY(double _PSNR_Blue MEMBER m_PSNR_Blue)
    Q_PROPERTY(double _PSNR_Green MEMBER m_PSNR_Green)
    Q_PROPERTY(double _PSNR_Red MEMBER m_PSNR_Red)

public:
    C_PSNR_MSE_Analysis()
    {
        m_MSE        = 0.0;
        m_PSNR       = 0.0;
        m_PSNR_Blue  = 0.0;
        m_PSNR_Green = 0.0;
        m_PSNR_Red   = 0.0;
    }

    double m_MSE;
    double m_PSNR;
    double m_PSNR_Blue;
    double m_PSNR_Green;
    double m_PSNR_Red;

    ~C_PSNR_MSE_Analysis();
};
extern PluginManager g_pluginManager;

#endif  // CPIMAGEANALYSIS_H
