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

#ifndef QDECLARATIVEREPEATER_H
#define QDECLARATIVEREPEATER_H

#include "qdeclarativeitem.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeRepeaterPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeRepeater : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")

public:
    QDeclarativeRepeater(QDeclarativeItem *parent=0);
    virtual ~QDeclarativeRepeater();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int count() const;

    Q_INVOKABLE Q_REVISION(1) QDeclarativeItem *itemAt(int index) const;

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void countChanged();

    Q_REVISION(1) void itemAdded(int index, QDeclarativeItem *item);
    Q_REVISION(1) void itemRemoved(int index, QDeclarativeItem *item);

private:
    void clear();
    void regenerate();

protected:
    virtual void componentComplete();
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

private Q_SLOTS:
    void itemsInserted(int,int);
    void itemsRemoved(int,int);
    void itemsMoved(int,int,int);
    void modelReset();

private:
    Q_DISABLE_COPY(QDeclarativeRepeater)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeRepeater)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeRepeater)

#endif // QDECLARATIVEREPEATER_H
