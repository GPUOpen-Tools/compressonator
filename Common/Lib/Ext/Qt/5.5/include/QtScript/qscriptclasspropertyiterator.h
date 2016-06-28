/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTCLASSPROPERTYITERATOR_H
#define QSCRIPTCLASSPROPERTYITERATOR_H

#include <QtCore/qstring.h>

#include <QtCore/qscopedpointer.h>
#include <QtScript/qscriptvalue.h>

QT_BEGIN_NAMESPACE


class QScriptClassPropertyIteratorPrivate;
class Q_SCRIPT_EXPORT QScriptClassPropertyIterator
{
protected:
    QScriptClassPropertyIterator(const QScriptValue &object);

public:
    virtual ~QScriptClassPropertyIterator();

    QScriptValue object() const;

    virtual bool hasNext() const = 0;
    virtual void next() = 0;

    virtual bool hasPrevious() const = 0;
    virtual void previous() = 0;

    virtual void toFront() = 0;
    virtual void toBack() = 0;

    virtual QScriptString name() const = 0;
    virtual uint id() const;
    virtual QScriptValue::PropertyFlags flags() const;

protected:
    QScriptClassPropertyIterator(const QScriptValue &object, QScriptClassPropertyIteratorPrivate &dd);
    QScopedPointer<QScriptClassPropertyIteratorPrivate> d_ptr;

private:
    Q_DECLARE_PRIVATE(QScriptClassPropertyIterator)
    Q_DISABLE_COPY(QScriptClassPropertyIterator)
};

QT_END_NAMESPACE

#endif
