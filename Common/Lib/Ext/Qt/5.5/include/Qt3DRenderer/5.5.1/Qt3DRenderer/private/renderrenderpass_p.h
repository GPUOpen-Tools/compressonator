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

#ifndef QT3D_RENDER_RENDERRENDERPASS_H
#define QT3D_RENDER_RENDERRENDERPASS_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/private/renderparametermapping_p.h>
#include <Qt3DRenderer/private/parameterpack_p.h>
#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QRenderPass;
class QAbstractShader;
class QParameterMapping;
class QAnnotation;
class QRenderState;

namespace Render {

class RenderPassManager;
class RenderState;

class Q_AUTOTEST_EXPORT RenderRenderPass : public QBackendNode
{
public:
    RenderRenderPass();
    ~RenderRenderPass();

    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

    QNodeId shaderProgram() const;
    QList<RenderParameterMapping> bindings() const;
    QList<QNodeId> annotations() const;
    QList<RenderState *> renderStates() const;
    QList<QNodeId> parameters() const;

private:
    void appendAnnotation(const QNodeId &criterionId);
    void removeAnnotation(const QNodeId &criterionId);

    void appendBinding(const RenderParameterMapping &binding);
    void removeBinding(const QNodeId &bindingId);

    void appendRenderState(const QNodeId &id, RenderState *renderState);
    void removeRenderState(const QNodeId &renderStateId);

    QNodeId m_shaderUuid;
    QHash<QNodeId, RenderParameterMapping> m_bindings;
    QHash<QNodeId, RenderState *> m_renderStates;
    QList<QNodeId> m_annotationList;
    ParameterPack m_parameterPack;
};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERRENDERPASS_H
