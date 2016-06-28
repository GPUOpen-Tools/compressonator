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

#ifndef QT3D_RENDER_SHADERVARIABLES_P_H
#define QT3D_RENDER_SHADERVARIABLES_P_H

#include <QtGlobal>
#include <QOpenGLContext>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

struct ShaderAttribute
{
    QString m_name;
    GLenum m_type;
    int m_size;
    int m_location;
};

struct ShaderUniform
{
    ShaderUniform()
        : m_size(0)
        , m_offset(-1)
        , m_location(-1)
        , m_blockIndex(-1)
    {}

    QString m_name;
    GLenum m_type;
    int m_size;
    int m_offset; // -1 default, >= 0 if uniform defined in uniform block
    int m_location; // -1 if uniform defined in a uniform block
    int m_blockIndex; // -1 is the default, >= 0 if uniform defined in uniform block
    int m_arrayStride; // -1 is the default, >= 0 if uniform defined in uniform block and if it's an array
    int m_matrixStride; // -1 is the default, >= 0 uniform defined in uniform block and is a matrix
    uint m_rawByteSize; // contains byte size (size / type / strides)
    // size, offset and strides are in bytes
};

struct ShaderUniformBlock
{
    ShaderUniformBlock()
        : m_index(-1)
        , m_binding(-1)
        , m_activeUniformsCount(0)
        , m_size(0)
    {}

    QString m_name;
    int m_index;
    int m_binding;
    int m_activeUniformsCount;
    int m_size;
};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_SHADERVARIABLES_P_H
