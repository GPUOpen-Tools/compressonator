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

#ifndef QT3D_QBACKENDNODE_H
#define QT3D_QBACKENDNODE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qscenechange.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QBackendNodeFactory;
class QBackendNodePrivate;
class QBackendNode;
class QAspectEngine;

class QT3DCORESHARED_EXPORT QBackendNodeFunctor
{
public:
    virtual ~QBackendNodeFunctor() {}
    virtual QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const = 0;
    virtual QBackendNode *get(const QNodeId &id) const = 0;
    virtual void destroy(const QNodeId &id) const = 0;
};

typedef QSharedPointer<QBackendNodeFunctor> QBackendNodeFunctorPtr;

class QT3DCORESHARED_EXPORT QBackendNode
{
public:
    enum Mode {
        ReadOnly = 0,
        ReadWrite
    };

    explicit QBackendNode(Mode mode = ReadOnly);
    virtual ~QBackendNode();

    void setFactory(const QBackendNodeFactory *factory);

    void setPeer(QNode *peer);
    QNodeId peerUuid() const;

    Mode mode() const;
    virtual void updateFromPeer(QNode *peer) = 0;

protected:
    QBackendNode *createBackendNode(QNode *frontend) const;
    void notifyObservers(const QSceneChangePtr &e);
    virtual void sceneChangeEvent(const QSceneChangePtr &e) = 0;

    QBackendNode(QBackendNodePrivate &dd);

private:
    Q_DECLARE_PRIVATE(QBackendNode)
    QBackendNodePrivate *d_ptr;
    friend class QBackendScenePropertyChange;
};


} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QBACKENDNODE_H
