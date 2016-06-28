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

#ifndef QT3D_RENDER_QUICK_QUICK3DRENDERTARGETSELECTOR_H
#define QT3D_RENDER_QUICK_QUICK3DRENDERTARGETSELECTOR_H

#include <Qt3DQuickRenderer/qt3dquickrenderer_global.h>
#include <Qt3DRenderer/qrenderattachment.h>
#include <Qt3DRenderer/qrendertargetselector.h>
#include <QQmlListProperty>
#include <QVariantList>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

class QT3DQUICKRENDERERSHARED_EXPORT Quick3DRenderTargetSelector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList drawBuffers READ drawBuffers WRITE setDrawBuffers NOTIFY drawBuffersChanged)
public:
    explicit Quick3DRenderTargetSelector(QObject *parent = 0);
    ~Quick3DRenderTargetSelector();

    inline QRenderTargetSelector *parentRenderTargetSelector() const { return qobject_cast<QRenderTargetSelector *>(parent()); }

    QVariantList drawBuffers() const;
    void setDrawBuffers(const QVariantList &drawBuffers);

Q_SIGNALS:
    void drawBuffersChanged();
};

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QUICK3DRENDERTARGETSELECTOR_H
