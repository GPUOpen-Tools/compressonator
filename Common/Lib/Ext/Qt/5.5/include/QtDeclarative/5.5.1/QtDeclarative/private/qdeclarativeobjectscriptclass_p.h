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

#ifndef QDECLARATIVEOBJECTSCRIPTCLASS_P_H
#define QDECLARATIVEOBJECTSCRIPTCLASS_P_H

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

#include "private/qdeclarativepropertycache_p.h"
#include "private/qdeclarativetypenamecache_p.h"

#include <private/qscriptdeclarativeclass_p.h>
#include <QtScript/qscriptengine.h>

QT_BEGIN_NAMESPACE

class QDeclarativeEngine;
class QScriptContext;
class QScriptEngine;
class QDeclarativeContextData;
class MethodData;

class Q_AUTOTEST_EXPORT QDeclarativeObjectMethodScriptClass : public QScriptDeclarativeClass
{
public:
    QDeclarativeObjectMethodScriptClass(QDeclarativeEngine *);
    ~QDeclarativeObjectMethodScriptClass();

    QScriptValue newMethod(QObject *, const QDeclarativePropertyCache::Data *);

protected:
    virtual Value call(Object *, QScriptContext *);
    virtual QScriptClass::QueryFlags queryProperty(Object *, const Identifier &, QScriptClass::QueryFlags flags);
    virtual Value property(Object *, const Identifier &);

private:
    int enumType(const QMetaObject *, const QString &);

    Value callPrecise(QObject *, const QDeclarativePropertyCache::Data &, QScriptContext *);
    Value callOverloaded(MethodData *, QScriptContext *);
    Value callMethod(QObject *, int index, int returnType, int argCount, int *argTypes, QScriptContext *ctxt);

    int matchScore(const QScriptValue &, int, const QByteArray &);
    QDeclarativePropertyCache::Data *relatedMethod(QObject *, QDeclarativePropertyCache::Data *current,
                                                   QDeclarativePropertyCache::Data &dummy);

    PersistentIdentifier m_connectId;
    PersistentIdentifier m_disconnectId;
    QScriptValue m_connect;
    QScriptValue m_disconnect;

    static QScriptValue connect(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue disconnect(QScriptContext *context, QScriptEngine *engine);

    QDeclarativeEngine *engine;
};

class Q_AUTOTEST_EXPORT QDeclarativeObjectScriptClass : public QScriptDeclarativeClass
{
public:
    QDeclarativeObjectScriptClass(QDeclarativeEngine *);
    ~QDeclarativeObjectScriptClass();

    QScriptValue newQObject(QObject *, int type = QMetaType::QObjectStar);

    QObject *toQObject(const QScriptValue &) const;
    int objectType(const QScriptValue &) const;

    enum QueryHint {
        ImplicitObject = 0x01,
        SkipAttachedProperties = 0x02
    };
    Q_DECLARE_FLAGS(QueryHints, QueryHint)

    QScriptClass::QueryFlags queryProperty(QObject *, const Identifier &,
                                           QScriptClass::QueryFlags flags,
                                           QDeclarativeContextData *evalContext,
                                           QueryHints hints = 0);

    Value property(QObject *, const Identifier &);

    void setProperty(QObject *, const Identifier &name, const QScriptValue &,
                     QScriptContext *context, QDeclarativeContextData *evalContext = 0);
    virtual QStringList propertyNames(Object *);
    virtual bool compare(Object *, Object *);

protected:
    virtual QScriptClass::QueryFlags queryProperty(Object *, const Identifier &,
                                                   QScriptClass::QueryFlags flags);

    virtual Value property(Object *, const Identifier &);
    virtual void setProperty(Object *, const Identifier &name, const QScriptValue &);
    virtual bool isQObject() const;
    virtual QObject *toQObject(Object *, bool *ok = 0);

private:
    friend class QDeclarativeObjectMethodScriptClass;
    QDeclarativeObjectMethodScriptClass methods;

    QDeclarativeTypeNameCache::Data *lastTNData;
    QDeclarativePropertyCache::Data *lastData;
    QDeclarativePropertyCache::Data local;

    PersistentIdentifier m_destroyId;
    PersistentIdentifier m_toStringId;
    QScriptValue m_destroy;
    QScriptValue m_toString;

    static QScriptValue tostring(QScriptContext *context, QScriptEngine *engine);
    static QScriptValue destroy(QScriptContext *context, QScriptEngine *engine);

    QDeclarativeEngine *engine;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeObjectScriptClass::QueryHints);

QT_END_NAMESPACE

#endif // QDECLARATIVEOBJECTSCRIPTCLASS_P_H

