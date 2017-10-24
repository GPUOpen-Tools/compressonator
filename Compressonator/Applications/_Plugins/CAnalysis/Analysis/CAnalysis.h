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

#ifndef _Plugin_Canalysis_H
#define _Plugin_Canalysis_H

#include <string>
#include "PluginInterface.h"
#include "MIPS.h"
#include "PluginManager.h"
#include "cvmatandqimage.h"

#define USE_QT_IMAGELOAD

#ifdef USE_QT_IMAGELOAD
#include <QtCore/QCoreApplication>
#include <QtCore/qstandardpaths.h>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qfileinfo.h>
#include <QtGui/qimage.h>
#include <QtGui/qcolor.h>
#endif

#include "cpImageLoader.h"
#include "TextureIO.h"
#include "SSIM.h"
#include <opencv2/core/core.hpp>     
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O
#include <QtCore/qmath.h>

#ifndef _WIN32
#define MAX_PATH 260
#endif

#define TC_PLUGIN_VERSION_MAJOR	1
#define TC_PLUGIN_VERSION_MINOR	0

class Plugin_Canalysis : public PluginInterface_Analysis
{
public: 
		Plugin_Canalysis();
		virtual ~Plugin_Canalysis();

		int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
		int TC_ImageDiff(const char * in1, const char * in2, const char *out, char *resultsFile, void *pluginManager, void **cmipImages, CMP_Feedback_Proc pFeedbackProc = NULL);
        int TC_PSNR_MSE(const char * in1, const char * in2, char *resultsFile, void *pluginManager, CMP_Feedback_Proc pFeedbackProc = NULL);
        int TC_SSIM(const char * in1, const char * in2,  char *resultsFile, void *pluginManager, CMP_Feedback_Proc pFeedbackProc = NULL);



private:
        void write(REPORT_DATA data, char *resultsFile, char option);
        void generateBCtestResult(QImage *src, QImage *dest, REPORT_DATA &myReport); //for testing only
        bool psnr(QImage *src, QImage *dest, REPORT_DATA &myReport, CMP_Feedback_Proc pFeedbackProc = NULL);
		char m_results_path[MAX_PATH];
        string m_srcFile;
        string m_destFile;
		double m_rmse, m_psnr, m_mabse;
        double tolerance_mse, tolerance_psnr, tolerance_psnrb, tolerance_psnrg, tolerance_psnrr;
        double tolerance_ssim, tolerance_ssimb, tolerance_ssimg, tolerance_ssimr;
		PluginManager *m_pluginManager;
		CImageLoader                     *m_imageloader;
		CMipImages                       *m_MipSrcImages;
		CMipImages                       *m_MipDestImages;
        CMipImages                       *m_MipDiffImages;
        CMP_FORMAT                        m_Compressformat;
};

#endif
