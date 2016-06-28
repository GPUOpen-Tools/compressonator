/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTDECLARATIVECLASS_P_H
#define QSCRIPTDECLARATIVECLASS_P_H

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

#include <QtCore/qobjectdefs.h>
#include <QtScript/qscriptvalue.h>
#include <QtScript/qscriptclass.h>

QT_BEGIN_NAMESPACE

class QScriptDeclarativeClassPrivate;
class PersistentIdentifierPrivate;
class QScriptContext;
class Q_SCRIPT_EXPORT QScriptDeclarativeClass
{
public:
#define QT_HAVE_QSCRIPTDECLARATIVECLASS_VALUE
    class Q_SCRIPT_EXPORT Value
    {
    public:
        Value();
        Value(const Value &);

        Value(QScriptContext *, int);
        Value(QScriptContext *, uint);
        Value(QScriptContext *, bool);
        Value(QScriptContext *, double);
        Value(QScriptContext *, float);
        Value(QScriptContext *, const QString &);
        Value(QScriptContext *, const QScriptValue &);
        Value(QScriptEngine *, int);
        Value(QScriptEngine *, uint);
        Value(QScriptEngine *, bool);
        Value(QScriptEngine *, double);
        Value(QScriptEngine *, float);
        Value(QScriptEngine *, const QString &);
        Value(QScriptEngine *, const QScriptValue &);
        ~Value();

        QScriptValue toScriptValue(QScriptEngine *) const;
    private:
        char dummy[8];
    };

    typedef void* Identifier;

    struct Object { virtual ~Object() {} };

    static QScriptValue newObject(QScriptEngine *, QScriptDeclarativeClass *, Object *);
    static Value newObjectValue(QScriptEngine *, QScriptDeclarativeClass *, Object *);
    static QScriptDeclarativeClass *scriptClass(const QScriptValue &);
    static Object *object(const QScriptValue &);

    static QScriptValue function(const QScriptValue &, const Identifier &);
    static QScriptValue property(const QScriptValue &, const Identifier &);
    static Value functionValue(const QScriptValue &, const Identifier &);
    static Value propertyValue(const QScriptValue &, const Identifier &);

    static QScriptValue scopeChainValue(QScriptContext *, int index);
    static QScriptContext *pushCleanContext(QScriptEngine *);

    static QScriptValue newStaticScopeObject(
        QScriptEngine *, int propertyCount, const QString *names,
       const QScriptValue *values, const QScriptValue::PropertyFlags *flags);
    static QScriptValue newStaticScopeObject(QScriptEngine *);

    class Q_SCRIPT_EXPORT PersistentIdentifier 
    {
    public:
        Identifier identifier;

        PersistentIdentifier();
        ~PersistentIdentifier();
        PersistentIdentifier(const PersistentIdentifier &other);
        PersistentIdentifier &operator=(const PersistentIdentifier &other);

        QString toString() const;
    private:
        friend class QScriptDeclarativeClass;
        PersistentIdentifier(QScriptEnginePrivate *e) : identifier(0), engine(e), d(0) {}
        QScriptEnginePrivate *engine;
        void *d;
    };

    QScriptDeclarativeClass(QScriptEngine *engine);
    virtual ~QScriptDeclarativeClass();

    QScriptEngine *engine() const;

    bool supportsCall() const;
    void setSupportsCall(bool);

    PersistentIdentifier createPersistentIdentifier(const QString &);
    PersistentIdentifier createPersistentIdentifier(const Identifier &);

    QString toString(const Identifier &);
    bool startsWithUpper(const Identifier &);
    quint32 toArrayIndex(const Identifier &, bool *ok);

    virtual QScriptClass::QueryFlags queryProperty(Object *, const Identifier &, 
                                                   QScriptClass::QueryFlags flags);

    virtual Value property(Object *, const Identifier &);
    virtual void setProperty(Object *, const Identifier &name, const QScriptValue &);
    virtual QScriptValue::PropertyFlags propertyFlags(Object *, const Identifier &);
    virtual Value call(Object *, QScriptContext *);
    virtual bool compare(Object *, Object *);

    virtual QStringList propertyNames(Object *);

    virtual bool isQObject() const;
    virtual QObject *toQObject(Object *, bool *ok = 0);
    virtual QVariant toVariant(Object *, bool *ok = 0);

    QScriptContext *context() const;
protected:
    friend class QScriptDeclarativeClassPrivate;
    QScopedPointer<QScriptDeclarativeClassPrivate> d_ptr;
};

QT_END_NAMESPACE

#endif
