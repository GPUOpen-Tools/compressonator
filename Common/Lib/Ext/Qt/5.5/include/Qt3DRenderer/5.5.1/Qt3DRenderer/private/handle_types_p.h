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

#ifndef QT3D_RENDER_HANDLE_TYPES_P_H
#define QT3D_RENDER_HANDLE_TYPES_P_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/qhandle.h>

QT_BEGIN_NAMESPACE

class QMatrix4x4;
class QOpenGLVertexArrayObject;

namespace Qt3D {

class TexImageData;

namespace Render {

class RenderAttachment;
class RenderCameraLens;
class RenderAnnotation;
class RenderEffect;
class RenderEntity;
class RenderShader;
class FrameGraphNode;
class RenderLayer;
class RenderMaterial;
class SortCriterion;
class RenderTechnique;
class RenderTexture;
class RenderTransform;
class RenderTarget;
class RenderRenderPass;
class RenderParameter;
class RenderShaderData;
class RenderTextureImage;
class RenderBuffer;
class RenderAttribute;
class RenderGeometry;
class RenderGeometryRenderer;

typedef QHandle<RenderAttachment, 16> HAttachment;
typedef QHandle<RenderCameraLens, 8> HCamera;
typedef QHandle<RenderAnnotation, 16> HCriterion;
typedef QHandle<RenderEffect, 16> HEffect;
typedef QHandle<RenderEntity, 16> HEntity;
typedef QHandle<FrameGraphNode *, 8> HFrameGraphNode;
typedef QHandle<RenderLayer, 16> HLayer;
typedef QHandle<RenderMaterial, 16> HMaterial;
typedef QHandle<QMatrix4x4, 16> HMatrix;
typedef QHandle<RenderShader, 16> HShader;
typedef QHandle<QOpenGLVertexArrayObject*, 16> HVao;
typedef QHandle<RenderShader, 16> HShader;
typedef QHandle<SortCriterion, 8> HSortCriterion;
typedef QHandle<RenderTechnique, 16> HTechnique;
typedef QHandle<RenderTexture, 16> HTexture;
typedef QHandle<RenderTransform, 16> HTransform;
typedef QHandle<RenderTarget, 8> HTarget;
typedef QHandle<RenderRenderPass, 16> HRenderPass;
typedef QHandle<TexImageData, 16> HTextureData;
typedef QHandle<RenderParameter, 16> HParameter;
typedef QHandle<RenderShaderData, 16> HShaderData;
typedef QHandle<RenderTextureImage, 16> HTextureImage;
typedef QHandle<RenderBuffer, 16> HBuffer;
typedef QHandle<RenderAttribute, 16> HAttribute;
typedef QHandle<RenderGeometry, 16> HGeometry;
typedef QHandle<RenderGeometryRenderer, 16> HGeometryRenderer;

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_HANDLE_TYPES_P_H
