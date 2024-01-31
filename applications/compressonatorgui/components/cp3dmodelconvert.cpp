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

#include "cp3dmodelconvert.h"

#include "plugininterface.h"
#include "cpmaincomponents.h"

#ifdef USE_ASSIMP
#include <assimp/scene.h>

#include <assimp/Exporter.hpp>
#include <assimp/Importer.hpp>
#endif

extern PluginManager g_pluginManager;
extern CMIPS*        g_GUI_CMIPS;

CModelConvert::CModelConvert(const QString title, QWidget* parent)
    : QWidget(parent)
    , m_title(title)
    , m_parent(parent)
{
    setWindowTitle(title);
    setWindowFlags(Qt::Dialog);
    Qt::WindowFlags flags    = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags                    = flags & (~helpFlag);
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);
    resize(300, 80);

    m_RecentDir             = "";
    m_SupportedModelFormats = "Model Files (*.gltf *.obj)";

    m_statusBar = new QStatusBar(this);
    m_statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey; border-bottom: 1px outset grey;}");
    m_labelStatus = new QLabel(this);
    m_labelStatus->setText("");
    m_labelStatus->setAlignment(Qt::AlignLeft);
    m_statusBar->addPermanentWidget(m_labelStatus, 40);

    QVBoxLayout* pVerticalLayout = new QVBoxLayout;

    // Buttons
    QPushButton* m_PBFile1 = new QPushButton("...");
    connect(m_PBFile1, SIGNAL(pressed()), this, SLOT(onOpenSourceFile()));

    QPushButton* m_PBFile2 = new QPushButton("...");
    connect(m_PBFile2, SIGNAL(pressed()), this, SLOT(onOpenDestinationFile()));

    //Add file1 and file2 ui component for diff
    QHBoxLayout* file1Layout = new QHBoxLayout;
    QLabel*      file1Label  = new QLabel(this);
    file1Label->setText("Source Model      : ");

    m_sourceFile = new QLineEdit;
    m_sourceFile->dragEnabled();
    m_sourceFile->setMinimumSize(300, 12);
    file1Layout->addWidget(file1Label);
    file1Layout->addWidget(m_sourceFile);
    file1Layout->addWidget(m_PBFile1);

    QHBoxLayout* file2Layout = new QHBoxLayout;
    QLabel*      file2Label  = new QLabel(this);
    file2Label->setText("Destination Model: ");

    m_destFile = new QLineEdit;
    m_destFile->setMinimumSize(300, 12);
    file2Layout->addWidget(file2Label);
    file2Layout->addWidget(m_destFile);
    file2Layout->addWidget(m_PBFile2);

    pVerticalLayout->addLayout(file1Layout);
    pVerticalLayout->addLayout(file2Layout);
    pVerticalLayout->setMargin(10);

    // Buttons
    QHBoxLayout* layoutButtons = new QHBoxLayout;
    PBConvert                  = new QPushButton("Convert");
    PBConvert->setEnabled(false);
    connect(PBConvert, SIGNAL(clicked()), this, SLOT(onConvert()));

    layoutButtons->addWidget(PBConvert);
    QPushButton* PBExit = new QPushButton("Exit");
    layoutButtons->addWidget(PBExit);
    connect(PBExit, SIGNAL(clicked()), this, SLOT(onExit()));

    QGridLayout* layout = new QGridLayout(this);
    layout->addLayout(pVerticalLayout, 0, 0);
    layout->addLayout(layoutButtons, 1, 0);
    layout->addWidget(m_statusBar, 2, 0);

    layout->setAlignment(Qt::AlignTop);

    this->setLayout(layout);
}

CModelConvert::~CModelConvert()
{
}

void CModelConvert::dragEnterEvent(QDragEnterEvent* event)
{
    qDebug() << "drag entering: ";
    event->acceptProposedAction();
}

void CModelConvert::dropEvent(QDropEvent* event)
{
    Q_UNUSED(event);
    qDebug() << "drop received!";
}

QString CModelConvert::SetDefaultExt(QString FilePathName)
{
    // Set the Destination Path Name the same but default it to .gltf ext
    QFileInfo info(FilePathName);
    QString   ext = info.suffix();

    if (ext.compare("gltf", Qt::CaseInsensitive) == 0)
        ext = ".obj";
    else
        ext = ".gltf";

    return info.path() + "/" + info.completeBaseName() + ext;
}

void CModelConvert::setSourceFile(const QString source)
{
    m_destFile->setText("");

    if (source.length() > 0)
    {
        m_sourceFile->setText(source);
        PBConvert->setEnabled(true);
        QString strNewName = SetDefaultExt(source);
        m_destFile->setText(strNewName);
        m_destFile->setEnabled(true);
    }
    else
    {
        m_sourceFile->setText("Select source model using ... or Drag and Drop file here");
        PBConvert->setEnabled(false);
        m_destFile->setEnabled(false);
    }
}

void CModelConvert::clearStatus()
{
    m_labelStatus->setText("");
}

void CModelConvert::onOpenSourceFile()
{
    //QFileDialog to open image file #1
    QString file1 = QFileDialog::getOpenFileName(this, tr("Open source model file"), m_RecentDir, m_SupportedModelFormats);
    if (!file1.isEmpty())
    {
        m_sourceFile->setText(file1);
        PBConvert->setEnabled(true);
        QString strNewName = SetDefaultExt(file1);
        m_destFile->setText(strNewName);
        m_destFile->setEnabled(true);
    }
}

void CModelConvert::onOpenDestinationFile()
{
    //QFileDialog to open image file #2
    QString file2 = QFileDialog::getOpenFileName(this, tr("Open destination model file"), m_RecentDir, m_SupportedModelFormats);
    if (!file2.isEmpty())
    {
        m_destFile->setText(file2);
    }
}

void CModelConvert::onConvert()
{
#ifdef USE_ASSIMP
    PBConvert->setEnabled(false);
    Assimp::Importer importer;
    Assimp::Exporter exporter;
    const aiScene*   inputScene;
    char             drive[_MAX_DRIVE];
    char             dir[_MAX_DIR];
    char             fname[_MAX_FNAME];
    char             ext[_MAX_EXT];

    m_labelStatus->setText("Please wait: loading model ...");
    // We are goint to use a simple load and only build normals if there are none
    // for viewers we use Trianagles for rendering.
    inputScene = importer.ReadFile(m_sourceFile->text().toLatin1(), 0u);

    m_labelStatus->setText("Please wait: saving model ...");
    // Ready to save!
    // Determine the save type based on detination file extension: In future users will be able to
    // decide if they want to save to another model file type
    _splitpath_s(m_destFile->text().toLatin1(), drive, _MAX_DRIVE, dir, _MAX_DIR, fname, _MAX_FNAME, ext, _MAX_EXT);
    std::string Ext(ext);
    size_t      offset = Ext.find(".");
    if (offset < Ext.length() - 1)
        Ext.erase(offset, 1);
    aiReturn rv = exporter.Export(inputScene, Ext.c_str(), m_destFile->text().toLatin1(), 0);

    if (rv != aiReturn_SUCCESS)
    {
        m_labelStatus->setText("Error saving destination file");
    }

    m_labelStatus->setText("Conversion done");

    PBConvert->setEnabled(true);
#endif
}

void CModelConvert::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}

void CModelConvert::onExit()
{
    hide();
}
