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

#ifndef QSCRIPTDEBUGGERAGENT_P_H
#define QSCRIPTDEBUGGERAGENT_P_H

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

#include <QtScript/qscriptengineagent.h>

#include <QtCore/qpair.h>

#include "qscriptbreakpointdata_p.h"
#include "qscriptscriptdata_p.h"

QT_BEGIN_NAMESPACE

class QScriptDebuggerBackendPrivate;

class QScriptDebuggerAgentPrivate;
class Q_AUTOTEST_EXPORT QScriptDebuggerAgent : public QScriptEngineAgent
{
public:
    QScriptDebuggerAgent(QScriptDebuggerBackendPrivate *backend,
                         QScriptEngine *engine);
    ~QScriptDebuggerAgent();

    void enterStepIntoMode(int count = 1);
    void enterStepOverMode(int count = 1);
    void enterStepOutMode();
    void enterContinueMode();
    void enterInterruptMode();
    void enterRunToLocationMode(const QString &fileName, int lineNumber);
    void enterRunToLocationMode(qint64 scriptId, int lineNumber);
    void enterReturnByForceMode(int contextIndex, const QScriptValue &value);

    int setBreakpoint(const QScriptBreakpointData &data);
    bool deleteBreakpoint(int id);
    void deleteAllBreakpoints();
    QScriptBreakpointData breakpointData(int id) const;
    bool setBreakpointData(int id, const QScriptBreakpointData &data);
    QScriptBreakpointMap breakpoints() const;

    QScriptScriptMap scripts() const;
    QScriptScriptData scriptData(qint64 id) const;
    void scriptsCheckpoint();
    QPair<QList<qint64>, QList<qint64> > scriptsDelta() const;
    qint64 resolveScript(const QString &fileName) const;

    QList<qint64> contextIds() const;
    QPair<QList<qint64>, QList<qint64> > contextsCheckpoint();

    void nullifyBackendPointer();

    // reimplemented
    void scriptLoad(qint64 id, const QString &program,
                    const QString &fileName, int baseLineNumber);
    void scriptUnload(qint64 id);

    void contextPush();
    void contextPop();

    void functionEntry(qint64 scriptId);
    void functionExit(qint64 scriptId,
                      const QScriptValue &returnValue);

    void positionChange(qint64 scriptId,
                        int lineNumber, int columnNumber);

    void exceptionThrow(qint64 scriptId,
                        const QScriptValue &exception,
                        bool hasHandler);
    void exceptionCatch(qint64 scriptId,
                        const QScriptValue &exception);

    bool supportsExtension(Extension extension) const;
    QVariant extension(Extension extension,
                       const QVariant &argument = QVariant());

private:
    QScriptDebuggerAgentPrivate *d_ptr;
    Q_DECLARE_PRIVATE(QScriptDebuggerAgent)
    Q_DISABLE_COPY(QScriptDebuggerAgent)
};

QT_END_NAMESPACE

#endif
