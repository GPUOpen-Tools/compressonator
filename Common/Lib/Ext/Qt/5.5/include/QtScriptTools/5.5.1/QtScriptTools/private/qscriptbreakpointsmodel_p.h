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

#ifndef QSCRIPTBREAKPOINTSMODEL_P_H
#define QSCRIPTBREAKPOINTSMODEL_P_H

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

#include <QtCore/qabstractitemmodel.h>

#include "qscriptbreakpointdata_p.h"

QT_BEGIN_NAMESPACE

class QScriptDebuggerJobSchedulerInterface;
class QScriptDebuggerCommandSchedulerInterface;

class QScriptBreakpointsModelPrivate;
class Q_AUTOTEST_EXPORT QScriptBreakpointsModel
    : public QAbstractItemModel
{
    Q_OBJECT
public:
    QScriptBreakpointsModel(QScriptDebuggerJobSchedulerInterface *jobScheduler,
                            QScriptDebuggerCommandSchedulerInterface *commandScheduler,
                            QObject *parent = 0);
    ~QScriptBreakpointsModel();

    void setBreakpoint(const QScriptBreakpointData &data);
    void setBreakpointData(int id, const QScriptBreakpointData &data);
    void deleteBreakpoint(int id);

    void addBreakpoint(int id, const QScriptBreakpointData &data);
    void modifyBreakpoint(int id, const QScriptBreakpointData &data);
    void removeBreakpoint(int id);

    int breakpointIdAt(int row) const;
    QScriptBreakpointData breakpointDataAt(int row) const;
    QScriptBreakpointData breakpointData(int id) const;

    int resolveBreakpoint(qint64 scriptId, int lineNumber) const;
    int resolveBreakpoint(const QString &fileName, int lineNumber) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    QVariant headerData(int section, Qt::Orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    Q_DECLARE_PRIVATE(QScriptBreakpointsModel)
    Q_DISABLE_COPY(QScriptBreakpointsModel)
};

QT_END_NAMESPACE

#endif
