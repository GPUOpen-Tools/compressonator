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

#ifndef QT3D_CAMERALENS_H
#define QT3D_CAMERALENS_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DCore/qt3dcore_global.h>

#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QCameraLensPrivate;

class QT3DCORESHARED_EXPORT QCameraLens : public QComponent
{
    Q_OBJECT
    Q_PROPERTY(ProjectionType projectionType READ projectionType WRITE setProjectionType NOTIFY projectionTypeChanged)
    Q_PROPERTY(float nearPlane READ nearPlane WRITE setNearPlane NOTIFY nearPlaneChanged)
    Q_PROPERTY(float farPlane READ farPlane WRITE setFarPlane NOTIFY farPlaneChanged)
    Q_PROPERTY(float fieldOfView READ fieldOfView WRITE setFieldOfView NOTIFY fieldOfViewChanged)
    Q_PROPERTY(float aspectRatio READ aspectRatio WRITE setAspectRatio NOTIFY aspectRatioChanged)
    Q_PROPERTY(float left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(float right READ right WRITE setRight NOTIFY rightChanged)
    Q_PROPERTY(float bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(float top READ top WRITE setTop NOTIFY topChanged)
    Q_PROPERTY(QMatrix4x4 projectionMatrix READ projectionMatrix NOTIFY projectionMatrixChanged)

public:
    explicit QCameraLens(QNode *parent = 0);
    ~QCameraLens();

    enum ProjectionType {
        OrthogonalProjection,
        PerspectiveProjection,
        FrustumProjection
    };
    Q_ENUM(ProjectionType)

    void setProjectionType(ProjectionType projectionType);
    ProjectionType projectionType() const;

    void setNearPlane(float nearPlane);
    float nearPlane() const;

    void setFarPlane(float nearPlane);
    float farPlane() const;

    void setFieldOfView(float fieldOfView);
    float fieldOfView() const;

    void setAspectRatio(float aspectRatio);
    float aspectRatio() const;

    void setLeft(float left);
    float left() const;

    void setRight(float right);
    float right() const;

    void setBottom(float bottom);
    float bottom() const;

    void setTop(float top);
    float top() const;

    QMatrix4x4 projectionMatrix() const;

    void setOrthographicProjection(float left, float right,
                                   float bottom, float top,
                                   float nearPlane, float farPlane);

    void setFrustumProjection(float left, float right,
                              float bottom, float top,
                              float nearPlane, float farPlane);

    void setPerspectiveProjection(float fieldOfView, float aspect,
                                  float nearPlane, float farPlane);

Q_SIGNALS:
    void projectionTypeChanged();
    void nearPlaneChanged();
    void farPlaneChanged();
    void fieldOfViewChanged();
    void aspectRatioChanged();
    void leftChanged();
    void rightChanged();
    void bottomChanged();
    void topChanged();
    void projectionMatrixChanged();

protected:
    QCameraLens(QCameraLensPrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QCameraLens)
    QT3D_CLONEABLE(QCameraLens)
};

} // Qt3D

QT_END_NAMESPACE

#endif // CAMERALENS_H
