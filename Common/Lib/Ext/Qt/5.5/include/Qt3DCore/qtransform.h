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

#ifndef QT3D_QTRANSFORM_H
#define QT3D_QTRANSFORM_H

#include <Qt3DCore/qcomponent.h>

#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractTransform;

class QTransformPrivate;
class QT3DCORESHARED_EXPORT QTransform : public QComponent
{
    Q_OBJECT
    Q_PROPERTY(QMatrix4x4 matrix READ matrix NOTIFY matrixChanged)

public:
    explicit QTransform(QNode *parent = 0);
    QTransform(QList<QAbstractTransform *> transforms, QNode *parent = 0);
    QTransform(QAbstractTransform *transform, QNode *parent = 0);
    ~QTransform();

    QList<QAbstractTransform *> transforms() const;
    void addTransform(QAbstractTransform *xform);
    void removeTransform(QAbstractTransform *xform);

    QMatrix4x4 matrix() const;

Q_SIGNALS:
    void matrixChanged();
    void transformsChanged();

protected:
    QTransform(QTransformPrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QTransform)
    QT3D_CLONEABLE(QTransform)
    Q_PRIVATE_SLOT(d_func(), void _q_transformDestroyed(QObject *obj))
    Q_PRIVATE_SLOT(d_func(), void _q_update())
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QTRANSFORM_H
