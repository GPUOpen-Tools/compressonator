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

#ifndef QDECLARATIVELIST_H
#define QDECLARATIVELIST_H

#include <QtDeclarative/qtdeclarativeglobal.h>
#include <QtCore/qlist.h>
#include <QtCore/qvariant.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QObject;
struct QMetaObject;

#ifndef QDECLARATIVELISTPROPERTY
#define QDECLARATIVELISTPROPERTY
template<typename T>
class QDeclarativeListProperty {
public:
    typedef void (*AppendFunction)(QDeclarativeListProperty<T> *, T*);
    typedef int (*CountFunction)(QDeclarativeListProperty<T> *);
    typedef T *(*AtFunction)(QDeclarativeListProperty<T> *, int);
    typedef void (*ClearFunction)(QDeclarativeListProperty<T> *);

    QDeclarativeListProperty()
        : object(0), data(0), append(0), count(0), at(0), clear(0), dummy1(0), dummy2(0) {}
    QDeclarativeListProperty(QObject *o, QList<T *> &list)
        : object(o), data(&list), append(qlist_append), count(qlist_count), at(qlist_at),
          clear(qlist_clear), dummy1(0), dummy2(0) {}
    QDeclarativeListProperty(QObject *o, void *d, AppendFunction a, CountFunction c = 0, AtFunction t = 0,
                    ClearFunction r = 0)
        : object(o), data(d), append(a), count(c), at(t), clear(r), dummy1(0), dummy2(0) {}

    bool operator==(const QDeclarativeListProperty &o) const {
        return object == o.object &&
               data == o.data &&
               append == o.append &&
               count == o.count &&
               at == o.at &&
               clear == o.clear;
    }

    QObject *object;
    void *data;

    AppendFunction append;

    CountFunction count;
    AtFunction at;

    ClearFunction clear;

    void *dummy1;
    void *dummy2;

private:
    static void qlist_append(QDeclarativeListProperty *p, T *v) {
        reinterpret_cast<QList<T *> *>(p->data)->append(v);
    }
    static int qlist_count(QDeclarativeListProperty *p) {
        return reinterpret_cast<QList<T *> *>(p->data)->count();
    }
    static T *qlist_at(QDeclarativeListProperty *p, int idx) {
        return reinterpret_cast<QList<T *> *>(p->data)->at(idx);
    }
    static void qlist_clear(QDeclarativeListProperty *p) {
        return reinterpret_cast<QList<T *> *>(p->data)->clear();
    }
};
#endif

class QDeclarativeEngine;
class QDeclarativeListReferencePrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeListReference
{
public:
    QDeclarativeListReference();
    QDeclarativeListReference(QObject *, const char *property, QDeclarativeEngine * = 0);
    QDeclarativeListReference(const QDeclarativeListReference &);
    QDeclarativeListReference &operator=(const QDeclarativeListReference &);
    ~QDeclarativeListReference();

    bool isValid() const;

    QObject *object() const;
    const QMetaObject *listElementType() const;

    bool canAppend() const;
    bool canAt() const;
    bool canClear() const;
    bool canCount() const;

    bool append(QObject *) const;
    QObject *at(int) const;
    bool clear() const;
    int count() const;

private:
    friend class QDeclarativeListReferencePrivate;
    QDeclarativeListReferencePrivate* d;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QDeclarativeListReference)

#endif // QDECLARATIVELIST_H
