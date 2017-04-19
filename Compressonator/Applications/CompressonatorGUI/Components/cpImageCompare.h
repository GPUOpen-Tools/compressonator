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

#ifndef _IMAGECOMPARE_H
#define _IMAGECOMPARE_H

#include <QtWidgets>
#include "cpImageView.h"
#include "cpImageAnalysis.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "acCustomDockWidget.h"
#include "cpProjectData.h"

class CImageCompare : public acCustomDockWidget
{
    Q_OBJECT

public:
    CImageCompare(const QString title, QString file1, QString file2, bool isCompressed, QMainWindow *parent);
    ~CImageCompare();

    void createImageView(bool isCompressedCompare);
    void setDefaultView();
    void setHorizontalView();
    bool setAnalysisResultView();
    void emitUpdateData();
    CMipImages* getMdiffMips();

    void showProgressDialog(QString header);
    void hideProgressDialog();

    void showProgressBusy(QString Message);
    void hideProgressBusy(QString Message);

private:
    void showEvent(QShowEvent *ev);
    void paintEvent(QPaintEvent * event);

    bool                       m_setHorizontalView;
    bool                       ssimAnalysis;
    bool                       psnrAnalysis;

    acVirtualMouseHub          m_mousehub;

    QWidget                   *m_newWidget;
    QWidget                   *m_newInnerWidget;
    QMainWindow               *m_MainWindow;
    QVBoxLayout               *m_layout;
    QGridLayout               *m_innerlayout;

    QHBoxLayout               *m_innerHlayout;
    QVBoxLayout               *m_innerVlayout;

    cpImageView               *m_imageviewFile1;
    cpImageView               *m_imageviewDiff;
    cpImageView               *m_imageviewFile2;
    C_AnalysisData            *m_imageAnalysis;
    C_SSIM_Analysis            *m_ssimAnalysis;
    C_MSE_PSNR_Analysis        *m_allAnalysis;
    C_PSNR_MSE_Analysis        *m_psnrAnalysis;
 
    const QString              m_title;
    QString                       m_sourceFile;
    QString                       m_destFile;
    string                       m_analyzed;
    CMipImages                  *m_diffMips;
    QMainWindow               *m_parent;
    QToolBar                  *m_dockToolBar;
    QAction                   *hlayoutAct;
    QAction                   *orilayoutAct;

    QAction                   *psnrmseAct;
    QAction                   *ssimAct;

public slots:
    void changeLayout();
    void resetLayout();
    void runSSIM();
    void runPsnrMse();

Q_SIGNALS:
    void UpdateData(QObject *data);
    

};


#endif