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

#ifndef QT3D_RENDER_QFORWARDRENDERER_H
#define QT3D_RENDER_QFORWARDRENDERER_H

#include <Qt3DRenderer/qtechniquefilter.h>
#include <QRectF>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QForwardRendererPrivate;

class QT3DRENDERERSHARED_EXPORT QForwardRenderer : public QTechniqueFilter
{
    Q_OBJECT
    Q_PROPERTY(QRectF viewportRect READ viewportRect WRITE setViewportRect NOTIFY viewportRectChanged)
    Q_PROPERTY(QColor clearColor READ clearColor WRITE setClearColor NOTIFY clearColorChanged)
    Q_PROPERTY(QEntity *camera READ camera WRITE setCamera NOTIFY cameraChanged)
public:
    explicit QForwardRenderer(QNode *parent = 0);
    ~QForwardRenderer();

    void setViewportRect(const QRectF &viewportRect);
    void setClearColor(const QColor &clearColor);
    void setCamera(QEntity *camera);

    QRectF viewportRect() const;
    QColor clearColor() const;
    QEntity *camera() const;

Q_SIGNALS:
    void viewportRectChanged();
    void clearColorChanged();
    void cameraChanged();

private:
    Q_DECLARE_PRIVATE(QForwardRenderer)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDERER_QFORWARDRENDERER_H
