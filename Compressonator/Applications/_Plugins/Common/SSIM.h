//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================

#pragma once
#include "TestReport.h"

#include <opencv2/opencv.hpp>

using namespace cv;

int GetSSIM(const char* file1, const char *file2, REPORT_DATA *error);
int GetSSIMBYTES(IplImage* imgsrcfile, IplImage* imgcompfile, REPORT_DATA *error, CMP_Feedback_Proc pFeedbackProc = NULL);
double ssim(Mat & img_src, Mat & img_compressed, int block_size, CMP_Feedback_Proc pFeedbackProc = NULL);
Scalar getMatSSIM(const Mat& i1, const Mat& i2, REPORT_DATA *error);

