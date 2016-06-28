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

#ifndef QDECLARATIVEIMPLICITSIZEITEM_H
#define QDECLARATIVEIMPLICITSIZEITEM_H

#include "qdeclarativepainteditem_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeImplicitSizeItemPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeImplicitSizeItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged REVISION 1)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged REVISION 1)

public:
    QDeclarativeImplicitSizeItem(QDeclarativeItem *parent = 0);

protected:
    QDeclarativeImplicitSizeItem(QDeclarativeImplicitSizeItemPrivate &dd, QDeclarativeItem *parent);

Q_SIGNALS:
    Q_REVISION(1) void implicitWidthChanged();
    Q_REVISION(1) void implicitHeightChanged();

private:
    Q_DISABLE_COPY(QDeclarativeImplicitSizeItem)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeImplicitSizeItem)
};

class QDeclarativeImplicitSizePaintedItemPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeImplicitSizePaintedItem : public QDeclarativePaintedItem
{
    Q_OBJECT
    Q_PROPERTY(qreal implicitWidth READ implicitWidth NOTIFY implicitWidthChanged REVISION 1)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight NOTIFY implicitHeightChanged REVISION 1)

public:
    QDeclarativeImplicitSizePaintedItem(QDeclarativeItem *parent = 0);

protected:
    QDeclarativeImplicitSizePaintedItem(QDeclarativeImplicitSizePaintedItemPrivate &dd, QDeclarativeItem *parent);
    virtual void drawContents(QPainter *, const QRect &) {};

Q_SIGNALS:
    Q_REVISION(1) void implicitWidthChanged();
    Q_REVISION(1) void implicitHeightChanged();

private:
    Q_DISABLE_COPY(QDeclarativeImplicitSizePaintedItem)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeImplicitSizePaintedItem)
};

QT_END_NAMESPACE

#endif // QDECLARATIVEIMPLICITSIZEITEM_H
