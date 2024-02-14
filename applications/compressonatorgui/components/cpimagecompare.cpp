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

// for XML file processing
#include <cmp_rapidxml.hpp>

#include "cpimagecompare.h"
#include "cpmaincomponents.h"

CImageCompare::CImageCompare(const QString title, QString file1, QString file2, bool isCompressed, QMainWindow* parent)
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

    ssimAnalysis = false;
    psnrAnalysis = false;
    m_newWidget  = new QWidget(this);
    m_layout     = new QVBoxLayout();
    m_MainWindow = new QMainWindow(this);

    m_innerHlayout = new QHBoxLayout();
    m_innerVlayout = new QVBoxLayout();

    m_newInnerWidget = new QWidget(this);
    m_imageAnalysis  = new C_AnalysisData();
    m_ssimAnalysis   = new C_SSIM_Analysis();
    m_psnrAnalysis   = new C_PSNR_MSE_Analysis();
    m_allAnalysis    = new C_MSE_PSNR_Analysis();

    m_dockToolBar = new QToolBar(tr("dockToolbar"));
    m_dockToolBar->setObjectName("dockImageViewToolBar");

    ssimAct    = new QAction(QIcon(":/compressonatorgui/images/ssim.png"), tr("&Run SSIM Analysis"), this);
    psnrmseAct = new QAction(QIcon(":/compressonatorgui/images/psnrmse.png"), tr("&Run PSNR & MSE Analysis"), this);

    hlayoutAct   = new QAction(QIcon(":/compressonatorgui/images/horizontal.png"), tr("&Change to horizontal view"), this);
    orilayoutAct = new QAction(QIcon(":/compressonatorgui/images/orilayout.png"), tr("&Change to default view"), this);
    orilayoutAct->setDisabled(true);

    m_dockToolBar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea | Qt::RightToolBarArea | Qt::LeftToolBarArea);

    //add spacer to right align tool buttons
    QWidget* spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_dockToolBar->addWidget(spacer);

    if (ssimAct)
    {
        m_dockToolBar->addAction(ssimAct);
        connect(ssimAct, SIGNAL(triggered()), this, SLOT(runSSIM()));
    }
    if (psnrmseAct)
    {
        m_dockToolBar->addAction(psnrmseAct);
        connect(psnrmseAct, SIGNAL(triggered()), this, SLOT(runPsnrMse()));
    }

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

    showProgressBusy("Loading Image Difference... This may take some time. Please wait.");
    showProgressDialog("Loading Image Difference");
    if (g_pProgressDlg)
        g_pProgressDlg->SetLabelText("Generating Image(s): Process may cycle a few times...");

    m_diffMips = m_imageAnalysis->GenerateDiffImage(m_sourceFile.toStdString().c_str(), m_destFile.toStdString().c_str());

    if (m_diffMips != NULL)
    {
        m_analyzed = "Image Diff";

        if (g_pProgressDlg)
            g_pProgressDlg->SetLabelText("Processing Image View");
        createImageView(isCompressed);
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

void CImageCompare::showProgressDialog(QString header)
{
    if (g_pProgressDlg)
    {
        g_pProgressDlg->SetValue(0);
        g_pProgressDlg->SetHeader(header);
        g_pProgressDlg->SetLabelText("");
        g_pProgressDlg->show();
    }
}

void CImageCompare::hideProgressDialog()
{
    if (g_pProgressDlg)
        g_pProgressDlg->hide();
}

void CImageCompare::showProgressBusy(QString Message)
{
    m_parent->statusBar()->showMessage(Message);
    qApp->setOverrideCursor(Qt::BusyCursor);
}

void CImageCompare::hideProgressBusy(QString Message)
{
    m_parent->statusBar()->showMessage(Message);
    qApp->restoreOverrideCursor();
}

CMipImages* CImageCompare::getMdiffMips()
{
    return this->m_diffMips;
}

void CImageCompare::showEvent(QShowEvent* ev)
{
    Q_UNUSED(ev);
    // emit UpdateData(m_imageAnalysis);
}

void CImageCompare::changeLayout()
{
    setHorizontalView();
    orilayoutAct->setDisabled(false);
    hlayoutAct->setDisabled(true);
    return;
}

void CImageCompare::resetLayout()
{
    setDefaultView();
    orilayoutAct->setDisabled(true);
    hlayoutAct->setDisabled(false);
    return;
}

void CImageCompare::emitUpdateData()
{
    if (ssimAnalysis && !psnrAnalysis)
        emit UpdateData(m_ssimAnalysis);
    else if (!ssimAnalysis && psnrAnalysis)
        emit UpdateData(m_psnrAnalysis);
    else if (ssimAnalysis && psnrAnalysis)
        emit UpdateData(m_allAnalysis);
    else
        emit UpdateData(nullptr);
    return;
}

void CImageCompare::runSSIM()
{
    ssimAnalysis = true;

    showProgressBusy("Running SSIM Analysis... This may take some time. Please Wait.");
    showProgressDialog("Running SSIM Analysis");

    m_imageAnalysis->GenerateSSIMAnalysis(m_sourceFile.toStdString().c_str(), m_destFile.toStdString().c_str());
    if (setAnalysisResultView())
    {
        emitUpdateData();
    }

    hideProgressDialog();
    hideProgressBusy("Ready");
    return;
}

void CImageCompare::runPsnrMse()
{
    psnrAnalysis = true;

    showProgressBusy("Running PSNR and MSE Analysis... This may take some time. Please Wait.");
    showProgressDialog("Running PSNR and MSE Analysis");

    m_imageAnalysis->GeneratePSNRMSEAnalysis(m_sourceFile.toStdString().c_str(), m_destFile.toStdString().c_str());

    if (setAnalysisResultView())
    {
        emitUpdateData();
    }

    hideProgressDialog();
    hideProgressBusy("Ready");

    return;
}

void CImageCompare::createImageView(bool isCompressedCompare)
{
    if (m_sourceFile.length() == 0)
        return;

    Setting* setting = new Setting;
    //setting->onBrightness  = false;
    setting->fDiffContrast = g_Application_Options.m_imagediff_contrast;

    QString file1Title = m_sourceFile;
    QString file2Title = m_destFile;
    if (isCompressedCompare)
    {
        file1Title = "Original";
        file2Title = "Compressed";
    }
    else
    {
        file1Title = "File#1";
        file2Title = "File#2";
    }

    setting->input_image = eImageViewState::isOriginal;
    m_imageviewFile1     = new cpImageView(m_sourceFile, "  " + file1Title + " Image", m_newInnerWidget, nullptr, setting);

    // Notes: BugFix added change of m_FileName in construct above to prevent main applications FindFile to use these images as
    // Been found as Main Apps Tabs views
    m_imageviewFile1->m_fileName = file1Title + ": " + m_sourceFile;

    m_imageviewFile1->custTitleBar->setButtonCloseEnabled(false);
    m_imageviewFile1->setFeatures(NoDockWidgetFeatures);
    setting->input_image = eImageViewState::isProcessed;
    m_imageviewFile2     = new cpImageView(m_destFile, "  " + file2Title + " Image", m_newInnerWidget, nullptr, setting);

    m_imageviewFile2->m_fileName = file2Title + ": " + m_destFile;

    m_imageviewFile2->custTitleBar->setButtonCloseEnabled(false);
    m_imageviewFile2->setFeatures(NoDockWidgetFeatures);

    setting->onBrightness = true;
    setting->generateDiff = true;
    setting->input_image  = eImageViewState::isDiff;
    QString diffFile      = QString::fromUtf8(m_analyzed.c_str());

    m_imageviewDiff = new cpImageView(diffFile, "  Diff Image", m_newInnerWidget, m_diffMips, setting);

    m_imageviewDiff->m_useOriginalImageCursor = true;
    m_imageviewDiff->showToobar(false);
    m_imageviewDiff->m_acImageView->m_isDiffView = true;
    setting->generateDiff                        = false;
    m_imageviewDiff->m_fileName                  = "Diff: " + diffFile;

    m_imageviewDiff->custTitleBar->setButtonCloseEnabled(false);
    m_imageviewDiff->setFeatures(NoDockWidgetFeatures);

    // Virtual Mouse Setup
    connect(m_imageviewFile1->m_acImageView,
            SIGNAL(acImageViewMousePosition(QPointF*, QPointF*, int)),
            &m_mousehub,
            SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));
    connect(m_imageviewFile2->m_acImageView,
            SIGNAL(acImageViewMousePosition(QPointF*, QPointF*, int)),
            &m_mousehub,
            SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));
    connect(m_imageviewDiff->m_acImageView,
            SIGNAL(acImageViewMousePosition(QPointF*, QPointF*, int)),
            &m_mousehub,
            SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));

    connect(&m_mousehub,
            SIGNAL(VirtialMousePosition(QPointF*, QPointF*, int)),
            m_imageviewFile1->m_acImageView,
            SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));
    connect(&m_mousehub,
            SIGNAL(VirtialMousePosition(QPointF*, QPointF*, int)),
            m_imageviewFile2->m_acImageView,
            SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));
    connect(&m_mousehub,
            SIGNAL(VirtialMousePosition(QPointF*, QPointF*, int)),
            m_imageviewDiff->m_acImageView,
            SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));
}

void CImageCompare::setDefaultView()
{
    if (m_setHorizontalView)
    {
        m_innerHlayout->addWidget(m_imageviewFile1);
        m_innerHlayout->addWidget(m_imageviewDiff);
        m_innerHlayout->addWidget(m_imageviewFile2);
        m_setHorizontalView = false;
    }

    m_innerHlayout->addWidget(m_imageviewDiff);
    m_innerVlayout->addWidget(m_imageviewFile1);
    m_innerVlayout->addWidget(m_imageviewFile2);
    m_innerHlayout->addLayout(m_innerVlayout);

    m_imageviewDiff->show();
    m_imageviewFile1->show();
    m_imageviewFile2->show();

    return;
}

void CImageCompare::setHorizontalView()
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

//
// Add logic to this code to varify that
// 1 the AnlysisResults file exists
// 2 The DockWidget was created
// etc ..
//
bool CImageCompare::setAnalysisResultView()
{
    if (m_analyzed.length() < 1 || m_imageviewFile1 == nullptr || m_imageviewFile2 == nullptr || m_imageviewDiff == nullptr)
    {
        return false;
    }

    // check file exists
    if (CMP_FileExists(m_imageAnalysis->m_analysisFile))
    {
        // populate tree structure pt
        rapidxml::file<>*        xmlResultsFile = new rapidxml::file<>(m_imageAnalysis->m_analysisFile.c_str());
        rapidxml::xml_document<> xmlDoc;
        xmlDoc.parse<rapidxml::parse_no_data_nodes>(xmlResultsFile->data());

        // traverse pt
        rapidxml::xml_node<>* levelElement = xmlDoc.first_node("ANALYSIS");
        for (rapidxml::xml_node<>* child = levelElement->first_node(); child != NULL; child = child->next_sibling())
        {
            if (std::string(child->name()) == "DATA")
            {
                if (ssimAnalysis && !psnrAnalysis)
                {
                    m_ssimAnalysis->m_SSIM       = std::stod(child->first_node("SSIM")->value());
                    m_ssimAnalysis->m_SSIM_Blue  = std::stod(child->first_node("SSIM_BLUE")->value());
                    m_ssimAnalysis->m_SSIM_Green = std::stod(child->first_node("SSIM_GREEN")->value());
                    m_ssimAnalysis->m_SSIM_Red   = std::stod(child->first_node("SSIM_RED")->value());
                }
                else if (!ssimAnalysis && psnrAnalysis)
                {
                    m_psnrAnalysis->m_MSE        = std::stod(child->first_node("MSE")->value());
                    m_psnrAnalysis->m_PSNR       = std::stod(child->first_node("PSNR")->value());
                    m_psnrAnalysis->m_PSNR_Blue  = std::stod(child->first_node("PSNR_BLUE")->value());
                    m_psnrAnalysis->m_PSNR_Green = std::stod(child->first_node("PSNR_GREEN")->value());
                    m_psnrAnalysis->m_PSNR_Red   = std::stod(child->first_node("PSNR_RED")->value());
                }
                else if (ssimAnalysis && psnrAnalysis)
                {
                    m_allAnalysis->m_SSIM       = std::stod(child->first_node("SSIM")->value());
                    m_allAnalysis->m_SSIM_Blue  = std::stod(child->first_node("SSIM_BLUE")->value());
                    m_allAnalysis->m_SSIM_Green = std::stod(child->first_node("SSIM_GREEN")->value());
                    m_allAnalysis->m_SSIM_Red   = std::stod(child->first_node("SSIM_RED")->value());
                    m_allAnalysis->m_MSE        = std::stod(child->first_node("MSE")->value());
                    m_allAnalysis->m_PSNR       = std::stod(child->first_node("PSNR")->value());
                    m_allAnalysis->m_PSNR_Blue  = std::stod(child->first_node("PSNR_BLUE")->value());
                    m_allAnalysis->m_PSNR_Green = std::stod(child->first_node("PSNR_GREEN")->value());
                    m_allAnalysis->m_PSNR_Red   = std::stod(child->first_node("PSNR_RED")->value());
                }
            }
        }
        // QFile::remove(QString::fromStdString(m_analyzedResult));
        xmlDoc.clear();
        delete xmlResultsFile;
        return true;
    }

    // something went wronge return failed
    return false;
}

CImageCompare::~CImageCompare()
{
    if ((m_diffMips != NULL) && (m_imageviewDiff != nullptr))
    {
        CImageLoader ImageLoader;
        ImageLoader.clearMipImages(&m_diffMips);
    }
    if (m_imageAnalysis)
    {
        delete m_imageAnalysis;
        m_imageAnalysis = nullptr;
    }
    if (m_ssimAnalysis)
    {
        delete m_ssimAnalysis;
        m_ssimAnalysis = nullptr;
    }
    if (m_psnrAnalysis)
    {
        delete m_psnrAnalysis;
        m_psnrAnalysis = nullptr;
    }
    if (m_allAnalysis)
    {
        delete m_allAnalysis;
        m_allAnalysis = nullptr;
    }
    if (m_imageviewFile1)
    {
        delete m_imageviewFile1;
        m_imageviewFile1 = nullptr;
    }
    if (m_imageviewFile2)
    {
        delete m_imageviewFile2;
        m_imageviewFile2 = nullptr;
    }
    if (m_imageviewDiff)
    {
        delete m_imageviewDiff;
        m_imageviewDiff = nullptr;
    }
}

void CImageCompare::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    //    emit UpdateData(m_imageAnalysis);
}
