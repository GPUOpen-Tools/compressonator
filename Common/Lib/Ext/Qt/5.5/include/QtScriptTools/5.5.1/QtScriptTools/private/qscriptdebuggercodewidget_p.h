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

#ifndef QSCRIPTDEBUGGERCODEWIDGET_P_H
#define QSCRIPTDEBUGGERCODEWIDGET_P_H

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

#include "qscriptdebuggercodewidgetinterface_p.h"

QT_BEGIN_NAMESPACE

class QScriptDebuggerCodeWidgetPrivate;
class Q_AUTOTEST_EXPORT QScriptDebuggerCodeWidget:
    public QScriptDebuggerCodeWidgetInterface
{
    Q_OBJECT
public:
    QScriptDebuggerCodeWidget(QWidget *parent = 0);
    ~QScriptDebuggerCodeWidget();

    QScriptDebuggerScriptsModel *scriptsModel() const;
    void setScriptsModel(QScriptDebuggerScriptsModel *model);

    void setToolTipProvider(QScriptToolTipProviderInterface *toolTipProvider);

    qint64 currentScriptId() const;
    void setCurrentScript(qint64 scriptId);

    void invalidateExecutionLineNumbers();

    QScriptBreakpointsModel *breakpointsModel() const;
    void setBreakpointsModel(QScriptBreakpointsModel *model);

    QScriptDebuggerCodeViewInterface *currentView() const;

private:
    Q_DECLARE_PRIVATE(QScriptDebuggerCodeWidget)
    Q_DISABLE_COPY(QScriptDebuggerCodeWidget)

    Q_PRIVATE_SLOT(d_func(), void _q_onBreakpointToggleRequest(int,bool))
    Q_PRIVATE_SLOT(d_func(), void _q_onBreakpointEnableRequest(int,bool))
    Q_PRIVATE_SLOT(d_func(), void _q_onBreakpointsAboutToBeRemoved(const QModelIndex &,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_onBreakpointsInserted(const QModelIndex &,int,int))
    Q_PRIVATE_SLOT(d_func(), void _q_onBreakpointsDataChanged(const QModelIndex &, const QModelIndex &))
    Q_PRIVATE_SLOT(d_func(), void _q_onScriptsChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_onToolTipRequest(const QPoint &, int, const QStringList &))
};

QT_END_NAMESPACE

#endif
