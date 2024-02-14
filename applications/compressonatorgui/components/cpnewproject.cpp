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
//=====================================================================

#include <QtWidgets>

#include "cpnewproject.h"

cpNewProject::cpNewProject(QWidget* parent)
    : m_parent(parent)
{
    setWindowTitle("New Project");
    Qt::WindowFlags flags(Qt::Dialog | Qt::WindowCloseButtonHint | Qt::WindowTitleHint);
    setWindowFlags(flags);

    // //--------------------------------
    // // Destination Name
    // //--------------------------------
    QLabel* LName = new QLabel(tr("Name    "));
    LName->setToolTip("Enter a name for the new project");
    m_LEName = new QLineEdit;
    m_LEName->setEchoMode(QLineEdit::Normal);
    m_HlayoutName = new QHBoxLayout;
    m_HlayoutName->addWidget(LName);
    m_HlayoutName->addWidget(m_LEName);

    // =================================================P
    // Destination File
    // =================================================
    QLabel* m_lFolder = new QLabel;
    m_lFolder->setText("Location");
    m_lFolder->setToolTip("Set the project location");
    m_DestinationFolder = new QLineEdit;
    m_PBDestFileFolder  = new QPushButton("...", this);
    m_PBDestFileFolder->setAutoDefault(true);
    m_PBDestFileFolder->setToolTip("Open file browser");
    m_PBDestFileFolder->setMaximumWidth(30);
    QObject::connect(m_PBDestFileFolder, SIGNAL(clicked()), this, SLOT(onDestFileFolder()));

    m_HlayoutDestination = new QHBoxLayout;
    m_HlayoutDestination->addWidget(m_lFolder);
    m_HlayoutDestination->addWidget(m_DestinationFolder);
    m_HlayoutDestination->addWidget(m_PBDestFileFolder);

    //================
    // Buttons
    //================
    m_PBOk     = new QPushButton("Ok");
    m_PBCancel = new QPushButton("Cancel");
    m_PBOk->setAutoDefault(true);
    m_PBCancel->setAutoDefault(true);
    QObject::connect(m_PBOk, SIGNAL(clicked()), this, SLOT(onPBOk()));
    QObject::connect(m_PBCancel, SIGNAL(clicked()), this, SLOT(onPBCancel()));

    m_HlayoutButtons = new QHBoxLayout;
    m_HlayoutButtons->addStretch();
    m_HlayoutButtons->addWidget(m_PBOk);
    m_HlayoutButtons->addWidget(m_PBCancel);

    m_VlayoutWindow = new QVBoxLayout;
    m_VlayoutWindow->addLayout(m_HlayoutName);
    m_VlayoutWindow->addLayout(m_HlayoutDestination);
    m_VlayoutWindow->addLayout(m_HlayoutButtons);

    setLayout(m_VlayoutWindow);
    resize(400, 80);
}

void cpNewProject::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        this->close();
    }
    if (event->modifiers().testFlag(Qt::AltModifier))
    {
        m_LEName->clearFocus();
        m_DestinationFolder->clearFocus();
        if (event->key() == Qt::Key_O)
        {
            onPBOk();
        }
        if (event->key() == Qt::Key_C)
        {
            onPBCancel();
        }
    }
    //enter is detected as Key_Return
    if (event->key() == Qt::Key_Return)
    {
        this->onPBOk();
    }
}

void cpNewProject::GetNewFilePathName(QString OldFilePathName)
{
    m_LEName->setText("");

    // Strip old project name and get path only
    QFileInfo FilePathName(OldFilePathName);
    QDir      dir(FilePathName.absoluteDir());

    // Geth the Project Dir
    QString ProjectFilePath = dir.absolutePath();
    ProjectFilePath.replace("/", "\\");
    m_DestinationFolder->setText(ProjectFilePath);

    m_LEName->setFocus();
    show();
}

void cpNewProject::onPBCancel()
{
    hide();
}

void cpNewProject::onPBOk()
{
    // Get file name as enetered by the user
    QString FileName = m_LEName->text();

    // Remove cprj extension from file name
    if (FileName.contains(".cprj"))
    {
        QFileInfo FileInfo(FileName);
        FileName = FileInfo.baseName();
    }

    // Check if we have a name
    if (FileName.size() == 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Please enter a project name");
        msgBox.exec();
        return;
    }

    // Get the Destination folder
    QString Destination = m_DestinationFolder->text();
    if (Destination.size() == 0)
        Destination = "./";

    // Cover any cononical setting such as ./ and ../ to full path
    QFileInfo FilePathInfo(Destination);
    if (FilePathInfo.canonicalFilePath().size() > 0)
        Destination = FilePathInfo.canonicalFilePath();

    QCharRef c = Destination[Destination.length() - 1];

    if ((c != '/') || (c != '\\'))
        Destination.append("/");

    // now add the filename
    Destination.append(FileName);

    // check for pre-existing project that will get replaced with this new one
    QFile fileexist(Destination + ".cprj");
    if (fileexist.exists())
    {
        if (QMessageBox::question(this, "Project", "File already exists\nDo you want to continue?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            return;
    }

    emit OnSetNewProject(Destination);
    hide();
}

void cpNewProject::onDestFileFolder()
{
    QString fileFolder = QFileDialog::getExistingDirectory(this, tr("Destination Folder"), m_DestinationFolder->text());
    if (fileFolder.length() > 0)
    {
        m_DestinationFolder->setText(fileFolder);
    }
}