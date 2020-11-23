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

#include "cpcompressstatus.h"

CompressStatusDialog::CompressStatusDialog(const QString title, QWidget *parent) :
    QDockWidget(parent)
    // acCustomDockWidget(title, parent)
{
    m_parent       = parent;
    m_menu         = NULL;
    m_btnClearText = NULL;
    m_layoutH      = NULL;
    m_layoutV      = NULL;
    m_clear        = NULL;
    m_textBrowser  = NULL;
    m_newWidget    = NULL;

    // if (custTitleBar)
    // {
    //     custTitleBar->setTitle(title);
    //     custTitleBar->setButtonToolBarEnabled(false);
    // }
    //setAttribute(Qt::WA_DeleteOnClose);
    //setAttribute(Qt::WA_GroupLeader);
    //
    setWindowTitle("Output");
    //setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);

    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setMinimumHeight(32);
    m_textBrowser->setReadOnly(true);
    m_textBrowser->setAcceptRichText(true);

    /*
    This code has a clear button and widget above the editor
    Enable it if you want to use it or add addition items to the output window
    by defining USE_CLEAR_WIDGET
    */
#ifdef USE_CLEAR_WIDGET

    m_newWidget = new QWidget(parent);
    if (!m_newWidget) return;

    m_textBrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_textBrowser, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(OnshowContextMenu(const QPoint&)));

    m_menu = m_textBrowser->createStandardContextMenu();
    m_clear = new QAction(tr("&Clear text"), this);
    if (m_clear) {
        connect(m_clear, SIGNAL(triggered()), this, SLOT(onClearText()));
    }
    m_menu->addAction(m_clear);


    m_newWidget = new QWidget(this);
    m_btnClearText = new QPushButton("Clear ALL", this);
    //m_btnClearText->setMaximumHeight(16);

    m_layoutH = new QHBoxLayout(this);
    m_layoutH->addWidget(m_btnClearText);
    m_layoutH->addStretch();


    connect(m_btnClearText, SIGNAL(pressed()), this, SLOT(onClearText()));

   QGridLayout* m_layout = new QGridLayout(m_newWidget);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    m_layout->addLayout(m_layoutH, 0, 0);
    m_layout->addWidget(m_textBrowser, 1, 0);

    m_newWidget->setLayout(m_layout);
    setWidget(m_newWidget);
#else
    setWidget(m_textBrowser);
#endif
}

CompressStatusDialog::~CompressStatusDialog() {
    delete m_textBrowser;
}


void CompressStatusDialog::showOutput() {
    show();
}

void CompressStatusDialog::hideOutput() {
    hide();
}


void CompressStatusDialog::appendText(QString text) {
    if (m_textBrowser)
        m_textBrowser->append(text);
}

void CompressStatusDialog::onClearText() {
    if (m_textBrowser)
        m_textBrowser->clear();
}

void CompressStatusDialog::onShowContextMenu(const QPoint& point) {
    Q_UNUSED(point);
    if (m_menu)
        m_menu->exec(QCursor::pos());
}


void CompressStatusDialog::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}