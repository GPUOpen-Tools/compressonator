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

#ifndef QDECLARATIVESMOOTHEDANIMATION_P_H
#define QDECLARATIVESMOOTHEDANIMATION_P_H

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

#include "private/qdeclarativesmoothedanimation_p.h"
#include "private/qdeclarativeanimation_p.h"

#include "private/qdeclarativeanimation_p_p.h"

#include <qparallelanimationgroup.h>

#include <private/qobject_p.h>
#include <QTimer>

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QSmoothedAnimation : public QAbstractAnimation
{
public:
    QSmoothedAnimation(QObject *parent=0);

    qreal to;
    qreal velocity;
    int userDuration;

    int maximumEasingTime;
    QDeclarativeSmoothedAnimation::ReversingMode reversingMode;

    qreal initialVelocity;
    qreal trackVelocity;

    QDeclarativeProperty target;

    int duration() const;
    void restart();
    void init();

protected:
    virtual void updateCurrentTime(int);
    virtual void updateState(QAbstractAnimation::State, QAbstractAnimation::State);

private:
    qreal easeFollow(qreal);
    qreal initialValue;

    bool invert;

    int finalDuration;

    // Parameters for use in updateCurrentTime()
    qreal a;  // Acceleration
    qreal d;  // Deceleration
    qreal tf; // Total time
    qreal tp; // Time at which peak velocity occurs
    qreal td; // Time at which decelleration begins
    qreal vp; // Velocity at tp
    qreal sp; // Displacement at tp
    qreal sd; // Displacement at td
    qreal vi; // "Normalized" initialvelocity
    qreal s;  // Total s

    int lastTime;

    bool recalc();
    void delayedStop();

    QTimer delayedStopTimer;
};

class QDeclarativeSmoothedAnimationPrivate : public QDeclarativePropertyAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeSmoothedAnimation)
public:
    QDeclarativeSmoothedAnimationPrivate();
    void updateRunningAnimations();

    QParallelAnimationGroup *wrapperGroup;
    QSmoothedAnimation *anim;
    QHash<QDeclarativeProperty, QSmoothedAnimation*> activeAnimations;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESMOOTHEDANIMATION_P_H
