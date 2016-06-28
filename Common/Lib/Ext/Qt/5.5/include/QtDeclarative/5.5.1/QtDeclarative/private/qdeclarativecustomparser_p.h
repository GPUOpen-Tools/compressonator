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

#ifndef QDECLARATIVECUSTOMPARSER_H
#define QDECLARATIVECUSTOMPARSER_H

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

#include "private/qdeclarativemetatype_p.h"
#include "qdeclarativeerror.h"
#include "private/qdeclarativeparser_p.h"
#include "private/qdeclarativebinding_p.h"

#include <QtCore/qbytearray.h>
#include <QtCore/qxmlstream.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeCompiler;

class QDeclarativeCustomParserPropertyPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeCustomParserProperty
{
public:
    QDeclarativeCustomParserProperty();
    QDeclarativeCustomParserProperty(const QDeclarativeCustomParserProperty &);
    QDeclarativeCustomParserProperty &operator=(const QDeclarativeCustomParserProperty &);
    ~QDeclarativeCustomParserProperty();

    QByteArray name() const;
    QDeclarativeParser::Location location() const;

    bool isList() const;
    // Will be one of QDeclarativeParser::Variant, QDeclarativeCustomParserProperty or
    // QDeclarativeCustomParserNode
    QList<QVariant> assignedValues() const;

private:
    friend class QDeclarativeCustomParserNodePrivate;
    friend class QDeclarativeCustomParserPropertyPrivate;
    QDeclarativeCustomParserPropertyPrivate *d;
};

class QDeclarativeCustomParserNodePrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeCustomParserNode
{
public:
    QDeclarativeCustomParserNode();
    QDeclarativeCustomParserNode(const QDeclarativeCustomParserNode &);
    QDeclarativeCustomParserNode &operator=(const QDeclarativeCustomParserNode &);
    ~QDeclarativeCustomParserNode();

    QByteArray name() const;
    QDeclarativeParser::Location location() const;

    QList<QDeclarativeCustomParserProperty> properties() const;

private:
    friend class QDeclarativeCustomParserNodePrivate;
    QDeclarativeCustomParserNodePrivate *d;
};

class Q_DECLARATIVE_EXPORT QDeclarativeCustomParser
{
public:
    enum Flag {
        NoFlag                    = 0x00000000,
        AcceptsAttachedProperties = 0x00000001
    };
    Q_DECLARE_FLAGS(Flags, Flag)

    QDeclarativeCustomParser() : compiler(0), object(0), m_flags(NoFlag) {}
    QDeclarativeCustomParser(Flags f) : compiler(0), object(0), m_flags(f) {}
    virtual ~QDeclarativeCustomParser() {}

    void clearErrors();
    Flags flags() const { return m_flags; }

    virtual QByteArray compile(const QList<QDeclarativeCustomParserProperty> &)=0;
    virtual void setCustomData(QObject *, const QByteArray &)=0;

    QList<QDeclarativeError> errors() const { return exceptions; }

protected:
    void error(const QString& description);
    void error(const QDeclarativeCustomParserProperty&, const QString& description);
    void error(const QDeclarativeCustomParserNode&, const QString& description);

    int evaluateEnum(const QByteArray&) const;

    const QMetaObject *resolveType(const QByteArray&) const;

    QDeclarativeBinding::Identifier rewriteBinding(const QString&, const QByteArray&);

private:
    QList<QDeclarativeError> exceptions;
    QDeclarativeCompiler *compiler;
    QDeclarativeParser::Object *object;
    Flags m_flags;
    friend class QDeclarativeCompiler;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeCustomParser::Flags);

#if 0
#define QML_REGISTER_CUSTOM_TYPE(URI, VERSION_MAJ, VERSION_MIN, NAME, TYPE, CUSTOMTYPE) \
            qmlRegisterCustomType<TYPE>(#URI, VERSION_MAJ, VERSION_MIN, #NAME, #TYPE, new CUSTOMTYPE)
#endif

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeCustomParserProperty)
Q_DECLARE_METATYPE(QDeclarativeCustomParserNode)

#endif
