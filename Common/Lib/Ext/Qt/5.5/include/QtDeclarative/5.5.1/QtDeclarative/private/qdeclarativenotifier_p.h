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

#ifndef QDECLARATIVENOTIFIER_P_H
#define QDECLARATIVENOTIFIER_P_H

#include "private/qdeclarativeguard_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeNotifierEndpoint;
class QDeclarativeNotifier
{
public:
    inline QDeclarativeNotifier();
    inline ~QDeclarativeNotifier();
    inline void notify();

private:
    friend class QDeclarativeNotifierEndpoint;

    static void emitNotify(QDeclarativeNotifierEndpoint *);
    QDeclarativeNotifierEndpoint *endpoints;
};

class QDeclarativeNotifierEndpoint
{
public:
    inline QDeclarativeNotifierEndpoint();
    inline QDeclarativeNotifierEndpoint(QObject *t, int m);
    inline ~QDeclarativeNotifierEndpoint();

    QObject *target;
    int targetMethod;

    inline bool isConnected();
    inline bool isConnected(QObject *source, int sourceSignal);
    inline bool isConnected(QDeclarativeNotifier *);

    void connect(QObject *source, int sourceSignal);
    inline void connect(QDeclarativeNotifier *);
    inline void disconnect();

    void copyAndClear(QDeclarativeNotifierEndpoint &other);

private:
    friend class QDeclarativeNotifier;

    struct Signal {
        QDeclarativeGuard<QObject> source;
        int sourceSignal;
    };

    struct Notifier {
        QDeclarativeNotifier *notifier;
        QDeclarativeNotifierEndpoint **disconnected;

        QDeclarativeNotifierEndpoint  *next;
        QDeclarativeNotifierEndpoint **prev;
    };

    enum { InvalidType, SignalType, NotifierType } type;
    union {
        struct {
            Signal *signal;
            union {
                char signalData[sizeof(Signal)];
                qint64 q_for_alignment_1;
                double q_for_alignment_2;
            };
        } signal;
        Notifier notifier;
    };

    inline Notifier *toNotifier();
    inline Notifier *asNotifier();
    inline Signal *toSignal();
    inline Signal *asSignal();
};

QDeclarativeNotifier::QDeclarativeNotifier()
: endpoints(0)
{
}

QDeclarativeNotifier::~QDeclarativeNotifier()
{
    QDeclarativeNotifierEndpoint *endpoint = endpoints;
    while (endpoint) {
        QDeclarativeNotifierEndpoint::Notifier *n = endpoint->asNotifier();
        endpoint = n->next;

        n->next = 0;
        n->prev = 0;
        n->notifier = 0;
        if (n->disconnected) *n->disconnected = 0;
        n->disconnected = 0;
    }
    endpoints = 0;
}

void QDeclarativeNotifier::notify()
{
    if (endpoints) emitNotify(endpoints);
}

QDeclarativeNotifierEndpoint::QDeclarativeNotifierEndpoint()
: target(0), targetMethod(0), type(InvalidType)
{
}

QDeclarativeNotifierEndpoint::QDeclarativeNotifierEndpoint(QObject *t, int m)
: target(t), targetMethod(m), type(InvalidType)
{
}

QDeclarativeNotifierEndpoint::~QDeclarativeNotifierEndpoint()
{
    disconnect();
    if (SignalType == type) {
        Signal *s = asSignal();
        s->~Signal();
    }
}

bool QDeclarativeNotifierEndpoint::isConnected()
{
    if (SignalType == type) {
        return asSignal()->source;
    } else if (NotifierType == type) {
        return asNotifier()->notifier;
    } else {
        return false;
    }
}

bool QDeclarativeNotifierEndpoint::isConnected(QObject *source, int sourceSignal)
{
    return SignalType == type && asSignal()->source == source && asSignal()->sourceSignal == sourceSignal;
}

bool QDeclarativeNotifierEndpoint::isConnected(QDeclarativeNotifier *notifier)
{
    return NotifierType == type && asNotifier()->notifier == notifier;
}

void QDeclarativeNotifierEndpoint::connect(QDeclarativeNotifier *notifier)
{
    Notifier *n = toNotifier();

    if (n->notifier == notifier)
        return;

    disconnect();

    n->next = notifier->endpoints;
    if (n->next) { n->next->asNotifier()->prev = &n->next; }
    notifier->endpoints = this;
    n->prev = &notifier->endpoints;
    n->notifier = notifier;
}

void QDeclarativeNotifierEndpoint::disconnect()
{
    if (type == SignalType) {
        Signal *s = asSignal();
        if (s->source) {
            QMetaObject::disconnectOne(s->source, s->sourceSignal, target, targetMethod);
            s->source = 0;
        }
    } else if (type == NotifierType) {
        Notifier *n = asNotifier();

        if (n->next) n->next->asNotifier()->prev = n->prev;
        if (n->prev) *n->prev = n->next;
        if (n->disconnected) *n->disconnected = 0;
        n->next = 0;
        n->prev = 0;
        n->disconnected = 0;
        n->notifier = 0;
    }
}

QDeclarativeNotifierEndpoint::Notifier *QDeclarativeNotifierEndpoint::toNotifier()
{
    if (NotifierType == type)
        return asNotifier();

    if (SignalType == type) {
        disconnect();
        Signal *s = asSignal();
        s->~Signal();
    }

    type = NotifierType;
    Notifier *n = asNotifier();
    n->next = 0;
    n->prev = 0;
    n->disconnected = 0;
    n->notifier = 0;
    return n;
}

QDeclarativeNotifierEndpoint::Notifier *QDeclarativeNotifierEndpoint::asNotifier()
{
    Q_ASSERT(type == NotifierType);
    return &notifier;
}

QDeclarativeNotifierEndpoint::Signal *QDeclarativeNotifierEndpoint::toSignal()
{
    if (SignalType == type)
        return asSignal();

    disconnect();
    signal.signal = new (&signal.signalData) Signal;
    type = SignalType;
    return signal.signal;
}

QDeclarativeNotifierEndpoint::Signal *QDeclarativeNotifierEndpoint::asSignal()
{
    Q_ASSERT(type == SignalType);
    return signal.signal;
}

QT_END_NAMESPACE

#endif // QDECLARATIVENOTIFIER_P_H

