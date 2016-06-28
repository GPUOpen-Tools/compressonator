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

#ifndef QT3D_QPLANEMESH_H
#define QT3D_QPLANEMESH_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/qgeometryrenderer.h>
#include <QSize>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QT3DRENDERERSHARED_EXPORT QPlaneMesh : public QGeometryRenderer
{
    Q_OBJECT
    Q_PROPERTY(float width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(float height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(QSize meshResolution READ meshResolution WRITE setMeshResolution NOTIFY meshResolutionChanged)

public:
    explicit QPlaneMesh(QNode *parent = 0);
    ~QPlaneMesh();

    void setWidth(float width);
    float width() const;

    void setHeight(float height);
    float height() const;

    void setMeshResolution(const QSize &resolution);
    QSize meshResolution() const;

Q_SIGNALS:
    void widthChanged();
    void heightChanged();
    void meshResolutionChanged();

private:
    // As this is a default provided geometry renderer, no one should be able
    // to modify the QGeometryRenderer's properties

    void setInstanceCount(int instanceCount);
    void setPrimitiveCount(int primitiveCount);
    void setBaseVertex(int baseVertex);
    void setBaseInstance(int baseInstance);
    void setRestartIndex(int index);
    void setPrimitiveRestart(bool enabled);
    void setGeometry(QGeometry *geometry);
    void setPrimitiveType(PrimitiveType primitiveType);
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QPLANEMESH_H
