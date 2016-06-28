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

#ifndef QT3D_QTECHNIQUE_H
#define QT3D_QTECHNIQUE_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DRenderer/qrenderpass.h>
#include <Qt3DRenderer/qannotation.h>
#include <QList>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QParameter;
class QTechniquePrivate;
class QOpenGLFilter;

class QT3DRENDERERSHARED_EXPORT QTechnique : public QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QOpenGLFilter *openGLFilter READ openGLFilter)

public:
    explicit QTechnique(QNode *parent = 0);
    ~QTechnique();

    void addAnnotation(QAnnotation *criterion);
    void removeAnnotation(QAnnotation *criterion);
    QList<QAnnotation *> annotations() const;

    void addParameter(QParameter *p);
    void removeParameter(QParameter *p);
    QList<QParameter *> parameters() const;

    void addPass(QRenderPass *pass);
    void removePass(QRenderPass *pass);
    QList<QRenderPass *> renderPasses() const;

    QOpenGLFilter *openGLFilter();

protected:
    QTechnique(QTechniquePrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QTechnique)
    QT3D_CLONEABLE(QTechnique)
    Q_PRIVATE_SLOT(d_func(), void _q_openGLFilterChanged())
};

}

QT_END_NAMESPACE

#endif // QT3D_QTECHNIQUE_H
