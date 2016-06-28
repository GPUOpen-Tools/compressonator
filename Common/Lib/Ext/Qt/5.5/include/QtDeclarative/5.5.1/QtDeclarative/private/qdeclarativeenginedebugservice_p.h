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

#ifndef QDECLARATIVEENGINEDEBUGSERVICE_P_H
#define QDECLARATIVEENGINEDEBUGSERVICE_P_H

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

#include <private/qdeclarativedebugservice_p.h>

#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>
#include <QtCore/qpointer.h>

QT_BEGIN_NAMESPACE

class QDeclarativeEngine;
class QDeclarativeContext;
class QDeclarativeWatcher;
class QDataStream;
class QDeclarativeState;

class QDeclarativeEngineDebugService : public QDeclarativeDebugService
{
    Q_OBJECT
public:
    QDeclarativeEngineDebugService(QObject * = 0);

    struct QDeclarativeObjectData {
        QUrl url;
        int lineNumber;
        int columnNumber;
        QString idString;
        QString objectName;
        QString objectType;
        int objectId;
        int contextId;
        int parentId;
    };

    struct QDeclarativeObjectProperty {
        enum Type { Unknown, Basic, Object, List, SignalProperty, Variant };
        Type type;
        QString name;
        QVariant value;
        QString valueTypeName;
        QString binding;
        bool hasNotifySignal;
    };

    void addEngine(QDeclarativeEngine *);
    void remEngine(QDeclarativeEngine *);
    void objectCreated(QDeclarativeEngine *, QObject *);

    static QDeclarativeEngineDebugService *instance();

protected:
    virtual void messageReceived(const QByteArray &);

private Q_SLOTS:
    void propertyChanged(int id, int objectId, const QMetaProperty &property, const QVariant &value);

private:
    void prepareDeferredObjects(QObject *);
    void buildObjectList(QDataStream &, QDeclarativeContext *);
    void buildObjectDump(QDataStream &, QObject *, bool, bool);
    void buildStatesList(QDeclarativeContext *, bool);
    void buildStatesList(QObject *obj);
    QDeclarativeObjectData objectData(QObject *);
    QDeclarativeObjectProperty propertyData(QObject *, int);
    QVariant valueContents(const QVariant &defaultValue) const;
    void setBinding(int objectId, const QString &propertyName, const QVariant &expression, bool isLiteralValue, QString filename = QString(), int line = -1);
    void resetBinding(int objectId, const QString &propertyName);
    void setMethodBody(int objectId, const QString &method, const QString &body);

    QList<QDeclarativeEngine *> m_engines;
    QDeclarativeWatcher *m_watch;
    QList<QPointer<QDeclarativeState> > m_allStates;
};
Q_DECLARATIVE_PRIVATE_EXPORT QDataStream &operator<<(QDataStream &, const QDeclarativeEngineDebugService::QDeclarativeObjectData &);
Q_DECLARATIVE_PRIVATE_EXPORT QDataStream &operator>>(QDataStream &, QDeclarativeEngineDebugService::QDeclarativeObjectData &);
Q_DECLARATIVE_PRIVATE_EXPORT QDataStream &operator<<(QDataStream &, const QDeclarativeEngineDebugService::QDeclarativeObjectProperty &);
Q_DECLARATIVE_PRIVATE_EXPORT QDataStream &operator>>(QDataStream &, QDeclarativeEngineDebugService::QDeclarativeObjectProperty &);

QT_END_NAMESPACE

#endif // QDECLARATIVEENGINEDEBUGSERVICE_P_H

