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

#ifndef QDECLARATIVEEXPRESSION_H
#define QDECLARATIVEEXPRESSION_H

#include <QtDeclarative/qdeclarativeerror.h>

#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QString;
class QDeclarativeRefCount;
class QDeclarativeEngine;
class QDeclarativeContext;
class QDeclarativeExpressionPrivate;
class QDeclarativeContextData;
class QScriptValue;
class Q_DECLARATIVE_EXPORT QDeclarativeExpression : public QObject
{
    Q_OBJECT
public:
    QDeclarativeExpression();
    QDeclarativeExpression(QDeclarativeContext *, QObject *, const QString &, QObject * = 0);
    virtual ~QDeclarativeExpression();

    QDeclarativeEngine *engine() const;
    QDeclarativeContext *context() const;

    QString expression() const;
    void setExpression(const QString &);

    bool notifyOnValueChanged() const;
    void setNotifyOnValueChanged(bool);

    QString sourceFile() const;
    int lineNumber() const;
    void setSourceLocation(const QString &fileName, int line);

    QObject *scopeObject() const;

    bool hasError() const;
    void clearError();
    QDeclarativeError error() const;

    QVariant evaluate(bool *valueIsUndefined = 0);

Q_SIGNALS:
    void valueChanged();

protected:
    QDeclarativeExpression(QDeclarativeContextData *, QObject *, const QString &,
                           QDeclarativeExpressionPrivate &dd);
    QDeclarativeExpression(QDeclarativeContextData *, QObject *, const QScriptValue &,
                           QDeclarativeExpressionPrivate &dd);
    QDeclarativeExpression(QDeclarativeContextData *, void *, QDeclarativeRefCount *rc,
                           QObject *me, const QString &, int, QDeclarativeExpressionPrivate &dd);

private:
    QDeclarativeExpression(QDeclarativeContextData *, QObject *, const QString &);

    Q_DISABLE_COPY(QDeclarativeExpression)
    Q_DECLARE_PRIVATE(QDeclarativeExpression)
    Q_PRIVATE_SLOT(d_func(), void _q_notify())
    friend class QDeclarativeDebugger;
    friend class QDeclarativeContext;
    friend class QDeclarativeVME;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEEXPRESSION_H

