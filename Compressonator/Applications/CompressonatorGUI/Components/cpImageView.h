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

#ifndef _IMAGEVIEW_H
#define _IMAGEVIEW_H

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <qtimer.h>

//#include "qteditorfactory.h"
//#include "qttreepropertybrowser.h"
//#include "qtbuttonpropertybrowser.h"
//#include "qtgroupboxpropertybrowser.h"
//#include "qtpropertymanager.h"
//#include "qtvariantproperty.h"
//#include "objectcontroller.h"
#include "cpImageView.h"
#include "acImageView.h"
#include "cpImageLoader.h"
#include "acCustomDockWidget.h"

typedef struct{
    bool onBrightness;
} Setting;

class PushButtonAction : public QWidgetAction
{
public:
    explicit PushButtonAction(const QIcon & icon, const QString & text, QObject *parent = 0)
        : QWidgetAction(parent)
    {
        setIcon(icon);
        setObjectName(text);
    }

protected:
    virtual QWidget * createWidget(QWidget * parent)
    {
        return new QPushButton(icon(), objectName(), parent);
    }

    virtual void deleteWidget(QWidget * widget)
    {
        delete widget;
        widget = NULL;
    }
};

class cpImageView : public acCustomDockWidget
{
    Q_OBJECT
public:

    cpImageView(const QString fileName, const QString Title, QWidget *parent, CMipImages *MipImages, Setting *setting);
    ~cpImageView();

    void InitData();


    void showToobar(bool show);
    void showToobarButton(bool show);

    void EnableMipLevelDisplay(int level);

    CImageLoader        *m_imageLoader;
    acImageView         *m_acImageView;
    CMipImages          *m_MipImages;


    QSize               m_imageSize;
    int                 ID;
    bool                m_localMipImages;
    bool                m_bOnacScaleChange;
    bool                m_useOriginalImageCursor;

public slots:
    void OnToolBarClicked();                                // Hook into the CustomeWidgets TitleBars On Tool Button Clicked events
    void oncpImageViewMousePosition(QPointF *scenePos, QPointF *localPos, int onID);
    void oncpImageViewVirtualMousePosition(QPointF *scenePos, QPointF *localPos, int onID);
    void onDecompressUsing(int useDecomp);
    void oncpResetImageView();
    void onZoomLevelChanged(int value);
    void onacScaleChange(int value);

    
private:
    void showEvent(QShowEvent *);
    void paintEvent(QPaintEvent * event);
    bool m_originalImage;                                   // True if the image been viewed is the original image

    // Common for all
    QWidget            *m_newWidget;
    QGridLayout        *m_layout;
    QWidget            *m_parent;
    QToolButton        *m_button2;
    QToolButton        *m_button;
    QToolBar           *m_toolBar;
    QStatusBar         *m_statusBar;
    QPushButton        *m_buttonNavigate;

    QLabel              *m_labelColorTxt;
    QLabel              *m_labelColorRGBA;
    QLabel              *m_labelPos;
    QWidget             *m_pMyWidget;
    QSpinBox            *m_ZoomLevel;

    QStyle              *Plastique_style;                   // Combobox Style

    QPixmap             *m_pixmap;

    int                 m_MipLevels;
    bool                m_FitOnShow;                        // Flaged used to to indicate a fit image into view when widget is shown

    // Image View Actions
    QAction             *imageview_ResetImageView;
    QAction             *imageview_ToggleChannelR;
    QAction             *imageview_ToggleChannelG;
    QAction             *imageview_ToggleChannelB;
    QAction             *imageview_ToggleChannelA;
    QAction             *imageview_ToggleGrayScale;
    QAction             *imageview_InvertImage;
    QAction             *imageview_ImageBrightnessUp;
    QAction             *imageview_ImageBrightnessDown;
    QAction             *imageview_MirrorHorizontal;
    QAction             *imageview_MirrorVirtical;
    QAction             *imageview_RotateRight;
    QAction             *imageview_RotateLeft;
    QAction             *imageview_ZoomIn;
    QAction             *imageview_ZoomOut;
    QAction             *imageview_ViewImageOriginalSize;
    QAction             *imageview_FitInWindow;
    QComboBox           *m_CBimageview_GridBackground;
    QComboBox           *m_CBimageview_MipLevel;
    QComboBox           *m_CBimage_DecompressUsing;

Q_SIGNALS:
    void UpdateData(QObject *data);
    void OnSetScale(int value);

};


#endif // _IMAGEVIEW_H