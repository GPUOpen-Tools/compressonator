//=====================================================================
// Copyright 2017-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _3DMODELCOMPARE_H
#define _3DMODELCOMPARE_H

#include <QtWidgets>
#include "cp3dmodelview.h"

#ifdef USE_IMGDIFF
#include "cpimageview.h"
#endif

#include "cpimageanalysis.h"
#include "accustomdockwidget.h"
#include "cpprojectdata.h"

class C3DModelCompare : public acCustomDockWidget
{
    Q_OBJECT

public:
    C3DModelCompare(const QString title, QString file1, QString file2, bool isCompressed, QMainWindow* parent);
    ~C3DModelCompare();

    bool createImageViews(bool isCompressedCompare);
    void setDefaultView();
    void setHorizontalView();
    void emitUpdateData();

    void showProgressDialog(QString header);
    void hideProgressDialog();

    void showProgressBusy(QString Message);
    void hideProgressBusy(QString Message);

private:
    bool m_setHorizontalView;
    int  m_manualModelViewFlip;

    QWidget*     m_newWidget;
    QWidget*     m_newInnerWidget;
    QMainWindow* m_MainWindow;
    QVBoxLayout* m_layout;
    QGridLayout* m_innerlayout;

    QHBoxLayout* m_innerHlayout;
    QVBoxLayout* m_innerVlayout;

    cp3DModelView* m_imageviewFile1;

#ifdef USE_IMGDIFF
    cpImageView* m_imageviewDiff;
    CMipImages*  m_diffMips;
#else
    cp3DModelView* m_imageviewDiff;
#endif

    cp3DModelView* m_imageviewFile2;

    C_AnalysisData* m_imageAnalysis;

    const QString m_title;
    QString       m_sourceFile;
    QString       m_destFile;

    QMainWindow* m_parent;
    QToolBar*    m_dockToolBar;
    QAction*     hlayoutAct;
    QAction*     orilayoutAct;
    QComboBox*   m_CBManual_renderView;

public slots:
    void changeLayout();
    void resetLayout();
    void onManual_renderView(int mode);

Q_SIGNALS:
    void UpdateData(QObject* data);
};

#endif