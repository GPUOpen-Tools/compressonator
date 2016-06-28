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

#ifndef QDECLARATIVEDATA_P_H
#define QDECLARATIVEDATA_P_H

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

#include <QtScript/qscriptvalue.h>
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeGuardImpl;
class QDeclarativeCompiledData;
class QDeclarativeAbstractBinding;
class QDeclarativeContext;
class QDeclarativePropertyCache;
class QDeclarativeContextData;
class QDeclarativeNotifier;
class QDeclarativeDataExtended;
// This class is structured in such a way, that simply zero'ing it is the
// default state for elemental object allocations.  This is crucial in the
// workings of the QDeclarativeInstruction::CreateSimpleObject instruction.
// Don't change anything here without first considering that case!
class Q_AUTOTEST_EXPORT QDeclarativeData : public QAbstractDeclarativeData
{
public:
    QDeclarativeData()
        : ownedByQml1(true), ownMemory(true), ownContext(false), indestructible(true), explicitIndestructibleSet(false),
          context(0), outerContext(0), bindings(0), nextContextObject(0), prevContextObject(0), bindingBitsSize(0),
          bindingBits(0), lineNumber(0), columnNumber(0), deferredComponent(0), deferredIdx(0),
          scriptValue(0), objectDataRefCount(0), propertyCache(0), guards(0), extendedData(0) {
          init();
      }

    static inline void init() {
        QAbstractDeclarativeData::destroyed_qml1 = destroyed;
    }

    static void destroyed(QAbstractDeclarativeData *, QObject *);

    void destroyed(QObject *);

    void setImplicitDestructible() {
        if (!explicitIndestructibleSet) indestructible = false;
    }

    quint32 ownedByQml1:1; // This bit is shared with QML2's QQmlData.
    quint32 ownMemory:1;
    quint32 ownContext:1;
    quint32 indestructible:1;
    quint32 explicitIndestructibleSet:1;
    quint32 dummy:27;

    // The context that created the C++ object
    QDeclarativeContextData *context;
    // The outermost context in which this object lives
    QDeclarativeContextData *outerContext;

    QDeclarativeAbstractBinding *bindings;

    // Linked list for QDeclarativeContext::contextObjects
    QDeclarativeData *nextContextObject;
    QDeclarativeData**prevContextObject;

    int bindingBitsSize;
    quint32 *bindingBits;
    bool hasBindingBit(int) const;
    void clearBindingBit(int);
    void setBindingBit(QObject *obj, int);

    ushort lineNumber;
    ushort columnNumber;

    QDeclarativeCompiledData *deferredComponent; // Can't this be found from the context?
    unsigned int deferredIdx;

    // ### Can we make this QScriptValuePrivate so we incur no additional allocation
    // cost?
    QScriptValue *scriptValue;
    quint32 objectDataRefCount;
    QDeclarativePropertyCache *propertyCache;

    QDeclarativeGuardImpl *guards;

    static QDeclarativeData *get(const QObject *object, bool create = false) {
        QObjectPrivate *priv = QObjectPrivate::get(const_cast<QObject *>(object));
        if (priv->wasDeleted) {
            Q_ASSERT(!create);
            return 0;
        } else if (priv->declarativeData) {
            return static_cast<QDeclarativeData *>(priv->declarativeData);
        } else if (create) {
            priv->declarativeData = new QDeclarativeData;
            return static_cast<QDeclarativeData *>(priv->declarativeData);
        } else {
            return 0;
        }
    }

    bool hasExtendedData() const { return extendedData != 0; }
    QHash<int, QObject *> *attachedProperties() const;

private:
    // For objectNameNotifier and attachedProperties
    mutable QDeclarativeDataExtended *extendedData;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEDATA_P_H
