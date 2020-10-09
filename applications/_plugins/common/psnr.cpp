
#include "psnr.h"

#include <opencv2/core/core.hpp>

using namespace cv;

// OpenCV (Open Source Computer Vision Library: http://opencv.org) is an open-source BSD-licensed library
// http://docs.opencv.org/doc/tutorials/highgui/video-input-psnr-ssim/video-input-psnr-ssim.html
// Ref to basic structures of OpenCV http://docs.opencv.org/modules/core/doc/basic_structures.html
// Mat - The Basic Image Container http://docs.opencv.org/doc/tutorials/core/mat_the_basic_image_container/mat_the_basic_image_container.html
//

void CalcPSNR(const Mat& I1, const Mat& I2, REPORT_DATA *stats) {
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);        // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) { // for small values return zero
        stats->MSE	= 0;
        stats->PSNR = 100.1234;
    } else {
        stats->MSE  = sse / (double)(I1.channels() * I1.total());
        if (stats->MSE != 0) {
            stats->PSNR = 10.0 * log10((255 * 255) / stats->MSE);
            stats->PSNR_Blue = 10.0 * log10((255 * 255) / (s.val[0] / (double)(I1.channels() * I1.total())));
            stats->PSNR_Green = 10.0 * log10((255 * 255) / (s.val[1] / (double)(I1.channels() * I1.total())));
            stats->PSNR_Red = 10.0 * log10((255 * 255) / (s.val[2] / (double)(I1.channels() * I1.total())));
        } else
            stats->PSNR = 100.1234;
    }
}

int GetPSNR(const char* file1, const char *file2, REPORT_DATA *stats) {
    cv::Mat img1, img2;

    img1 = cv::imread(file1);
    if (img1.empty()) return 1;
    img2 = cv::imread(file2);
    if (img2.empty()) return 1;

    CalcPSNR(img1,img2,stats);

    return 0;
}
