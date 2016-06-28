/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QCAPSULECOLLIDER_H
#define QT3D_QCAPSULECOLLIDER_H

#include <Qt3DCollision/qabstractcollider.h>
#include <Qt3DCollision/qt3dcollision_global.h>
#include <QtGui/qvector3d.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QCapsuleColliderPrivate;

class QT3DCOLLISIONSHARED_EXPORT QCapsuleCollider : public QAbstractCollider
{
    Q_OBJECT
    Q_PROPERTY(QVector3D center READ center WRITE setCenter NOTIFY centerChanged)
    Q_PROPERTY(float radius READ radius WRITE setRadius NOTIFY radiusChanged)
    Q_PROPERTY(float length READ length WRITE setLength NOTIFY lengthChanged)
    Q_PROPERTY(Direction axisDirection READ axisDirection WRITE setAxisDirection NOTIFY axisDirectionChanged)

public:
    explicit QCapsuleCollider(QNode *parent = 0);
    ~QCapsuleCollider();

    enum Direction {
        XAxis,
        YAxis,
        ZAxis
    };
    Q_ENUM(Direction)

    QVector3D center() const;
    float radius() const;
    float length() const;
    Direction axisDirection() const;

public Q_SLOTS:
    void setCenter(const QVector3D &center);
    void setRadius(float radius);
    void setLength(float length);
    void setAxisDirection(Qt3D::QCapsuleCollider::Direction axisDirection);

Q_SIGNALS:
    void centerChanged(QVector3D center);
    void radiusChanged(float radius);
    void lengthChanged(float length);
    void axisDirectionChanged(Qt3D::QCapsuleCollider::Direction axisDirection);

protected:
    QCapsuleCollider(QCapsuleColliderPrivate &dd, QNode *parent = 0);

private:
    Q_DECLARE_PRIVATE(QCapsuleCollider)
    QT3D_CLONEABLE(QCapsuleCollider)
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QCAPSULECOLLIDER_H
