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

#ifndef QT3D_INPUT_MOUSECONTROLLER_H
#define QT3D_INPUT_MOUSECONTROLLER_H

#include <Qt3DCore/qbackendnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

class InputHandler;

class MouseController : public QBackendNode
{
public:
    MouseController();
    ~MouseController();

    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    void setInputHandler(InputHandler *handler);

    void addMouseInput(const QNodeId &input);
    void removeMouseInput(const QNodeId &input);

    QVector<QNodeId> mouseInputs() const;

protected:
    void sceneChangeEvent(const QSceneChangePtr &e) Q_DECL_OVERRIDE;

private:
    QVector<QNodeId> m_mouseInputs;
    InputHandler *m_inputHandler;
};

class MouseControllerFunctor : public QBackendNodeFunctor
{
public:
    explicit MouseControllerFunctor(InputHandler *handler);

    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE;

private:
    InputHandler *m_handler;
};

} // Input

} // Qt3D

QT_END_NAMESPACE

#endif // MOUSECONTROLLER_H
