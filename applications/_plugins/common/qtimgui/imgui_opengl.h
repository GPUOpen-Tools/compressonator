//=====================================================================
// Copyright 2018 (c), Advanced Micro Devices, Inc. All rights reserved.
//=====================================================================
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
#ifndef  QTIMGUI_H
#define  QTIMGUI_H

#include "imgui_openglrenderer.h"
#include "QtWidgets/qwidget.h"

class QImGUI_OpenGLWidgetWindowWrapper : public QImGUI_OpenGLWindowWrapper {
  public:
    QImGUI_OpenGLWidgetWindowWrapper(QWidget *w) : w(w) {}

    void installEventFilter(QObject *object) override {
        return w->installEventFilter(object);
    }

    QSize size() const override {
        return w->size();
    }

    qreal devicePixelRatio() const override {
        return w->devicePixelRatioF();
    }

    bool isActive() const override {
        return w->isActiveWindow();
    }

    QPoint mapFromGlobal(const QPoint &p) const override {
        return w->mapFromGlobal(p);
    }

  private:
    QWidget *w;
};

#endif
