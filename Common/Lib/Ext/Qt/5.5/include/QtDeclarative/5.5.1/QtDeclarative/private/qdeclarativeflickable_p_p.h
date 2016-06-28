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

#ifndef QDECLARATIVEFLICKABLE_P_H
#define QDECLARATIVEFLICKABLE_P_H

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

#include "private/qdeclarativeflickable_p.h"

#include "private/qdeclarativeitem_p.h"
#include "private/qdeclarativeitemchangelistener_p.h"

#include <qdeclarative.h>
#include <qdeclarativetimeline_p_p.h>
#include <qdeclarativeanimation_p_p.h>

#include <qdatetime.h>
#include "qplatformdefs.h"

QT_BEGIN_NAMESPACE

// Really slow flicks can be annoying.
#ifndef QML_FLICK_MINVELOCITY
#define QML_FLICK_MINVELOCITY 175
#endif

const qreal MinimumFlickVelocity = QML_FLICK_MINVELOCITY;

class QDeclarativeFlickableVisibleArea;
class QDeclarativeFlickablePrivate : public QDeclarativeItemPrivate, public QDeclarativeItemChangeListener
{
    Q_DECLARE_PUBLIC(QDeclarativeFlickable)

public:
    QDeclarativeFlickablePrivate();
    void init();

    struct Velocity : public QDeclarativeTimeLineValue
    {
        Velocity(QDeclarativeFlickablePrivate *p)
            : parent(p) {}
        virtual void setValue(qreal v) {
            if (v != value()) {
                QDeclarativeTimeLineValue::setValue(v);
                parent->updateVelocity();
            }
        }
        QDeclarativeFlickablePrivate *parent;
    };

    struct AxisData {
        AxisData(QDeclarativeFlickablePrivate *fp, void (QDeclarativeFlickablePrivate::*func)(qreal))
            : move(fp, func), viewSize(-1), smoothVelocity(fp), atEnd(false), atBeginning(true)
            , fixingUp(false), inOvershoot(false), moving(false), flicking(false)
        {}

        void reset() {
            velocityBuffer.clear();
            dragStartOffset = 0;
            fixingUp = false;
            inOvershoot = false;
        }

        void addVelocitySample(qreal v, qreal maxVelocity);
        void updateVelocity();

        QDeclarativeTimeLineValueProxy<QDeclarativeFlickablePrivate> move;
        qreal viewSize;
        qreal pressPos;
        qreal dragStartOffset;
        qreal dragMinBound;
        qreal dragMaxBound;
        qreal velocity;
        qreal flickTarget;
        QDeclarativeFlickablePrivate::Velocity smoothVelocity;
        QPODVector<qreal,10> velocityBuffer;
        bool atEnd : 1;
        bool atBeginning : 1;
        bool fixingUp : 1;
        bool inOvershoot : 1;
        bool moving : 1;
        bool flicking : 1;
    };

    void flickX(qreal velocity);
    void flickY(qreal velocity);
    virtual void flick(AxisData &data, qreal minExtent, qreal maxExtent, qreal vSize,
                        QDeclarativeTimeLineCallback::Callback fixupCallback, qreal velocity);

    void fixupX();
    void fixupY();
    virtual void fixup(AxisData &data, qreal minExtent, qreal maxExtent);

    void updateBeginningEnd();

    bool isOutermostPressDelay() const;
    void captureDelayedPress(QGraphicsSceneMouseEvent *event);
    void clearDelayedPress();

    void setRoundedViewportX(qreal x);
    void setRoundedViewportY(qreal y);

    qreal overShootDistance(qreal size);

    void itemGeometryChanged(QDeclarativeItem *, const QRectF &, const QRectF &);

public:
    QDeclarativeItem *contentItem;

    AxisData hData;
    AxisData vData;

    QDeclarativeTimeLine timeline;
    bool hMoved : 1;
    bool vMoved : 1;
    bool stealMouse : 1;
    bool pressed : 1;
    bool interactive : 1;
    bool calcVelocity : 1;
    QElapsedTimer lastPosTime;
    QPointF lastPos;
    QPointF pressPos;
    QElapsedTimer pressTime;
    qreal deceleration;
    qreal maxVelocity;
    QElapsedTimer velocityTime;
    QPointF lastFlickablePosition;
    qreal reportedVelocitySmoothing;
    QGraphicsSceneMouseEvent *delayedPressEvent;
    QGraphicsItem *delayedPressTarget;
    QBasicTimer delayedPressTimer;
    int pressDelay;
    int fixupDuration;

    enum FixupMode { Normal, Immediate, ExtentChanged };
    FixupMode fixupMode;

    static void fixupY_callback(void *);
    static void fixupX_callback(void *);

    void updateVelocity();
    int vTime;
    QDeclarativeTimeLine velocityTimeline;
    QDeclarativeFlickableVisibleArea *visibleArea;
    QDeclarativeFlickable::FlickableDirection flickableDirection;
    QDeclarativeFlickable::BoundsBehavior boundsBehavior;

    void handleMousePressEvent(QGraphicsSceneMouseEvent *);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *);

    // flickableData property
    static void data_append(QDeclarativeListProperty<QObject> *, QObject *);
    static int data_count(QDeclarativeListProperty<QObject> *);
    static QObject *data_at(QDeclarativeListProperty<QObject> *, int);
    static void data_clear(QDeclarativeListProperty<QObject> *);
};

class QDeclarativeFlickableVisibleArea : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal xPosition READ xPosition NOTIFY xPositionChanged)
    Q_PROPERTY(qreal yPosition READ yPosition NOTIFY yPositionChanged)
    Q_PROPERTY(qreal widthRatio READ widthRatio NOTIFY widthRatioChanged)
    Q_PROPERTY(qreal heightRatio READ heightRatio NOTIFY heightRatioChanged)

public:
    QDeclarativeFlickableVisibleArea(QDeclarativeFlickable *parent=0);

    qreal xPosition() const;
    qreal widthRatio() const;
    qreal yPosition() const;
    qreal heightRatio() const;

    void updateVisible();

signals:
    void xPositionChanged(qreal xPosition);
    void yPositionChanged(qreal yPosition);
    void widthRatioChanged(qreal widthRatio);
    void heightRatioChanged(qreal heightRatio);

private:
    QDeclarativeFlickable *flickable;
    qreal m_xPosition;
    qreal m_widthRatio;
    qreal m_yPosition;
    qreal m_heightRatio;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeFlickableVisibleArea)

#endif
