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

#ifndef QT3D_RENDER_QUICK_QUICK3DEFFECT_H
#define QT3D_RENDER_QUICK_QUICK3DEFFECT_H

#include <Qt3DQuickRenderer/qt3dquickrenderer_global.h>
#include <QQmlListProperty>
#include <Qt3DRenderer/qeffect.h>
#include <Qt3DRenderer/qtechnique.h>


QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace Quick {

class QT3DQUICKRENDERERSHARED_EXPORT Quick3DEffect : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3D::QTechnique> techniques READ techniqueList)
    Q_PROPERTY(QQmlListProperty<Qt3D::QParameter> parameters READ parameterList)

public:
    explicit Quick3DEffect(QObject *parent = 0);

    inline QEffect *parentEffect() const { return qobject_cast<QEffect*>(parent()); }

    QQmlListProperty<Qt3D::QTechnique> techniqueList();
    QQmlListProperty<Qt3D::QParameter> parameterList();

private:
    static void appendTechnique(QQmlListProperty<QTechnique> *list, QTechnique *bar);
    static QTechnique *techniqueAt(QQmlListProperty<QTechnique> *list, int index);
    static int techniqueCount(QQmlListProperty<QTechnique> *list);
    static void clearTechniqueList(QQmlListProperty<QTechnique> *list);

    static void appendParameter(QQmlListProperty<QParameter> *list, QParameter *param);
    static QParameter *parameterAt(QQmlListProperty<QParameter> *list, int index);
    static int parametersCount(QQmlListProperty<QParameter> *list);
    static void clearParameterList(QQmlListProperty<QParameter> *list);
};

} // Quick

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QUICK_QUICK3DEFFECT_H
