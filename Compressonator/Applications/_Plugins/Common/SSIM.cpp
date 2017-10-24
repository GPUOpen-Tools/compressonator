/*
 * The equivalent of Zhou Wang's SSIM matlab code using OpenCV.
 * from http://www.cns.nyu.edu/~zwang/files/research/ssim/index.html
 * The measure is described in :
 * "Image quality assessment: From error measurement to structural similarity"
 * C++ code by Rabah Mehdi. http://mehdi.rabah.free.fr/SSIM
 *
 * This implementation is under the public domain.
 * @see http://creativecommons.org/licenses/publicdomain/
 * The original work may be under copyrights. 
 */
#ifdef _WIN32
#include <Windows.h>
#endif
#include <iostream>
#include <stdio.h>
#include "TestReport.h"

#include <opencv/cv.h>
#include <opencv2/core/core.hpp>        // Basic OpenCV structures (cv::Mat, Scalar)
#include <opencv2/imgproc/imgproc.hpp>  // Gaussian Blur
#include <opencv2/highgui/highgui.hpp>  


using namespace std;
using namespace cv;

/*
 * Parameters : complete path to the two image to be compared
 * The file format must be supported by your OpenCV build
 */
int GetSSIM(const char* file1, const char *file2, REPORT_DATA *error)
{

	// default settings
	double C1 = 6.5025, C2 = 58.5225;

	IplImage
		*img1=NULL, *img2=NULL, *img1_img2=NULL,
		*img1_temp=NULL, *img2_temp=NULL,
		*img1_sq=NULL, *img2_sq=NULL,
		*mu1=NULL, *mu2=NULL,
		*mu1_sq=NULL, *mu2_sq=NULL, *mu1_mu2=NULL,
		*sigma1_sq=NULL, *sigma2_sq=NULL, *sigma12=NULL,
		*ssim_map=NULL, *temp1=NULL, *temp2=NULL, *temp3=NULL;
	

	/***************************** INITS **********************************/
	img1_temp = cvLoadImage(file1);
	img2_temp = cvLoadImage(file2); 

	if(img1_temp==NULL || img2_temp==NULL)
		return -1;

	int x=img1_temp->width, y=img1_temp->height;
	int nChan=img1_temp->nChannels, d=IPL_DEPTH_32F;
	CvSize size = cvSize(x, y);

	img1 = cvCreateImage( size, d, nChan);
	img2 = cvCreateImage( size, d, nChan);

	cvConvert(img1_temp, img1);
	cvConvert(img2_temp, img2);
	cvReleaseImage(&img1_temp);
	cvReleaseImage(&img2_temp);

	
	img1_sq = cvCreateImage( size, d, nChan);
	img2_sq = cvCreateImage( size, d, nChan);
	img1_img2 = cvCreateImage( size, d, nChan);
	
	cvPow( img1, img1_sq, 2 );
	cvPow( img2, img2_sq, 2 );
	cvMul( img1, img2, img1_img2, 1 );

	mu1 = cvCreateImage( size, d, nChan);
	mu2 = cvCreateImage( size, d, nChan);

	mu1_sq = cvCreateImage( size, d, nChan);
	mu2_sq = cvCreateImage( size, d, nChan);
	mu1_mu2 = cvCreateImage( size, d, nChan);
	

	sigma1_sq = cvCreateImage( size, d, nChan);
	sigma2_sq = cvCreateImage( size, d, nChan);
	sigma12 = cvCreateImage( size, d, nChan);

	temp1 = cvCreateImage( size, d, nChan);
	temp2 = cvCreateImage( size, d, nChan);
	temp3 = cvCreateImage( size, d, nChan);

	ssim_map = cvCreateImage( size, d, nChan);
	/*************************** END INITS **********************************/


	//////////////////////////////////////////////////////////////////////////
	// PRELIMINARY COMPUTING
	cvSmooth( img1, mu1, CV_GAUSSIAN, 11, 11, 1.5 );
	cvSmooth( img2, mu2, CV_GAUSSIAN, 11, 11, 1.5 );
	
	cvPow( mu1, mu1_sq, 2 );
	cvPow( mu2, mu2_sq, 2 );
	cvMul( mu1, mu2, mu1_mu2, 1 );


	cvSmooth( img1_sq, sigma1_sq, CV_GAUSSIAN, 11, 11, 1.5 );
	cvAddWeighted( sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq );
	
	cvSmooth( img2_sq, sigma2_sq, CV_GAUSSIAN, 11, 11, 1.5 );
	cvAddWeighted( sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq );

	cvSmooth( img1_img2, sigma12, CV_GAUSSIAN, 11, 11, 1.5 );
	cvAddWeighted( sigma12, 1, mu1_mu2, -1, 0, sigma12 );
	

	//////////////////////////////////////////////////////////////////////////
	// FORMULA

	// (2*mu1_mu2 + C1)
	cvScale( mu1_mu2, temp1, 2 );
	cvAddS( temp1, cvScalarAll(C1), temp1 );

	// (2*sigma12 + C2)
	cvScale( sigma12, temp2, 2 );
	cvAddS( temp2, cvScalarAll(C2), temp2 );

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
	cvMul( temp1, temp2, temp3, 1 );

	// (mu1_sq + mu2_sq + C1)
	cvAdd( mu1_sq, mu2_sq, temp1 );
	cvAddS( temp1, cvScalarAll(C1), temp1 );

	// (sigma1_sq + sigma2_sq + C2)
	cvAdd( sigma1_sq, sigma2_sq, temp2 );
	cvAddS( temp2, cvScalarAll(C2), temp2 );

	// ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvMul( temp1, temp2, temp1, 1 );

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvDiv( temp3, temp1, ssim_map, 1 );


	CvScalar index_scalar = cvAvg( ssim_map );
	
	// through observation, there is approximately 
	// 1% error max with the original matlab program

	error->SSIM_Red		= index_scalar.val[0];
	error->SSIM_Green	= index_scalar.val[1];
	error->SSIM_Blue	= index_scalar.val[2];
	error->SSIM			= (index_scalar.val[2] + index_scalar.val[1] + index_scalar.val[0]) / 3;

	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img1_sq);
	cvReleaseImage(&img2_sq);
	cvReleaseImage(&img1_img2);
	cvReleaseImage(&mu1);
	cvReleaseImage(&mu2);
	cvReleaseImage(&mu1_sq);
	cvReleaseImage(&mu2_sq);
	cvReleaseImage(&mu1_mu2);
	cvReleaseImage(&sigma1_sq);
	cvReleaseImage(&sigma2_sq);
	cvReleaseImage(&sigma12);
	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
	cvReleaseImage(&temp3);
	cvReleaseImage(&ssim_map);



	return 0;
}

/*
* Parameters : two IplImage image to be compared
* this function added based on function above 
*/
int GetSSIMBYTES(IplImage* imgsrcfile, IplImage* imgcompfile, REPORT_DATA *error, CMP_Feedback_Proc pFeedbackProc)
{

	// default settings
	double C1 = 6.5025, C2 = 58.5225;

	IplImage
		*img1 = NULL, *img2 = NULL, *img1_img2 = NULL,
		*img1_temp = NULL, *img2_temp = NULL,
		*img1_sq = NULL, *img2_sq = NULL,
		*mu1 = NULL, *mu2 = NULL,
		*mu1_sq = NULL, *mu2_sq = NULL, *mu1_mu2 = NULL,
		*sigma1_sq = NULL, *sigma2_sq = NULL, *sigma12 = NULL,
		*ssim_map = NULL, *temp1 = NULL, *temp2 = NULL, *temp3 = NULL;


	/***************************** INITS **********************************/
	img1_temp = imgsrcfile;
	img2_temp = imgcompfile;
    float fProgress = 0.0;

	if (img1_temp == NULL || img2_temp == NULL)
		return -1;

	int x = img1_temp->width, y = img1_temp->height;
	int nChan = img1_temp->nChannels, d = IPL_DEPTH_32F;
	CvSize size = cvSize(x, y);

    try {
        img1 = cvCreateImage(size, d, nChan);
        img2 = cvCreateImage(size, d, nChan);

        cvConvert(img1_temp, img1);
        cvConvert(img2_temp, img2);
        //cvReleaseImage(&img1_temp);
        //cvReleaseImage(&img2_temp);


        img1_sq = cvCreateImage(size, d, nChan);
        img2_sq = cvCreateImage(size, d, nChan);
        img1_img2 = cvCreateImage(size, d, nChan);

        cvPow(img1, img1_sq, 2);
        cvPow(img2, img2_sq, 2);
        cvMul(img1, img2, img1_img2, 1);

        mu1 = cvCreateImage(size, d, nChan);
        mu2 = cvCreateImage(size, d, nChan);

        mu1_sq = cvCreateImage(size, d, nChan);
        mu2_sq = cvCreateImage(size, d, nChan);
        mu1_mu2 = cvCreateImage(size, d, nChan);


        sigma1_sq = cvCreateImage(size, d, nChan);
        sigma2_sq = cvCreateImage(size, d, nChan);
        sigma12 = cvCreateImage(size, d, nChan);

        temp1 = cvCreateImage(size, d, nChan);
        temp2 = cvCreateImage(size, d, nChan);
        temp3 = cvCreateImage(size, d, nChan);

        ssim_map = cvCreateImage(size, d, nChan);

        // Progress
        if (pFeedbackProc)
        {
            fProgress = 30.0;
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                return -1; //abort
            }
        }
        /*************************** END INITS **********************************/


        //////////////////////////////////////////////////////////////////////////
        // PRELIMINARY COMPUTING
        cvSmooth(img1, mu1, CV_GAUSSIAN, 11, 11, 1.5);
        cvSmooth(img2, mu2, CV_GAUSSIAN, 11, 11, 1.5);

        cvPow(mu1, mu1_sq, 2);
        cvPow(mu2, mu2_sq, 2);
        cvMul(mu1, mu2, mu1_mu2, 1);

        // Progress
        if (pFeedbackProc)
        {
            fProgress = 60.0;
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                return -1; //abort
            }
        }

        cvSmooth(img1_sq, sigma1_sq, CV_GAUSSIAN, 11, 11, 1.5);
        cvAddWeighted(sigma1_sq, 1, mu1_sq, -1, 0, sigma1_sq);

        cvSmooth(img2_sq, sigma2_sq, CV_GAUSSIAN, 11, 11, 1.5);
        cvAddWeighted(sigma2_sq, 1, mu2_sq, -1, 0, sigma2_sq);

        cvSmooth(img1_img2, sigma12, CV_GAUSSIAN, 11, 11, 1.5);
        cvAddWeighted(sigma12, 1, mu1_mu2, -1, 0, sigma12);

        // Progress
        if (pFeedbackProc)
        {
            fProgress = 90.0;
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                return -1; //abort
            }
        }

    }
    catch (exception e)
    {
        if(&img1)
            cvReleaseImage(&img1);
        if(&img2)
            cvReleaseImage(&img2);
        if(&img1_sq)
            cvReleaseImage(&img1_sq);
        if(&img2_sq)
            cvReleaseImage(&img2_sq);
        if (&img1_img2)
            cvReleaseImage(&img1_img2);
        if (&mu1)
            cvReleaseImage(&mu1);
        if (&mu2)
            cvReleaseImage(&mu2);
        if (&mu1_sq)
            cvReleaseImage(&mu1_sq);
        if (&mu2_sq)
            cvReleaseImage(&mu2_sq);
        if (&mu1_mu2)
            cvReleaseImage(&mu1_mu2);
        if (&sigma1_sq)
            cvReleaseImage(&sigma1_sq);
        if (&sigma2_sq)
            cvReleaseImage(&sigma2_sq);
        if (&sigma12)
            cvReleaseImage(&sigma12);
        if (&temp1)
            cvReleaseImage(&temp1);
        if (&temp2)
            cvReleaseImage(&temp2);
        if (&temp3)
            cvReleaseImage(&temp3);
        if (&ssim_map)
            cvReleaseImage(&ssim_map);

        return -1;
    }
	//////////////////////////////////////////////////////////////////////////
	// FORMULA

	// (2*mu1_mu2 + C1)
	cvScale(mu1_mu2, temp1, 2);
	cvAddS(temp1, cvScalarAll(C1), temp1);

	// (2*sigma12 + C2)
	cvScale(sigma12, temp2, 2);
	cvAddS(temp2, cvScalarAll(C2), temp2);

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
	cvMul(temp1, temp2, temp3, 1);

	// (mu1_sq + mu2_sq + C1)
	cvAdd(mu1_sq, mu2_sq, temp1);
	cvAddS(temp1, cvScalarAll(C1), temp1);

	// (sigma1_sq + sigma2_sq + C2)
	cvAdd(sigma1_sq, sigma2_sq, temp2);
	cvAddS(temp2, cvScalarAll(C2), temp2);

	// ((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvMul(temp1, temp2, temp1, 1);

	// ((2*mu1_mu2 + C1).*(2*sigma12 + C2))./((mu1_sq + mu2_sq + C1).*(sigma1_sq + sigma2_sq + C2))
	cvDiv(temp3, temp1, ssim_map, 1);


	CvScalar index_scalar = cvAvg(ssim_map);

	// through observation, there is approximately 
	// 1% error max with the original matlab program

	error->SSIM_Blue = index_scalar.val[0];
	error->SSIM_Green = index_scalar.val[1];
	error->SSIM_Red = index_scalar.val[2];
	error->SSIM = (index_scalar.val[2] + index_scalar.val[1] + index_scalar.val[0]) / 3;

	cvReleaseImage(&img1);
	cvReleaseImage(&img2);
	cvReleaseImage(&img1_sq);
	cvReleaseImage(&img2_sq);
	cvReleaseImage(&img1_img2);
	cvReleaseImage(&mu1);
	cvReleaseImage(&mu2);
	cvReleaseImage(&mu1_sq);
	cvReleaseImage(&mu2_sq);
	cvReleaseImage(&mu1_mu2);
	cvReleaseImage(&sigma1_sq);
	cvReleaseImage(&sigma2_sq);
	cvReleaseImage(&sigma12);
	cvReleaseImage(&temp1);
	cvReleaseImage(&temp2);
	cvReleaseImage(&temp3);
	cvReleaseImage(&ssim_map);

    // Progress
    if (pFeedbackProc)
    {
        fProgress = 100.0;
        if (pFeedbackProc(fProgress, NULL, NULL))
        {
            return -1; //abort
        }
    }

	return 0;
}

//reserved for compute mssim
Scalar getMatSSIM( const Mat& i1, const Mat& i2, REPORT_DATA *error)
{
 const double C1 = 6.5025, C2 = 58.5225;
 /***************************** INITS **********************************/
 int d     = CV_32F;

 Mat I1, I2;
 i1.convertTo(I1, d);           // cannot calculate on one byte large values
 i2.convertTo(I2, d);

 Mat I2_2   = I2.mul(I2);        // I2^2
 Mat I1_2   = I1.mul(I1);        // I1^2
 Mat I1_I2  = I1.mul(I2);        // I1 * I2

 /***********************PRELIMINARY COMPUTING ******************************/

 Mat mu1, mu2;   //
 GaussianBlur(I1, mu1, Size(11, 11), 1.5);
 GaussianBlur(I2, mu2, Size(11, 11), 1.5);

 Mat mu1_2   =   mu1.mul(mu1);
 Mat mu2_2   =   mu2.mul(mu2);
 Mat mu1_mu2 =   mu1.mul(mu2);

 Mat sigma1_2, sigma2_2, sigma12;

 GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
 sigma1_2 -= mu1_2;

 GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
 sigma2_2 -= mu2_2;

 GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
 sigma12 -= mu1_mu2;

 ///////////////////////////////// FORMULA ////////////////////////////////
 Mat t1, t2, t3;

 t1 = 2 * mu1_mu2 + C1;
 t2 = 2 * sigma12 + C2;
 t3 = t1.mul(t2);              // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))

 t1 = mu1_2 + mu2_2 + C1;
 t2 = sigma1_2 + sigma2_2 + C2;
 t1 = t1.mul(t2);               // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))

 Mat ssim_map;
 divide(t3, t1, ssim_map);      // ssim_map =  t3./t1;

 Scalar mssim = mean( ssim_map ); // mssim = average of ssim map
 return mssim;
}

#define C1 (float) (0.01 * 255 * 0.01  * 255)
#define C2 (float) (0.03 * 255 * 0.03  * 255)


// sigma on block_size
double sigma(Mat & m, int i, int j, int block_size)
{
    double sd = 0;

    Mat m_tmp = m(Range(i, i + block_size), Range(j, j + block_size));
    Mat m_squared(block_size, block_size, CV_8U);

    multiply(m_tmp, m_tmp, m_squared);

    double avg = mean(m_tmp)[0];
    double avg_2 = mean(m_squared)[0];

    sd = sqrt(abs(avg_2 - avg * avg));

    return sd;
}

// Covariance
double cov(Mat & m1, Mat & m2, int i, int j, int block_size)
{
    Mat m3 = Mat::zeros(block_size, block_size, m1.depth());
    Mat m1_tmp = m1(Range(i, i + block_size), Range(j, j + block_size));
    Mat m2_tmp = m2(Range(i, i + block_size), Range(j, j + block_size));


    multiply(m1_tmp, m2_tmp, m3);

    double avg_ro = mean(m3)[0]; // E(XY)
    double avg_r = mean(m1_tmp)[0]; // E(X)
    double avg_o = mean(m2_tmp)[0]; // E(Y)


    double sd_ro = avg_ro - avg_o * avg_r; // E(XY) - E(X)E(Y)

    return sd_ro;
}


/**
* Compute the SSIM between 2 images
*/
double ssim(Mat & img_src, Mat & img_compressed, int block_size, CMP_Feedback_Proc pFeedbackProc)
{
    double ssim = 0;
    float fProgress = 0.0;
//    int nbBlockPerHeight = img_src.rows / block_size;
//    int nbBlockPerWidth = img_src.cols / block_size;

    int nbBlockPerHeight = img_src.rows ;
    int nbBlockPerWidth = img_src.cols;

    for (int k = 0; k < nbBlockPerHeight; k++)
    {
        for (int l = 0; l < nbBlockPerWidth; l++)
        {
           
            double avg_o = mean(img_src(Range(k, k + block_size), Range(l, l + block_size)))[0];
            if (isnan(avg_o)) avg_o = 0;
            double avg_r = mean(img_compressed(Range(k, k + block_size), Range(l, l + block_size)))[0];
            if (isnan(avg_r)) avg_r = 0;
            double sigma_o = sigma(img_src, k, l, block_size);  //m,n
            if (isnan(sigma_o)) sigma_o = 0;
            double sigma_r = sigma(img_compressed, k, l, block_size);  //m,n
            if (isnan(sigma_r)) sigma_r = 0;
            double sigma_ro = cov(img_src, img_compressed, k, l, block_size);  //m,n
            if (isnan(sigma_ro)) sigma_ro = 0;
           
            ssim += ((2 * avg_o * avg_r + C1) * (2 * sigma_ro + C2)) / ((avg_o * avg_o + avg_r * avg_r + C1) * (sigma_o * sigma_o + sigma_r * sigma_r + C2));
      

        }
        // Progress
        if (pFeedbackProc)
        {
            fProgress = (float)(((double)k) / nbBlockPerHeight) * 100;
            if (pFeedbackProc(fProgress, NULL, NULL))
            {
                return -1; //abort
            }
        }

    }
    ssim /= nbBlockPerHeight * nbBlockPerWidth;

    //if (show_progress)
    //{
    //    cout << "\r>>SSIM [100%]" << endl;
    //    cout << "SSIM : " << ssim << endl;
    //}

    return abs(ssim);
}