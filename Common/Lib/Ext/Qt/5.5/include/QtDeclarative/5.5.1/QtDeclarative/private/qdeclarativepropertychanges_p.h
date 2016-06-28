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

#ifndef QDECLARATIVEPROPERTYCHANGES_H
#define QDECLARATIVEPROPERTYCHANGES_H

#include "private/qdeclarativestateoperations_p.h"
#include <private/qdeclarativecustomparser_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativePropertyChangesPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativePropertyChanges : public QDeclarativeStateOperation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyChanges)

    Q_PROPERTY(QObject *target READ object WRITE setObject)
    Q_PROPERTY(bool restoreEntryValues READ restoreEntryValues WRITE setRestoreEntryValues)
    Q_PROPERTY(bool explicit READ isExplicit WRITE setIsExplicit)
public:
    QDeclarativePropertyChanges();
    ~QDeclarativePropertyChanges();

    QObject *object() const;
    void setObject(QObject *);

    bool restoreEntryValues() const;
    void setRestoreEntryValues(bool);

    bool isExplicit() const;
    void setIsExplicit(bool);

    virtual ActionList actions();

    bool containsProperty(const QString &name) const;
    bool containsValue(const QString &name) const;
    bool containsExpression(const QString &name) const;
    void changeValue(const QString &name, const QVariant &value);
    void changeExpression(const QString &name, const QString &expression);
    void removeProperty(const QString &name);
    QVariant value(const QString &name) const;
    QString expression(const QString &name) const;

    void detachFromState();
    void attachToState();

    QVariant property(const QString &name) const;
};

class QDeclarativePropertyChangesParser : public QDeclarativeCustomParser
{
public:
    QDeclarativePropertyChangesParser()
    : QDeclarativeCustomParser(AcceptsAttachedProperties) {}

    void compileList(QList<QPair<QByteArray, QVariant> > &list, const QByteArray &pre, const QDeclarativeCustomParserProperty &prop);

    virtual QByteArray compile(const QList<QDeclarativeCustomParserProperty> &);
    virtual void setCustomData(QObject *, const QByteArray &);
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePropertyChanges)

#endif // QDECLARATIVEPROPERTYCHANGES_H
