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

#ifndef QT3D_QABSTRACTASPECT_H
#define QT3D_QABSTRACTASPECT_H

#include <QObject>
#include <QSharedPointer>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qaspectjobproviderinterface.h>
#include <Qt3DCore/qbackendnodefactory.h>
#include <Qt3DCore/qsceneobserverinterface.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAspectManager;
class QNode;
class QBackendNode;
class QEntity;
class QAbstractAspectPrivate;
class QAbstractAspectJobManager;
class QBackendNodeFunctor;
class QServiceLocator;

typedef QSharedPointer<QBackendNodeFunctor> QBackendNodeFunctorPtr;

class QT3DCORESHARED_EXPORT QAbstractAspect
        : public QObject
        , public QAspectJobProviderInterface
        , public QSceneObserverInterface
        , public QBackendNodeFactory
{
    Q_OBJECT

public:
    enum AspectType {
        AspectRenderer,
        AspectAnimation,
        AspectCollision,
        AspectPhysics,
        AspectPhysicsAndCollision,
        AspectAI,
        AspectAudio,
        AspectOther
    };

    explicit QAbstractAspect(AspectType aspectType, QObject *parent = 0);

    AspectType aspectType() const;

    void registerAspect(QEntity *rootObject);

    QServiceLocator *services() const;
    QAbstractAspectJobManager *jobManager() const;

    bool isShuttingDown() const;

protected:
    QAbstractAspect(QAbstractAspectPrivate &dd, QObject *parent = 0);

    QBackendNode *createBackendNode(QNode *frontend) const Q_DECL_OVERRIDE;
    QBackendNode *getBackendNode(QNode *frontend) const;
    void clearBackendNode(QNode *frontend) const;

    template<class Frontend>
    void registerBackendType(const QBackendNodeFunctorPtr &functor);
    void registerBackendType(const QMetaObject &, const QBackendNodeFunctorPtr &functor);

private:
    virtual void setRootEntity(QEntity *rootObject) = 0;
    virtual void onInitialize(const QVariantMap &data) = 0;
    virtual void onStartup();
    virtual void onShutdown();
    virtual void onCleanup() = 0;

    Q_DECLARE_PRIVATE(QAbstractAspect)
    friend class QAspectManager;
};

template<class Frontend>
void QAbstractAspect::registerBackendType(const QBackendNodeFunctorPtr &functor)
{
    registerBackendType(Frontend::staticMetaObject, functor);
}

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_ABSTRACTASPECT_H
