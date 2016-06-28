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

#ifndef QT3D_QHANDLE_H
#define QT3D_QHANDLE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/QDebug>

QT_BEGIN_NAMESPACE

namespace Qt3D {

template <typename T, uint INDEXBITS>
class QHandleManager;

template <typename T, uint INDEXBITS = 16>
class QHandle
{
public:
    QHandle()
        : m_handle(0)
    {}


    quint32 index() const { return d.m_index; }
    quint32 counter() const { return d.m_counter; }
    quint32 handle() const { return m_handle; }
    bool isNull() const { return !m_handle; }

    operator quint32() const { return m_handle; }

    static quint32 maxIndex() { return MaxIndex; }
    static quint32 maxCounter() { return MaxCounter; }


private:
    enum {
        // Sizes to use for bit fields
        IndexBits = INDEXBITS,
        CounterBits = 32 - INDEXBITS - 2, // We use 2 bits for book-keeping in QHandleManager

        // Sizes to compare against for asserting dereferences
        MaxIndex = (1 << IndexBits) - 1,
        MaxCounter = (1 << CounterBits) - 1
    };

    QHandle(quint32 i, quint32 count)
    {
        d.m_index = i;
        d.m_counter = count;
        d.m_unused = 0;
        Q_ASSERT(i < MaxIndex);
        Q_ASSERT(count < MaxCounter);
    }


    friend class QHandleManager<T, INDEXBITS>;

    struct Data {
        quint32 m_index : IndexBits;
        quint32 m_counter : CounterBits;
        quint32 m_unused : 2;
    };
    union {
        Data d;
        quint32 m_handle;
    };
};

template <typename T, uint INDEXBITS>
QDebug operator<<(QDebug dbg, const QHandle<T, INDEXBITS> &h)
{
    QDebugStateSaver saver(dbg);
    QString binNumber = QString::number(h.handle(), 2).rightJustified(32, QChar::fromLatin1('0'));
    dbg.nospace() << "index = " << h.index()
                  << " magic/counter = " << h.counter()
                  << " m_handle = " << h.handle()
                  << " = " << binNumber;
    return dbg;
}

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QRHANDLE_H
