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

#ifndef QT3D_QTRANSLATETRANSFORM_H
#define QT3D_QTRANSLATETRANSFORM_H

#include <Qt3DCore/qabstracttransform.h>

#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QTranslateTransformPrivate;

class QT3DCORESHARED_EXPORT QTranslateTransform : public QAbstractTransform
{
    Q_OBJECT

    Q_PROPERTY(float dx READ dx WRITE setDx NOTIFY translateChanged)
    Q_PROPERTY(float dy READ dy WRITE setDy NOTIFY translateChanged)
    Q_PROPERTY(float dz READ dz WRITE setDz NOTIFY translateChanged)
    Q_PROPERTY(QVector3D translation READ translation WRITE setTranslation NOTIFY translateChanged)

public:
    explicit QTranslateTransform(QObject *parent = Q_NULLPTR);

    float dx() const;
    float dy() const;
    float dz() const;

    QVector3D translation() const;

    QMatrix4x4 transformMatrix() const Q_DECL_OVERRIDE;

public Q_SLOTS:
    void setDx(float arg);
    void setDy(float arg);
    void setDz(float arg);

    void setTranslation(const QVector3D &arg);

Q_SIGNALS:
    void translateChanged();

protected:
    QTranslateTransform(QTranslateTransformPrivate &dd, QObject *parent = Q_NULLPTR);

private:
    Q_DECLARE_PRIVATE(QTranslateTransform)
    Q_DISABLE_COPY(QTranslateTransform)
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QTRANSLATETRANSFORM_H
