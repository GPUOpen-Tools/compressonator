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

#ifndef OBJLOADER_H
#define OBJLOADER_H

#include <QVector>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>

#include <limits>

QT_BEGIN_NAMESPACE

class QString;
class QIODevice;

namespace Qt3D {

class QGeometry;

struct FaceIndices
{
    FaceIndices()
        : positionIndex(std::numeric_limits<unsigned int>::max())
        , texCoordIndex(std::numeric_limits<unsigned int>::max())
        , normalIndex(std::numeric_limits<unsigned int>::max())
    {}

    FaceIndices(unsigned int posIndex, unsigned int tcIndex, unsigned int nIndex)
        : positionIndex(posIndex)
        , texCoordIndex(tcIndex)
        , normalIndex(nIndex)
    {}

    bool operator == (const FaceIndices &other) const
    {
        return positionIndex == other.positionIndex &&
               texCoordIndex == other.texCoordIndex &&
               normalIndex == other.normalIndex;
    }

    unsigned int positionIndex;
    unsigned int texCoordIndex;
    unsigned int normalIndex;
};

class ObjLoader
{
public:
    ObjLoader();

    void setLoadTextureCoordinatesEnabled( bool b ) { m_loadTextureCoords = b; }
    bool isLoadTextureCoordinatesEnabled() const { return m_loadTextureCoords; }

    void setTangentGenerationEnabled( bool b ) { m_generateTangents = b; }
    bool isTangentGenerationEnabled() const { return m_generateTangents; }

    void setMeshCenteringEnabled( bool b ) { m_centerMesh = b; }
    bool isMeshCenteringEnabled() const { return m_centerMesh; }

    bool hasNormals() const { return !m_normals.isEmpty(); }
    bool hasTextureCoordinates() const { return !m_texCoords.isEmpty(); }
    bool hasTangents() const { return !m_tangents.isEmpty(); }

    bool load( const QString& fileName );
    bool load( ::QIODevice* ioDev );

    QVector<QVector3D> vertices() const { return m_points; }
    QVector<QVector3D> normals() const { return m_normals; }
    QVector<QVector2D> textureCoordinates() const { return m_texCoords; }
    QVector<QVector4D> tangents() const { return m_tangents; }
    QVector<unsigned int> indices() const { return m_indices; }

    QGeometry *geometry() const;

private:
    void updateIndices(const QVector<QVector3D> &positions,
                       const QVector<QVector3D> &normals,
                       const QVector<QVector2D> &texCoords,
                       const QHash<FaceIndices, unsigned int> &faceIndexMap,
                       const QVector<FaceIndices> &faceIndexVector);
    void generateAveragedNormals( const QVector<QVector3D>& points,
                                  QVector<QVector3D>& normals,
                                  const QVector<unsigned int>& faces ) const;
    void generateTangents( const QVector<QVector3D>& points,
                           const QVector<QVector3D>& normals,
                           const QVector<unsigned int>& faces,
                           const QVector<QVector2D>& texCoords,
                           QVector<QVector4D>& tangents ) const;
    void center( QVector<QVector3D>& points );

    bool m_loadTextureCoords;
    bool m_generateTangents;
    bool m_centerMesh;

    QVector<QVector3D> m_points;
    QVector<QVector3D> m_normals;
    QVector<QVector2D> m_texCoords;
    QVector<QVector4D> m_tangents;
    QVector<unsigned int> m_indices;
};

}

QT_END_NAMESPACE

#endif // OBJLOADER_H
