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

#ifndef QT3D_Q3DMOUSEEVENT_H
#define QT3D_Q3DMOUSEEVENT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <QObject>
#include <QMouseEvent>
#include <QtGlobal>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QT3DINPUTSHARED_EXPORT Q3DMouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x CONSTANT)
    Q_PROPERTY(int y READ y CONSTANT)
    Q_PROPERTY(bool wasHeld READ wasHeld CONSTANT)
    Q_PROPERTY(Qt3D::Q3DMouseEvent::Buttons button READ button CONSTANT)
    Q_PROPERTY(int buttons READ buttons CONSTANT)
    Q_PROPERTY(Qt3D::Q3DMouseEvent::Modifiers modifier READ modifier CONSTANT)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    enum Buttons {
        LeftButton = Qt::MouseButton::LeftButton,
        RightButton = Qt::MouseButton::RightButton,
        MiddleButton = Qt::MouseButton::MiddleButton,
        BackButton = Qt::MouseButton::BackButton,
        NoButton = Qt::MouseButton::NoButton
    };
    Q_ENUM(Buttons)

    enum Modifiers {
        NoModifier = Qt::NoModifier,
        ShiftModifier = Qt::ShiftModifier,
        ControlModifier = Qt::ControlModifier,
        AltModifier = Qt::AltModifier,
        MetaModifier = Qt::MetaModifier,
        KeypadModifier = Qt::KeypadModifier
    };
    Q_ENUM(Modifiers)

    explicit Q3DMouseEvent(const QMouseEvent &e);
    ~Q3DMouseEvent();

    inline int x() const { return m_event.x(); }
    inline int y() const { return m_event.y(); }
    inline bool wasHeld() const { return static_cast<Qt::GestureType>(m_event.type()) == Qt::TapAndHoldGesture; }
    Buttons button() const;
    int buttons() const;
    Modifiers modifier() const;

    inline bool isAccepted() const { return m_event.isAccepted(); }
    inline void setAccepted(bool accepted) { m_event.setAccepted(accepted); }
    inline QEvent::Type type() const { return m_event.type(); }

private:
    QMouseEvent m_event;
};

typedef QSharedPointer<Q3DMouseEvent> Q3DMouseEventPtr;

class QT3DINPUTSHARED_EXPORT Q3DWheelEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x CONSTANT)
    Q_PROPERTY(int y READ y CONSTANT)
    Q_PROPERTY(QPoint angleDelta READ angleDelta CONSTANT)
    Q_PROPERTY(int buttons READ buttons CONSTANT)
    Q_PROPERTY(Qt3D::Q3DWheelEvent::Modifiers modifier READ modifier CONSTANT)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    enum Buttons {
        LeftButton = Qt::MouseButton::LeftButton,
        RightButton = Qt::MouseButton::RightButton,
        MiddleButton = Qt::MouseButton::MiddleButton,
        BackButton = Qt::MouseButton::BackButton,
        NoButton = Qt::MouseButton::NoButton
    };
    Q_ENUM(Buttons)

    enum Modifiers {
        NoModifier = Qt::NoModifier,
        ShiftModifier = Qt::ShiftModifier,
        ControlModifier = Qt::ControlModifier,
        AltModifier = Qt::AltModifier,
        MetaModifier = Qt::MetaModifier,
        KeypadModifier = Qt::KeypadModifier
    };
    Q_ENUM(Modifiers)

    explicit Q3DWheelEvent(const QWheelEvent &e);
    ~Q3DWheelEvent();

    inline int x() const { return m_event.x(); }
    inline int y() const { return m_event.y(); }
    inline QPoint angleDelta() const { return m_event.angleDelta(); }
    int buttons() const;
    Modifiers modifier() const;

    inline bool isAccepted() const { return m_event.isAccepted(); }
    inline void setAccepted(bool accepted) { m_event.setAccepted(accepted); }
    inline QEvent::Type type() const { return m_event.type(); }

private:
    QWheelEvent m_event;
};

typedef QSharedPointer<Q3DWheelEvent> Q3DWheelEventPtr;

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::Q3DMouseEvent*)
Q_DECLARE_METATYPE(Qt3D::Q3DWheelEvent*)

#endif // QT3D_Q3DMOUSEEVENT_H
