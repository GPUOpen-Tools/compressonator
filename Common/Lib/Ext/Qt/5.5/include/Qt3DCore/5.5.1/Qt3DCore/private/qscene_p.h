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

#ifndef QT3D_QSCENE_P_H
#define QT3D_QSCENE_P_H

#include <Qt3DCore/private/qt3dcore_global_p.h>
#include <Qt3DCore/private/qobservableinterface_p.h>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QScenePrivate;
class QAspectEngine;

typedef QList<QObservableInterface *> QObservableList;

class QT3DCORE_PRIVATE_EXPORT QScene
{
public:
    QScene(QAspectEngine *engine = Q_NULLPTR);
    ~QScene();

    QAspectEngine *engine() const;

    void addObservable(QObservableInterface *observable, const QNodeId &id);
    void addObservable(QNode *observable);
    void removeObservable(QObservableInterface *observable, const QNodeId &id);
    void removeObservable(QNode *observable);
    QObservableList lookupObservables(const QNodeId &id) const;

    QNode *lookupNode(const QNodeId &id) const;
    QVector<QNode *> lookupNodes(const QVector<QNodeId> &ids) const;
    QNodeId nodeIdFromObservable(QObservableInterface *observable) const;

    void setArbiter(Qt3D::QLockableObserverInterface *arbiter);
    Qt3D::QLockableObserverInterface *arbiter() const;

    // Component -> Entities
    QList<QNodeId> entitiesForComponent(const QNodeId &id) const;
    void addEntityForComponent(const QNodeId &componentUuid, const QNodeId &entityUuid);
    void removeEntityForComponent(const QNodeId &componentUuid, const QNodeId &entityUuid);
    bool hasEntityForComponent(const QNodeId &componentUuid, const QNodeId &entityUuid);

private:
    Q_DECLARE_PRIVATE(QScene)
    QScopedPointer<QScenePrivate> d_ptr;
};

} // Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::QScene*)

#endif // QT3D_QSCENE_P_H
