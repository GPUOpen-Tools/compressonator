/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEIMAGEITEM_H
#define QDECLARATIVEIMAGEITEM_H

#include "qdeclarativeitem.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativePaintedItemPrivate;
class Q_AUTOTEST_EXPORT QDeclarativePaintedItem : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QSize contentsSize READ contentsSize WRITE setContentsSize NOTIFY contentsSizeChanged)
    Q_PROPERTY(QColor fillColor READ fillColor WRITE setFillColor NOTIFY fillColorChanged)
    Q_PROPERTY(int pixelCacheSize READ pixelCacheSize WRITE setPixelCacheSize)
    Q_PROPERTY(bool smoothCache READ smoothCache WRITE setSmoothCache)
    Q_PROPERTY(qreal contentsScale READ contentsScale WRITE setContentsScale NOTIFY contentsScaleChanged)


public:
    QDeclarativePaintedItem(QDeclarativeItem *parent=0);
    ~QDeclarativePaintedItem();

    QSize contentsSize() const;
    void setContentsSize(const QSize &);

    qreal contentsScale() const;
    void setContentsScale(qreal);

    int pixelCacheSize() const;
    void setPixelCacheSize(int pixels);

    bool smoothCache() const;
    void setSmoothCache(bool on);

    QColor fillColor() const;
    void setFillColor(const QColor&);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

protected:
    QDeclarativePaintedItem(QDeclarativePaintedItemPrivate &dd, QDeclarativeItem *parent);

    virtual void drawContents(QPainter *p, const QRect &) = 0;
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    virtual QVariant itemChange(GraphicsItemChange change,
                                const QVariant &value);

    void setCacheFrozen(bool);
    QRectF boundingRect() const;

Q_SIGNALS:
    void fillColorChanged();
    void contentsSizeChanged();
    void contentsScaleChanged();

protected Q_SLOTS:
    void dirtyCache(const QRect &);
    void clearCache();

private:
    Q_DISABLE_COPY(QDeclarativePaintedItem)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativePaintedItem)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePaintedItem)

#endif
