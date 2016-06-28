/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_DEPENDENCYHANDLER_P_H
#define QT3D_DEPENDENCYHANDLER_P_H

#include "task_p.h"

#include <QtCore/QMutex>
#include <QtCore/QVector>

QT_BEGIN_NAMESPACE

namespace Qt3D {

struct Dependency
{
    Dependency() {}
    Dependency(RunnableInterface *depender, RunnableInterface *dependee)
        : depender(qMove(depender)),
          dependee(qMove(dependee)) {}

    RunnableInterface *depender;
    RunnableInterface *dependee;
};

} // namespace Qt3D

template <>
class QTypeInfo<Qt3D::Dependency> : public QTypeInfoMerger<Qt3D::Dependency, Qt3D::RunnableInterface *> {};

namespace Qt3D {

inline bool operator==(const Dependency &left, const Dependency &right)
{
    return left.depender == right.depender && left.dependee == right.dependee;
}

inline bool operator!=(const Dependency &left, const Dependency &right)
{
    return !operator==(left, right);
}

class DependencyHandler
{
public:
    DependencyHandler();

    void addDependencies(QVector<Dependency> dependencies);
    bool hasDependency(const RunnableInterface *depender);
    QVector<RunnableInterface *> freeDependencies(const RunnableInterface *task);
    void setMutex(QMutex *mutex) { m_mutex = mutex; }

private:
    Q_DISABLE_COPY(DependencyHandler)

    QVector<Dependency> m_dependencyMap;
    QMutex *m_mutex;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_DEPENDENCYHANDLER_P_H

