/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtSCriptTools module of the Qt Toolkit.
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

#ifndef QSCRIPTDEBUGGERBACKEND_P_P_H
#define QSCRIPTDEBUGGERBACKEND_P_P_H

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

#include <QtCore/qhash.h>
#include <QtCore/qlist.h>
#include <QtScript/qscriptvalue.h>

#include "qscriptdebuggerbackend_p.h"

QT_BEGIN_NAMESPACE

class QEvent;
class QString;
class QScriptContext;
class QScriptEngine;
class QScriptValueIterator;
class QScriptObjectSnapshot;
class QScriptDebuggerAgent;
class QScriptDebuggerCommandExecutor;

class QScriptDebuggerBackend;
class Q_AUTOTEST_EXPORT QScriptDebuggerBackendPrivate
{
    Q_DECLARE_PUBLIC(QScriptDebuggerBackend)
public:
    QScriptDebuggerBackendPrivate();
    virtual ~QScriptDebuggerBackendPrivate();

    void postEvent(QEvent *e);
    virtual bool event(QEvent *e);

    // events reported by agent
    virtual void stepped(qint64 scriptId, int lineNumber, int columnNumber,
                         const QScriptValue &result);
    virtual void locationReached(qint64 scriptId, int lineNumber, int columnNumber);
    virtual void interrupted(qint64 scriptId, int lineNumber, int columnNumber);
    virtual void breakpoint(qint64 scriptId, int lineNumber, int columnNumber,
                            int breakpointId);
    virtual void exception(qint64 scriptId, const QScriptValue &exception,
                           bool hasHandler);
    virtual void debuggerInvocationRequest(qint64 scriptId, int lineNumber,
                                           int columnNumber);
    virtual void forcedReturn(qint64 scriptId, int lineNumber, int columnNumber,
                              const QScriptValue &value);

    static QScriptValue trace(QScriptContext *context,
                              QScriptEngine *engine);
    static QScriptValue qsassert(QScriptContext *context,
                                 QScriptEngine *engine);
    static QScriptValue fileName(QScriptContext *context,
                                 QScriptEngine *engine);
    static QScriptValue lineNumber(QScriptContext *context,
                                   QScriptEngine *engine);

    void agentDestroyed(QScriptDebuggerAgent *);

    QScriptDebuggerAgent *agent;
    QScriptDebuggerCommandExecutor *commandExecutor;

    int pendingEvaluateContextIndex;
    QString pendingEvaluateProgram;
    QString pendingEvaluateFileName;
    int pendingEvaluateLineNumber;
    bool ignoreExceptions;

    int nextScriptValueIteratorId;
    QMap<int, QScriptValueIterator*> scriptValueIterators;

    int nextScriptObjectSnapshotId;
    QMap<int, QScriptObjectSnapshot*> scriptObjectSnapshots;

    QObject *eventReceiver;

    QScriptDebuggerBackend *q_ptr;

    QScriptValue origTraceFunction;
    QScriptValue origFileNameFunction;
    QScriptValue origLineNumberFunction;
};

QT_END_NAMESPACE

#endif
