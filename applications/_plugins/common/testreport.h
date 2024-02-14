//=====================================================================
// Copyright 2023-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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
//=====================================================================

#pragma once

#include <cstring>
#include <iostream>

typedef struct
{
    int DataSize;

    float TotalError;
    float AbsError;  //< Total Absolute Error
    float SqError;   //< Total Squared Error
    float MeanError;
    float MeanAbsError;

    // Test results
    float MSE;       // Mean Square Error
    float RMSError;  //< Root Mean Square Error

    float SSIM;
    float SSIM_Red;
    float SSIM_Green;
    float SSIM_Blue;

    float PSNR;
    float PSNR_Red;
    float PSNR_Green;
    float PSNR_Blue;

    char srcdecodePattern[17];
    char destdecodePattern[17];

    float encodeTime;

    // Pass Conditions
    float pMSE;       // Mean Square Error
    float pRMSError;  //< Root Mean Square Error
    float pSSIM_Red;
    float pSSIM_Green;
    float pSSIM_Blue;
    float pPSNR;
    float pencodeTime;

    // SQL data
    int maxcount;  // Maxumiun number of test items in the data base
    int test;

    // BC7 options
    int           nCompressionSpeed;
    int           dwnumThreads;
    float         fquality;
    int           brestrictColour;
    int           brestrictAlpha;
    unsigned long dwmodeMask;

    // Compression Type
    char Compression[128];

    // The source file to test on
    char SourceFile[128];
    char FullSourceFile[128];
    char FullTargetBMP[128];
    char FullSourceBMP[128];
    char FullSourceDDS[128];
    char ext[128];    // file extension
    char fname[128];  // just the file name without extension and path
    char dir[128];    // Typically null! But in some special cases user may define a full path for Source File, overiding configuration defaults
    char drive[128];  // Typically null!

} REPORT_DATA;

class MY_REPORT_DATA
{
public:
    MY_REPORT_DATA()
    {
        memset(&data, 0, sizeof(REPORT_DATA));
    };
    friend std::ostream& operator<<(std::ostream& os, const MY_REPORT_DATA& dt)
    {
        REPORT_DATA my_data = dt.data;
        //os << "Absolute Error: \t"        << my_data.AbsError << "\n";
        os /*<< "MSE: \t"            */ << my_data.MSE << "\n";
        os /*<< "SSIM: \t"            */ << my_data.SSIM << "\n";
        os /*<< "SSIM Blue: \t"        */ << my_data.SSIM_Blue << "\n";
        os /*<< "SSIM Green: \t"    */ << my_data.SSIM_Green << "\n";
        os /*<< "SSIM Red: \t"        */ << my_data.SSIM_Red << "\n";
        /*                       */
        os /*<< "PSNR: \t"            */ << my_data.PSNR << "\n";
        os /*<< "PSNR Blue: \t"        */ << my_data.PSNR_Blue << "\n";
        os /*<< "PSNR Green: \t"    */ << my_data.PSNR_Green << "\n";
        os /*<< "PSNR Red: \t"        */ << my_data.PSNR_Red << "\n";
        return os;
    };

    REPORT_DATA data;
};
