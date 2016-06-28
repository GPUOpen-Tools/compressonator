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

#ifndef QDECLARATIVEDEBUGTRACE_P_H
#define QDECLARATIVEDEBUGTRACE_P_H

#include "qtdeclarativeglobal.h"

#include <private/qdeclarativedebugservice_p.h>
#include <qelapsedtimer.h>

QT_BEGIN_NAMESPACE

struct QDeclarativeDebugData
{
    qint64 time;
    int messageType;
    int detailType;

    //###
    QString detailData; //used by RangeData and RangeLocation
    int line;           //used by RangeLocation

    QByteArray toByteArray() const;
};

class QUrl;
class Q_DECLARATIVE_EXPORT QDeclarativeDebugTrace : public QDeclarativeDebugService
{
public:
    enum Message {
        Event,
        RangeStart,
        RangeData,
        RangeLocation,
        RangeEnd,
        Complete,

        MaximumMessage
    };

    enum EventType {
        FramePaint,
        Mouse,
        Key,

        MaximumEventType
    };

    enum RangeType {
        Painting,
        Compiling,
        Creating,
        Binding,            //running a binding
        HandlingSignal,     //running a signal handler

        MaximumRangeType
    };

    static void addEvent(EventType);

    static void startRange(RangeType);
    static void rangeData(RangeType, const QString &);
    static void rangeData(RangeType, const QUrl &);
    static void rangeLocation(RangeType, const QString &, int);
    static void rangeLocation(RangeType, const QUrl &, int);
    static void endRange(RangeType);

    QDeclarativeDebugTrace();
#ifdef CUSTOM_DECLARATIVE_DEBUG_TRACE_INSTANCE
public:
    static QDeclarativeDebugTrace* globalInstance();
    static void setGlobalInstance(QDeclarativeDebugTrace *custom_instance);
protected:
    virtual void messageReceived(const QByteArray &);
protected:
    virtual void addEventImpl(EventType);
    virtual void startRangeImpl(RangeType);
    virtual void rangeDataImpl(RangeType, const QString &);
    virtual void rangeDataImpl(RangeType, const QUrl &);
    virtual void rangeLocationImpl(RangeType, const QString &, int);
    virtual void rangeLocationImpl(RangeType, const QUrl &, int);
    virtual void endRangeImpl(RangeType);
#else
protected:
    virtual void messageReceived(const QByteArray &);
private:
    void addEventImpl(EventType);
    void startRangeImpl(RangeType);
    void rangeDataImpl(RangeType, const QString &);
    void rangeDataImpl(RangeType, const QUrl &);
    void rangeLocationImpl(RangeType, const QString &, int);
    void rangeLocationImpl(RangeType, const QUrl &, int);
    void endRangeImpl(RangeType);
#endif
    void processMessage(const QDeclarativeDebugData &);
    void sendMessages();
    QElapsedTimer m_timer;
    bool m_enabled;
    bool m_deferredSend;
    bool m_messageReceived;
    QList<QDeclarativeDebugData> m_data;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEDEBUGTRACE_P_H

