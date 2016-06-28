/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef QT3D_RENDER_STATE_IMPLS_H
#define QT3D_RENDER_STATE_IMPLS_H

#include <QList>

#include <Qt3DRenderer/private/renderstate_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Render {

template <typename Derived, typename T>
class GenericState1 : public RenderState
{
public:

    bool isEqual(const Derived& i) const
    { return (m_1 == i.m_1); }


protected:
    GenericState1(T t) :
        m_1(t)
    {}

    T m_1;

};

template <typename Derived, typename T, typename S>
class GenericState2 : public RenderState
{
public:
    bool isEqual(const Derived& i) const
    { return (m_1 == i.m_1) && (m_2 == i.m_2); }
protected:
    GenericState2(T t, S s) :
        m_1(t),
        m_2(s)
    {}


    T m_1;
    S m_2;
};

template <typename Derived, typename T, typename S, typename U>
class GenericState3 : public RenderState
{
public:
    bool isEqual(const Derived& i) const
    { return (m_1 == i.m_1) && (m_2 == i.m_2) && (m_3 == i.m_3); }

protected:
    GenericState3(T t, S s, U u) :
        m_1(t),
        m_2(s),
        m_3(u)
    {}

    T m_1;
    S m_2;
    U m_3;
};

template <typename Derived, typename T, typename S, typename U, typename Z>
class GenericState4 : public RenderState
{
public:
    bool isEqual(const Derived& i) const
    { return (m_1 == i.m_1) && (m_2 == i.m_2) && (m_3 == i.m_3) && (m_4 == i.m_4); }

protected:
    GenericState4(T t, S s, U u, Z z) :
        m_1(t),
        m_2(s),
        m_3(u),
        m_4(z)
    {}

    T m_1;
    S m_2;
    U m_3;
    Z m_4;
};

template <typename Derived, typename T, typename S, typename U, typename V, typename W, typename Z>
class GenericState6 : public RenderState
{
public:
    bool isEqual(const Derived& i) const
    { return (m_1 == i.m_1) && (m_2 == i.m_2) && (m_3 == i.m_3) && (m_4 == i.m_4) && (m_5 == i.m_5) && (m_6 == i.m_6); }

protected:
    GenericState6(T t, S s, U u, V v, W w, Z z)
        : m_1(t)
        , m_2(s)
        , m_3(u)
        , m_4(v)
        , m_5(w)
        , m_6(z)
    {}

    T m_1;
    S m_2;
    U m_3;
    V m_4;
    W m_5;
    Z m_6;
};

} // Render
} // Qt3D of namespace

QT_END_NAMESPACE

#endif // STATE_IMPLS_H
