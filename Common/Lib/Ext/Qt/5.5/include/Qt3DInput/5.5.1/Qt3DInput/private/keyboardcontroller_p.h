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

#ifndef QT3D_INPUT_KEYBOARDCONTROLLER_P_H
#define QT3D_INPUT_KEYBOARDCONTROLLER_P_H

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DInput/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

class InputHandler;

class KeyboardController : public QBackendNode
{
public:
    KeyboardController();
    void updateFromPeer(QNode *peer) Q_DECL_OVERRIDE;
    void requestFocusForInput(const QNodeId &inputId);
    void setInputHandler(InputHandler *handler);

    void addKeyboardInput(const QNodeId &input);
    void removeKeyboardInput(const QNodeId &input);

    void setCurrentFocusItem(const QNodeId &input);

    inline QNodeId currentFocusItem() const { return m_currentFocusItem; }
    inline QNodeId lastKeyboardInputRequester() const { return m_lastRequester; }
    inline QVector<QNodeId> keyboardInputs() const { return m_keyboardInputs; }
    inline QVector<HKeyboardInput> keyboardInputsHandles() const { return m_keyboardInputHandles; }

protected:
    void sceneChangeEvent(const QSceneChangePtr &) Q_DECL_OVERRIDE;

private:
    InputHandler *m_inputHandler;
    QVector<QNodeId> m_keyboardInputs;
    QVector<HKeyboardInput> m_keyboardInputHandles;
    QNodeId m_lastRequester;
    QNodeId m_currentFocusItem;
};

class KeyboardControllerFunctor : public QBackendNodeFunctor
{
public:
    explicit KeyboardControllerFunctor(InputHandler *handler);

    QBackendNode *create(QNode *frontend, const QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    QBackendNode *get(const QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const QNodeId &id) const Q_DECL_OVERRIDE;

private:
    InputHandler *m_handler;
};

} // Input

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_INPUT_KEYBOARDCONTROLLER_P_H
