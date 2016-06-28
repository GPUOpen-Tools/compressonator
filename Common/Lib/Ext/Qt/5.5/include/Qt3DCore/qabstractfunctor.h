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

#ifndef QT3D_QABSTRACTFUNCTOR_H
#define QT3D_QABSTRACTFUNCTOR_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

// This will generate a unique id() function per type
// <=> 1 unique function address per type
template<class T>
struct FunctorType
{
    static qintptr id()
    {
        // The MSVC linker can under some cases optimize all the template
        // functions into a single function. The code below is there to ensure
        // that the linker won't collapse all these distincts functions into one
        static T *t = 0;
        return reinterpret_cast<qintptr>(t);
    }
};

template<class T>
qintptr functorTypeId()
{
    return reinterpret_cast<qintptr>(&FunctorType<T>::id);
}

#define QT3D_FUNCTOR(Class)                     \
   qintptr id() const Q_DECL_OVERRIDE {         \
        return Qt3D::functorTypeId<Class>();    \
   }


class QT3DCORESHARED_EXPORT QAbstractFunctor
{
public:
    virtual ~QAbstractFunctor();
    virtual qintptr id() const = 0;

    template<class T>
    const T *functor_cast(const QAbstractFunctor *other) const
    {
        if (other->id() == functorTypeId<T>())
            return static_cast<const T *>(other);
        return Q_NULLPTR;
    }
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTFUNCTOR_H
