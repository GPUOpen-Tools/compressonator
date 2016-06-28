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

#ifndef QT3D_INPUT_INPUTMANAGERS_P_H
#define QT3D_INPUT_INPUTMANAGERS_P_H

#include <QtGlobal>
#include <Qt3DInput/private/handle_types_p.h>
#include <Qt3DInput/private/keyboardcontroller_p.h>
#include <Qt3DInput/private/keyboardinput_p.h>
#include <Qt3DInput/private/mousecontroller_p.h>
#include <Qt3DInput/private/mouseinput_p.h>
#include <Qt3DCore/private/qresourcemanager_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Input {

class KeyboardInputManager : public QResourceManager<
        KeyboardInput,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    KeyboardInputManager() {}
};

class KeyboardControllerManager : public QResourceManager<
        KeyboardController,
        QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    KeyboardControllerManager() {}
};

class MouseControllerManager : public QResourceManager<
        MouseController,
        QNodeId,
        8,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    MouseControllerManager() {}
};

class MouseInputManager : public QResourceManager<
        MouseInput,
        QNodeId,
        16,
        Qt3D::ArrayAllocatingPolicy>
{
public:
    MouseInputManager() {}
};

} // Input

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_INPUT_INPUTMANAGERS_P_H
