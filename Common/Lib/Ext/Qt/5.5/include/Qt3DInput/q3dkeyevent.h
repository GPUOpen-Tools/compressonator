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

#ifndef QT3D_Q3DKEYEVENT_H
#define QT3D_Q3DKEYEVENT_H

#include <QObject>
#include <QKeyEvent>
#include <Qt3DInput/qt3dinput_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class Q3DKeyEventPrivate;
class Q3DKeyEvent;

typedef QSharedPointer<Q3DKeyEvent> Q3DKeyEventPtr;

class QT3DINPUTSHARED_EXPORT Q3DKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key CONSTANT)
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(int modifiers READ modifiers CONSTANT)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat CONSTANT)
    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(quint32 nativeScanCode READ nativeScanCode CONSTANT)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    Q3DKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text=QString(), bool autorep=false, ushort count=1);
    Q3DKeyEvent(const QKeyEvent &ke);

    inline int key() const { return m_event.key(); }
    inline QString text() const { return m_event.text(); }
    inline int modifiers() const { return m_event.modifiers(); }
    inline bool isAutoRepeat() const { return m_event.isAutoRepeat(); }
    inline int count() const { return m_event.count(); }
    inline quint32 nativeScanCode() const { return m_event.nativeScanCode(); }
    inline bool isAccepted() const { return m_event.isAccepted(); }
    inline void setAccepted(bool accepted) { m_event.setAccepted(accepted); }
    inline QEvent::Type type() const { return m_event.type(); }
    Q_INVOKABLE bool matches(QKeySequence::StandardKey key_) const { return m_event.matches(key_); }

private:
    QKeyEvent m_event;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_Q3DKEYEVENT_H
