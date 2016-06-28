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

#ifndef QDECLARATIVETRANSITION_H
#define QDECLARATIVETRANSITION_H

#include "private/qdeclarativestate_p.h"

#include <qdeclarative.h>

#include <QtCore/qobject.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeAbstractAnimation;
class QDeclarativeTransitionPrivate;
class QDeclarativeTransitionManager;
class Q_DECLARATIVE_EXPORT QDeclarativeTransition : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeTransition)

    Q_PROPERTY(QString from READ fromState WRITE setFromState NOTIFY fromChanged)
    Q_PROPERTY(QString to READ toState WRITE setToState NOTIFY toChanged)
    Q_PROPERTY(bool reversible READ reversible WRITE setReversible NOTIFY reversibleChanged)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeAbstractAnimation> animations READ animations)
    Q_CLASSINFO("DefaultProperty", "animations")
    Q_CLASSINFO("DeferredPropertyNames", "animations")

public:
    QDeclarativeTransition(QObject *parent=0);
    ~QDeclarativeTransition();

    QString fromState() const;
    void setFromState(const QString &);

    QString toState() const;
    void setToState(const QString &);

    bool reversible() const;
    void setReversible(bool);

    QDeclarativeListProperty<QDeclarativeAbstractAnimation> animations();

    void prepare(QDeclarativeStateOperation::ActionList &actions,
                 QList<QDeclarativeProperty> &after,
                 QDeclarativeTransitionManager *end);

    void setReversed(bool r);
    void stop();

Q_SIGNALS:
    void fromChanged();
    void toChanged();
    void reversibleChanged();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeTransition)

#endif // QDECLARATIVETRANSITION_H
