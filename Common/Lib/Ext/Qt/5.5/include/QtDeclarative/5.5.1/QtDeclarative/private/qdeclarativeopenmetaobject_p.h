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

#ifndef QDECLARATIVEOPENMETAOBJECT_H
#define QDECLARATIVEOPENMETAOBJECT_H

#include <QtCore/QMetaObject>
#include <QtCore/QObject>

#include <private/qtdeclarativeglobal_p.h>
#include <private/qdeclarativerefcount_p.h>
#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeEngine;
class QMetaPropertyBuilder;
class QDeclarativeOpenMetaObjectTypePrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeOpenMetaObjectType : public QDeclarativeRefCount
{
public:
    QDeclarativeOpenMetaObjectType(const QMetaObject *base, QDeclarativeEngine *engine);
    ~QDeclarativeOpenMetaObjectType();

    int createProperty(const QByteArray &name);

    int propertyOffset() const;
    int signalOffset() const;

protected:
    virtual void propertyCreated(int, QMetaPropertyBuilder &);

private:
    QDeclarativeOpenMetaObjectTypePrivate *d;
    friend class QDeclarativeOpenMetaObject;
    friend class QDeclarativeOpenMetaObjectPrivate;
};

class QDeclarativeOpenMetaObjectPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeOpenMetaObject : public QAbstractDynamicMetaObject
{
public:
    QDeclarativeOpenMetaObject(QObject *, bool = true);
    QDeclarativeOpenMetaObject(QObject *, QDeclarativeOpenMetaObjectType *, bool = true);
    ~QDeclarativeOpenMetaObject();

    QVariant value(const QByteArray &) const;
    void setValue(const QByteArray &, const QVariant &);
    QVariant value(int) const;
    void setValue(int, const QVariant &);
    QVariant &operator[](const QByteArray &);
    QVariant &operator[](int);
    bool hasValue(int) const;

    int count() const;
    QByteArray name(int) const;

    QObject *object() const;
    virtual QVariant initialValue(int);

    // Be careful - once setCached(true) is called createProperty() is no
    // longer automatically called for new properties.
    void setCached(bool);

    QDeclarativeOpenMetaObjectType *type() const;

protected:
    virtual int metaCall(QMetaObject::Call _c, int _id, void **_a);
    virtual int createProperty(const char *, const char *);

    virtual void propertyRead(int);
    virtual void propertyWrite(int);
    virtual void propertyWritten(int);
    virtual void propertyCreated(int, QMetaPropertyBuilder &);

    QAbstractDynamicMetaObject *parent() const;

private:
    QDeclarativeOpenMetaObjectPrivate *d;
    friend class QDeclarativeOpenMetaObjectType;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEOPENMETAOBJECT_H
