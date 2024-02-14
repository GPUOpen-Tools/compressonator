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
/// \file acCustomDockWidget.cpp
/// \version 2.21
//
//=====================================================================

#include "accustomdockwidget.h"

acCustomDockWidget::acCustomDockWidget(const QString title, QWidget* parent)
    : QDockWidget(title, parent)
{
    custTitleBar = new acDockWidgetTitlebar(this);

    setTitleBarWidget(custTitleBar);
    QObject::connect(this, SIGNAL(topLevelChanged(bool)), this, SLOT(OntopLevelChanged(bool)));

    m_CustomTitle   = custTitleBar->getTitle();
    m_CentralWidget = new QWidget(this);
    m_CentralWidget->setStyleSheet("border: 1px solid black");
    setWidget(m_CentralWidget);
}

void acCustomDockWidget::OntopLevelChanged(bool topLevel)
{
    if (topLevel)
    {
        // Undocked
        custTitleBar->setTitle(this->windowTitle());
        custTitleBar->setButtonMinMaxEnabled(true);
        m_CentralWidget->setStyleSheet("border: 1px solid black");
    }
    else
    {
        // Docked
        custTitleBar->setTitle(m_CustomTitle);
        custTitleBar->setButtonMinMaxEnabled(false);
        m_CentralWidget->setStyleSheet("border: 1px solid black");
        //raise();
        QTabBar* tabBar = this->parent()->findChild<QTabBar*>();
        if (tabBar)
        {
            int     currentIndex = tabBar->currentIndex();
            QString tabText      = tabBar->tabText(currentIndex);
            int     numTab       = tabBar->count();

            for (int i = 0; i < numTab; i++)
            {
                if (tabBar->tabText(i).compare("") == 0)
                {
                    tabBar->setStyleSheet("QTabBar::tab:disabled { width: 0; height: 0; margin: 0; padding: 0; border: none; }");
                    tabBar->setTabEnabled(i, false);
                }
            }
        }
    }
}
