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

#ifndef QSCRIPTENGINEDEBUGGER_H
#define QSCRIPTENGINEDEBUGGER_H

#include <QtCore/qobject.h>
#include <QtScript/qtscriptglobal.h>

QT_BEGIN_NAMESPACE


class QAction;
class QScriptEngine;
class QWidget;
#ifndef QT_NO_MAINWINDOW
class QMainWindow;
#endif
class QMenu;
class QToolBar;

class QScriptEngineDebuggerPrivate;
class Q_SCRIPTTOOLS_EXPORT QScriptEngineDebugger : public QObject
{
    Q_OBJECT
public:
    enum DebuggerWidget {
        ConsoleWidget,
        StackWidget,
        ScriptsWidget,
        LocalsWidget,
        CodeWidget,
        CodeFinderWidget,
        BreakpointsWidget,
        DebugOutputWidget,
        ErrorLogWidget
    };

    enum DebuggerAction {
        InterruptAction,
        ContinueAction,
        StepIntoAction,
        StepOverAction,
        StepOutAction,
        RunToCursorAction,
        RunToNewScriptAction,
        ToggleBreakpointAction,
        ClearDebugOutputAction,
        ClearErrorLogAction,
        ClearConsoleAction,
        FindInScriptAction,
        FindNextInScriptAction,
        FindPreviousInScriptAction,
        GoToLineAction
    };

    enum DebuggerState {
        RunningState,
        SuspendedState
    };

    QScriptEngineDebugger(QObject *parent = 0);
    ~QScriptEngineDebugger();

    void attachTo(QScriptEngine *engine);
    void detach();

    bool autoShowStandardWindow() const;
    void setAutoShowStandardWindow(bool autoShow);

#ifndef QT_NO_MAINWINDOW
    QMainWindow *standardWindow() const;
#endif
    QToolBar *createStandardToolBar(QWidget *parent = 0);
    QMenu *createStandardMenu(QWidget *parent = 0);

    QWidget *widget(DebuggerWidget widget) const;
    QAction *action(DebuggerAction action) const;

    DebuggerState state() const;

Q_SIGNALS:
    void evaluationSuspended();
    void evaluationResumed();

private:
    Q_DECLARE_PRIVATE(QScriptEngineDebugger)
    Q_DISABLE_COPY(QScriptEngineDebugger)

    Q_PRIVATE_SLOT(d_func(), void _q_showStandardWindow())
};

QT_END_NAMESPACE

#endif
