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

#ifndef QT3D_RENDER_QGOOCHMATERIAL_H
#define QT3D_RENDER_QGOOCHMATERIAL_H

#include <Qt3DRenderer/qmaterial.h>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QGoochMaterialPrivate;

class QGoochMaterial : public QMaterial
{
    Q_OBJECT
    Q_PROPERTY(QColor diffuse READ diffuse WRITE setDiffuse NOTIFY diffuseChanged)
    Q_PROPERTY(QColor specular READ specular WRITE setSpecular NOTIFY specularChanged)
    Q_PROPERTY(QColor cool READ cool WRITE setCool NOTIFY coolChanged)
    Q_PROPERTY(QColor warm READ warm WRITE setWarm NOTIFY warmChanged)
    Q_PROPERTY(float alpha READ alpha WRITE setAlpha NOTIFY alphaChanged)
    Q_PROPERTY(float beta READ beta WRITE setBeta NOTIFY betaChanged)
    Q_PROPERTY(float shininess READ shininess WRITE setShininess NOTIFY shininessChanged)

public:
    explicit QGoochMaterial(QNode *parent = 0);
    QColor diffuse() const;
    QColor specular() const;
    QColor cool() const;
    QColor warm() const;
    float alpha() const;
    float beta() const;
    float shininess() const;

public Q_SLOTS:
    void setDiffuse(const QColor &diffuse);
    void setSpecular(const QColor &specular);
    void setCool(const QColor &cool);
    void setWarm(const QColor &warm);
    void setAlpha(float alpha);
    void setBeta(float beta);
    void setShininess(float shininess);

Q_SIGNALS:
    void diffuseChanged();
    void specularChanged();
    void coolChanged();
    void warmChanged();
    void alphaChanged();
    void betaChanged();
    void shininessChanged();

protected:
    QGoochMaterial(QGoochMaterialPrivate &dd, QNode *parent = 0);

private:
    Q_DECLARE_PRIVATE(QGoochMaterial)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QGOOCHMATERIAL_H
