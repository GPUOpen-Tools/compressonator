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

#ifndef QDECLARATIVEEXPRESSION_P_H
#define QDECLARATIVEEXPRESSION_P_H

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

#include "qdeclarativeexpression.h"

#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativeguard_p.h"

#include <QtScript/qscriptvalue.h>

QT_BEGIN_NAMESPACE

class QDeclarativeAbstractExpression
{
public:
    QDeclarativeAbstractExpression();
    virtual ~QDeclarativeAbstractExpression();

    bool isValid() const;

    QDeclarativeContextData *context() const;
    void setContext(QDeclarativeContextData *);

    virtual void refresh();

private:
    friend class QDeclarativeContext;
    friend class QDeclarativeContextData;
    friend class QDeclarativeContextPrivate;
    QDeclarativeContextData *m_context;
    QDeclarativeAbstractExpression **m_prevExpression;
    QDeclarativeAbstractExpression  *m_nextExpression;
};

class QDeclarativeDelayedError
{
public:
    inline QDeclarativeDelayedError() : nextError(0), prevError(0) {}
    inline ~QDeclarativeDelayedError() { removeError(); }

    QDeclarativeError error;

    bool addError(QDeclarativeEnginePrivate *);

    inline void removeError() {
        if (!prevError) return;
        if (nextError) nextError->prevError = prevError;
        *prevError = nextError;
        nextError = 0;
        prevError = 0;
    }

private:
    QDeclarativeDelayedError  *nextError;
    QDeclarativeDelayedError **prevError;
};

class QDeclarativeQtScriptExpression : public QDeclarativeAbstractExpression,
                                       public QDeclarativeDelayedError
{
public:
    enum Mode { SharedContext, ExplicitContext };

    enum EvaluateFlag { RequiresThisObject = 0x01 };
    Q_DECLARE_FLAGS(EvaluateFlags, EvaluateFlag)

    QDeclarativeQtScriptExpression();
    virtual ~QDeclarativeQtScriptExpression();

    QDeclarativeRefCount *dataRef;

    QString expression;

    Mode expressionFunctionMode;
    QScriptValue expressionFunction;

    QScriptValue expressionContext; // Only used in ExplicitContext
    QObject *scopeObject;           // Only used in SharedContext

    bool notifyOnValueChange() const;
    void setNotifyOnValueChange(bool);
    void resetNotifyOnChange();
    void setNotifyObject(QObject *, int );

    void setEvaluateFlags(EvaluateFlags flags);
    EvaluateFlags evaluateFlags() const;

    QScriptValue scriptValue(QObject *secondaryScope, bool *isUndefined);

    class DeleteWatcher {
    public:
        inline DeleteWatcher(QDeclarativeQtScriptExpression *data);
        inline ~DeleteWatcher();
        inline bool wasDeleted() const;
    private:
        bool *m_wasDeleted;
        bool m_wasDeletedStorage;
        QDeclarativeQtScriptExpression *m_d;
    };

private:
    void clearGuards();
    QScriptValue eval(QObject *secondaryScope, bool *isUndefined);
    void updateGuards(const QPODVector<QDeclarativeEnginePrivate::CapturedProperty, 16> &properties);

    bool trackChange;

    QDeclarativeNotifierEndpoint *guardList;
    int guardListLength;

    QObject *guardObject;
    int guardObjectNotifyIndex;
    bool *deleted;

    EvaluateFlags evalFlags;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeQtScriptExpression::EvaluateFlags)


class QDeclarativeExpression;
class QString;
class QDeclarativeExpressionPrivate : public QObjectPrivate, public QDeclarativeQtScriptExpression
{
    Q_DECLARE_PUBLIC(QDeclarativeExpression)
public:
    QDeclarativeExpressionPrivate();
    ~QDeclarativeExpressionPrivate();

    void init(QDeclarativeContextData *, const QString &, QObject *);
    void init(QDeclarativeContextData *, const QScriptValue &, QObject *);
    void init(QDeclarativeContextData *, void *, QDeclarativeRefCount *, QObject *, const QString &, int);

    QVariant value(QObject *secondaryScope = 0, bool *isUndefined = 0);
    QScriptValue scriptValue(QObject *secondaryScope = 0, bool *isUndefined = 0);

    static QDeclarativeExpressionPrivate *get(QDeclarativeExpression *expr) {
        return static_cast<QDeclarativeExpressionPrivate *>(QObjectPrivate::get(expr));
    }
    static QDeclarativeExpression *get(QDeclarativeExpressionPrivate *expr) {
        return expr->q_func();
    }

    void _q_notify();
    virtual void emitValueChanged();

    static void exceptionToError(QScriptEngine *, QDeclarativeError &);
    static QScriptValue evalInObjectScope(QDeclarativeContextData *, QObject *, const QString &, const QString &,
                                          int, QScriptValue *);
    static QScriptValue evalInObjectScope(QDeclarativeContextData *, QObject *, const QScriptProgram &,
                                          QScriptValue *);

    bool expressionFunctionValid:1;

    QString url; // This is a QString for a reason.  QUrls are slooooooow...
    int line;
    QByteArray name; //function name, hint for the debugger
};

QDeclarativeQtScriptExpression::DeleteWatcher::DeleteWatcher(QDeclarativeQtScriptExpression *data)
: m_wasDeletedStorage(false), m_d(data)
{
    if (!m_d->deleted)
        m_d->deleted = &m_wasDeletedStorage;
    m_wasDeleted = m_d->deleted;
}

QDeclarativeQtScriptExpression::DeleteWatcher::~DeleteWatcher()
{
    if (false == *m_wasDeleted && m_wasDeleted == m_d->deleted)
        m_d->deleted = 0;
}

bool QDeclarativeQtScriptExpression::DeleteWatcher::wasDeleted() const
{
    return *m_wasDeleted;
}

QT_END_NAMESPACE

#endif // QDECLARATIVEEXPRESSION_P_H
