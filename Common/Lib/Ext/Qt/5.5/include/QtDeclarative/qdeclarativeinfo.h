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

#ifndef QDECLARATIVEINFO_H
#define QDECLARATIVEINFO_H

#include <QtCore/qdebug.h>
#include <QtCore/qurl.h>
#include <QtDeclarative/qdeclarativeerror.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeInfo;

namespace QtDeclarative {
    // declared in namespace to avoid symbol conflicts with QtQml
    Q_DECLARATIVE_EXPORT QDeclarativeInfo qmlInfo(const QObject *me);
    Q_DECLARATIVE_EXPORT QDeclarativeInfo qmlInfo(const QObject *me,
                                                  const QDeclarativeError &error);
    Q_DECLARATIVE_EXPORT QDeclarativeInfo qmlInfo(const QObject *me,
                                                  const QList<QDeclarativeError> &errors);
}
using namespace QtDeclarative;

class QDeclarativeInfoPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeInfo : public QDebug
{
public:
    QDeclarativeInfo(const QDeclarativeInfo &);
    ~QDeclarativeInfo();

    inline QDeclarativeInfo &operator<<(QChar t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(bool t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(char t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(signed short t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(unsigned short t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(signed int t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(unsigned int t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(signed long t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(unsigned long t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(qint64 t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(quint64 t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(float t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(double t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(const char* t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(const QString & t) { QDebug::operator<<(t.toLocal8Bit().constData()); return *this; }
    inline QDeclarativeInfo &operator<<(const QStringRef & t) { return operator<<(t.toString()); }
    inline QDeclarativeInfo &operator<<(const QLatin1String &t) { QDebug::operator<<(t.latin1()); return *this; }
    inline QDeclarativeInfo &operator<<(const QByteArray & t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(const void * t) { QDebug::operator<<(t); return *this; }
    inline QDeclarativeInfo &operator<<(QTextStreamFunction f) { QDebug::operator<<(f); return *this; }
    inline QDeclarativeInfo &operator<<(QTextStreamManipulator m) { QDebug::operator<<(m); return *this; }
#ifndef QT_NO_DEBUG_STREAM
    inline QDeclarativeInfo &operator<<(const QUrl &t) { static_cast<QDebug &>(*this) << t; return *this; }
#endif

private:
    friend Q_DECLARATIVE_EXPORT QDeclarativeInfo QtDeclarative::qmlInfo(const QObject *me);
    friend Q_DECLARATIVE_EXPORT QDeclarativeInfo QtDeclarative::qmlInfo(const QObject *me,
                                                                        const QDeclarativeError &error);
    friend Q_DECLARATIVE_EXPORT QDeclarativeInfo QtDeclarative::qmlInfo(const QObject *me,
                                                                        const QList<QDeclarativeError> &errors);
    QDeclarativeInfo(QDeclarativeInfoPrivate *);
    QDeclarativeInfoPrivate *d;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEINFO_H
