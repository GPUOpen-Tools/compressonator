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

#ifndef QT3D_QBUFFER_H
#define QT3D_QBUFFER_H

#include <Qt3DCore/qabstractbuffer.h>
#include <Qt3DRenderer/qt3drenderer_global.h>
#include <QSharedPointer>
#include <QOpenGLBuffer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

GLint elementType(GLint type);
GLint tupleSizeFromType(GLint type);
GLuint byteSizeFromType(GLint type);

class QBufferPrivate;
class QBufferFunctor;
typedef QSharedPointer<QBufferFunctor> QBufferFunctorPtr;

class QT3DRENDERERSHARED_EXPORT QBuffer : public QAbstractBuffer
{
    Q_OBJECT
    Q_PROPERTY(BufferType type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(UsageType usage READ usage WRITE setUsage NOTIFY usageChanged)

public:
    enum BufferType
    {
        VertexBuffer        = 0x8892, // GL_ARRAY_BUFFER
        IndexBuffer         = 0x8893, // GL_ELEMENT_ARRAY_BUFFER
        PixelPackBuffer     = 0x88EB, // GL_PIXEL_PACK_BUFFER
        PixelUnpackBuffer   = 0x88EC  // GL_PIXEL_UNPACK_BUFFER
    };
    Q_ENUM(BufferType)

    enum UsageType
    {
        StreamDraw          = 0x88E0, // GL_STREAM_DRAW
        StreamRead          = 0x88E1, // GL_STREAM_READ
        StreamCopy          = 0x88E2, // GL_STREAM_COPY
        StaticDraw          = 0x88E4, // GL_STATIC_DRAW
        StaticRead          = 0x88E5, // GL_STATIC_READ
        StaticCopy          = 0x88E6, // GL_STATIC_COPY
        DynamicDraw         = 0x88E8, // GL_DYNAMIC_DRAW
        DynamicRead         = 0x88E9, // GL_DYNAMIC_READ
        DynamicCopy         = 0x88EA  // GL_DYNAMIC_COPY
    };
    Q_ENUM(UsageType)

    QBuffer(BufferType ty = QBuffer::VertexBuffer, QNode *parent = 0);
    ~QBuffer();

    void setUsage(UsageType usage);
    UsageType usage() const;

    void setType(BufferType type);
    BufferType type() const;

    void setBufferFunctor(const QBufferFunctorPtr &functor);
    QBufferFunctorPtr bufferFunctor() const;

protected:
    QBuffer(QBufferPrivate &dd, QBuffer::BufferType ty, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

Q_SIGNALS:
    void typeChanged();
    void usageChanged();

private:
    Q_DECLARE_PRIVATE(QBuffer)
    QT3D_CLONEABLE(QBuffer)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QBUFFER_H
