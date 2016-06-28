/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef QT3D_RENDER_RENDERNODE_H
#define QT3D_RENDER_RENDERNODE_H

#include <Qt3DRenderer/private/renderer_p.h>
#include <Qt3DRenderer/private/handle_types_p.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qhandle.h>
#include <QVector>

QT_BEGIN_NAMESPACE

class QMatrix4x4;

namespace Qt3D {

class QRenderAspect;
class Sphere;
class QNode;
class QEntity;
class QComponent;

namespace Render {

class Renderer;

class Q_AUTOTEST_EXPORT RenderEntity : public QBackendNode
{
public:
    RenderEntity();
    ~RenderEntity();
    void cleanup();

    void setParentHandle(HEntity parentHandle);
    void setRenderer(Renderer *renderer);
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;

    void dump() const;

    void  setHandle(HEntity handle);
    HEntity handle() const { return m_handle; }
    RenderEntity *parent() const;
    HEntity parentHandle() const { return m_parentHandle; }

    void appendChildHandle(HEntity childHandle);
    void removeChildHandle(HEntity childHandle);
    QVector<HEntity> childrenHandles() const { return m_childrenHandles; }
    QVector<RenderEntity *> children() const;

    QMatrix4x4 *worldTransform();
    const QMatrix4x4 *worldTransform() const;
    Sphere *localBoundingVolume() { return m_localBoundingVolume; }
    Sphere *worldBoundingVolume() { return m_worldBoundingVolume; }

    void addComponent(QComponent *component);
    void removeComponent(const QNodeId &nodeId);

    template<class Backend, uint INDEXBITS>
    QHandle<Backend, INDEXBITS> componentHandle() const
    {
        return QHandle<Backend, INDEXBITS>();
    }

    template<class Backend, uint INDEXBITS>
    QList<QHandle<Backend, INDEXBITS> > componentsHandle() const
    {
        return QList<QHandle<Backend, INDEXBITS> >();
    }

    template<class Backend>
    Backend *renderComponent() const
    {
        return Q_NULLPTR;
    }

    template<class Backend>
    QList<Backend *> renderComponents() const
    {
        return QList<Backend *>();
    }

    template<class Backend>
    QNodeId componentUuid() const
    {
        return QNodeId();
    }

    template<class Backend>
    QList<QNodeId> componentsUuid() const
    {
        return QList<QNodeId>();
    }

private:

    template<class Frontend, class Backend, class Manager>
    void createRenderComponentHelper(Frontend *frontend, Manager *manager)
    {
        // We index using the Frontend id
        if (!manager->contains(frontend->id())) {
            Backend *backend = manager->getOrCreateResource(frontend->id());
            backend->setRenderer(m_renderer);
            backend->setPeer(frontend);
        }
    }

    Renderer *m_renderer;
    HEntity m_handle;
    HEntity m_parentHandle;
    QVector<HEntity > m_childrenHandles;

    HMatrix m_worldTransform;
    Sphere *m_localBoundingVolume;
    Sphere *m_worldBoundingVolume;

    // Handles to Components
    QNodeId m_transformComponent;
    QNodeId m_materialComponent;
    QNodeId m_cameraComponent;
    QList<QNodeId> m_layerComponents;
    QList<QNodeId> m_shaderDataComponents;
    QNodeId m_geometryRendererComponent;

    QString m_objectName;
};

template<>
HMaterial RenderEntity::componentHandle<RenderMaterial>() const;

template<>
RenderMaterial *RenderEntity::renderComponent<RenderMaterial>() const;

template<>
HCamera RenderEntity::componentHandle<RenderCameraLens>() const;

template<>
RenderCameraLens *RenderEntity::renderComponent<RenderCameraLens>() const;

template<>
HTransform RenderEntity::componentHandle<RenderTransform>() const;

template<>
RenderTransform *RenderEntity::renderComponent<RenderTransform>() const;

template<>
Q_AUTOTEST_EXPORT HGeometryRenderer RenderEntity::componentHandle<RenderGeometryRenderer>() const;

template<>
Q_AUTOTEST_EXPORT RenderGeometryRenderer *RenderEntity::renderComponent<RenderGeometryRenderer>() const;

template<>
Q_AUTOTEST_EXPORT QNodeId RenderEntity::componentUuid<RenderTransform>() const;

template<>
Q_AUTOTEST_EXPORT QNodeId RenderEntity::componentUuid<RenderCameraLens>() const;

template<>
Q_AUTOTEST_EXPORT QNodeId RenderEntity::componentUuid<RenderMaterial>() const;

template<>
QList<HLayer> RenderEntity::componentsHandle<RenderLayer>() const;

template<>
QList<RenderLayer *> RenderEntity::renderComponents<RenderLayer>() const;

template<>
Q_AUTOTEST_EXPORT QList<QNodeId> RenderEntity::componentsUuid<RenderLayer>() const;

template<>
QList<HShaderData> RenderEntity::componentsHandle<RenderShaderData>() const;

template<>
QList<RenderShaderData *> RenderEntity::renderComponents<RenderShaderData>() const;

template<>
Q_AUTOTEST_EXPORT QList<QNodeId> RenderEntity::componentsUuid<RenderShaderData>() const;

template<>
Q_AUTOTEST_EXPORT QNodeId RenderEntity::componentUuid<RenderGeometryRenderer>() const;

class RenderEntityFunctor : public QBackendNodeFunctor
{
public:
    explicit RenderEntityFunctor(Renderer *renderer);
    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE;

private:
    Renderer *m_renderer;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERNODE_H
