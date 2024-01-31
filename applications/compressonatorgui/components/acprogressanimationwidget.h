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

#ifndef ACPROGRESSANIMATIONWIDGET_H
#define ACPROGRESSANIMATIONWIDGET_H

#include <QWidget>
#include <QColor>
#include <QPainter>

class QTimerEvent;

// ----------------------------------------------------------------------------------
// Class Name:          acProgressAnimationWidget
// General Description: QT progress widget which displays busy animation
//
//
// Author:              Naama Zur
// Creation Date:
// ----------------------------------------------------------------------------------
class acProgressAnimationWidget : public QWidget
{
    Q_OBJECT
public:
    acProgressAnimationWidget(QWidget* parent = 0);

    /// Sets the color of the widget
    void SetColor(const QColor& color);

    /// Sets the delay between animation steps in milliseconds
    void SetAnimationDelay(int delay);

public slots:
    /// Starts the spin animation
    void StartAnimation();

    /// Stops the spin animation
    void StopAnimation();

    /// Obtains the animation status
    bool IsAnimationStarted() const;

    /// Progresses the spin animation
    void Increment(unsigned int value = 1);
    /// Progresses the spin animation
    void SetProgressValue(unsigned int value);

protected:
    virtual void paintEvent(QPaintEvent* event);

    /// Progress animation styles
    void PaintLineCircle(QPainter& p);
    void PaintDotsCircle(QPainter& p);
    void PaintGradientCircle(QPainter& p);
    void PaintCircle(QPainter& p);
    void paintDoubleCircle(QPainter& p);

private:
    QColor       m_color;
    unsigned int m_angle;
    unsigned int m_animationDelay;
    unsigned int m_animationDelayAccumulator;
    bool         m_isAnimationStarted;
};

#endif  // ACPROGRESSANIMATIONWIDGET_H
