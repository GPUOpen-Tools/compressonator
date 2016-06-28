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

#ifndef QDECLARATIVEDOM_P_P_H
#define QDECLARATIVEDOM_P_P_H

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

#include "private/qdeclarativeparser_p.h"

#include <QtCore/QtGlobal>

QT_BEGIN_NAMESPACE

class QDeclarativeDomDocumentPrivate : public QSharedData
{
public:
    QDeclarativeDomDocumentPrivate();
    QDeclarativeDomDocumentPrivate(const QDeclarativeDomDocumentPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomDocumentPrivate();

    QList<QDeclarativeError> errors;
    QList<QDeclarativeDomImport> imports;
    QDeclarativeParser::Object *root;
    QList<int> automaticSemicolonOffsets;
};

class QDeclarativeDomObjectPrivate : public QSharedData
{
public:
    QDeclarativeDomObjectPrivate();
    QDeclarativeDomObjectPrivate(const QDeclarativeDomObjectPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomObjectPrivate();

    typedef QList<QPair<QDeclarativeParser::Property *, QByteArray> > Properties;
    Properties properties() const;
    Properties properties(QDeclarativeParser::Property *) const;

    QDeclarativeParser::Object *object;
};

class QDeclarativeDomPropertyPrivate : public QSharedData
{
public:
    QDeclarativeDomPropertyPrivate();
    QDeclarativeDomPropertyPrivate(const QDeclarativeDomPropertyPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomPropertyPrivate();

    QByteArray propertyName;
    QDeclarativeParser::Property *property;
};

class QDeclarativeDomDynamicPropertyPrivate : public QSharedData
{
public:
    QDeclarativeDomDynamicPropertyPrivate();
    QDeclarativeDomDynamicPropertyPrivate(const QDeclarativeDomDynamicPropertyPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomDynamicPropertyPrivate();

    bool valid;
    QDeclarativeParser::Object::DynamicProperty property;
};

class QDeclarativeDomValuePrivate : public QSharedData
{
public:
    QDeclarativeDomValuePrivate();
    QDeclarativeDomValuePrivate(const QDeclarativeDomValuePrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomValuePrivate();

    QDeclarativeParser::Property *property;
    QDeclarativeParser::Value *value;
};

class QDeclarativeDomBasicValuePrivate : public QSharedData
{
public:
    QDeclarativeDomBasicValuePrivate();
    QDeclarativeDomBasicValuePrivate(const QDeclarativeDomBasicValuePrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomBasicValuePrivate();

    QDeclarativeParser::Value *value;
};

class QDeclarativeDomImportPrivate : public QSharedData
{
public:
    QDeclarativeDomImportPrivate();
    QDeclarativeDomImportPrivate(const QDeclarativeDomImportPrivate &o)
    : QSharedData(o) { qFatal("Not impl"); }
    ~QDeclarativeDomImportPrivate();

    enum Type { Library, File };

    Type type;
    QString uri;
    QString version;
    QString qualifier;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEDOM_P_P_H

