/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_RENDER_RENDERBUFFER_H
#define QT3D_RENDER_RENDERBUFFER_H

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qbufferfunctor.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

class BufferManager;

class Q_AUTOTEST_EXPORT RenderBuffer : public QBackendNode
{
public:
    RenderBuffer();
    ~RenderBuffer();
    void cleanup();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

    void setManager(BufferManager *manager);
    void executeFunctor();

    inline QBuffer::BufferType type() const { return m_type; }
    inline QBuffer::UsageType usage() const { return m_usage; }
    inline QByteArray data() const { return m_data; }
    inline bool isDirty() const { return m_bufferDirty; }
    inline QBufferFunctorPtr bufferFunctor() const { return m_functor; }
    void unsetDirty();

private:
    QBuffer::BufferType m_type;
    QBuffer::UsageType m_usage;
    QByteArray m_data;
    bool m_bufferDirty;
    QBufferFunctorPtr m_functor;
    BufferManager *m_manager;
};

class RenderBufferFunctor : public QBackendNodeFunctor
{
public:
    explicit RenderBufferFunctor(BufferManager *manager);
    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE;
private:
    BufferManager *m_manager;
};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERBUFFER_H
