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

#ifndef QSCRIPTSTRING_H
#define QSCRIPTSTRING_H

#include <QtCore/qstring.h>

#include <QtCore/qsharedpointer.h>
#include <QtScript/qtscriptglobal.h>

QT_BEGIN_NAMESPACE


class QScriptStringPrivate;
class Q_SCRIPT_EXPORT QScriptString
{
public:
    QScriptString();
    QScriptString(const QScriptString &other);
    ~QScriptString();

    QScriptString &operator=(const QScriptString &other);

    bool isValid() const;

    bool operator==(const QScriptString &other) const;
    bool operator!=(const QScriptString &other) const;

    quint32 toArrayIndex(bool *ok = 0) const;

    QString toString() const;
    operator QString() const;

private:
    QExplicitlySharedDataPointer<QScriptStringPrivate> d_ptr;
    friend class QScriptValue;
    Q_DECLARE_PRIVATE(QScriptString)
};

Q_SCRIPT_EXPORT uint qHash(const QScriptString &key);

QT_END_NAMESPACE

#endif // QSCRIPTSTRING_H
