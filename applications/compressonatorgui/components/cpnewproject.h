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
//=====================================================================

#ifndef CPNEWPROJECT_H
#define CPNEWPROJECT_H

#include <QtWidgets>

class cpNewProject : public QWidget {
    Q_OBJECT

  public:
    cpNewProject(QWidget *parent = NULL);
    void GetNewFilePathName(QString OldFilePathName);


  private:
    // Common for all
    QHBoxLayout                 *m_HlayoutName;
    QHBoxLayout                 *m_HlayoutDestination;
    QHBoxLayout                 *m_HlayoutButtons;
    QVBoxLayout                 *m_VlayoutWindow;
    QVBoxLayout                 *m_VlayoutDestination;

    QWidget                     *m_newWidget;
    const QString                m_title;
    QWidget                     *m_parent;
    QPushButton                 *m_PBOk;
    QPushButton                 *m_PBCompress;
    QPushButton                 *m_PBCancel;
    QPushButton                 *m_PBDestFileFolder;
    QLineEdit                   *m_DestinationFile;
    QLineEdit                   *m_LEName;
    QLineEdit                   *m_DestinationFolder;
    QComboBox                   *m_CBCompression;
    QComboBox                   *m_fileFormats;
    QStringList                 m_AllFileTypes;
    QGridLayout                 *m_layout;               // This Widgets layout is a Single GraphicsView

    QString                     m_FilePathName;

  protected:
    void keyPressEvent(QKeyEvent *event);

  Q_SIGNALS:

    void    OnSetNewProject(QString &FilePathName);

  public Q_SLOTS:

    void    onPBOk();
    void    onPBCancel();
    void    onDestFileFolder();

};

#endif