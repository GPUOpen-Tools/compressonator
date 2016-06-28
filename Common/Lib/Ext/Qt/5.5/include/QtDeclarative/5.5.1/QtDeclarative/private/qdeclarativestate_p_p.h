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

#ifndef QDECLARATIVESTATE_P_H
#define QDECLARATIVESTATE_P_H

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

#include "private/qdeclarativestate_p.h"

#include "private/qdeclarativeanimation_p_p.h"
#include "private/qdeclarativetransitionmanager_p_p.h"

#include <private/qdeclarativeproperty_p.h>
#include <private/qdeclarativeguard_p.h>

#include <private/qdeclarativebinding_p.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeSimpleAction
{
public:
    enum State { StartState, EndState };
    QDeclarativeSimpleAction(const QDeclarativeAction &a, State state = StartState)
    {
        m_property = a.property;
        m_specifiedObject = a.specifiedObject;
        m_specifiedProperty = a.specifiedProperty;
        m_event = a.event;
        if (state == StartState) {
            m_value = a.fromValue;
            if (QDeclarativePropertyPrivate::binding(m_property)) {
                m_binding = QDeclarativeAbstractBinding::getPointer(QDeclarativePropertyPrivate::binding(m_property));
            }
            m_reverseEvent = true;
        } else {
            m_value = a.toValue;
            m_binding = a.toBinding;
            m_reverseEvent = false;
        }
    }

    ~QDeclarativeSimpleAction()
    {
    }

    QDeclarativeSimpleAction(const QDeclarativeSimpleAction &other)
        :  m_property(other.m_property),
        m_value(other.m_value),
        m_binding(QDeclarativeAbstractBinding::getPointer(other.binding())),
        m_specifiedObject(other.m_specifiedObject),
        m_specifiedProperty(other.m_specifiedProperty),
        m_event(other.m_event),
        m_reverseEvent(other.m_reverseEvent)
    {
    }

    QDeclarativeSimpleAction &operator =(const QDeclarativeSimpleAction &other)
    {
        m_property = other.m_property;
        m_value = other.m_value;
        m_binding = QDeclarativeAbstractBinding::getPointer(other.binding());
        m_specifiedObject = other.m_specifiedObject;
        m_specifiedProperty = other.m_specifiedProperty;
        m_event = other.m_event;
        m_reverseEvent = other.m_reverseEvent;

        return *this;
    }

    void setProperty(const QDeclarativeProperty &property)
    {
        m_property = property;
    }

    const QDeclarativeProperty &property() const
    {
        return m_property;
    }

    void setValue(const QVariant &value)
    {
        m_value = value;
    }

    const QVariant &value() const
    {
        return m_value;
    }

    void setBinding(QDeclarativeAbstractBinding *binding)
    {
        m_binding = QDeclarativeAbstractBinding::getPointer(binding);
    }

    QDeclarativeAbstractBinding *binding() const
    {
        return m_binding.data();
    }

    QObject *specifiedObject() const
    {
        return m_specifiedObject;
    }

    const QString &specifiedProperty() const
    {
        return m_specifiedProperty;
    }

    QDeclarativeActionEvent *event() const
    {
        return m_event;
    }

    bool reverseEvent() const
    {
        return m_reverseEvent;
    }

private:
    QDeclarativeProperty m_property;
    QVariant m_value;
    QDeclarativeAbstractBinding::Pointer m_binding;
    QObject *m_specifiedObject;
    QString m_specifiedProperty;
    QDeclarativeActionEvent *m_event;
    bool m_reverseEvent;
};

class QDeclarativeStateOperationPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeStateOperation)

public:

    QDeclarativeStateOperationPrivate()
    : m_state(0) {}

    QDeclarativeState *m_state;
};

class QDeclarativeStatePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeState)

public:
    QDeclarativeStatePrivate()
    : when(0), named(false), inState(false), group(0) {}

    typedef QList<QDeclarativeSimpleAction> SimpleActionList;

    QString name;
    QDeclarativeBinding *when;
    bool named;

    struct OperationGuard : public QDeclarativeGuard<QDeclarativeStateOperation>
    {
        OperationGuard(QObject *obj, QList<OperationGuard> *l) : list(l) {
            setObject(static_cast<QDeclarativeStateOperation *>(obj));
        }
        QList<OperationGuard> *list;
        void objectDestroyed(QDeclarativeStateOperation *) {
            // we assume priv will always be destroyed after objectDestroyed calls
            list->removeOne(*this);
        }
    };
    QList<OperationGuard> operations;

    static void operations_append(QDeclarativeListProperty<QDeclarativeStateOperation> *prop, QDeclarativeStateOperation *op) {
        QList<OperationGuard> *list = static_cast<QList<OperationGuard> *>(prop->data);
        op->setState(qobject_cast<QDeclarativeState*>(prop->object));
        list->append(OperationGuard(op, list));
    }
    static void operations_clear(QDeclarativeListProperty<QDeclarativeStateOperation> *prop) {
        QList<OperationGuard> *list = static_cast<QList<OperationGuard> *>(prop->data);
        QMutableListIterator<OperationGuard> listIterator(*list);
        while(listIterator.hasNext())
            listIterator.next()->setState(0);
        list->clear();
    }
    static int operations_count(QDeclarativeListProperty<QDeclarativeStateOperation> *prop) {
        QList<OperationGuard> *list = static_cast<QList<OperationGuard> *>(prop->data);
        return list->count();
    }
    static QDeclarativeStateOperation *operations_at(QDeclarativeListProperty<QDeclarativeStateOperation> *prop, int index) {
        QList<OperationGuard> *list = static_cast<QList<OperationGuard> *>(prop->data);
        return list->at(index);
    }

    QDeclarativeTransitionManager transitionManager;

    SimpleActionList revertList;
    QList<QDeclarativeProperty> reverting;
    QString extends;
    mutable bool inState;
    QDeclarativeStateGroup *group;

    QDeclarativeStateOperation::ActionList generateActionList(QDeclarativeStateGroup *) const;
    void complete();
};

QT_END_NAMESPACE

#endif // QDECLARATIVESTATE_P_H
