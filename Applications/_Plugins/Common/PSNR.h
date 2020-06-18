#pragma once
#include "TestReport.h"
#include <iomanip>  // for controlling float print precision
#include <iostream> // for standard I/O
#include <sstream>  // string to number conversion
#include <string>   // for strings

#include <opencv2/core/core.hpp>       // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/highgui/highgui.hpp> // OpenCV window I/O
#include <opencv2/imgproc/imgproc.hpp> // Gaussian Blur

using namespace cv;

int GetPSNR(const char *file1, const char *file2, REPORT_DATA *stats);
