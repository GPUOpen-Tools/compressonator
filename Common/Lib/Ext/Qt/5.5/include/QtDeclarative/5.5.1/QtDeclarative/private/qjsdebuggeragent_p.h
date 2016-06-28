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

#ifndef QJSDEBUGGERAGENT_P_H
#define QJSDEBUGGERAGENT_P_H

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
#include <QtCore/qdatastream.h>
#include <QtCore/qset.h>

QT_BEGIN_NAMESPACE
class QScriptValue;
class QDeclarativeEngine;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QJSDebuggerAgentPrivate;

enum JSDebuggerState
{
    NoState,
    SteppingIntoState,
    SteppingOverState,
    SteppingOutState,
    StoppedState
};

enum JSCoverageMessage {
    CoverageLocation,
    CoverageScriptLoad,
    CoveragePosChange,
    CoverageFuncEntry,
    CoverageFuncExit,
    CoverageComplete,

    CoverageMaximumMessage
};

struct JSAgentWatchData
{
    QByteArray exp;
    QByteArray name;
    QByteArray value;
    QByteArray type;
    bool hasChildren;
    quint64 objectId;
};

inline QDataStream &operator<<(QDataStream &s, const JSAgentWatchData &data)
{
    return s << data.exp << data.name << data.value
             << data.type << data.hasChildren << data.objectId;
}

inline QDataStream &operator>>(QDataStream &s, JSAgentWatchData &data)
{
    return s >> data.exp >> data.name >> data.value
             >> data.type >> data.hasChildren >> data.objectId;
}

struct JSAgentStackData
{
    QByteArray functionName;
    QByteArray fileUrl;
    qint32 lineNumber;
};

inline QDataStream &operator<<(QDataStream &s, const JSAgentStackData &data)
{
    return s << data.functionName << data.fileUrl << data.lineNumber;
}

inline QDataStream &operator>>(QDataStream &s, JSAgentStackData &data)
{
    return s >> data.functionName >> data.fileUrl >> data.lineNumber;
}

struct JSAgentBreakpointData
{
    QByteArray functionName;
    QByteArray fileUrl;
    qint32 lineNumber;
};

typedef QSet<JSAgentBreakpointData> JSAgentBreakpoints;

inline QDataStream &operator<<(QDataStream &s, const JSAgentBreakpointData &data)
{
    return s << data.functionName << data.fileUrl << data.lineNumber;
}

inline QDataStream &operator>>(QDataStream &s, JSAgentBreakpointData &data)
{
    return s >> data.functionName >> data.fileUrl >> data.lineNumber;
}

inline bool operator==(const JSAgentBreakpointData &b1, const JSAgentBreakpointData &b2)
{
    return b1.lineNumber == b2.lineNumber && b1.fileUrl == b2.fileUrl;
}

inline uint qHash(const JSAgentBreakpointData &b)
{
    return b.lineNumber ^ qHash(b.fileUrl);
}


class QJSDebuggerAgent : public QObject, public QScriptEngineAgent
{
    Q_OBJECT

public:
    QJSDebuggerAgent(QScriptEngine *engine, QObject *parent = 0);
    QJSDebuggerAgent(QDeclarativeEngine *engine, QObject *parent = 0);
    ~QJSDebuggerAgent();

    bool isInitialized() const;

    void setBreakpoints(const JSAgentBreakpoints &);
    void setWatchExpressions(const QStringList &);

    void stepOver();
    void stepInto();
    void stepOut();
    void continueExecution();
    void setCoverageEnabled(bool enabled);

    JSAgentWatchData executeExpression(const QString &expr);
    QList<JSAgentWatchData> expandObjectById(quint64 objectId);
    QList<JSAgentWatchData> locals();
    QList<JSAgentWatchData> localsAtFrame(int frameId);
    QList<JSAgentStackData> backtrace();
    QList<JSAgentWatchData> watches();
    void setProperty(qint64 objectId,
                     const QString &property,
                     const QString &value);

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

Q_SIGNALS:
    void stopped(bool becauseOfException,
                 const QString &exception);

private:
    friend class QJSDebuggerAgentPrivate;
    QJSDebuggerAgentPrivate *d;
};

QT_END_NAMESPACE

#endif // QJSDEBUGGERAGENT_P_H
