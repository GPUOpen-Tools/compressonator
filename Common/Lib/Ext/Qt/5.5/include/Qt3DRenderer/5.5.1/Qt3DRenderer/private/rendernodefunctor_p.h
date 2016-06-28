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

#ifndef QT3D_RENDER_RENDERNODEFUNCTOR_P_H
#define QT3D_RENDER_RENDERNODEFUNCTOR_P_H

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

template<class Backend, class Manager>
class RenderNodeFunctor : public QBackendNodeFunctor
{
public:
    explicit RenderNodeFunctor(Manager *manager)
        : m_manager(manager)
    {
    }

    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_FINAL
    {
        Backend *backend = m_manager->getOrCreateResource(frontend->id());
        backend->setFactory(factory);
        backend->setPeer(frontend);
        return backend;
    }

    QBackendNode *get(const QNodeId &id) const Q_DECL_FINAL
    {
        return m_manager->lookupResource(id);
    }

    void destroy(const QNodeId &id) const Q_DECL_FINAL
    {
        m_manager->releaseResource(id);
    }

private:
    Manager *m_manager;
};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_RENDERNODEFUNCTOR_P_H
