/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
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

#ifndef QSCRIPTVALUE_P_H
#define QSCRIPTVALUE_P_H

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
#include <QtCore/qshareddata.h>

#include "wtf/Platform.h"
#include "JSValue.h"

QT_BEGIN_NAMESPACE

class QString;
class QScriptEnginePrivate;

class QScriptValue;
class QScriptValuePrivate : public QSharedData
{
    Q_DISABLE_COPY(QScriptValuePrivate)
public:
    inline void* operator new(size_t, QScriptEnginePrivate*);
    inline void operator delete(void*);

    enum Type {
        JavaScriptCore,
        Number,
        String
    };

    inline QScriptValuePrivate(QScriptEnginePrivate*);
    inline ~QScriptValuePrivate();

    inline void initFrom(JSC::JSValue value);
    inline void initFrom(qsreal value);
    inline void initFrom(const QString &value);

    inline bool isJSC() const;
    inline bool isObject() const;

    static inline QScriptValuePrivate *get(const QScriptValue &q)
    {
        return q.d_ptr.data();
    }

    static inline QScriptValue toPublic(QScriptValuePrivate *d)
    {
        return QScriptValue(d);
    }

    static inline QScriptEnginePrivate *getEngine(const QScriptValue &q)
    {
        if (!q.d_ptr)
            return 0;
        return q.d_ptr->engine;
    }

    inline JSC::JSValue property(const JSC::Identifier &id,
                                 const QScriptValue::ResolveFlags &mode = QScriptValue::ResolvePrototype) const;
    inline JSC::JSValue property(quint32 index, const QScriptValue::ResolveFlags &mode = QScriptValue::ResolvePrototype) const;
    inline JSC::JSValue property(const JSC::UString &, const QScriptValue::ResolveFlags &mode = QScriptValue::ResolvePrototype) const;
    inline void setProperty(const JSC::UString &name, const JSC::JSValue &value,
                            const QScriptValue::PropertyFlags &flags = QScriptValue::KeepExistingFlags);
    inline void setProperty(const JSC::Identifier &id, const JSC::JSValue &value,
                            const QScriptValue::PropertyFlags &flags = QScriptValue::KeepExistingFlags);
    inline void setProperty(quint32 index, const JSC::JSValue &value,
                            const QScriptValue::PropertyFlags &flags = QScriptValue::KeepExistingFlags);
    inline QScriptValue::PropertyFlags propertyFlags(
        const JSC::Identifier &id, const QScriptValue::ResolveFlags &mode = QScriptValue::ResolvePrototype) const;

    void detachFromEngine();

    qint64 objectId()
    {
        if ( (type == JavaScriptCore) && (engine) && jscValue.isCell() )
            return (qint64)jscValue.asCell();
        else
            return -1;
    }

    QScriptEnginePrivate *engine;
    Type type;
    JSC::JSValue jscValue;
    qsreal numberValue;
    QString stringValue;

    // linked list of engine's script values
    QScriptValuePrivate *prev;
    QScriptValuePrivate *next;
};

inline QScriptValuePrivate::QScriptValuePrivate(QScriptEnginePrivate *e)
    : engine(e), prev(0), next(0)
{
}

inline bool QScriptValuePrivate::isJSC() const
{
    return (type == JavaScriptCore);
}

inline bool QScriptValuePrivate::isObject() const
{
    return isJSC() && jscValue && jscValue.isObject();
}

// Rest of inline functions implemented in qscriptengine_p.h

QT_END_NAMESPACE

#endif
