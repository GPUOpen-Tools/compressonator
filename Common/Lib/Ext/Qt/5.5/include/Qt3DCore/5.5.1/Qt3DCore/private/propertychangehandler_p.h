/****************************************************************************
**
** Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Milian Wolff <milian.wolff@kdab.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtWebChannel module of the Qt Toolkit.
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

#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <QObject>
#include <QHash>
#include <QVector>
#include <QMetaMethod>
#include <QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

/**
 * The property change handler is similar to QSignalSpy, but geared towards the usecase of Qt3D.
 *
 * It allows connecting to any number of property change signals of the receiver object and forwards
 * the signal invocations to the Receiver by calling its propertyChanged function.
 */
template<class Receiver>
class PropertyChangeHandler : public QObject
{
public:
    PropertyChangeHandler(Receiver *receiver, QObject *parent = Q_NULLPTR);

    /**
     * Connect to the change signal of @p property in @p object.
     */
    void connectToPropertyChange(const QObject *object, int propertyIndex);

    /**
     * Disconnect from the change signal of @p property in @p object.
     */
    void disconnectFromPropertyChange(const QObject *object, int propertyIndex);

    /**
     * @internal
     *
     * Custom implementation of qt_metacall which calls propertyChanged() on the receiver for
     * connected signals.
     */
    int qt_metacall(QMetaObject::Call call, int methodId, void **args) Q_DECL_OVERRIDE;

private:
    Receiver *m_receiver;
};

template<class Receiver>
PropertyChangeHandler<Receiver>::PropertyChangeHandler(Receiver *receiver, QObject *parent)
    : QObject(parent)
    , m_receiver(receiver)
{
}

template<class Receiver>
void PropertyChangeHandler<Receiver>::connectToPropertyChange(const QObject *object, int propertyIndex)
{
    const QMetaObject *metaObject = object->metaObject();
    const QMetaProperty property = metaObject->property(propertyIndex);
    if (!property.hasNotifySignal())
        return;

    static const int memberOffset = QObject::staticMetaObject.methodCount();
    QMetaObject::Connection connection = QMetaObject::connect(object, property.notifySignalIndex(),
                                                              this, memberOffset + propertyIndex,
                                                              Qt::DirectConnection, 0);
    Q_ASSERT(connection);
    Q_UNUSED(connection);
}

template<class Receiver>
void PropertyChangeHandler<Receiver>::disconnectFromPropertyChange(const QObject *object, int propertyIndex)
{
    const QMetaObject *metaObject = object->metaObject();
    const QMetaProperty property = metaObject->property(propertyIndex);
    if (!property.hasNotifySignal())
        return;

    static const int memberOffset = QObject::staticMetaObject.methodCount();
    QMetaObject::disconnect(object, property.notifySignalIndex(), this, memberOffset + propertyIndex);
}

template<class Receiver>
int PropertyChangeHandler<Receiver>::qt_metacall(QMetaObject::Call call, int methodId, void **args)
{
    methodId = QObject::qt_metacall(call, methodId, args);
    if (methodId < 0)
        return methodId;

    if (call == QMetaObject::InvokeMetaMethod) {
        m_receiver->propertyChanged(methodId);
        return -1;
    }
    return methodId;
}

}

QT_END_NAMESPACE

#endif // SIGNALHANDLER_H
