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
/// \file acCustomGraphics.h
/// \version 2.21
//
//=====================================================================

#ifndef _ACCUSTOMGRAPHICS_H
#define _ACCUSTOMGRAPHICS_H

#include <QtWidgets>
#include <QGraphicsSceneEvent>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGLWidget>
#include "cpprojectdata.h"

extern C_Application_Options g_Application_Options;


// Available Item Types:
enum acCustomGraphicsTypes {
    IMAGE = 1,
    TABLE_DIALOG,
    NAVIGATION_IMAGE,
    NAVIGATION_WINDOW,
};


// ----------------------------------------------------------------------------------
// Class Name:          acCustomGraphicsNavImageItem
// General Description: This class re-implements QGraphicsPixmapItem  properties
//                      under a custom type, it is used to identify itself for visiblity
// Creation Date:       4/9/2015
// ----------------------------------------------------------------------------------
class acCustomGraphicsNavImageItem : public QGraphicsPixmapItem {
  public:
    int ID;

    acCustomGraphicsNavImageItem(QPixmap &PixItem);

    // Enable the use of qgraphicsitem_cast with this item.
    int type() const {
        return (UserType + acCustomGraphicsTypes::NAVIGATION_IMAGE);
    }
};

// ----------------------------------------------------------------------------------
// Class Name:          acCustomGraphicsNavWindow
// General Description: This class re-implements QGraphicsRectItem  properties
//                      under a custom type, it is used to identify itself during
//                      scale and moves
// Creation Date:       4/9/2015
// ----------------------------------------------------------------------------------
class acCustomGraphicsNavWindow : public QGraphicsRectItem {
  public:
    int ID;

    acCustomGraphicsNavWindow(QRectF &Rec, QGraphicsItem *parent);

    // Enable the use of qgraphicsitem_cast with this item.
    int type() const {
        return (UserType + acCustomGraphicsTypes::NAVIGATION_WINDOW);
    }

};


// ----------------------------------------------------------------------------------
// Class Name:          acCustomGraphicsScene
// General Description: This class re-implements QGraphicsScene  properties
//                      Used to capture mouse events
// Creation Date:       4/9/2015
// ----------------------------------------------------------------------------------

enum eCustomGraphicsScene_Grids {
    Block = 0,
    None,
    Lines,
    Dots
};

class acCustomGraphicsScene : public QGraphicsScene {
    Q_OBJECT

  public:
    int ID;

    acCustomGraphicsScene(QObject *parent=nullptr);
    void gridEnabled(eCustomGraphicsScene_Grids enable);
    bool isGridEnabled();
    bool isDebug;

    int cursorBlockX;
    int cursorBlockY;

  Q_SIGNALS:
    void sceneMousePosition(QPointF *pos, int ID);

  protected:

    int  m_gridStep;
    eCustomGraphicsScene_Grids m_gridenabled;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void drawBackground(QPainter *painter, const QRectF &rect);
};


// ----------------------------------------------------------------------------------
// Class Name:          acVirtualMouseHub
// General Description: This class is a host for multiple signals from Graphics Scenes
// Creation Date:       4/9/2015
// ----------------------------------------------------------------------------------
class acVirtualMouseHub: public QObject {
    Q_OBJECT

  Q_SIGNALS:
    void VirtialMousePosition(QPointF *scenePos, QPointF *localPos, int ID);
    void VirtualSignalWheelEvent(QWheelEvent* theEvent, int ID);

  public slots:
    void onVirtualMouseMoveEvent(QPointF *scenePos, QPointF *localPos, int onID);
    void onVirtualSignalWheelEvent(QWheelEvent* theEvent, int ID);
};



// ----------------------------------------------------------------------------------
// Class Name:          acCustomGraphicsView
// General Description: This class re-implements QGraphicsView  properties
//                      Used to capture mouse events and mouse wheel movements
// Creation Date:       4/9/2015
// ----------------------------------------------------------------------------------
class acCustomGraphicsView : public QGraphicsView {
    Q_OBJECT

  public:
    int ID;

    acCustomGraphicsView();
    bool mousechanged;;

  Q_SIGNALS:
    void resetImageView();
    void ResizeEvent(QResizeEvent *event);
    void signalWheelEvent(QWheelEvent* event);
    void OnMouseHandDown();
    void OnMouseHandD();
    void OnWheelScaleUp(QPointF &pos);
    void OnWheelScaleDown(QPointF &pos);

  protected:
    qreal zoomFactor;

    virtual void wheelEvent(QWheelEvent* event);
    virtual void mousePressEvent(QMouseEvent  *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

};


// ----------------------------------------------------------------------------------
// Class Name:          acCustomGraphicsImageItem
// General Description: This class re-implements QGraphicsPixmapItem  properties
//                      under a custom type, it is used to identify itself during
//                      scale and moves
// Creation Date:       4/9/2015
// ----------------------------------------------------------------------------------
class acCustomGraphicsImageItem : public QGraphicsPixmapItem {
  public:
    int ID;

    acCustomGraphicsImageItem(QPixmap &ProcessedPixItem, QImage *OriginalImage);

    // Enable the use of qgraphicsitem_cast with this item.
    int type() const {
        return (UserType + acCustomGraphicsTypes::IMAGE);
    }

    void UpdateImage();
    void setDefaults();
    void changeImage(QImage image);
    void changeImageDiffRef(QImage *imageDiffRef);


    // Current Image Data Setting
    bool m_ChannelR;
    bool m_ChannelG;
    bool m_ChannelB;
    bool m_ChannelA;
    bool m_GrayScale;
    bool m_InvertImage;
    bool m_Mirrored;
    bool m_Mirrored_h;
    bool m_Mirrored_v;
    bool m_UseProcessedImage;
    bool m_ShowPixelDiff;
    bool m_ImageBrightness;
    int  m_alpha;
    QImage  m_ProcessedImage;
    QImage  *m_refImage  = NULL;
    int m_iBrightness;
    int m_fContrast;

};

#endif // _ACCUSTOMGRAPHICS_H
