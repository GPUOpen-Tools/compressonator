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

#ifndef QDECLARATIVEDEBUGSERVICE_H
#define QDECLARATIVEDEBUGSERVICE_H

#include <QtCore/qobject.h>

#include <private/qtdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeDebugServicePrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeDebugService : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeDebugService)
    Q_DISABLE_COPY(QDeclarativeDebugService)

public:
    explicit QDeclarativeDebugService(const QString &, QObject *parent = 0);
    ~QDeclarativeDebugService();

    QString name() const;

    enum Status { NotConnected, Unavailable, Enabled };
    Status status() const;

    void sendMessage(const QByteArray &);
    bool waitForMessage();

    static int idForObject(QObject *);
    static QObject *objectForId(int);

    static QString objectToString(QObject *obj);

    static bool isDebuggingEnabled();
    static bool hasDebuggingClient();

protected:
    virtual void statusChanged(Status);
    virtual void messageReceived(const QByteArray &);

private:
    friend class QDeclarativeDebugServer;
    friend class QDeclarativeDebugServerPrivate;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEDEBUGSERVICE_H

