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
/// \file acDockWidgetTitleBar.cpp
/// \version 2.21
//
//=====================================================================

#include "acdockwidgettitlebar.h"


acDockWidgetTitlebar::acDockWidgetTitlebar(QWidget *parent) : parent(parent) {
    QString PushButtonStyle("QPushButton {border:none; margin: 0px; padding: 0px } QPushButton:hover {border:1px solid black}");

    QPalette Pal(palette());
    Pal.setColor(QPalette::Background, Qt::lightGray);
    setAutoFillBackground(true);
    setPalette(Pal);
    setMaximumHeight(20);
    m_close = false;

    m_parent = parent;

    // Create a close button and set its icon to that of the OS
    m_ToolBarIcon = new QIcon(":/compressonatorgui/images/settings.png");

    m_buttonToolBar = new QPushButton(this);
    m_buttonToolBar->setIcon(*m_ToolBarIcon);
    m_buttonToolBar->setStyleSheet(PushButtonStyle);
    m_buttonToolBar->setToolTip("Show Tool Bar");
    m_buttonToolBarEnabled = true;


    // Get the OS close button icon
    QStyle*style = qApp->style();

    QIcon minimizeIcon = style->standardIcon(QStyle::SP_TitleBarMinButton);
    // Create a maximize button and set its icon to that of the OS
    m_buttonMinimize = new QPushButton(this);
    m_buttonMinimize->setIcon(minimizeIcon);
    m_buttonMinimize->setStyleSheet(PushButtonStyle);
    m_buttonMinimize->setToolTip("Minimize Window");

    QIcon maximizeIcon = style->standardIcon(QStyle::SP_TitleBarMaxButton);
    // Create a maximize button and set its icon to that of the OS
    m_buttonMaximize = new QPushButton(this);
    m_buttonMaximize->setIcon(maximizeIcon);
    m_buttonMaximize->setStyleSheet(PushButtonStyle);
    m_buttonMaximize->setToolTip("Maximize Window");

    QIcon normalIcon = style->standardIcon(QStyle::SP_TitleBarNormalButton);
    // Create a maximize button and set its icon to that of the OS
    m_buttonNormal = new QPushButton(this);
    m_buttonNormal->setIcon(normalIcon);
    m_buttonNormal->setStyleSheet(PushButtonStyle);
    m_buttonNormal->setToolTip("Normal Window");
    m_buttonNormal->hide();

    QIcon closeIcon = style->standardIcon(QStyle::SP_TitleBarCloseButton);

    // Create a close button and set its icon to that of the OS
    m_buttonClose = new QPushButton(this);
    //m_buttonClose->setIcon(qicon(":/compressonatorgui/images/cxclose.png"));
    m_buttonClose->setIcon(closeIcon);
    m_buttonClose->setStyleSheet(PushButtonStyle);
    m_buttonClose->setToolTip("Close Window");
    m_ButtonCloseEnabled = true;

    // Reserve a label for the Titlebar
    m_label = new QLabel("",this);

    // Set a layout for the new label and pushbutton
    m_layout = new QHBoxLayout(this);
    m_layout->addWidget(m_label,Qt::AlignLeading);
    m_layout->addWidget(m_buttonToolBar);
    m_layout->addWidget(m_buttonMinimize);
    m_layout->addWidget(m_buttonNormal);
    m_layout->addWidget(m_buttonMaximize);
    m_layout->addWidget(m_buttonClose);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_layout);

    connect(m_buttonMinimize, SIGNAL(clicked()), this, SLOT(onMinimize()));
    connect(m_buttonNormal, SIGNAL(clicked()), this, SLOT(onNormal()));
    connect(m_buttonMaximize, SIGNAL(clicked()), this, SLOT(onMaximize()));
    connect(m_buttonClose, SIGNAL(clicked()), this, SLOT(onClose()));
    connect(m_buttonToolBar, SIGNAL(clicked()), this, SLOT(OnToolBarClicked()));

    setButtonMinMaxEnabled(false);

}

QString acDockWidgetTitlebar::getTitle() {
    if (m_label)
        return (m_label->text());
    else
        return "_BadTitle_";
}

void acDockWidgetTitlebar::setTitle(QString title) {
    m_label->setText(title);
}

acDockWidgetTitlebar::~acDockWidgetTitlebar() {

}


void acDockWidgetTitlebar::onClose() {
    if (m_parent)
        m_parent->close();
    if (m_close) {
        QString labelText = m_label->text();
        emit OnAboutToClose(labelText);
    }
}


void acDockWidgetTitlebar::onMinimize() {
    if (m_parent)
        m_parent->showMinimized();
}

void acDockWidgetTitlebar::onMaximize() {
    m_buttonNormal->show();
    m_buttonMaximize->hide();
    if (m_parent)
        m_parent->showMaximized();
}


void acDockWidgetTitlebar::onNormal() {
    m_buttonNormal->hide();
    m_buttonMaximize->show();
    if (m_parent)
        m_parent->showNormal();
}



void acDockWidgetTitlebar::setButtonMinMaxEnabled(bool enabled) {
    if (enabled) {
        m_buttonMinimize->show();
        m_buttonNormal->hide();
        m_buttonMaximize->show();
    } else {
        m_buttonMinimize->hide();
        m_buttonNormal->hide();
        m_buttonMaximize->hide();
    }
}


void acDockWidgetTitlebar::setButtonCloseEnabled(bool enabled) {
    m_ButtonCloseEnabled = enabled;
    m_buttonClose->setEnabled(enabled);
    if (enabled) {
        m_buttonMinimize->show();
        m_buttonNormal->show();
        m_buttonMaximize->show();
        m_buttonClose->show();
    } else {
        m_buttonMinimize->hide();
        m_buttonNormal->hide();
        m_buttonMaximize->hide();
        m_buttonClose->hide();
    }
}


void acDockWidgetTitlebar::setButtonToolBarEnabled(bool enabled) {
    m_buttonToolBarEnabled = enabled;
    m_buttonToolBar->setEnabled(enabled);
    if (enabled)
        m_buttonToolBar->show();
    else
        m_buttonToolBar->hide();
}

void acDockWidgetTitlebar::setButtonToolBarShow(bool isShowing) {
    if (isShowing)
        m_buttonToolBar->show();
    else
        m_buttonToolBar->hide();
}


void acDockWidgetTitlebar::mousePressEvent(QMouseEvent *event) {
    //if (event->button() == Qt::LeftButton)
    //    cursor = event->globalPos() - geometry().topLeft();
    event->ignore();
}

void acDockWidgetTitlebar::mouseMoveEvent(QMouseEvent *event) {
    //if (event->buttons() & Qt::LeftButton)
    //    parent->move(event->globalPos() - cursor);
    event->ignore();
}

void acDockWidgetTitlebar::OnToolBarClicked() {
    emit ToolBarCliked();
}
