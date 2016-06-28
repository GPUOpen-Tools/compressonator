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

#ifndef QDECLARATIVECONTEXT_H
#define QDECLARATIVECONTEXT_H

#include <QtDeclarative/qtdeclarativeglobal.h>
#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtScript/qscriptvalue.h>
#include <QtCore/qmetatype.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QDeclarativeEngine;
class QDeclarativeRefCount;
class QDeclarativeContextPrivate;
class QDeclarativeCompositeTypeData;
class QDeclarativeContextData;

class Q_DECLARATIVE_EXPORT QDeclarativeContext : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeContext)

public:
    QDeclarativeContext(QDeclarativeEngine *parent, QObject *objParent=0);
    QDeclarativeContext(QDeclarativeContext *parent, QObject *objParent=0);
    virtual ~QDeclarativeContext();

    bool isValid() const;

    QDeclarativeEngine *engine() const;
    QDeclarativeContext *parentContext() const;

    QObject *contextObject() const;
    void setContextObject(QObject *);

    QVariant contextProperty(const QString &) const;
    void setContextProperty(const QString &, QObject *);
    void setContextProperty(const QString &, const QVariant &);

    QUrl resolvedUrl(const QUrl &);

    void setBaseUrl(const QUrl &);
    QUrl baseUrl() const;

private:
    friend class QDeclarativeVME;
    friend class QDeclarativeEngine;
    friend class QDeclarativeEnginePrivate;
    friend class QDeclarativeExpression;
    friend class QDeclarativeExpressionPrivate;
    friend class QDeclarativeContextScriptClass;
    friend class QDeclarativeObjectScriptClass;
    friend class QDeclarativeComponent;
    friend class QDeclarativeComponentPrivate;
    friend class QDeclarativeScriptPrivate;
    friend class QDeclarativeBoundSignalProxy;
    friend class QDeclarativeContextData;
    QDeclarativeContext(QDeclarativeContextData *);
    QDeclarativeContext(QDeclarativeEngine *, bool);
    Q_DISABLE_COPY(QDeclarativeContext)
};
QT_END_NAMESPACE

Q_DECLARE_METATYPE(QList<QObject*>)

#endif // QDECLARATIVECONTEXT_H
