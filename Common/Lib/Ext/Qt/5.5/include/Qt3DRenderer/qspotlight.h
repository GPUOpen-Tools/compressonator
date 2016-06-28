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

#ifndef QT3D_QSPOTLIGHT_H
#define QT3D_QSPOTLIGHT_H

#include <Qt3DRenderer/qabstractlight.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QSpotLightPrivate;

class QT3DRENDERERSHARED_EXPORT QSpotLight : public QAbstractLight
{
    Q_OBJECT
    Q_PROPERTY(QVector3D direction READ direction WRITE setDirection NOTIFY directionChanged)
    Q_PROPERTY(float cutOffAngle READ cutOffAngle WRITE setCutOffAngle NOTIFY cutOffAngleChanged)

public:
    explicit QSpotLight(QNode *parent = 0);

    QVector3D direction() const;
    float cutOffAngle() const;

    void setDirection(const QVector3D &direction);
    void setCutOffAngle(float cutOffAngle);

Q_SIGNALS:
    void directionChanged();
    void cutOffAngleChanged();

protected:
    QSpotLight(QSpotLightPrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QSpotLight)
    QT3D_CLONEABLE(QSpotLight)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QSPOTLIGHT_H
