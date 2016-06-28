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

#ifndef QT3D_RENDER_TEXTUREDATAMANAGER_H
#define QT3D_RENDER_TEXTUREDATAMANAGER_H

#include <Qt3DCore/private/qresourcemanager_p.h>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/texturedata.h>
#include <Qt3DRenderer/private/handle_types_p.h>

#include <QPair>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

typedef QPair<QTextureDataFunctorPtr, QVector<HTextureImage> > FunctorImageHandlesPair;
typedef QPair<QTextureDataFunctorPtr, HTextureData> FunctorTextureDataPair;

class TextureDataManager : public QResourceManager<TexImageData,
                                                   QNodeId,
                                                   16,
                                                   Qt3D::ArrayAllocatingPolicy,
                                                   Qt3D::ObjectLevelLockingPolicy>
{
public:
    TextureDataManager();
    void addToPendingTextures(const QNodeId &textureId);

    QVector<QNodeId> texturesPending();

    HTextureData textureDataFromFunctor(const QTextureDataFunctorPtr &functor) const;
    void addTextureDataForFunctor(HTextureData textureDataHandle, const QTextureDataFunctorPtr &functor);
    void removeTextureDataFunctor(const QTextureDataFunctorPtr &functor);

    void assignFunctorToTextureImage(const QTextureDataFunctorPtr &functor, HTextureImage imageHandle);

    QMutex *mutex() const;
    void cleanup();

private:
    QVector<QNodeId> m_texturesPending;
    QVector<FunctorTextureDataPair > m_textureDataFunctors;
    QVector<FunctorImageHandlesPair > m_texturesImagesPerFunctor;
    mutable QMutex m_mutex;
    QVector<HTextureData> m_textureHandlesToRelease;
};

} // Render

Q_DECLARE_RESOURCE_INFO(TexImageData, Q_REQUIRES_CLEANUP);

} // Qt3D


QT_END_NAMESPACE

#endif // TEXTUREDATAMANAGER_H
