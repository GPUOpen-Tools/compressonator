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

#ifndef QT3D_QCHANGEARBITER_P_H
#define QT3D_QCHANGEARBITER_P_H

#include <QObject>
#include <QFlags>
#include <QReadWriteLock>
#include <QVariant>
#include <QVector>
#include <QPair>
#include <QThreadStorage>
#include <QMutex>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qscenechange.h>
#include <Qt3DCore/private/qlockableobserverinterface_p.h>
#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QObservableInterface;
class QAbstractAspectJobManager;
class QSceneObserverInterface;
class QAbstractPostman;
class QScene;


class QT3DCORE_PRIVATE_EXPORT QAbstractArbiter : public QLockableObserverInterface
{
public:
    virtual QAbstractPostman *postman() const = 0;
};

class QT3DCORE_PRIVATE_EXPORT QChangeArbiter Q_DECL_FINAL
        : public QObject
        , public QAbstractArbiter
{
    Q_OBJECT
public:
    explicit QChangeArbiter(QObject *parent = 0);
    ~QChangeArbiter();

    void initialize(Qt3D::QAbstractAspectJobManager *jobManager);

    void syncChanges();

    void registerObserver(QObserverInterface *observer,
                          const QNodeId &nodeId,
                          ChangeFlags changeFlags = AllChanges);
    void unregisterObserver(QObserverInterface *observer,
                            const QNodeId &nodeId);

    void registerSceneObserver(QSceneObserverInterface *observer);
    void unregisterSceneObserver(QSceneObserverInterface *observer);

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;         // QLockableObserverInterface impl
    void sceneChangeEventWithLock(const QSceneChangePtr &e) Q_DECL_OVERRIDE; // QLockableObserverInterface impl
    void sceneChangeEventWithLock(const QSceneChangeList &e) Q_DECL_OVERRIDE; // QLockableObserverInterface impl

    Q_INVOKABLE void setPostman(Qt3D::QAbstractPostman *postman);
    Q_INVOKABLE void setScene(Qt3D::QScene *scene);

    QAbstractPostman *postman() const Q_DECL_FINAL;
    QScene *scene() const;

    static void createUnmanagedThreadLocalChangeQueue(void *changeArbiter);
    static void destroyUnmanagedThreadLocalChangeQueue(void *changeArbiter);
    static void createThreadLocalChangeQueue(void *changeArbiter);
    static void destroyThreadLocalChangeQueue(void *changeArbiter);

protected:
    typedef std::vector<QSceneChangePtr> QChangeQueue;
    typedef QPair<ChangeFlags, QObserverInterface *> QObserverPair;
    typedef QVector<QObserverPair> QObserverList;

    void distributeQueueChanges(QChangeQueue *queue);

    QThreadStorage<QChangeQueue *> *tlsChangeQueue();
    void appendChangeQueue(QChangeQueue *queue);
    void removeChangeQueue(QChangeQueue *queue);
    void appendLockingChangeQueue(QChangeQueue *queue);
    void removeLockingChangeQueue(QChangeQueue *queue);

private:
    QMutex m_mutex;
    QAbstractAspectJobManager *m_jobManager;

    // The lists of observers indexed by observable. We maintain two
    // distinct hashes:
    //
    // m_aspectObservations is for observables owned by aspects
    // m_nodeObservations is for observables in the main thread's object tree
    //
    // We keep these distinct because we do not manage the main thread which means
    // the mechanisms for working with objects there is different.
    QHash<QNodeId, QObserverList> m_nodeObservations;
    QList<QSceneObserverInterface *> m_sceneObservers;

    // Each thread has a TLS ChangeQueue so we never need to lock whilst
    // receiving a QSceneChange.
    QThreadStorage<QChangeQueue *> m_tlsChangeQueue;

    // We store a list of the ChangeQueue's from each thread. This will only
    // be accessed from the aspect thread during the syncChanges() phase.
    QList<QChangeQueue *> m_changeQueues;
    QList<QChangeQueue *> m_lockingChangeQueues;
    QAbstractPostman *m_postman;
    QScene *m_scene;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QCHANGEARBITER_P_H
