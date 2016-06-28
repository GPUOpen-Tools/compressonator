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

#ifndef QT3D_QRENDERASPECT_P_H
#define QT3D_QRENDERASPECT_P_H

#include <Qt3DCore/private/qabstractaspect_p.h>
#include <Qt3DRenderer/qrenderaspect.h>
#include <Qt3DRenderer/private/updateboundingvolumejob_p.h>
#include <Qt3DRenderer/private/updateworldtransformjob_p.h>
#include <Qt3DRenderer/private/framepreparationjob_p.h>
#include <Qt3DRenderer/private/framecleanupjob_p.h>
#include <Qt3DRenderer/private/platformsurfacefilter_p.h>

QT_BEGIN_NAMESPACE

class QSurface;

namespace Qt3D {

namespace Render {
class Renderer;
}

class QRenderAspectPrivate : public QAbstractAspectPrivate
{
    QRenderAspectPrivate(QRenderAspect::RenderType type);

    Q_DECLARE_PUBLIC(QRenderAspect)

    void setSurface(QSurface *surface);

    Render::Renderer *m_renderer;

    // The filter has affinity with the main thread so we have to delete it there
    // via QScopedPointerDeleteLater
    QScopedPointer<Render::PlatformSurfaceFilter, QScopedPointerDeleteLater> m_surfaceEventFilter;
    QSurface *m_surface;
    qint64 m_time;

    bool m_initialized;
    Render::FramePreparationJobPtr m_framePreparationJob;
    Render::FrameCleanupJobPtr m_cleanupJob;
    Render::UpdateWorldTransformJobPtr m_worldTransformJob;
    Render::UpdateBoundingVolumeJobPtr m_boundingVolumeJob;
};

}

QT_END_NAMESPACE

#endif // QT3D_QRENDERASPECT_P_H
