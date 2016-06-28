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

#ifndef QT3D_RENDER_RENDERTEXTURE_H
#define QT3D_RENDER_RENDERTEXTURE_H

#include <QOpenGLContext>
#include <QMutex>
#include <Qt3DRenderer/qtexture.h>
#include <Qt3DRenderer/texturedata.h>
#include <Qt3DCore/qbackendnode.h>
#include <Qt3DRenderer/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

class QOpenGLTexture;

namespace Qt3D {

class QAbstractTextureProvider;

namespace Render {

class TextureManager;
class TextureImageManager;
class TextureDataManager;

typedef uint TextureDNA;

class RenderTexture : public QBackendNode
{
public:
    RenderTexture();
    ~RenderTexture();
    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;

    QOpenGLTexture* getOrCreateGLTexture() ;

    GLint textureId();

    bool isTextureReset() const;

    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;
    TextureDNA dna() const;

    void setTextureManager(TextureManager *manager);
    void setTextureImageManager(TextureImageManager *manager);
    void setTextureDataManager(TextureDataManager *manager);

    void updateAndLoadTextureImage();
    void addTextureImageData(HTextureImage handle);
    void removeTextureImageData(HTextureImage handle);

    void requestTextureDataUpdate();
    void addToPendingTextureJobs();
    void setSize(int width, int height, int depth);
    void setFormat(QAbstractTextureProvider::TextureFormat format);

    inline QVector<HTextureImage> textureImages() const { return m_textureImages; }
    inline QAbstractTextureProvider::TextureFormat format() const { return m_format; }

private:
    QOpenGLTexture *m_gl;

    QOpenGLTexture *buildGLTexture();
    void setToGLTexture(RenderTextureImage *rImg, TexImageData *imgData);
    void updateWrapAndFilters();

    int m_width;
    int m_height;
    int m_depth;
    int m_layers;
    bool m_generateMipMaps;
    QAbstractTextureProvider::Target m_target;
    QAbstractTextureProvider::TextureFormat m_format;
    QAbstractTextureProvider::Filter m_magnificationFilter;
    QAbstractTextureProvider::Filter m_minificationFilter;
    QTextureWrapMode::WrapMode m_wrapModeX;
    QTextureWrapMode::WrapMode m_wrapModeY;
    QTextureWrapMode::WrapMode m_wrapModeZ;
    float m_maximumAnisotropy;
    QAbstractTextureProvider::ComparisonFunction m_comparisonFunction;
    QAbstractTextureProvider::ComparisonMode m_comparisonMode;

    QVector<HTextureImage> m_textureImages;

    bool m_isDirty;
    bool m_filtersAndWrapUpdated;
    bool m_dataUploadRequired;
    bool m_formatWasSpecified;
    bool m_unique;

    QMutex *m_lock;
    TextureDNA m_textureDNA;
    TextureManager *m_textureManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;

    void updateDNA();
};

class RenderTextureFunctor : public QBackendNodeFunctor
{
public:
    explicit RenderTextureFunctor(TextureManager *textureManager,
                                  TextureImageManager *textureImageManager,
                                  TextureDataManager *textureDataManager);

    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_FINAL;
    QBackendNode *get(const QNodeId &id) const Q_DECL_FINAL;
    void destroy(const QNodeId &id) const Q_DECL_FINAL;

private:
    TextureManager *m_textureManager;
    TextureImageManager *m_textureImageManager;
    TextureDataManager *m_textureDataManager;
};

} // namespace Render
} // namespace Qt3D

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::Render::RenderTexture*)

#endif // QT3D_RENDER_RENDERTEXTURE_H
