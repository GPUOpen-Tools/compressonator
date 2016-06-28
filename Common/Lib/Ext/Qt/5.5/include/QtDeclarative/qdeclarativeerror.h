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

#ifndef QDECLARATIVEERROR_H
#define QDECLARATIVEERROR_H

#include <QtDeclarative/qtdeclarativeglobal.h>
#include <QtCore/qurl.h>
#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDebug;
class QDeclarativeErrorPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeError
{
public:
    QDeclarativeError();
    QDeclarativeError(const QDeclarativeError &);
    QDeclarativeError &operator=(const QDeclarativeError &);
    ~QDeclarativeError();

    bool isValid() const;

    QUrl url() const;
    void setUrl(const QUrl &);
    QString description() const;
    void setDescription(const QString &);
    int line() const;
    void setLine(int);
    int column() const;
    void setColumn(int);

    QString toString() const;
private:
    QDeclarativeErrorPrivate *d;
};

QDebug Q_DECLARATIVE_EXPORT operator<<(QDebug debug, const QDeclarativeError &error);

QT_END_NAMESPACE

#endif // QDECLARATIVEERROR_H
