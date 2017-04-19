//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================

#pragma once
#include "TestReport.h"

#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  // OpenCV window I/O

using namespace cv;

int GetSSIM(const char* file1, const char *file2, REPORT_DATA *error);
int GetSSIMBYTES(IplImage* imgsrcfile, IplImage* imgcompfile, REPORT_DATA *error, CMP_Feedback_Proc pFeedbackProc = NULL);
double ssim(Mat & img_src, Mat & img_compressed, int block_size, CMP_Feedback_Proc pFeedbackProc = NULL);
