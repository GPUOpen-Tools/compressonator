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

#ifndef QDECLARATIVECONNECTIONS_H
#define QDECLARATIVECONNECTIONS_H

#include <qdeclarative.h>
#include <qdeclarativescriptstring.h>
#include <private/qdeclarativecustomparser_p.h>

#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeBoundSignal;
class QDeclarativeContext;
class QDeclarativeConnectionsPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeConnections : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeConnections)

    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(bool ignoreUnknownSignals READ ignoreUnknownSignals WRITE setIgnoreUnknownSignals)

public:
    QDeclarativeConnections(QObject *parent=0);
    ~QDeclarativeConnections();

    QObject *target() const;
    void setTarget(QObject *);

    bool ignoreUnknownSignals() const;
    void setIgnoreUnknownSignals(bool ignore);

Q_SIGNALS:
    void targetChanged();

private:
    void connectSignals();
    void classBegin();
    void componentComplete();
};

class QDeclarativeConnectionsParser : public QDeclarativeCustomParser
{
public:
    virtual QByteArray compile(const QList<QDeclarativeCustomParserProperty> &);
    virtual void setCustomData(QObject *, const QByteArray &);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeConnections)

#endif
