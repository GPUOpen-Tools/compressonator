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

#ifndef QDECLARATIVEFLIPABLE_H
#define QDECLARATIVEFLIPABLE_H

#include "qdeclarativeitem.h"

#include <QtCore/QObject>
#include <QtGui/QTransform>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeFlipablePrivate;
class Q_AUTOTEST_EXPORT QDeclarativeFlipable : public QDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(Side)
    Q_PROPERTY(QGraphicsObject *front READ front WRITE setFront NOTIFY frontChanged)
    Q_PROPERTY(QGraphicsObject *back READ back WRITE setBack NOTIFY backChanged)
    Q_PROPERTY(Side side READ side NOTIFY sideChanged)
    //### flipAxis
    //### flipRotation
public:
    QDeclarativeFlipable(QDeclarativeItem *parent=0);
    ~QDeclarativeFlipable();

    QGraphicsObject *front();
    void setFront(QGraphicsObject *);

    QGraphicsObject *back();
    void setBack(QGraphicsObject *);

    enum Side { Front, Back };
    Side side() const;

Q_SIGNALS:
    void frontChanged();
    void backChanged();
    void sideChanged();

private Q_SLOTS:
    void retransformBack();

private:
    Q_DISABLE_COPY(QDeclarativeFlipable)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeFlipable)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeFlipable)

#endif // QDECLARATIVEFLIPABLE_H
