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

#ifndef QDECLARATIVEDEBUGSERVER_H
#define QDECLARATIVEDEBUGSERVER_H

#include <private/qdeclarativeglobal_p.h>
#include <private/qdeclarativedebugserverconnection_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeDebugService;

class QDeclarativeDebugServerPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeDebugServer : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeDebugServer)
    Q_DISABLE_COPY(QDeclarativeDebugServer)
public:
    static QDeclarativeDebugServer *instance();

    void setConnection(QDeclarativeDebugServerConnection *connection);

    bool hasDebuggingClient() const;

    QList<QDeclarativeDebugService*> services() const;
    QStringList serviceNames() const;

    bool addService(QDeclarativeDebugService *service);
    bool removeService(QDeclarativeDebugService *service);

    void sendMessage(QDeclarativeDebugService *service, const QByteArray &message);
    void receiveMessage(const QByteArray &message);

    bool waitForMessage(QDeclarativeDebugService *service);

private:
    friend class QDeclarativeDebugService;
    friend class QDeclarativeDebugServicePrivate;
    QDeclarativeDebugServer();
    Q_PRIVATE_SLOT(d_func(), void _q_deliverMessage(QString, QByteArray))
};

QT_END_NAMESPACE

#endif // QDECLARATIVEDEBUGSERVICE_H
