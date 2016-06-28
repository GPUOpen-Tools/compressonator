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

#ifndef QDECLARATIVEINTEGERCACHE_P_H
#define QDECLARATIVEINTEGERCACHE_P_H

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

#include "private/qdeclarativerefcount_p.h"
#include "private/qdeclarativecleanup_p.h"

#include <QtCore/qhash.h>

#include <private/qscriptdeclarativeclass_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeType;
class QDeclarativeEngine;
class QDeclarativeIntegerCache : public QDeclarativeRefCount, public QDeclarativeCleanup
{
public:
    QDeclarativeIntegerCache(QDeclarativeEngine *);
    virtual ~QDeclarativeIntegerCache();

    inline int count() const;
    void add(const QString &, int);
    int value(const QString &);
    QString findId(int value) const;
    inline int value(const QScriptDeclarativeClass::Identifier &id) const;

protected:
    virtual void clear();

private:
    struct Data : public QScriptDeclarativeClass::PersistentIdentifier {
        Data(const QScriptDeclarativeClass::PersistentIdentifier &i, int v)
        : QScriptDeclarativeClass::PersistentIdentifier(i), value(v) {}

        int value;
    };

    typedef QHash<QString, Data *> StringCache;
    typedef QHash<QScriptDeclarativeClass::Identifier, Data *> IdentifierCache;

    StringCache stringCache;
    IdentifierCache identifierCache;
    QDeclarativeEngine *engine;
};

int QDeclarativeIntegerCache::value(const QScriptDeclarativeClass::Identifier &id) const
{
    Data *d = identifierCache.value(id);
    return d?d->value:-1;
}

int QDeclarativeIntegerCache::count() const
{
    return stringCache.count();
}

QT_END_NAMESPACE

#endif // QDECLARATIVEINTEGERCACHE_P_H

