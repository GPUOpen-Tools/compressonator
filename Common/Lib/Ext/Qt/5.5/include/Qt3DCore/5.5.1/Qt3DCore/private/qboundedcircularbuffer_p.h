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

#ifndef QT3D_QBOUNDEDCIRCULARBUFFER_P_H
#define QT3D_QBOUNDEDCIRCULARBUFFER_P_H

#include <Qt3DCore/private/qcircularbuffer_p.h>

#include <QReadWriteLock>
#include <QReadLocker>
#include <QSemaphore>
#include <QtGlobal>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template <class T>
class QBoundedCircularBuffer
{
public:
    explicit QBoundedCircularBuffer(int capacity_)
         : m_free( capacity_ ),
           m_used( capacity_ ),
           m_buffer( capacity_ )
    {
        m_used.acquire(capacity_);
    }

    int capacity() const { return m_buffer.capacity(); }
    int freeSize() const { return m_free.available(); }
    int size() const { return m_used.available(); }
    bool isFull() const { return m_free.available() == 0; }
    bool isEmpty() const { return m_used.available() == 0; }

    void push_back(const T &value)
    {
        m_free.acquire();
        QWriteLocker locker(&m_lock);
        m_buffer.append(value);
        locker.unlock();
        m_used.release();
    }

    void push(const T &t) { push_back(t); }
    void append(const T &t) { push_back(t); }

    T pop_front()
    {
        m_used.acquire();
        QWriteLocker locker(&m_lock);
        T value = m_buffer.first();
        m_buffer.remove(0);
        locker.unlock();
        m_free.release();
        return value;
    }

    T pop() { return pop_front(); }
    T takeFirst() { return pop_front(); }

    void clear()
    {
        QWriteLocker locker(&m_lock);
        m_buffer.clear();
        int used = m_used.available();
        m_free.release(used);
        m_used.acquire(used);
    }

    T at(int i) const
    {
        QReadLocker readLocker(&m_lock);
        return m_buffer.at(i);
    }

    T front() const
    {
        QReadLocker readLocker(&m_lock);
        return m_buffer.front();
    }

    T back() const
    {
        QReadLocker readLocker(&m_lock);
        return m_buffer.back();
    }

protected:
    Q_DISABLE_COPY(QBoundedCircularBuffer)

    QSemaphore m_free;
    QSemaphore m_used;
    QCircularBuffer<T> m_buffer;
    mutable QReadWriteLock m_lock;
};

} //Qt3D

QT_END_NAMESPACE

#endif // QT3D_QBOUNDEDCIRCULARBUFFER_P_H
