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

#ifndef QSCRIPTDEBUGGERAGENT_P_P_H
#define QSCRIPTDEBUGGERAGENT_P_P_H

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

#include <QtScript/qscriptvalue.h>
#include <QtCore/qdatetime.h>
#include <QtCore/qhash.h>
#include <QtCore/qmap.h>
#include <QtCore/qlist.h>

#include "qscriptscriptdata_p.h"
#include "qscriptbreakpointdata_p.h"

QT_BEGIN_NAMESPACE

class QScriptDebuggerAgent;
class QScriptDebuggerAgentPrivate
{
public:
    enum State {
        NoState,
        SteppingIntoState,
        SteppedIntoState,
        SteppingOverState,
        SteppedOverState,
        SteppingOutState,
        SteppedOutState,
        RunningToLocationState,
        ReachedLocationState,
        InterruptingState,
        InterruptedState,
        BreakpointState,
        ReturningByForceState,
        ReturnedByForceState
    };

    QScriptDebuggerAgentPrivate();
    ~QScriptDebuggerAgentPrivate();

    static QScriptDebuggerAgentPrivate *get(QScriptDebuggerAgent *);

    State state;
    int stepDepth;
    int stepCount;
    int targetScriptId;
    QString targetFileName;
    int targetLineNumber;
    QScriptValue stepResult;
    int returnCounter;
    QScriptValue returnValue;

    int nextBreakpointId;
    QHash<qint64, QList<int> > resolvedBreakpoints;
    QHash<QString, QList<int> > unresolvedBreakpoints;
    QScriptBreakpointMap breakpoints;
    int hitBreakpointId;

    QScriptScriptMap scripts;
    QScriptScriptMap checkpointScripts;
    QScriptScriptMap previousCheckpointScripts;
    QList<QList<qint64> > scriptIdStack;

    QList<qint64> contextIdStack;
    QList<qint64> checkpointContextIdStack;
    qint64 nextContextId;

    QTime processEventsTimer;
    int statementCounter;

    QScriptDebuggerBackendPrivate *backend;
};

QT_END_NAMESPACE

#endif
