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

#ifndef QT3D_QGEOMETRYRENDERER_H
#define QT3D_QGEOMETRYRENDERER_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRenderer/qgeometry.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QGeometryRendererPrivate;
class QGeometryFunctor;

typedef QSharedPointer<QGeometryFunctor> QGeometryFunctorPtr;

class QT3DRENDERERSHARED_EXPORT QGeometryRenderer : public QComponent
{
    Q_OBJECT
    Q_PROPERTY(int instanceCount READ instanceCount WRITE setInstanceCount NOTIFY instanceCountChanged)
    Q_PROPERTY(int primitiveCount READ primitiveCount WRITE setPrimitiveCount NOTIFY primitiveCountChanged)
    Q_PROPERTY(int baseVertex READ baseVertex WRITE setBaseVertex NOTIFY baseVertexChanged)
    Q_PROPERTY(int baseInstance READ baseInstance WRITE setBaseInstance NOTIFY baseInstanceChanged)
    Q_PROPERTY(int restartIndex READ restartIndex WRITE setRestartIndex NOTIFY restartIndexChanged)
    Q_PROPERTY(bool primitiveRestart READ primitiveRestart WRITE setPrimitiveRestart NOTIFY primitiveRestartChanged)
    Q_PROPERTY(Qt3D::QGeometry* geometry READ geometry WRITE setGeometry NOTIFY geometryChanged)
    Q_PROPERTY(PrimitiveType primitiveType READ primitiveType WRITE setPrimitiveType NOTIFY primitiveTypeChanged)

public:
    explicit QGeometryRenderer(QNode *parent = 0);
    ~QGeometryRenderer();

    enum PrimitiveType {
        Points = 0x0000,
        Lines = 0x0001,
        LineLoop = 0x0002,
        LineStrip = 0x0003,
        Triangles = 0x0004,
        TriangleStrip = 0x0005,
        TriangleFan = 0x0006,
        LinesAdjacency = 0x000A,
        TrianglesAdjacency = 0x000C,
        LineStripAdjacency = 0x000B,
        TriangleStripAdjacency = 0x000D,
        Patches = 0x000E
    };
    Q_ENUM(PrimitiveType)

    // how to figure out index count, bounding boxes, and all the fancy stuff that QMeshData provides for us?
    // also how to figure out which attribute(s?) hold the indices?

    int instanceCount() const;
    int primitiveCount() const;
    int baseVertex() const;
    int baseInstance() const;
    int restartIndex() const;
    bool primitiveRestart() const;
    QGeometry *geometry() const;
    PrimitiveType primitiveType() const;
    QGeometryFunctorPtr geometryFunctor() const;

    void setInstanceCount(int instanceCount);
    void setPrimitiveCount(int primitiveCount);
    void setBaseVertex(int baseVertex);
    void setBaseInstance(int baseInstance);
    void setRestartIndex(int index);
    void setPrimitiveRestart(bool enabled);
    void setGeometry(QGeometry *geometry);
    void setPrimitiveType(PrimitiveType primitiveType);
    void setGeometryFunctor(const QGeometryFunctorPtr &functor);

Q_SIGNALS:
    void instanceCountChanged();
    void primitiveCountChanged();
    void baseVertexChanged();
    void baseInstanceChanged();
    void restartIndexChanged();
    void primitiveRestartChanged();
    void geometryChanged();
    void primitiveTypeChanged();

protected:
    QGeometryRenderer(QGeometryRendererPrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const QSceneChangePtr &change) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QGeometryRenderer)
    QT3D_CLONEABLE(QGeometryRenderer)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QGEOMETRYRENDERER_H
