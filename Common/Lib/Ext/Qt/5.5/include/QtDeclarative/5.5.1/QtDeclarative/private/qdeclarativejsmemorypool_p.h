/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
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
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVEJSMEMORYPOOL_P_H
#define QDECLARATIVEJSMEMORYPOOL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "private/qdeclarativejsglobal_p.h"

#include <QtCore/qglobal.h>
#include <QtCore/qshareddata.h>

#include <string.h>

QT_QML_BEGIN_NAMESPACE

namespace QDeclarativeJS {

class QML_PARSER_EXPORT MemoryPool : public QSharedData
{
public:
    enum { maxBlockCount = -1 };
    enum { defaultBlockSize = 1 << 12 };

    MemoryPool() {
        m_blockIndex = maxBlockCount;
        m_currentIndex = 0;
        m_storage = 0;
        m_currentBlock = 0;
        m_currentBlockSize = 0;
    }

    virtual ~MemoryPool() {
        for (int index = 0; index < m_blockIndex + 1; ++index)
            free(m_storage[index]);

        free(m_storage);
    }

    char *allocate(int bytes) {
        bytes += (8 - bytes) & 7; // ensure multiple of 8 bytes (maintain alignment)
        if (m_currentBlock == 0 || m_currentBlockSize < m_currentIndex + bytes) {
            ++m_blockIndex;
            m_currentBlockSize = defaultBlockSize << m_blockIndex;

            m_storage = reinterpret_cast<char**>(realloc(m_storage, sizeof(char*) * (1 + m_blockIndex)));
            m_currentBlock = m_storage[m_blockIndex] = reinterpret_cast<char*>(malloc(m_currentBlockSize));
            memset(m_currentBlock, 0, m_currentBlockSize);

            m_currentIndex = (8 - quintptr(m_currentBlock)) & 7; // ensure first chunk is 64-bit aligned
            Q_ASSERT(m_currentIndex + bytes <= m_currentBlockSize);
        }

        char *p = reinterpret_cast<char *>
            (m_currentBlock + m_currentIndex);

        m_currentIndex += bytes;

        return p;
    }

    int bytesAllocated() const {
        int bytes = 0;
        for (int index = 0; index < m_blockIndex; ++index)
            bytes += (defaultBlockSize << index);
        bytes += m_currentIndex;
        return bytes;
    }

private:
    int m_blockIndex;
    int m_currentIndex;
    char *m_currentBlock;
    int m_currentBlockSize;
    char **m_storage;

private:
    Q_DISABLE_COPY(MemoryPool)
};

} // namespace QDeclarativeJS

QT_QML_END_NAMESPACE

#endif
