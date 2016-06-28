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

#ifndef QDECLARATIVEBORDERIMAGE_H
#define QDECLARATIVEBORDERIMAGE_H

#include "private/qdeclarativeimagebase_p.h"

#include <QtNetwork/qnetworkreply.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeScaleGrid;
class QDeclarativeGridScaledImage;
class QDeclarativeBorderImagePrivate;
class Q_AUTOTEST_EXPORT QDeclarativeBorderImage : public QDeclarativeImageBase
{
    Q_OBJECT
    Q_ENUMS(TileMode)

    Q_PROPERTY(QDeclarativeScaleGrid *border READ border CONSTANT)
    Q_PROPERTY(TileMode horizontalTileMode READ horizontalTileMode WRITE setHorizontalTileMode NOTIFY horizontalTileModeChanged)
    Q_PROPERTY(TileMode verticalTileMode READ verticalTileMode WRITE setVerticalTileMode NOTIFY verticalTileModeChanged)

public:
    QDeclarativeBorderImage(QDeclarativeItem *parent=0);
    ~QDeclarativeBorderImage();

    QDeclarativeScaleGrid *border();

    enum TileMode { Stretch = Qt::StretchTile, Repeat = Qt::RepeatTile, Round = Qt::RoundTile };

    TileMode horizontalTileMode() const;
    void setHorizontalTileMode(TileMode);

    TileMode verticalTileMode() const;
    void setVerticalTileMode(TileMode);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    void setSource(const QUrl &url);

    void setSourceSize(const QSize&);

Q_SIGNALS:
    void horizontalTileModeChanged();
    void verticalTileModeChanged();

protected:
    virtual void load();

private:
    void setGridScaledImage(const QDeclarativeGridScaledImage& sci);

private Q_SLOTS:
    void doUpdate();
    void requestFinished();
    void sciRequestFinished();

private:
    Q_DISABLE_COPY(QDeclarativeBorderImage)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeBorderImage)
};

QT_END_NAMESPACE
QML_DECLARE_TYPE(QDeclarativeBorderImage)

#endif // QDECLARATIVEBORDERIMAGE_H
