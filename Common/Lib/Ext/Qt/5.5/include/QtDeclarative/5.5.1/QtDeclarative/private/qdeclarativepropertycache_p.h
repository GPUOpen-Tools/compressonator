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

#ifndef QDECLARATIVEPROPERTYCACHE_P_H
#define QDECLARATIVEPROPERTYCACHE_P_H

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
#include "private/qdeclarativenotifier_p.h"

#include <QtCore/qvector.h>

#include <QtScript/private/qscriptdeclarativeclass_p.h>
QT_BEGIN_NAMESPACE

class QDeclarativeEngine;
class QMetaProperty;

class Q_AUTOTEST_EXPORT QDeclarativePropertyCache : public QDeclarativeRefCount, public QDeclarativeCleanup
{
public:
    QDeclarativePropertyCache(QDeclarativeEngine *);
    QDeclarativePropertyCache(QDeclarativeEngine *, const QMetaObject *);
    virtual ~QDeclarativePropertyCache();

    struct Data {
        inline Data();
        inline bool operator==(const Data &);

        enum Flag {
                    NoFlags           = 0x00000000,

                    // Can apply to all properties, except IsFunction
                    IsConstant        = 0x00000001,
                    IsWritable        = 0x00000002,
                    IsResettable      = 0x00000004,
                    IsAlias           = 0x00000008,

                    // These are mutualy exclusive
                    IsFunction        = 0x00000010,
                    IsQObjectDerived  = 0x00000020,
                    IsEnumType        = 0x00000040,
                    IsQList           = 0x00000080,
                    IsQmlBinding      = 0x00000100,
                    IsQScriptValue    = 0x00000200,

                    // Apply only to IsFunctions
                    IsVMEFunction     = 0x00000400,
                    HasArguments      = 0x00000800,
                    IsSignal          = 0x00001000,
                    IsVMESignal       = 0x00002000
        };
        Q_DECLARE_FLAGS(Flags, Flag)

        bool isValid() const { return coreIndex != -1; }

        Flags flags;
        int propType;
        int coreIndex;
        union {
            int notifyIndex; // When !IsFunction
            int relatedIndex; // When IsFunction
        };
        uint overrideIndexIsProperty : 1;
        signed int overrideIndex : 31;
        int revision;
        int metaObjectOffset;

        static Flags flagsForProperty(const QMetaProperty &, QDeclarativeEngine *engine = 0);
        void load(const QMetaProperty &, QDeclarativeEngine *engine = 0);
        void load(const QMetaMethod &);
        QString name(QObject *);
        QString name(const QMetaObject *);
    };

    struct ValueTypeData {
        inline ValueTypeData();
        inline bool operator==(const ValueTypeData &);
        Data::Flags flags;     // flags of the access property on the value type proxy object
        int valueTypeCoreIdx;  // The prop index of the access property on the value type proxy object
        int valueTypePropType; // The QVariant::Type of access property on the value type proxy object
    };

    void update(QDeclarativeEngine *, const QMetaObject *);

    QDeclarativePropertyCache *copy() const;
    void append(QDeclarativeEngine *, const QMetaObject *, Data::Flag propertyFlags = Data::NoFlags,
                Data::Flag methodFlags = Data::NoFlags, Data::Flag signalFlags = Data::NoFlags);
    void append(QDeclarativeEngine *, const QMetaObject *, int revision, Data::Flag propertyFlags = Data::NoFlags,
                Data::Flag methodFlags = Data::NoFlags, Data::Flag signalFlags = Data::NoFlags);

    static Data create(const QMetaObject *, const QString &);

    inline Data *property(const QScriptDeclarativeClass::Identifier &id) const;
    Data *property(const QString &) const;
    Data *property(int) const;
    Data *method(int) const;
    QStringList propertyNames() const;

    inline Data *overrideData(Data *) const;
    inline bool isAllowedInRevision(Data *) const;

    inline QDeclarativeEngine *qmlEngine() const;
    static Data *property(QDeclarativeEngine *, QObject *, const QScriptDeclarativeClass::Identifier &, Data &);
    static Data *property(QDeclarativeEngine *, QObject *, const QString &, Data &);

protected:
    virtual void clear();

private:
    friend class QDeclarativeEnginePrivate;

    struct RData : public Data, public QDeclarativeRefCount {
        QScriptDeclarativeClass::PersistentIdentifier identifier;
    };

    typedef QVector<RData *> IndexCache;
    typedef QHash<QString, RData *> StringCache;
    typedef QHash<QScriptDeclarativeClass::Identifier, RData *> IdentifierCache;
    typedef QVector<int> AllowedRevisionCache;

    void updateRecur(QDeclarativeEngine *, const QMetaObject *);

    QDeclarativeEngine *engine;
    IndexCache indexCache;
    IndexCache methodIndexCache;
    StringCache stringCache;
    IdentifierCache identifierCache;
    AllowedRevisionCache allowedRevisionCache;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativePropertyCache::Data::Flags);

QDeclarativePropertyCache::Data::Data()
: flags(0), propType(0), coreIndex(-1), notifyIndex(-1), overrideIndexIsProperty(false), overrideIndex(-1),
  revision(0), metaObjectOffset(-1)
{
}

bool QDeclarativePropertyCache::Data::operator==(const QDeclarativePropertyCache::Data &other)
{
    return flags == other.flags &&
           propType == other.propType &&
           coreIndex == other.coreIndex &&
           notifyIndex == other.notifyIndex &&
           revision == other.revision;
}

QDeclarativePropertyCache::Data *
QDeclarativePropertyCache::overrideData(Data *data) const
{
    if (data->overrideIndex < 0)
        return 0;

    if (data->overrideIndexIsProperty)
        return indexCache.at(data->overrideIndex);
    else
        return methodIndexCache.at(data->overrideIndex);
}

QDeclarativePropertyCache::Data *
QDeclarativePropertyCache::property(const QScriptDeclarativeClass::Identifier &id) const
{
    return identifierCache.value(id);
}

QDeclarativePropertyCache::ValueTypeData::ValueTypeData()
: flags(QDeclarativePropertyCache::Data::NoFlags), valueTypeCoreIdx(-1), valueTypePropType(0)
{
}

bool QDeclarativePropertyCache::ValueTypeData::operator==(const ValueTypeData &o)
{
    return flags == o.flags &&
           valueTypeCoreIdx == o.valueTypeCoreIdx &&
           valueTypePropType == o.valueTypePropType;
}

bool QDeclarativePropertyCache::isAllowedInRevision(Data *data) const
{
    return (data->metaObjectOffset == -1 && data->revision == 0) ||
           (allowedRevisionCache[data->metaObjectOffset] >= data->revision);
}

QDeclarativeEngine *QDeclarativePropertyCache::qmlEngine() const
{
    return engine;
}

QT_END_NAMESPACE

#endif // QDECLARATIVEPROPERTYCACHE_P_H
