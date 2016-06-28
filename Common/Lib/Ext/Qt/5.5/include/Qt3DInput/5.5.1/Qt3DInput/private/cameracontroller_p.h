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

#ifndef CAMERA_CONTROLLER_H
#define CAMERA_CONTROLLER_H

#include <QObject>

#include <QPoint>

QT_BEGIN_NAMESPACE

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;
class QTimer;

namespace Qt3D {

class QCamera;
class QEntity;

namespace Input {

class CameraController : public QObject
{
    Q_OBJECT

    Q_PROPERTY( float linearSpeed READ linearSpeed WRITE setLinearSpeed NOTIFY linearSpeedChanged )
    Q_PROPERTY( float orbitRate READ orbitRate WRITE setOrbitRate NOTIFY orbitRateChanged )
    Q_PROPERTY( float lookRate READ lookRate WRITE setLookRate NOTIFY lookRateChanged )
    Q_PROPERTY( bool multisampleEnabled READ isMultisampleEnabled NOTIFY multisampleEnabledChanged )

public:
    explicit CameraController(QObject *parent = 0);

    void setCamera( QCamera* cam );
    QCamera *camera() const;

    void setLinearSpeed( float speed );
    float linearSpeed() const;

    float orbitRate() const;
    void setOrbitRate( float rate );

    float lookRate() const;
    void setLookRate( float rate );

    void mousePressEvent( QMouseEvent* aEvent );
    void mouseReleaseEvent( QMouseEvent* aEvent );
    void mouseMoveEvent( QMouseEvent* aEvent );

    bool keyPressEvent( QKeyEvent* aEvent );
    bool keyReleaseEvent( QKeyEvent* aEvent );

    void update( double t );

    bool isMultisampleEnabled() const;
public Q_SLOTS:
    void toggleMSAA();

protected:
    bool eventFilter(QObject *receiver, QEvent *event) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void linearSpeedChanged();
    void orbitRateChanged();
    void lookRateChanged();

    void multisampleEnabledChanged();

private Q_SLOTS:
    void onUpdate();

private:
    QCamera* m_camera;
    QEntity* m_cameraEntity;

    float m_vx;
    float m_vy;
    float m_vz;
    bool m_viewCenterFixed;
    float m_panAngle;
    float m_tiltAngle;

    bool m_leftButtonPressed;
    QPoint m_prevPos;
    QPoint m_pos;
    bool m_orbitMode;

    float m_linearSpeed;
    float m_time;
    float m_orbitRate;
    float m_lookRate;

    bool m_translateFast;
    bool m_multisampleEnabled;

    QTimer *m_updateTimer;
};

} // of namespace Input

} // of namespace Qt3D

QT_END_NAMESPACE

#endif // of CAMERA_CONTROLLER_H
