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

#ifndef QSCRIPTPROGRAM_H
#define QSCRIPTPROGRAM_H

#include <QtCore/qsharedpointer.h>

#include <QtCore/qstring.h>
#include <QtScript/qtscriptglobal.h>

QT_BEGIN_NAMESPACE


class QScriptProgramPrivate;
class Q_SCRIPT_EXPORT QScriptProgram
{
public:
    QScriptProgram();
    QScriptProgram(const QString &sourceCode,
                   const QString fileName = QString(),
                   int firstLineNumber = 1);
    QScriptProgram(const QScriptProgram &other);
    ~QScriptProgram();

    QScriptProgram &operator=(const QScriptProgram &other);

    bool isNull() const;

    QString sourceCode() const;
    QString fileName() const;
    int firstLineNumber() const;

    bool operator==(const QScriptProgram &other) const;
    bool operator!=(const QScriptProgram &other) const;

private:
    QExplicitlySharedDataPointer<QScriptProgramPrivate> d_ptr;
    Q_DECLARE_PRIVATE(QScriptProgram)
};

QT_END_NAMESPACE

#endif // QSCRIPTPROGRAM_H
