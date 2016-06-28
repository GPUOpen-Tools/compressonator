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

#ifndef QT3D_QAXISALIGNEDBOUNDINGBOX_H
#define QT3D_QAXISALIGNEDBOUNDINGBOX_H

#include <Qt3DCore/qt3dcore_global.h>

#include <QMatrix4x4>
#include <QVector>
#include <QVector3D>

QT_BEGIN_NAMESPACE

class QDebug;

namespace Qt3D {

class QT3DCORESHARED_EXPORT QAxisAlignedBoundingBox
{
public:
    inline QAxisAlignedBoundingBox()
        : m_center(),
          m_radii()
    {}

    inline explicit QAxisAlignedBoundingBox(const QVector<QVector3D> &points)
        : m_center(),
          m_radii()
    {
        update(points);
    }

    void clear()
    {
        m_center = QVector3D();
        m_radii = QVector3D();
    }

    void update(const QVector<QVector3D> &points);

    inline QVector3D center() const { return m_center; }
    inline QVector3D radii() const { return m_radii; }

    inline QVector3D minPoint() const { return m_center - m_radii; }
    inline QVector3D maxPoint() const { return m_center + m_radii; }

    inline float xExtent() const { return 2.0f * m_radii.x(); }
    inline float yExtent() const { return 2.0f * m_radii.y(); }
    inline float zExtent() const { return 2.0f * m_radii.z(); }

    inline float maxExtent() const { return qMax( xExtent(), qMax( yExtent(), zExtent() ) ); }
    inline float minExtent() const { return qMin( xExtent(), qMin( yExtent(), zExtent() ) ); }

    inline bool contains( const QVector3D& pt ) const
    {
        QVector3D minP(minPoint()), maxP(maxPoint());
        if ((pt.x() < minP.x()) || (pt.x() > maxP.x()) ||
            (pt.y() < minP.y()) || (pt.y() > maxP.y()) ||
            (pt.z() < minP.z()) || (pt.z() > maxP.z()) )
        {
            return false;
        }
        return true;
    }

    inline void expandToContain(const QVector3D &pt)
    {
        QVector<QVector3D> pts = QVector<QVector3D>() << pt;
        update(pts);
    }

    inline void expandToContain(const QAxisAlignedBoundingBox &other)
    {
        QVector<QVector3D> pts = QVector<QVector3D>() << other.minPoint() << other.maxPoint();
        update(pts);
    }

    inline QAxisAlignedBoundingBox transformBy(const QMatrix4x4 &mat) const
    {
        QAxisAlignedBoundingBox r;
        r.m_center = mat.map(m_center);
        r.m_radii = mat.map(m_radii);
        return r;
    }

    inline QAxisAlignedBoundingBox &transform(const QMatrix4x4 &mat)
    {
        m_center = mat.map(m_center);
        m_radii = mat.map(m_radii);
        return *this;
    }

private:
    QVector3D m_center;
    QVector3D m_radii;

    friend bool intersects(const QAxisAlignedBoundingBox &a,
                           const QAxisAlignedBoundingBox &b);
};

QT3DCORESHARED_EXPORT QDebug operator<<(QDebug dbg, const QAxisAlignedBoundingBox &c);

inline bool intersects(const QAxisAlignedBoundingBox &a,
                       const QAxisAlignedBoundingBox &b)
{
    // Test y axis last as this is often the least discriminatory in OpenGL applications
    // where worlds tend to be mostly in the xz-plane
    if (qAbs(a.m_center[0] - b.m_center[0]) > a.m_radii[0] + b.m_radii[0])
        return false;
    if (qAbs(a.m_center[2] - b.m_center[2]) > a.m_radii[2] + b.m_radii[2])
        return false;
    if (qAbs(a.m_center[1] - b.m_center[1]) > a.m_radii[1] + b.m_radii[1])
        return false;
    return true;
}

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QAXISALIGNEDBOUNDINGBOX_H
