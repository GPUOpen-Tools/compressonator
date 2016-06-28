/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTSTRING_P_H
#define QSCRIPTSTRING_P_H

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

#include <QtCore/qobjectdefs.h>
#include <QtCore/qshareddata.h>

#include "Identifier.h"

QT_BEGIN_NAMESPACE

class QScriptEnginePrivate;
class QScriptStringPrivate : public QSharedData
{
public:
    enum AllocationType {
        StackAllocated,
        HeapAllocated
    };

    inline QScriptStringPrivate(QScriptEnginePrivate *engine, const JSC::Identifier &id,
                                AllocationType type);
    inline ~QScriptStringPrivate();
    static inline void init(QScriptString &q, QScriptStringPrivate *d);

    static inline QScriptStringPrivate *get(const QScriptString &q);

    inline void detachFromEngine();

    static inline bool isValid(const QScriptString &q);

    QScriptEnginePrivate *engine;
    JSC::Identifier identifier;
    AllocationType type;

    // linked list of engine's script values
    QScriptStringPrivate *prev;
    QScriptStringPrivate *next;
};

inline QScriptStringPrivate::QScriptStringPrivate(QScriptEnginePrivate *e, const JSC::Identifier &id,
                                                  AllocationType tp)
    : engine(e), identifier(id), type(tp), prev(0), next(0)
{
}

inline QScriptStringPrivate::~QScriptStringPrivate()
{
}

inline void QScriptStringPrivate::init(QScriptString &q, QScriptStringPrivate *d)
{
    q.d_ptr = d;
}

inline QScriptStringPrivate *QScriptStringPrivate::get(const QScriptString &q)
{
    return const_cast<QScriptStringPrivate*>(q.d_func());
}

inline void QScriptStringPrivate::detachFromEngine()
{
    engine = 0;
    identifier = JSC::Identifier();
}

inline bool QScriptStringPrivate::isValid(const QScriptString &q)
{
    return (q.d_ptr && q.d_ptr->engine);
}

QT_END_NAMESPACE

#endif
