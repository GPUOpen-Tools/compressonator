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

#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>

#include "cp3dmodelcompare.h"
#include "cpmaincomponents.h"

C3DModelCompare::C3DModelCompare(const QString title, QString file1, QString file2, bool isCompressed, QMainWindow* parent)
    : acCustomDockWidget(title, parent)
{
    m_parent = parent;

    // Hide the Title bar Tool Botton and show title bar text
    if (custTitleBar)
    {
        custTitleBar->setButtonToolBarShow(false);
        custTitleBar->setTitle(title);
    }

    m_imageviewFile1 = nullptr;
    m_imageviewDiff  = nullptr;
    m_imageviewFile2 = nullptr;

    m_manualModelViewFlip = 0;

    // Add this to the class Construtor.....
    m_fileName   = title;
    m_sourceFile = file1;
    m_destFile   = file2;

    QFile     f(file1);
    QFileInfo fileInfo(f.fileName());
    m_tabName = fileInfo.fileName();

    QFile     f2(file2);
    QFileInfo fileInfo2(f2.fileName());
    m_tabName.append(" with " + fileInfo2.fileName());

    setWindowTitle(m_tabName);

    m_newWidget  = new QWidget(this);
    m_layout     = new QVBoxLayout();
    m_MainWindow = new QMainWindow(this);

    m_innerHlayout = new QHBoxLayout();
    m_innerVlayout = new QVBoxLayout();

    m_newInnerWidget = new QWidget(this);

#ifdef USE_IMGDIFF
    m_diffMips      = NULL;
    m_analyzed      = "3DModel Diff";
    m_imageAnalysis = new C_AnalysisData();
#endif

    m_dockToolBar = new QToolBar(tr("dockToolbar"));
    m_dockToolBar->setObjectName("dockImageViewToolBar");

    m_CBManual_renderView = new QComboBox(this);
    m_CBManual_renderView->setToolTip("Manaul View: Enables 3D render view flips using spacebar, Auto View cycles original and processed every 2 frames");
    m_CBManual_renderView->addItem(tr("Auto View"));
    m_CBManual_renderView->addItem(tr("Manual View"));
    connect(m_CBManual_renderView, SIGNAL(currentIndexChanged(int)), this, SLOT(onManual_renderView(int)));

    m_dockToolBar->addWidget(m_CBManual_renderView);

    hlayoutAct   = new QAction(QIcon(":/compressonatorgui/images/horizontal.png"), tr("&Change to horizontal view"), this);
    orilayoutAct = new QAction(QIcon(":/compressonatorgui/images/orilayout.png"), tr("&Change to default view"), this);
    orilayoutAct->setDisabled(true);

    m_dockToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea | Qt::RightToolBarArea | Qt::LeftToolBarArea);

    //add spacer to right align tool buttons
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_dockToolBar->addWidget(spacer);

    if (hlayoutAct)
    {
        m_dockToolBar->addAction(hlayoutAct);
        connect(hlayoutAct, SIGNAL(triggered()), this, SLOT(changeLayout()));
    }
    if (orilayoutAct)
    {
        m_dockToolBar->addAction(orilayoutAct);
        connect(orilayoutAct, SIGNAL(triggered()), this, SLOT(resetLayout()));
    }

    m_MainWindow->addToolBar(m_dockToolBar);

    showProgressBusy("Loading Models Difference... This may take some time. Please wait.");
    showProgressDialog("Loading Model Difference");
    if (g_pProgressDlg)
        g_pProgressDlg->SetLabelText("Generating Models(s): Process may cycle a few times...");

    if (g_pProgressDlg)
        g_pProgressDlg->SetLabelText("Processing Model View");

    // Set a render desult size to start with
    m_newInnerWidget->resize(640, 480);

    if (createImageViews(isCompressed))
    {
        m_setHorizontalView = false;
        setDefaultView();

        m_newInnerWidget->setLayout(m_innerHlayout);
        m_MainWindow->setCentralWidget(m_newInnerWidget);
        m_layout->addWidget(m_MainWindow);

        emitUpdateData();

        m_newWidget->setLayout(m_layout);
        setWidget(m_newWidget);
    }

    hideProgressDialog();
    hideProgressBusy("Ready");
}

void C3DModelCompare::showProgressDialog(QString header)
{
    if (g_pProgressDlg)
    {
        g_pProgressDlg->SetValue(0);
        g_pProgressDlg->SetHeader(header);
        g_pProgressDlg->SetLabelText("");
        g_pProgressDlg->show();
    }
}

void C3DModelCompare::hideProgressDialog()
{
    if (g_pProgressDlg)
        g_pProgressDlg->hide();
}

void C3DModelCompare::showProgressBusy(QString Message)
{
    m_parent->statusBar()->showMessage(Message);
    qApp->setOverrideCursor(Qt::BusyCursor);
}

void C3DModelCompare::hideProgressBusy(QString Message)
{
    m_parent->statusBar()->showMessage(Message);
    qApp->restoreOverrideCursor();
}

void C3DModelCompare::onManual_renderView(int mode)
{
    if (m_imageviewDiff)
    {
        m_imageviewDiff->setManualViewFlip(mode);
        m_manualModelViewFlip = mode;
        if (m_imageviewDiff->custTitleBar)
        {
            if (mode == 0)
                m_imageviewDiff->custTitleBar->setTitle("3D Model Diff");
            else
                m_imageviewDiff->custTitleBar->setTitle("3D Model Diff: Original");
        }
    }
}

void C3DModelCompare::changeLayout()
{
    setHorizontalView();
    orilayoutAct->setDisabled(false);
    hlayoutAct->setDisabled(true);
    return;
}

void C3DModelCompare::resetLayout()
{
    setDefaultView();
    orilayoutAct->setDisabled(true);
    hlayoutAct->setDisabled(false);
    return;
}

void C3DModelCompare::emitUpdateData()
{
    emit UpdateData(NULL);
    return;
}

bool C3DModelCompare::createImageViews(bool isCompressedCompare)
{
    Setting* setting       = new Setting;
    setting->onBrightness  = false;
    setting->fDiffContrast = g_Application_Options.m_imagediff_contrast;

    QString file1Title = m_sourceFile;
    QString file2Title = m_destFile;
    if (isCompressedCompare)
    {
        file1Title = "Original";
        file2Title = "Processed";
    }
    else
    {
        file1Title = "File#1";
        file2Title = "File#2";
    }

    if (g_pProgressDlg)
    {
        g_pProgressDlg->SetLabelText("Loading source model...");
    }
    m_imageviewFile1 = new cp3DModelView(m_sourceFile, "", "  " + file1Title + " Model", m_newInnerWidget);
    if (m_imageviewFile1 == NULL)
        return false;
    if (m_imageviewFile1->custTitleBar)
    {
        m_imageviewFile1->custTitleBar->setButtonToolBarShow(false);
        m_imageviewFile1->custTitleBar->setButtonCloseEnabled(false);
    }

    // Notes: BugFix added change of m_FileName in construct above to prevent main applications FindFile to use these images as
    // Been found as Main Apps Tabs views
    m_imageviewFile1->m_fileName = file1Title + ": " + m_sourceFile;

    m_imageviewFile1->setFeatures(NoDockWidgetFeatures);

    if (g_pProgressDlg)
    {
        g_pProgressDlg->SetLabelText("Loading destination model...");
    }
    m_imageviewFile2 = new cp3DModelView(m_destFile, "", "  " + file2Title + " Model", m_newInnerWidget);
    if (m_imageviewFile2 == NULL)
    {
        delete (m_imageviewFile1);
        return false;
    }

    m_imageviewFile2->m_fileName = file2Title + ": " + m_destFile;
    if (m_imageviewFile2->custTitleBar)
    {
        m_imageviewFile2->custTitleBar->setButtonToolBarShow(false);
        m_imageviewFile2->custTitleBar->setButtonCloseEnabled(false);
    }

    m_imageviewFile2->setFeatures(NoDockWidgetFeatures);

#ifdef USE_IMGDIFF
    setting->onBrightness                                   = true;
    setting->generateDiff                                   = true;
    QString                                        diffFile = QString::fromUtf8(m_analyzed.c_str());
    setting->input_image = eImageViewState::isDiff m_imageviewDiff = new cpImageView(diffFile, "  Diff Image", m_newInnerWidget, m_diffMips, setting);
    if (m_imageviewDiff == NULL)
    {
        delete (m_imageviewFile1);
        delete (m_imageviewFile2);
        return false;
    }

    m_imageviewDiff->m_useOriginalImageCursor = true;
    m_imageviewDiff->showToobar(true);
    m_imageviewDiff->m_acImageView->m_isDiffView = true;
    setting->generateDiff                        = false;
    for (int i = 0; i < DEFAULT_BRIGHTNESS_LEVEL; i++)
    {
        m_imageviewDiff->m_acImageView->onToggleImageBrightnessUp();
    }
    m_imageviewDiff->m_fileName = "Diff: " + diffFile;

    m_imageviewDiff->custTitleBar->setButtonCloseEnabled(false);
    m_imageviewDiff->setFeatures(NoDockWidgetFeatures);
#else
    if (g_pProgressDlg)
    {
        g_pProgressDlg->SetLabelText("Generating model difference...");
    }

    m_imageviewDiff = new cp3DModelView(m_sourceFile, m_destFile, "3D Model Diff View", m_newInnerWidget);
    if (m_imageviewDiff == NULL)
    {
        delete (m_imageviewFile1);
        delete (m_imageviewFile2);
        return false;
    }

    m_imageviewDiff->m_fileName = file2Title + ": " + m_destFile;
    if (m_imageviewDiff->custTitleBar)
    {
        m_imageviewDiff->custTitleBar->setTitle("3D Model Diff View");
        m_imageviewDiff->custTitleBar->setButtonToolBarShow(false);
        m_imageviewDiff->custTitleBar->setButtonCloseEnabled(false);
    }
    m_imageviewDiff->setFeatures(NoDockWidgetFeatures);
#endif

    return true;
}

void C3DModelCompare::setDefaultView()
{
    if (m_setHorizontalView)
    {
        if (m_imageviewFile1)
            m_innerHlayout->addWidget(m_imageviewFile1);
        if (m_imageviewDiff)
            m_innerHlayout->addWidget(m_imageviewDiff);
        if (m_imageviewFile2)
            m_innerHlayout->addWidget(m_imageviewFile2);
        m_setHorizontalView = false;
    }

    if (m_imageviewDiff)
        m_innerHlayout->addWidget(m_imageviewDiff);
    if (m_imageviewFile1)
        m_innerVlayout->addWidget(m_imageviewFile1);
    if (m_imageviewFile2)
        m_innerVlayout->addWidget(m_imageviewFile2);

    m_innerHlayout->addLayout(m_innerVlayout);

    if (m_imageviewDiff)
        m_imageviewDiff->show();

    if (m_imageviewFile1)
        m_imageviewFile1->show();

    if (m_imageviewFile2)
        m_imageviewFile2->show();

    return;
}

void C3DModelCompare::setHorizontalView()
{
    // Remove the Default Layout
    m_imageviewDiff->hide();
    m_imageviewFile1->hide();
    m_imageviewFile2->hide();
    m_innerHlayout->removeWidget(m_imageviewDiff);
    m_innerVlayout->removeWidget(m_imageviewFile1);
    m_innerVlayout->removeWidget(m_imageviewFile2);
    m_innerHlayout->removeItem(m_innerVlayout);

    // Set the new layout
    m_innerHlayout->addWidget(m_imageviewFile1);
    m_innerHlayout->addWidget(m_imageviewDiff);
    m_innerHlayout->addWidget(m_imageviewFile2);
    m_imageviewDiff->show();
    m_imageviewFile1->show();
    m_imageviewFile2->show();
    m_setHorizontalView = true;
    return;
}

C3DModelCompare::~C3DModelCompare()
{
#ifdef USE_IMGDIFF
    if (m_diffMips)
    {
        CImageLoader ImageLoader;
        ImageLoader.clearMipImages(&m_diffMips);
    }

    if (m_imageAnalysis)
    {
        delete m_imageAnalysis;
    }
#endif

    if (m_imageviewFile1)
    {
        delete m_imageviewFile1;
    }

    if (m_imageviewFile2)
    {
        delete m_imageviewFile2;
    }

    if (m_imageviewDiff)
    {
        delete m_imageviewDiff;
    }
}
