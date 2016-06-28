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

#ifndef QT3D_RENDER_MANAGERS_P_H
#define QT3D_RENDER_MANAGERS_P_H

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRenderer/private/renderattachment_p.h>
#include <Qt3DRenderer/private/rendercameralens_p.h>
#include <Qt3DRenderer/private/renderannotation_p.h>
#include <Qt3DRenderer/private/rendereffect_p.h>
#include <Qt3DRenderer/private/renderentity_p.h>
#include <Qt3DRenderer/private/renderlayer_p.h>
#include <Qt3DRenderer/private/rendermaterial_p.h>
#include <Qt3DRenderer/private/rendershader_p.h>
#include <Qt3DRenderer/private/sortcriterion_p.h>
#include <Qt3DRenderer/private/rendertechnique_p.h>
#include <Qt3DRenderer/private/rendertexture_p.h>
#include <Qt3DRenderer/private/rendertransform_p.h>
#include <Qt3DRenderer/private/rendertarget_p.h>
#include <Qt3DRenderer/private/renderrenderpass_p.h>
#include <Qt3DRenderer/private/renderparameter_p.h>
#include <Qt3DRenderer/private/rendershaderdata_p.h>
#include <Qt3DRenderer/private/handle_types_p.h>
#include <Qt3DRenderer/private/uniformbuffer_p.h>
#include <Qt3DRenderer/private/rendertextureimage_p.h>
#include <Qt3DRenderer/private/renderattribute_p.h>
#include <Qt3DRenderer/private/rendergeometry_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

class AttachmentManager : public QResourceManager<
        RenderAttachment,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    AttachmentManager() {}
};

class CameraManager : public QResourceManager<
        RenderCameraLens,
        QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    CameraManager() {}
};

class CriterionManager : public QResourceManager<
        RenderAnnotation,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>

{
public:
    CriterionManager() {}
};

class EffectManager : public QResourceManager<
        RenderEffect,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    EffectManager() {}
};

class EntityManager : public QResourceManager<RenderEntity, QNodeId, 16>
{
public:
    EntityManager() {}
};

class FrameGraphManager : public QResourceManager<
        FrameGraphNode *,
        QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    FrameGraphManager() {}
};

class LayerManager : public QResourceManager<
        RenderLayer,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    LayerManager() {}
};

class MaterialManager : public QResourceManager<
        RenderMaterial,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    MaterialManager() {}
};

class MatrixManager : public QResourceManager<QMatrix4x4, QNodeId, 16>
{
public:
    MatrixManager() {}
};

class ShaderManager : public QResourceManager<
        RenderShader,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ShaderManager() {}
};

class SortCriterionManager : public QResourceManager<
        SortCriterion,
        QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    SortCriterionManager() {}
};

class TechniqueManager : public QResourceManager<
        RenderTechnique,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TechniqueManager() {}
};

class TextureManager : public QResourceManager<
        RenderTexture,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TextureManager() {}
};

class TransformManager : public QResourceManager<
        RenderTransform,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    TransformManager() {}
};

class VAOManager : public QResourceManager<
        QOpenGLVertexArrayObject *,
        QPair<HGeometry, HShader>,
        16>
{
public:
    VAOManager() {}
};

class RenderTargetManager : public QResourceManager<
        RenderTarget,
        QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    RenderTargetManager() {}
};

class RenderPassManager : public QResourceManager<
        RenderRenderPass,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    RenderPassManager() {}
};


class ParameterManager : public QResourceManager<
        RenderParameter,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ParameterManager() {}
};

class ShaderDataManager : public QResourceManager<
        RenderShaderData,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
public:
    ShaderDataManager() {}
};

class UBOManager : public QResourceManager<
        UniformBuffer,
        ShaderDataShaderUboKey,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

class TextureImageManager : public QResourceManager<
        RenderTextureImage,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

class AttributeManager : public QResourceManager<
        RenderAttribute,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

class GeometryManager : public QResourceManager<
        RenderGeometry,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy,
        Qt3D::ObjectLevelLockingPolicy>
{
};

} // Render

Q_DECLARE_RESOURCE_INFO(Render::RenderAnnotation, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderEffect, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderEntity, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderLayer, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderMaterial, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderShader, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::SortCriterion, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTarget, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTechnique, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTexture, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderRenderPass, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderTextureImage, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderAttribute, Q_REQUIRES_CLEANUP);
Q_DECLARE_RESOURCE_INFO(Render::RenderGeometry, Q_REQUIRES_CLEANUP);

} // Qt3D

QT_END_NAMESPACE


#endif // QT3D_RENDER_MANAGERS_P_H
