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
/// \file acCustomGraphics.cpp
/// \version 2.21
//
//=====================================================================

#include "accustomgraphics.h"

//=========================================
// Graphics View
//=========================================

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView
// Description: Constructor
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
acCustomGraphicsView::acCustomGraphicsView()
{
    ID           = 0;
    mousechanged = false;
    //setCacheMode(QGraphicsView::CacheBackground);
    zoomFactor = 1;
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView::wheelEvent
// Description: captures mouse wheel events on a view
//              checks if an image is under the mouse
//              and scales it
// Arguments:   QMouseEvent *event
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsView::wheelEvent(QWheelEvent* event)
{
    QGraphicsItem* itemPicked = itemAt(event->pos());
    // Found an item under the cursor
    if (itemPicked)
    {
        // Is the item our custome image
        if (itemPicked->type() == (itemPicked->UserType + acCustomGraphicsTypes::IMAGE))
        {
            acCustomGraphicsImageItem* item    = (acCustomGraphicsImageItem*)itemPicked;
            QPointF                    pos     = mapToScene(event->pos());
            QPointF                    localPt = item->mapFromScene(pos);
            //qDebug() << "Image X" << localPt.rx() << " Y" << localPt.ry();

            //item->setTransformOriginPoint(localPt);

            // If wheel and Shift key move the scale in large chunks
            // else we move in smaller incriments
            //double courseness = (event->modifiers() == Qt::ShiftModifier) ? 2.5 : 0.15;
            //double scaleFactor;
            // Zoom in
            if (event->delta() > 0)
            {
                emit OnWheelScaleUp(pos);
                // scaleFactor = item->scale() + courseness;
                // if (scaleFactor > 100)  scaleFactor = 100;
                // item->setScale(scaleFactor);
            }
            // Zooming out
            else
            {
                //scaleFactor = item->scale() - courseness;
                //if (scaleFactor <= 0)    scaleFactor = 0.1;
                //item->setScale(scaleFactor);
                emit OnWheelScaleDown(pos);
            }

            event->accept();
        }
    }

    emit signalWheelEvent(event);
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView::mousePressEvent
// Description: captures mouse press events on a view
//              checks if an image is under the mouse
//              and determins if user want to move an item
//              or send a message to reset a view
// Arguments:   QMouseEvent *event
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsView::mousePressEvent(QMouseEvent* event)
{
    QGraphicsItem* itemPicked = itemAt(event->pos());
    // Found an item under the cursor
    if (itemPicked)
    {
        // Is the item our custome image
        if (itemPicked->type() == (itemPicked->UserType + acCustomGraphicsTypes::IMAGE))
        {
            // User pressed the left mouse change its widget to Hand Cursor
            if (event->button() == Qt::LeftButton)
            {
                setCursor(Qt::ClosedHandCursor);
                mousechanged = true;
                emit OnMouseHandDown();
            }
        }

        // Item is Navigation and user selected Alt+left mouse Click
        // to resize the Image to fit current view window
        if (itemPicked->type() == (itemPicked->UserType + acCustomGraphicsTypes::NAVIGATION_IMAGE))
        {
            if ((event->button() == Qt::LeftButton) && (event->modifiers() == Qt::ShiftModifier))
            {
                emit resetImageView();
            }
        }
    }
    QGraphicsView::mousePressEvent(event);
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsView::mouseReleaseEvent
// Description: captures mouse release events on a view
//              checks if mouse image has changed if it has it will restore
//              original cursor (Arrow)
// Arguments:   QMouseEvent *event
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
    QGraphicsView::mouseReleaseEvent(event);
    // User released the left mouse change restore the cursor widget
    if (mousechanged)
    {
        setCursor(Qt::ArrowCursor);
        mousechanged = false;
        emit OnMouseHandD();
    }
}

void acCustomGraphicsView::resizeEvent(QResizeEvent* event)
{
    emit ResizeEvent(event);
}

//=========================================
// Custom Scene
//=========================================
static int acCustomGraphicsScene_ID = 0;

acCustomGraphicsScene::acCustomGraphicsScene(QObject* parent)
    : QGraphicsScene(parent)
{
    acCustomGraphicsScene_ID++;
    ID = acCustomGraphicsScene_ID;

    isDebug       = false;
    m_gridStep    = 25;
    m_gridenabled = eCustomGraphicsScene_Grids::Block;

    cursorBlockX = 4;
    cursorBlockY = 4;
}

bool acCustomGraphicsScene::isGridEnabled()
{
    return (!(m_gridenabled == eCustomGraphicsScene_Grids::None));
}

void acCustomGraphicsScene::gridEnabled(eCustomGraphicsScene_Grids enable)
{
    m_gridenabled = enable;
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsScene::mousePressEvent
// Description: captures mouse press events on a scene
// Arguments:   QGraphicsSceneMouseEvent *event
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QGraphicsScene::mousePressEvent(event);
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsScene::mousePressEvent
// Description: captures mouse press move on a scene
// Arguments:   QGraphicsSceneMouseEvent *event
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
void acCustomGraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF scenePos = event->scenePos();

    //qDebug() << "*** acCustomGraphicsScene::mouseMoveEvent **** " << ID << "scenePos: rx:" << scenePos.rx() << "ry: " << scenePos.ry();

    emit sceneMousePosition(&scenePos, ID);

    QGraphicsScene::mouseMoveEvent(event);
}

#include <QPainter>

void acCustomGraphicsScene::drawBackground(QPainter* painter, const QRectF& rect)
{
    // color Black
    QColor ColorBlack(0, 0, 0);

    // color White
    QColor ColorWhite(255, 255, 255);

    // Draw a line grid
    if (m_gridenabled != eCustomGraphicsScene_Grids::None)
    {
        painter->setPen(Qt::white);

        // Start point for Horizonal steps
        qreal startH = 0;

        //Start point for Virtical steps
        qreal startV = 0;

        switch (m_gridenabled)
        {
        case eCustomGraphicsScene_Grids::Block: {
            QImage image(":/compressonatorgui/images/gridsolid.png");
            QBrush brush(image);
            painter->fillRect(rect, brush);
            break;
        }

        case eCustomGraphicsScene_Grids::Lines: {
            // First fill the BackGround as black
            painter->fillRect(rect, ColorBlack);

            // draw horizontal grid lines
            painter->setPen(QPen(ColorWhite));

            for (qreal y = startH; y < rect.bottom();)
            {
                y += m_gridStep;
                painter->drawLine(rect.left(), y, rect.right(), y);
            }

            // draw virtical grid lines
            for (qreal x = startV; x < rect.right();)
            {
                x += m_gridStep;
                painter->drawLine(x, rect.top(), x, rect.bottom());
            }
            break;
        }

        case eCustomGraphicsScene_Grids::Dots: {
            // First fill the BackGround as black
            painter->fillRect(rect, ColorBlack);

            // draw points
            painter->setPen(QPen(ColorWhite));

            for (qreal y = startH; y < rect.bottom();)
            {
                y += m_gridStep;
                // draw virtical grid lines
                for (qreal x = startV; x < rect.right();)
                {
                    x += m_gridStep;
                    painter->drawPoint(x, y);
                }
            }

            break;
        }

        default: {
            painter->fillRect(rect, ColorBlack);
            break;
        }
        }
    }
    else
        painter->fillRect(rect, ColorBlack);
}

//=========================================
// Custom Graphics Items
//=========================================

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsImageItem
// Description: Constructor for Image Item
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------

acCustomGraphicsImageItem::acCustomGraphicsImageItem(QPixmap& ProcessedPixItem, QImage* OriginalImage)
    : QGraphicsPixmapItem(ProcessedPixItem)
{
    ID               = 0;
    m_ProcessedImage = ProcessedPixItem.toImage();
    m_refImage       = OriginalImage;
    setDefaults();
}

void acCustomGraphicsImageItem::setDefaults()
{
    m_alpha = 255;

    //====================================================
    // pix = contrast*pix + brightness
    //====================================================
    m_iBrightness = 0;
    m_fContrast   = 1.0f;

    m_ChannelR    = true;
    m_ChannelG    = true;
    m_ChannelB    = true;
    m_ChannelA    = false;
    m_GrayScale   = false;
    m_InvertImage = false;
    m_Mirrored    = false;
    m_Mirrored_h  = true;
    m_Mirrored_v  = false;

    m_ShowPixelDiff     = false;
    m_UseProcessedImage = false;
    m_ImageBrightness   = false;
}

// ---------------------------------------------------------------------------
// Name:   UpdateImage
// Description: Reimplements a PixMap depending on set properties
// Return Val:    none
// Date(DD/MM/YYYY):   02/11/2015
// ---------------------------------------------------------------------------

void acCustomGraphicsImageItem::UpdateImage()
{
    QImage image;

    if (m_ShowPixelDiff && m_refImage)
    {
        // Pixel Diff
        QColor src;
        QColor dest;
        QColor diff;
        int    r, g, b;
        image = *m_refImage;
        int w, h;

        QImage imageOriginal  = *m_refImage;
        QImage imageProcessed = m_ProcessedImage;

        w = image.width();
        h = image.height();

        for (int x = 0; x < w; x++)
        {
            for (int y = 0; y < h; y++)
            {
                src  = QColor(imageOriginal.pixel(x, y));
                dest = QColor(imageProcessed.pixel(x, y));

                r = qAbs(src.red() - dest.red());
                g = qAbs(src.green() - dest.green());
                b = qAbs(src.blue() - dest.blue());

                // g_Application_Options.m_imagediff_contrast min value is 1 max is set to 200
                r = r * g_Application_Options.m_imagediff_contrast;
                g = g * g_Application_Options.m_imagediff_contrast;
                b = b * g_Application_Options.m_imagediff_contrast;

                if (r > 255)
                    r = 255;
                if (g > 255)
                    g = 255;
                if (b > 255)
                    b = 255;

                diff.setRed(r);
                diff.setGreen(g);
                diff.setBlue(b);
                diff.setAlpha(255);

                if (m_ShowPixelDiff)
                    image.setPixel(x, y, diff.rgba());
            }
        }

        // switch off after first run
        m_ShowPixelDiff = false;
    }
    else
    {
        if (m_UseProcessedImage)
            image = m_ProcessedImage;
        else
            image = acCustomGraphicsImageItem::pixmap().toImage();

        // Set channel mapping and Alpha
        for (int x = 0; x < image.width(); x++)
        {
            for (int y = 0; y < image.height(); y++)
            {
                QColor color = image.pixelColor(x, y);
                if (m_GrayScale)
                {
                    // Note qGray() actually computes luminosity using the formula (r*11 + g*16 + b*5)/32.
                    int gray = (color.red() + color.green() + color.blue()) / 3;
                    image.setPixel(x, y, qRgb(gray, gray, gray));
                }
                else
                {
                    if (!m_ChannelR)
                        color.setRed(0);
                    if (!m_ChannelG)
                        color.setGreen(0);
                    if (!m_ChannelB)
                        color.setBlue(0);
                    if (m_ChannelA)
                        color.setAlpha(255);
                    image.setPixel(x, y, color.rgba());
                }
            }
        }

        if (m_InvertImage)
        {
            //Changed image.invertPixels(QImage::InvertRgba) to InvertRgb fix the exr invert issue-workaround for now--only exr falls into this if case
            //the InvertRgba seems not working, bmp and png both call else case InvertRgb, that's why they are working fine
            if (image.hasAlphaChannel())
                image.invertPixels(QImage::InvertRgb);
            else
                image.invertPixels(QImage::InvertRgb);
        }

        if (m_Mirrored)
        {
            // Note the flip from what qt defined!
            image = image.mirrored(m_Mirrored_v, m_Mirrored_h);
        }
    }

    if (m_ImageBrightness)
    {
        m_ImageBrightness = false;
        if (m_iBrightness > 100)
            m_iBrightness = 100;
        else if (m_iBrightness < -100)
            m_iBrightness = -100;

        // Set brightness
        int r, g, b;
        for (int x = 0; x < image.width(); x++)
        {
            for (int y = 0; y < image.height(); y++)
            {
                QColor color = image.pixelColor(x, y);
                r            = (color.red() * m_fContrast) + m_iBrightness;
                g            = (color.green() * m_fContrast) + m_iBrightness;
                b            = (color.blue() * m_fContrast) + m_iBrightness;

                if (r > 255)
                    r = 255;
                else if (r < 0)
                    r = 0;

                if (g > 255)
                    g = 255;
                else if (g < 0)
                    g = 0;

                if (b > 255)
                    b = 255;
                else if (b < 0)
                    b = 0;

                color.setRed(r);
                color.setGreen(g);
                color.setBlue(b);
                image.setPixel(x, y, color.rgba());
            }
        }
    }

    // Reset the graphics items view
    setPixmap(QPixmap::fromImage(image));
}

// ---------------------------------------------------------------------------
// Name:        changeImage
// Description: Sets the Pixel Maps view with that passed down
// Return Val:
// Date(M/D):   11/13/2015
// ---------------------------------------------------------------------------

void acCustomGraphicsImageItem::changeImage(QImage image)
{
    m_ProcessedImage = image;
    setPixmap(QPixmap::fromImage(image));
}

void acCustomGraphicsImageItem::changeImageDiffRef(QImage* imageRef)
{
    m_refImage = imageRef;
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsNavImageItem
// Description: Constructor for Navigation Item
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
acCustomGraphicsNavImageItem::acCustomGraphicsNavImageItem(QPixmap& PixItem)
    : QGraphicsPixmapItem(PixItem)
{
}

// ---------------------------------------------------------------------------
// Name:        acCustomGraphicsNavImageItem
// Description: Constructor for  Navigation window item
// Return Val:
// Date:        4/9/2015
// ---------------------------------------------------------------------------
acCustomGraphicsNavWindow::acCustomGraphicsNavWindow(QRectF& Rec, QGraphicsItem* parent)
    : QGraphicsRectItem(Rec, parent)
{
    ID = 0;
}

//
// Central HUB for multiple images to relay mouse positions and events...
//
void acVirtualMouseHub::onVirtualMouseMoveEvent(QPointF* scenePos, QPointF* localPos, int onID)
{
    //qDebug() << "onVirtualMouseHUB " << onID << " rx : " << scenePos->rx() << " ry: " << scenePos->ry();
    emit VirtialMousePosition(scenePos, localPos, onID);
}

void acVirtualMouseHub::onVirtualSignalWheelEvent(QWheelEvent* theEvent, int ID)
{
    Q_UNUSED(theEvent);
    Q_UNUSED(ID);
    //qDebug() << "onVirtualSignalWheelEvent " << ID;
    //emit VirtualSignalWheelEvent(theEvent, ID);
}
