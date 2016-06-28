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

#ifndef QDECLARATIVEDOM_P_H
#define QDECLARATIVEDOM_P_H

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

#include "qdeclarativeerror.h"

#include <QtCore/qlist.h>
#include <QtCore/qshareddata.h>

#include <private/qtdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QByteArray;
class QDeclarativeDomObject;
class QDeclarativeDomList;
class QDeclarativeDomValue;
class QDeclarativeEngine;
class QDeclarativeDomComponent;
class QDeclarativeDomImport;
class QIODevice;

class QDeclarativeDomDocumentPrivate;

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomDocument
{
public:
    QDeclarativeDomDocument();
    QDeclarativeDomDocument(const QDeclarativeDomDocument &);
    ~QDeclarativeDomDocument();
    QDeclarativeDomDocument &operator=(const QDeclarativeDomDocument &);

    QList<QDeclarativeDomImport> imports() const;

    QList<QDeclarativeError> errors() const;
    bool load(QDeclarativeEngine *, const QByteArray &, const QUrl & = QUrl());

    QDeclarativeDomObject rootObject() const;

private:
    QSharedDataPointer<QDeclarativeDomDocumentPrivate> d;
};

class QDeclarativeDomPropertyPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomProperty
{
public:
    QDeclarativeDomProperty();
    QDeclarativeDomProperty(const QDeclarativeDomProperty &);
    ~QDeclarativeDomProperty();
    QDeclarativeDomProperty &operator=(const QDeclarativeDomProperty &);

    bool isValid() const;

    QByteArray propertyName() const;
    QList<QByteArray> propertyNameParts() const;

    bool isDefaultProperty() const;

    QDeclarativeDomValue value() const;

    int position() const;
    int length() const;

private:
    friend class QDeclarativeDomObject;
    friend class QDeclarativeDomDynamicProperty;
    QSharedDataPointer<QDeclarativeDomPropertyPrivate> d;
};

class QDeclarativeDomDynamicPropertyPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomDynamicProperty
{
public:
    QDeclarativeDomDynamicProperty();
    QDeclarativeDomDynamicProperty(const QDeclarativeDomDynamicProperty &);
    ~QDeclarativeDomDynamicProperty();
    QDeclarativeDomDynamicProperty &operator=(const QDeclarativeDomDynamicProperty &);

    bool isValid() const;

    QByteArray propertyName() const;
    int propertyType() const;
    QByteArray propertyTypeName() const;

    bool isDefaultProperty() const;
    QDeclarativeDomProperty defaultValue() const;

    bool isAlias() const;

    int position() const;
    int length() const;

private:
    friend class QDeclarativeDomObject;
    QSharedDataPointer<QDeclarativeDomDynamicPropertyPrivate> d;
};

class QDeclarativeDomObjectPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomObject
{
public:
    QDeclarativeDomObject();
    QDeclarativeDomObject(const QDeclarativeDomObject &);
    ~QDeclarativeDomObject();
    QDeclarativeDomObject &operator=(const QDeclarativeDomObject &);

    bool isValid() const;

    QByteArray objectType() const;
    QByteArray objectClassName() const;

    int objectTypeMajorVersion() const;
    int objectTypeMinorVersion() const;

    QString objectId() const;

    QList<QDeclarativeDomProperty> properties() const;
    QDeclarativeDomProperty property(const QByteArray &) const;

    QList<QDeclarativeDomDynamicProperty> dynamicProperties() const;
    QDeclarativeDomDynamicProperty dynamicProperty(const QByteArray &) const;

    bool isCustomType() const;
    QByteArray customTypeData() const;

    bool isComponent() const;
    QDeclarativeDomComponent toComponent() const;

    int position() const;
    int length() const;

    QUrl url() const;
private:
    friend class QDeclarativeDomDocument;
    friend class QDeclarativeDomComponent;
    friend class QDeclarativeDomValue;
    friend class QDeclarativeDomValueValueSource;
    friend class QDeclarativeDomValueValueInterceptor;
    QSharedDataPointer<QDeclarativeDomObjectPrivate> d;
};

class QDeclarativeDomValuePrivate;
class QDeclarativeDomBasicValuePrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomValueLiteral
{
public:
    QDeclarativeDomValueLiteral();
    QDeclarativeDomValueLiteral(const QDeclarativeDomValueLiteral &);
    ~QDeclarativeDomValueLiteral();
    QDeclarativeDomValueLiteral &operator=(const QDeclarativeDomValueLiteral &);

    QString literal() const;

private:
    friend class QDeclarativeDomValue;
    QSharedDataPointer<QDeclarativeDomBasicValuePrivate> d;
};

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomValueBinding
{
public:
    QDeclarativeDomValueBinding();
    QDeclarativeDomValueBinding(const QDeclarativeDomValueBinding &);
    ~QDeclarativeDomValueBinding();
    QDeclarativeDomValueBinding &operator=(const QDeclarativeDomValueBinding &);

    QString binding() const;

private:
    friend class QDeclarativeDomValue;
    QSharedDataPointer<QDeclarativeDomBasicValuePrivate> d;
};

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomValueValueSource
{
public:
    QDeclarativeDomValueValueSource();
    QDeclarativeDomValueValueSource(const QDeclarativeDomValueValueSource &);
    ~QDeclarativeDomValueValueSource();
    QDeclarativeDomValueValueSource &operator=(const QDeclarativeDomValueValueSource &);

    QDeclarativeDomObject object() const;

private:
    friend class QDeclarativeDomValue;
    QSharedDataPointer<QDeclarativeDomBasicValuePrivate> d;
};

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomValueValueInterceptor
{
public:
    QDeclarativeDomValueValueInterceptor();
    QDeclarativeDomValueValueInterceptor(const QDeclarativeDomValueValueInterceptor &);
    ~QDeclarativeDomValueValueInterceptor();
    QDeclarativeDomValueValueInterceptor &operator=(const QDeclarativeDomValueValueInterceptor &);

    QDeclarativeDomObject object() const;

private:
    friend class QDeclarativeDomValue;
    QSharedDataPointer<QDeclarativeDomBasicValuePrivate> d;
};


class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomComponent : public QDeclarativeDomObject
{
public:
    QDeclarativeDomComponent();
    QDeclarativeDomComponent(const QDeclarativeDomComponent &);
    ~QDeclarativeDomComponent();
    QDeclarativeDomComponent &operator=(const QDeclarativeDomComponent &);

    QDeclarativeDomObject componentRoot() const;
};

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomValue
{
public:
    enum Type {
        Invalid,
        Literal,
        PropertyBinding,
        ValueSource,
        ValueInterceptor,
        Object,
        List
    };

    QDeclarativeDomValue();
    QDeclarativeDomValue(const QDeclarativeDomValue &);
    ~QDeclarativeDomValue();
    QDeclarativeDomValue &operator=(const QDeclarativeDomValue &);

    Type type() const;

    bool isInvalid() const;
    bool isLiteral() const;
    bool isBinding() const;
    bool isValueSource() const;
    bool isValueInterceptor() const;
    bool isObject() const;
    bool isList() const;

    QDeclarativeDomValueLiteral toLiteral() const;
    QDeclarativeDomValueBinding toBinding() const;
    QDeclarativeDomValueValueSource toValueSource() const;
    QDeclarativeDomValueValueInterceptor toValueInterceptor() const;
    QDeclarativeDomObject toObject() const;
    QDeclarativeDomList toList() const;

    int position() const;
    int length() const;

private:
    friend class QDeclarativeDomProperty;
    friend class QDeclarativeDomList;
    QSharedDataPointer<QDeclarativeDomValuePrivate> d;
};

class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomList
{
public:
    QDeclarativeDomList();
    QDeclarativeDomList(const QDeclarativeDomList &);
    ~QDeclarativeDomList();
    QDeclarativeDomList &operator=(const QDeclarativeDomList &);

    QList<QDeclarativeDomValue> values() const;

    int position() const;
    int length() const;

    QList<int> commaPositions() const;

private:
    friend class QDeclarativeDomValue;
    QSharedDataPointer<QDeclarativeDomValuePrivate> d;
};

class QDeclarativeDomImportPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeDomImport
{
public:
    enum Type { Library, File };

    QDeclarativeDomImport();
    QDeclarativeDomImport(const QDeclarativeDomImport &);
    ~QDeclarativeDomImport();
    QDeclarativeDomImport &operator=(const QDeclarativeDomImport &);

    Type type() const;
    QString uri() const;
    QString version() const;
    QString qualifier() const;

private:
    friend class QDeclarativeDomDocument;
    QSharedDataPointer<QDeclarativeDomImportPrivate> d;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEDOM_P_H
