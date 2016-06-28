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

#ifndef QT3D_QFRAMEALLOCATOR_H
#define QT3D_QFRAMEALLOCATOR_H


#ifdef QFRAMEALLOCATOR_DEBUG
#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>
#endif

#include <QDebug>
#include <QScopedPointer>
#include <QVector>
#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QFrameAllocatorPrivate;

class QT3DCORESHARED_EXPORT QFrameAllocator
{
public:
    explicit QFrameAllocator(uint maxObjectSize, uint alignment = 16, uint pageSize = 128);
    ~QFrameAllocator();

    template<typename T>
    T* allocate()
    {
        void* ptr = allocateRawMemory(sizeof(T));
        new (ptr) T(); // Don't forget to call the constructor of the object using the placement new operator
        return static_cast<T*>(ptr);
    }

    template<typename T>
    void deallocate(T *ptr)
    {
        ptr->~T(); // Call destructor
        deallocateRawMemory(ptr, sizeof(T));
    }

    void* allocateRawMemory(size_t size);

    void deallocateRawMemory(void *ptr, size_t size);

    void clear();
    void trim();
    uint maxObjectSize() const;
    uint totalChunkCount() const;
    int allocatorPoolSize() const;
    bool isEmpty() const;

private:
    Q_DECLARE_PRIVATE(QFrameAllocator)
    const QScopedPointer<QFrameAllocatorPrivate> d_ptr;
};

} // Qt3D

QT_END_NAMESPACE

#endif // QFRAMEALLOCATOR_H
