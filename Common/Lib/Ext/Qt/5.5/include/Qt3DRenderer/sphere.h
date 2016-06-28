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

#ifndef SPHERE_H
#define SPHERE_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DCore/qboundingsphere.h>

#include <QMatrix4x4>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QT3DRENDERERSHARED_EXPORT Sphere : public QBoundingSphere
{
public:
    inline Sphere(const QNodeId &i = QNodeId())
        : m_center()
        , m_radius(0.0f)
        , m_id(i)
    {}

    inline Sphere(const QVector3D &c, float r, const QNodeId &i = QNodeId())
        : m_center(c)
        , m_radius(r)
        , m_id(i)
    {}

    void setCenter(const QVector3D &c);
    QVector3D center() const;

    inline bool isNull() { return m_center == QVector3D() && m_radius == 0.0f; }

    void setRadius(float r);
    float radius() const;

    void clear();
    void initializeFromPoints(const QVector<QVector3D> &points);
    void expandToContain(const QVector3D &point);
    inline void expandToContain(const QVector<QVector3D> &points)
    {
        Q_FOREACH (const QVector3D &p, points)
            expandToContain(p);
    }

    void expandToContain(const Sphere &sphere);

    Sphere transformed(const QMatrix4x4 &mat);
    inline Sphere &transform(const QMatrix4x4 &mat)
    {
        *this = transformed(mat);
        return *this;
    }

    QNodeId id() const Q_DECL_FINAL;
    bool intersects(const QRay3D &ray, QVector3D *q) const Q_DECL_FINAL;
    Type type() const Q_DECL_FINAL;

    static Sphere fromPoints(const QVector<QVector3D> &points);

private:
    QVector3D m_center;
    float m_radius;
    QNodeId m_id;

    static const float ms_epsilon;
};

inline void Sphere::setCenter(const QVector3D &c)
{
    m_center = c;
}

inline QVector3D Sphere::center() const
{
    return m_center;
}

inline void Sphere::setRadius(float r)
{
    m_radius = r;
}

inline float Sphere::radius() const
{
    return m_radius;
}

inline void Sphere::clear()
{
    m_center = QVector3D();
    m_radius = 0.0f;
}

inline bool intersects(const Sphere &a, const Sphere &b)
{
    // Calculate squared distance between sphere centers
    const QVector3D d = a.center() - b.center();
    const float distSq = QVector3D::dotProduct(d, d);

    // Spheres intersect if squared distance is less than squared
    // sum of radii
    const float sumRadii = a.radius() + b.radius();
    return distSq <= sumRadii * sumRadii;
}

}

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3D::Sphere)

#endif // SPHERE_H
