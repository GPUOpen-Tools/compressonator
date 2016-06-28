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

#ifndef QSCRIPTABLE_H
#define QSCRIPTABLE_H

#include <QtCore/qobjectdefs.h>

#include <QtCore/qscopedpointer.h>
#include <QtScript/qtscriptglobal.h>

QT_BEGIN_NAMESPACE


#ifndef QT_NO_QOBJECT

class QScriptEngine;
class QScriptContext;
class QScriptValue;

class QScriptablePrivate;

class Q_SCRIPT_EXPORT QScriptable
{
public:
    QScriptable();
    ~QScriptable();

    QScriptEngine *engine() const;
    QScriptContext *context() const;
    QScriptValue thisObject() const;
    int argumentCount() const;
    QScriptValue argument(int index) const;

private:
    QScopedPointer<QScriptablePrivate> d_ptr;

    Q_DISABLE_COPY(QScriptable)
    Q_DECLARE_PRIVATE(QScriptable)
};

#endif // QT_NO_QOBJECT

QT_END_NAMESPACE

#endif // QSCRIPTABLE_H
