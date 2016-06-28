/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QLOOKATTRANSFORM_H
#define QT3D_QLOOKATTRANSFORM_H

#include <Qt3DCore/qabstracttransform.h>

#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QLookAtTransformPrivate;

class QT3DCORESHARED_EXPORT QLookAtTransform : public Qt3D::QAbstractTransform
{
    Q_OBJECT
    Q_PROPERTY(QVector3D position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(QVector3D upVector READ upVector WRITE setUpVector NOTIFY upVectorChanged)
    Q_PROPERTY(QVector3D viewCenter READ viewCenter WRITE setViewCenter NOTIFY viewCenterChanged)
    Q_PROPERTY(QVector3D viewVector READ viewVector NOTIFY viewVectorChanged)

public:
    explicit QLookAtTransform(QObject *parent = Q_NULLPTR);

    QMatrix4x4 transformMatrix() const Q_DECL_OVERRIDE;

    void setPosition(const QVector3D &position);
    QVector3D position() const;

    void setUpVector(const QVector3D &upVector);
    QVector3D upVector() const;

    void setViewCenter(const QVector3D &viewCenter);
    QVector3D viewCenter() const;

    void setViewVector(const QVector3D &viewVector);
    QVector3D viewVector() const;

Q_SIGNALS:
    void positionChanged();
    void upVectorChanged();
    void viewCenterChanged();
    void viewVectorChanged();

protected:
    QLookAtTransform(QLookAtTransformPrivate &dd, QObject *parent = Q_NULLPTR);

private:
    Q_DECLARE_PRIVATE(QLookAtTransform)
    Q_DISABLE_COPY(QLookAtTransform)
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QLOOKATTRANSFORM_H
