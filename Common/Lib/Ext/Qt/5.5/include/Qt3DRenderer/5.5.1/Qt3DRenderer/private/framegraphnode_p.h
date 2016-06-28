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

#ifndef QT3D_RENDER_FRAMEGRAPHNODE_H
#define QT3D_RENDER_FRAMEGRAPHNODE_H

#include <Qt3DCore/qhandle.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DRenderer/qframegraphnode.h>
#include <Qt3DRenderer/private/managers_p.h>
#include <qglobal.h>
#include <QVector>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

class FrameGraphNode : public QBackendNode
{
public:
    FrameGraphNode();
    virtual ~FrameGraphNode();

    enum FrameGraphNodeType {
        InvalidNodeType = 0,
        CameraSelector,
        LayerFilter,
        RenderPassFilter,
        RenderTarget,
        TechniqueFilter,
        Viewport,
        ClearBuffer,
        SortMethod,
        SubtreeSelector,
        StateSet,
        NoDraw
    };
    FrameGraphNodeType nodeType() const { return m_nodeType; }

    void setEnabled(bool enabled) { m_enabled = enabled; }
    inline bool isEnabled() const { return m_enabled; }

    void setFrameGraphManager(FrameGraphManager *manager);

    void setHandle(HFrameGraphNode handle);
    void setParentHandle(HFrameGraphNode parentHandle);
    void appendChildHandle(HFrameGraphNode childHandle);
    void removeChildHandle(HFrameGraphNode childHandle);

    HFrameGraphNode handle() const;
    HFrameGraphNode parentHandle() const;
    QList<HFrameGraphNode> childrenHandles() const;

    FrameGraphNode *parent() const;
    QList<FrameGraphNode *> children() const;

protected:
    FrameGraphNode(FrameGraphNodeType nodeType);

private:
    FrameGraphNodeType m_nodeType;
    bool m_enabled;
    HFrameGraphNode m_handle;
    HFrameGraphNode m_parentHandle;
    QList<HFrameGraphNode> m_childrenHandles;
    FrameGraphManager *m_manager;

    friend class FrameGraphVisitor;
};

template<typename Backend, typename Frontend>
class FrameGraphNodeFunctor : public QBackendNodeFunctor
{
public:
    explicit FrameGraphNodeFunctor(FrameGraphManager *manager)
        : m_manager(manager)
    {
    }

    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE
    {
        return createBackendFrameGraphNode(frontend, factory);
    }

    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE
    {
        FrameGraphNode **node = m_manager->lookupResource(id);
        if (node != Q_NULLPTR)
            return *node;
        return Q_NULLPTR;
    }

    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE
    {
        m_manager->releaseResource(id);
    }

protected:
    Backend *createBackendFrameGraphNode(QNode *n, const QBackendNodeFactory *factory) const
    {
        Frontend *f = qobject_cast<Frontend *>(n);
        if (f != Q_NULLPTR) {
            HFrameGraphNode handle = m_manager->lookupHandle(n->id());
            if (handle.isNull()) {
                handle = m_manager->getOrAcquireHandle(n->id());
                Backend *backend = new Backend();
                *m_manager->data(handle) = backend;
                backend->setFactory(factory);
                backend->setFrameGraphManager(m_manager);
                backend->setHandle(handle);
                backend->setPeer(f);
                QFrameGraphNode *parentFGNode = static_cast<QFrameGraphNode *>(n)->parentFrameGraphNode();
                if (parentFGNode)
                    backend->setParentHandle(m_manager->lookupHandle(parentFGNode->id()));
                return backend;
            }
            return static_cast<Backend *>(*m_manager->data(handle));
        }
        return Q_NULLPTR;
    }

private:
    FrameGraphManager *m_manager;
};

class FrameGraphComponentFunctor : public QBackendNodeFunctor
{
public:
    explicit FrameGraphComponentFunctor(Renderer *renderer);
    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE;

private:
    Renderer *m_renderer;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_FRAMEGRAPHNODE_H
