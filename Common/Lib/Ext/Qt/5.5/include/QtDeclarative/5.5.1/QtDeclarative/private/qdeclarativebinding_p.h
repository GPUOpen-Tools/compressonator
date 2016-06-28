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

#ifndef QDECLARATIVEBINDING_P_H
#define QDECLARATIVEBINDING_P_H

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

#include "qdeclarative.h"
#include "qdeclarativepropertyvaluesource.h"
#include "qdeclarativeexpression.h"
#include "qdeclarativeproperty.h"
#include "private/qdeclarativeproperty_p.h"

#include <QtCore/QObject>
#include <QtCore/QMetaProperty>

QT_BEGIN_NAMESPACE

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeAbstractBinding
{
public:
    typedef QWeakPointer<QDeclarativeAbstractBinding> Pointer;

    QDeclarativeAbstractBinding();

    virtual void destroy();

    virtual QString expression() const;

    enum Type { PropertyBinding, ValueTypeProxy };
    virtual Type bindingType() const { return PropertyBinding; }

    QObject *object() const;
    int propertyIndex() const;

    void setEnabled(bool e) { setEnabled(e, QDeclarativePropertyPrivate::DontRemoveBinding); }
    virtual void setEnabled(bool, QDeclarativePropertyPrivate::WriteFlags) = 0;

    void update() { update(QDeclarativePropertyPrivate::DontRemoveBinding); }
    virtual void update(QDeclarativePropertyPrivate::WriteFlags) = 0;

    void addToObject(QObject *, int);
    void removeFromObject();

    static Pointer getPointer(QDeclarativeAbstractBinding *p) { return p ? p->weakPointer() : Pointer(); }

protected:
    virtual ~QDeclarativeAbstractBinding();
    void clear();

private:
    Pointer weakPointer();

    friend class QDeclarativeData;
    friend class QDeclarativeComponentPrivate;
    friend class QDeclarativeValueTypeProxyBinding;
    friend class QDeclarativePropertyPrivate;
    friend class QDeclarativeVME;
    friend class QtSharedPointer::ExternalRefCount<QDeclarativeAbstractBinding>;

    QObject *m_object;
    int m_propertyIndex;
    QDeclarativeAbstractBinding **m_mePtr;
    QDeclarativeAbstractBinding **m_prevBinding;
    QDeclarativeAbstractBinding  *m_nextBinding;
    QSharedPointer<QDeclarativeAbstractBinding> m_selfPointer;
};

class QDeclarativeValueTypeProxyBinding : public QDeclarativeAbstractBinding
{
public:
    QDeclarativeValueTypeProxyBinding(QObject *o, int coreIndex);

    virtual Type bindingType() const { return ValueTypeProxy; }

    virtual void setEnabled(bool, QDeclarativePropertyPrivate::WriteFlags);
    virtual void update(QDeclarativePropertyPrivate::WriteFlags);

    QDeclarativeAbstractBinding *binding(int propertyIndex);

    void removeBindings(quint32 mask);

protected:
    ~QDeclarativeValueTypeProxyBinding();

private:
    void recursiveEnable(QDeclarativeAbstractBinding *, QDeclarativePropertyPrivate::WriteFlags);
    void recursiveDisable(QDeclarativeAbstractBinding *);

    friend class QDeclarativeAbstractBinding;
    QDeclarativeAbstractBinding *m_bindings;
};

class QDeclarativeContext;
class QDeclarativeBindingPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeBinding : public QDeclarativeExpression, public QDeclarativeAbstractBinding
{
Q_OBJECT
public:
    enum EvaluateFlag { RequiresThisObject = 0x01 };
    Q_DECLARE_FLAGS(EvaluateFlags, EvaluateFlag)

    QDeclarativeBinding(const QString &, QObject *, QDeclarativeContext *, QObject *parent=0);
    QDeclarativeBinding(const QString &, QObject *, QDeclarativeContextData *, QObject *parent=0);
    QDeclarativeBinding(void *, QDeclarativeRefCount *, QObject *, QDeclarativeContextData *,
                        const QString &, int, QObject *parent);
    QDeclarativeBinding(const QScriptValue &, QObject *, QDeclarativeContextData *, QObject *parent=0);

    void setTarget(const QDeclarativeProperty &);
    QDeclarativeProperty property() const;

    void setEvaluateFlags(EvaluateFlags flags);
    EvaluateFlags evaluateFlags() const;

    bool enabled() const;

    // Inherited from  QDeclarativeAbstractBinding
    virtual void setEnabled(bool, QDeclarativePropertyPrivate::WriteFlags flags);
    virtual void update(QDeclarativePropertyPrivate::WriteFlags flags);
    virtual QString expression() const;

    typedef int Identifier;
    static Identifier Invalid;
    static QDeclarativeBinding *createBinding(Identifier, QObject *, QDeclarativeContext *, const QString &, int, QObject *parent=0);

public Q_SLOTS:
    void update() { update(QDeclarativePropertyPrivate::DontRemoveBinding); }

protected:
    ~QDeclarativeBinding();
    void emitValueChanged();

private:
    Q_DECLARE_PRIVATE(QDeclarativeBinding)
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeBinding::EvaluateFlags)

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeBinding*)

#endif // QDECLARATIVEBINDING_P_H
