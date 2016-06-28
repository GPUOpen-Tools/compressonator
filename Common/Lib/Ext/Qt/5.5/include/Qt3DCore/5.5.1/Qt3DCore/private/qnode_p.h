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

#ifndef QT3D_QNODE_P_H
#define QT3D_QNODE_P_H

#include <private/qobject_p.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/private/qobservableinterface_p.h>
#include <Qt3DCore/private/qchangearbiter_p.h>
#include "propertychangehandler_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QNode;
class QAspectEngine;

class QT3DCORESHARED_EXPORT QNodePrivate : public QObjectPrivate, public QObservableInterface
{
public:
    QNodePrivate();

    void setScene(QScene *scene);
    QScene *scene() const;

    void setArbiter(QLockableObserverInterface *arbiter) Q_DECL_OVERRIDE;

    void notifyPropertyChange(const char *name, const QVariant &value);
    void notifyObservers(const QSceneChangePtr &change) Q_DECL_OVERRIDE;

    void insertTree(QNode *treeRoot, int depth = 0);

    Q_DECLARE_PUBLIC(QNode)

    // For now this just protects access to the m_changeArbiter.
    // Later on we may decide to extend support for multiple observers.
    QAbstractArbiter *m_changeArbiter;
    QScene *m_scene;
    mutable QNodeId m_id;
    bool m_blockNotifications;
    bool m_wasCleanedUp;

    static QNodePrivate *get(QNode *q);
    static void nodePtrDeleter(QNode *q);

private:
    void _q_addChild(QNode *childNode);
    void _q_removeChild(QNode *childNode);
    void registerNotifiedProperties();
    void unregisterNotifiedProperties();
    void propertyChanged(int propertyIndex);

    void setSceneHelper(QNode *root);
    void unsetSceneHelper(QNode *root);
    void addEntityComponentToScene(QNode *root);

    friend class PropertyChangeHandler<QNodePrivate>;
    bool m_propertyChangesSetup;
    PropertyChangeHandler<QNodePrivate> m_signals;

    static QHash<QNodeId, QNode *> m_clonesLookupTable;
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_NODE_P_H
