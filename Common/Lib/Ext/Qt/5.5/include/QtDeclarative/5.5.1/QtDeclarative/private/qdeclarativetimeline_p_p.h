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

#ifndef QDECLARATIVETIMELINE_H
#define QDECLARATIVETIMELINE_H

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

#include <QtCore/QObject>
#include <QtCore/QAbstractAnimation>

QT_BEGIN_NAMESPACE

class QEasingCurve;
class QDeclarativeTimeLineValue;
class QDeclarativeTimeLineCallback;
struct QDeclarativeTimeLinePrivate;
class QDeclarativeTimeLineObject;
class Q_AUTOTEST_EXPORT QDeclarativeTimeLine : public QAbstractAnimation
{
Q_OBJECT
public:
    QDeclarativeTimeLine(QObject *parent = 0);
    ~QDeclarativeTimeLine();

    enum SyncMode { LocalSync, GlobalSync };
    SyncMode syncMode() const;
    void setSyncMode(SyncMode);

    void pause(QDeclarativeTimeLineObject &, int);
    void callback(const QDeclarativeTimeLineCallback &);
    void set(QDeclarativeTimeLineValue &, qreal);

    int accel(QDeclarativeTimeLineValue &, qreal velocity, qreal accel);
    int accel(QDeclarativeTimeLineValue &, qreal velocity, qreal accel, qreal maxDistance);
    int accelDistance(QDeclarativeTimeLineValue &, qreal velocity, qreal distance);

    void move(QDeclarativeTimeLineValue &, qreal destination, int time = 500);
    void move(QDeclarativeTimeLineValue &, qreal destination, const QEasingCurve &, int time = 500);
    void moveBy(QDeclarativeTimeLineValue &, qreal change, int time = 500);
    void moveBy(QDeclarativeTimeLineValue &, qreal change, const QEasingCurve &, int time = 500);

    void sync();
    void setSyncPoint(int);
    int syncPoint() const;

    void sync(QDeclarativeTimeLineValue &);
    void sync(QDeclarativeTimeLineValue &, QDeclarativeTimeLineValue &);

    void reset(QDeclarativeTimeLineValue &);

    void complete();
    void clear();
    bool isActive() const;

    int time() const;

    virtual int duration() const;
Q_SIGNALS:
    void updated();
    void completed();

protected:
    virtual void updateCurrentTime(int);

private:
    void remove(QDeclarativeTimeLineObject *);
    friend class QDeclarativeTimeLineObject;
    friend struct QDeclarativeTimeLinePrivate;
    QDeclarativeTimeLinePrivate *d;
};

class Q_AUTOTEST_EXPORT QDeclarativeTimeLineObject
{
public:
    QDeclarativeTimeLineObject();
    virtual ~QDeclarativeTimeLineObject();

protected:
    friend class QDeclarativeTimeLine;
    friend struct QDeclarativeTimeLinePrivate;
    QDeclarativeTimeLine *_t;
};

class Q_AUTOTEST_EXPORT QDeclarativeTimeLineValue : public QDeclarativeTimeLineObject
{
public:
    QDeclarativeTimeLineValue(qreal v = 0.) : _v(v) {}

    virtual qreal value() const { return _v; }
    virtual void setValue(qreal v) { _v = v; }

    QDeclarativeTimeLine *timeLine() const { return _t; }

    operator qreal() const { return _v; }
    QDeclarativeTimeLineValue &operator=(qreal v) { setValue(v); return *this; }
private:
    friend class QDeclarativeTimeLine;
    friend struct QDeclarativeTimeLinePrivate;
    qreal _v;
};

class Q_AUTOTEST_EXPORT QDeclarativeTimeLineCallback
{
public:
    typedef void (*Callback)(void *);

    QDeclarativeTimeLineCallback();
    QDeclarativeTimeLineCallback(QDeclarativeTimeLineObject *b, Callback, void * = 0);
    QDeclarativeTimeLineCallback(const QDeclarativeTimeLineCallback &o);

    QDeclarativeTimeLineCallback &operator=(const QDeclarativeTimeLineCallback &o);
    QDeclarativeTimeLineObject *callbackObject() const;

private:
    friend struct QDeclarativeTimeLinePrivate;
    Callback d0;
    void *d1;
    QDeclarativeTimeLineObject *d2;
};

template<class T>
class QDeclarativeTimeLineValueProxy : public QDeclarativeTimeLineValue
{
public:
    QDeclarativeTimeLineValueProxy(T *cls, void (T::*func)(qreal), qreal v = 0.)
    : QDeclarativeTimeLineValue(v), _class(cls), _setFunctionReal(func), _setFunctionInt(0)
    {
        Q_ASSERT(_class);
    }

    QDeclarativeTimeLineValueProxy(T *cls, void (T::*func)(int), qreal v = 0.)
    : QDeclarativeTimeLineValue(v), _class(cls), _setFunctionReal(0), _setFunctionInt(func)
    {
        Q_ASSERT(_class);
    }

    virtual void setValue(qreal v)
    {
        QDeclarativeTimeLineValue::setValue(v);
        if (_setFunctionReal) (_class->*_setFunctionReal)(v);
        else if (_setFunctionInt) (_class->*_setFunctionInt)((int)v);
    }

private:
    T *_class;
    void (T::*_setFunctionReal)(qreal);
    void (T::*_setFunctionInt)(int);
};

QT_END_NAMESPACE

#endif
