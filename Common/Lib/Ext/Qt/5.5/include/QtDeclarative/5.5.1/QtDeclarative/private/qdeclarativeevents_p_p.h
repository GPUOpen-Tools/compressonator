/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
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
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEEVENTS_P_H
#define QDECLARATIVEEVENTS_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qdeclarative.h>

#include <QtCore/qobject.h>
#include <QtGui/qevent.h>

QT_BEGIN_NAMESPACE

class Q_DECLARATIVE_EXPORT QDeclarativeKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    QDeclarativeKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text=QString(), bool autorep=false, ushort count=1)
        : event(type, key, modifiers, text, autorep, count) { event.setAccepted(false); }
    QDeclarativeKeyEvent(const QKeyEvent &ke)
        : event(ke) { event.setAccepted(false); }

    int key() const { return event.key(); }
    QString text() const { return event.text(); }
    int modifiers() const { return event.modifiers(); }
    bool isAutoRepeat() const { return event.isAutoRepeat(); }
    int count() const { return event.count(); }

    bool isAccepted() { return event.isAccepted(); }
    void setAccepted(bool accepted) { event.setAccepted(accepted); }

private:
    QKeyEvent event;
};

class Q_DECLARATIVE_EXPORT QDeclarativeMouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool wasHeld READ wasHeld)
    Q_PROPERTY(bool isClick READ isClick)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    QDeclarativeMouseEvent(int x, int y, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers
                  , bool isClick=false, bool wasHeld=false)
        : _x(x), _y(y), _button(button), _buttons(buttons), _modifiers(modifiers)
          , _wasHeld(wasHeld), _isClick(isClick), _accepted(true) {}

    int x() const { return _x; }
    int y() const { return _y; }
    int button() const { return _button; }
    int buttons() const { return _buttons; }
    int modifiers() const { return _modifiers; }
    bool wasHeld() const { return _wasHeld; }
    bool isClick() const { return _isClick; }

    // only for internal usage
    void setX(int x) { _x = x; }
    void setY(int y) { _y = y; }

    bool isAccepted() { return _accepted; }
    void setAccepted(bool accepted) { _accepted = accepted; }

private:
    int _x;
    int _y;
    Qt::MouseButton _button;
    Qt::MouseButtons _buttons;
    Qt::KeyboardModifiers _modifiers;
    bool _wasHeld;
    bool _isClick;
    bool _accepted;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeKeyEvent)
QML_DECLARE_TYPE(QDeclarativeMouseEvent)

#endif // QDECLARATIVEEVENTS_P_H
