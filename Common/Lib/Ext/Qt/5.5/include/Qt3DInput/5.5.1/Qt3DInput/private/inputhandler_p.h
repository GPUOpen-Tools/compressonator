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

#ifndef QT3D_INPUT_INPUTHANDLER_P_H
#define QT3D_INPUT_INPUTHANDLER_P_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/private/handle_types_p.h>
#include <Qt3DCore/qaspectjob.h>
#include <QKeyEvent>
#include <QMutex>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

class KeyboardInputManager;
class KeyboardControllerManager;
class KeyboardEventFilter;
class MouseControllerManager;
class MouseInputManager;
class MouseEventFilter;

class InputHandler
{
public:
    InputHandler();

    void setEventSource(QObject *object);
    inline QObject *eventSource() const { return m_eventSource; }

    inline KeyboardControllerManager *keyboardControllerManager() const { return m_keyboardControllerManager; }
    inline KeyboardInputManager *keyboardInputManager() const  { return m_keyboardInputManager; }
    inline MouseControllerManager *mouseControllerManager() const { return m_mouseControllerManager; }
    inline MouseInputManager *mouseInputManager() const { return m_mouseInputManager; }

    void appendKeyEvent(const QKeyEvent &event);
    QList<QKeyEvent> pendingKeyEvents();
    void clearPendingKeyEvents();

    void appendMouseEvent(const QMouseEvent &event);
    QList<QMouseEvent> pendingMouseEvents();
    void clearPendingMouseEvents();

    void appendKeyboardController(HKeyboardController controller);
    void removeKeyboardController(HKeyboardController controller);

    void appendMouseController(HMouseController controller);
    void removeMouseController(HMouseController controller);

    QVector<QAspectJobPtr> keyboardJobs();
    QVector<QAspectJobPtr> mouseJobs();

private:
    KeyboardControllerManager *m_keyboardControllerManager;
    KeyboardInputManager *m_keyboardInputManager;
    MouseControllerManager *m_mouseControllerManager;
    MouseInputManager *m_mouseInputManager;
    QVector<HKeyboardController> m_activeKeyboardControllers;
    QVector<HMouseController> m_activeMouseControllers;
    QObject *m_eventSource;
    KeyboardEventFilter *m_keyboardEventFilter;
    QList<QKeyEvent> m_pendingEvents;
    MouseEventFilter *m_mouseEventFilter;
    QList<QMouseEvent> m_pendingMouseEvents;
    mutable QMutex m_mutex;
};

} // Input

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_INPUT_INPUTHANDLER_P_H
