/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#ifndef QT3D_QASPECTMANAGER_P_H
#define QT3D_QASPECTMANAGER_P_H

#include <QObject>
#include <Qt3DCore/qt3dcore_global.h>
#include <QList>
#include <QScopedPointer>
#include <QVariant>
#include <QSemaphore>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3D {

class QNode;
class QEntity;
class QScheduler;
class QChangeArbiter;
class QAbstractAspect;
class QAbstractAspectJobManager;
class QSceneObserverInterface;
class QServiceLocator;

class QT3DCORESHARED_EXPORT QAspectManager : public QObject
{
    Q_OBJECT
public:
    explicit QAspectManager(QObject *parent = 0);
    ~QAspectManager();

    bool isShuttingDown() const;

public Q_SLOTS:
    void initialize();
    void shutdown();

    void setRootEntity(Qt3D::QEntity *root);
    void setData(const QVariantMap &data);
    void registerAspect(Qt3D::QAbstractAspect *aspect);
    QVariantMap data() const;

    void exec();
    void quit();

    const QList<QAbstractAspect *> &aspects() const;
    QAbstractAspectJobManager *jobManager() const;
    QChangeArbiter *changeArbiter() const;
    QServiceLocator *serviceLocator() const;

private:
    QList<QAbstractAspect *> m_aspects;
    QEntity *m_root;
    QVariantMap m_data;
    QScheduler *m_scheduler;
    QAbstractAspectJobManager *m_jobManager;
    QChangeArbiter *m_changeArbiter;
    QAtomicInt m_runMainLoop;
    QAtomicInt m_terminated;
    QScopedPointer<QServiceLocator> m_serviceLocator;
    QSemaphore m_waitForEndOfExecLoop;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QASPECTMANAGER_P_H
