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

#ifndef MODELCONVERT_H
#define MODELCONVERT_H

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>


class CModelConvert : public QWidget {
    Q_OBJECT

  public:

    CModelConvert(const QString title, QWidget *parent);
    ~CModelConvert();
    void setSourceFile(const QString source);
    QString SetDefaultExt(QString FilePathName);
    void clearStatus();

  private:
    void closeEvent(QCloseEvent * event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);


    // Common for all
    QWidget                     *m_newWidget;
    QGridLayout                 *m_layout;
    const QString                m_title;
    QWidget                     *m_parent;
    QStatusBar                  *m_statusBar;
    QLabel                      *m_labelStatus;

    QString                      m_RecentDir;
    QString                      m_SupportedModelFormats;

    QLineEdit                   *m_sourceFile;
    QLineEdit                   *m_destFile;

    QPushButton                 *PBConvert;

  Q_SIGNALS:

    void convertModel(int levels, QTreeWidgetItem *item);

  public Q_SLOTS:
    void onExit();
    void onConvert();

    void onOpenSourceFile();
    void onOpenDestinationFile();

  public:
    QTreeWidgetItem *m_mipsitem;
};

#endif