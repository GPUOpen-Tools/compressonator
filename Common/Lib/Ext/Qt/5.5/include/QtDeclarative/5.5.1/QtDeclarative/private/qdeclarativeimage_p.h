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

#ifndef QDECLARATIVEIMAGE_H
#define QDECLARATIVEIMAGE_H

#include "private/qdeclarativeimagebase_p.h"

#include <QtNetwork/qnetworkreply.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeImagePrivate;
class Q_AUTOTEST_EXPORT QDeclarativeImage : public QDeclarativeImageBase
{
    Q_OBJECT
    Q_ENUMS(FillMode)

    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(qreal paintedWidth READ paintedWidth NOTIFY paintedGeometryChanged)
    Q_PROPERTY(qreal paintedHeight READ paintedHeight NOTIFY paintedGeometryChanged)

public:
    QDeclarativeImage(QDeclarativeItem *parent=0);
    ~QDeclarativeImage();

    enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop, Tile, TileVertically, TileHorizontally };
    FillMode fillMode() const;
    void setFillMode(FillMode);

    QPixmap pixmap() const;
    void setPixmap(const QPixmap &);

    qreal paintedWidth() const;
    qreal paintedHeight() const;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    QRectF boundingRect() const;

Q_SIGNALS:
    void fillModeChanged();
    void paintedGeometryChanged();

protected:
    QDeclarativeImage(QDeclarativeImagePrivate &dd, QDeclarativeItem *parent);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    void pixmapChange();
    void updatePaintedGeometry();

private:
    Q_DISABLE_COPY(QDeclarativeImage)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeImage)
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QDeclarativeImage)

#endif // QDECLARATIVEIMAGE_H
