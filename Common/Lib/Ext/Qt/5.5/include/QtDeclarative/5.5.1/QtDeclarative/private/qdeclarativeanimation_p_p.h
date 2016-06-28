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

#ifndef QDECLARATIVEANIMATION_P_H
#define QDECLARATIVEANIMATION_P_H

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

#include "private/qdeclarativeanimation_p.h"

#include "private/qdeclarativenullablevalue_p_p.h"
#include "private/qdeclarativetimeline_p_p.h"

#include <qdeclarative.h>
#include <qdeclarativeitem.h>
#include <qdeclarativecontext.h>

#include <QtCore/QPauseAnimation>
#include <QtCore/QVariantAnimation>
#include <QtCore/QAnimationGroup>
#include <QtGui/QColor>
#include <QDebug>

#include <private/qobject_p.h>
#include <private/qvariantanimation_p.h>

QT_BEGIN_NAMESPACE

//interface for classes that provide animation actions for QActionAnimation
class QAbstractAnimationAction
{
public:
    virtual ~QAbstractAnimationAction() {}
    virtual void doAction() = 0;
};

//templated animation action
//allows us to specify an action that calls a function of a class.
//(so that class doesn't have to inherit QDeclarativeAbstractAnimationAction)
template<class T, void (T::*method)()>
class QAnimationActionProxy : public QAbstractAnimationAction
{
public:
    QAnimationActionProxy(T *p) : m_p(p) {}
    virtual void doAction() { (m_p->*method)(); }

private:
    T *m_p;
};

//performs an action of type QAbstractAnimationAction
class Q_AUTOTEST_EXPORT QActionAnimation : public QAbstractAnimation
{
    Q_OBJECT
public:
    QActionAnimation(QObject *parent = 0) : QAbstractAnimation(parent), animAction(0), policy(KeepWhenStopped) {}
    QActionAnimation(QAbstractAnimationAction *action, QObject *parent = 0)
        : QAbstractAnimation(parent), animAction(action), policy(KeepWhenStopped) {}
    ~QActionAnimation() { if (policy == DeleteWhenStopped) { delete animAction; animAction = 0; } }
    virtual int duration() const { return 0; }
    void setAnimAction(QAbstractAnimationAction *action, DeletionPolicy p)
    {
        if (state() == Running)
            stop();
        if (policy == DeleteWhenStopped)
            delete animAction;
        animAction = action;
        policy = p;
    }
protected:
    virtual void updateCurrentTime(int) {}

    virtual void updateState(State newState, State /*oldState*/)
    {
        if (newState == Running) {
            if (animAction) {
                animAction->doAction();
                if (state() == Stopped && policy == DeleteWhenStopped) {
                    delete animAction;
                    animAction = 0;
                }
            }
        }
    }

private:
    QAbstractAnimationAction *animAction;
    DeletionPolicy policy;
};

class QDeclarativeBulkValueUpdater
{
public:
    virtual ~QDeclarativeBulkValueUpdater() {}
    virtual void setValue(qreal value) = 0;
};

//animates QDeclarativeBulkValueUpdater (assumes start and end values will be reals or compatible)
class Q_AUTOTEST_EXPORT QDeclarativeBulkValueAnimator : public QVariantAnimation
{
    Q_OBJECT
public:
    QDeclarativeBulkValueAnimator(QObject *parent = 0) : QVariantAnimation(parent), animValue(0), fromSourced(0), policy(KeepWhenStopped) {}
    ~QDeclarativeBulkValueAnimator() { if (policy == DeleteWhenStopped) { delete animValue; animValue = 0; } }
    void setAnimValue(QDeclarativeBulkValueUpdater *value, DeletionPolicy p)
    {
        if (state() == Running)
            stop();
        if (policy == DeleteWhenStopped)
            delete animValue;
        animValue = value;
        policy = p;
    }
    void setFromSourcedValue(bool *value)
    {
        fromSourced = value;
    }
protected:
    virtual void updateCurrentValue(const QVariant &value)
    {
        if (state() == QAbstractAnimation::Stopped)
            return;

        if (animValue)
            animValue->setValue(value.toReal());
    }
    virtual void updateState(State newState, State oldState)
    {
        QVariantAnimation::updateState(newState, oldState);
        if (newState == Running) {
            //check for new from every loop
            if (fromSourced)
                *fromSourced = false;
        }
    }

private:
    QDeclarativeBulkValueUpdater *animValue;
    bool *fromSourced;
    DeletionPolicy policy;
};

//an animation that just gives a tick
template<class T, void (T::*method)(int)>
class QTickAnimationProxy : public QAbstractAnimation
{
    //Q_OBJECT //doesn't work with templating
public:
    QTickAnimationProxy(T *p, QObject *parent = 0) : QAbstractAnimation(parent), m_p(p) {}
    virtual int duration() const { return -1; }
protected:
    virtual void updateCurrentTime(int msec) { (m_p->*method)(msec); }

private:
    T *m_p;
};

class QDeclarativeAbstractAnimationPrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeAbstractAnimation)
public:
    QDeclarativeAbstractAnimationPrivate()
    : running(false), paused(false), alwaysRunToEnd(false),
      connectedTimeLine(false), componentComplete(true),
      avoidPropertyValueSourceStart(false), disableUserControl(false),
      registered(false), loopCount(1), group(0) {}

    bool running:1;
    bool paused:1;
    bool alwaysRunToEnd:1;
    bool connectedTimeLine:1;
    bool componentComplete:1;
    bool avoidPropertyValueSourceStart:1;
    bool disableUserControl:1;
    bool registered:1;

    int loopCount;

    void commence();

    QDeclarativeProperty defaultProperty;

    QDeclarativeAnimationGroup *group;

    static QDeclarativeProperty createProperty(QObject *obj, const QString &str, QObject *infoObj);
};

class QDeclarativePauseAnimationPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePauseAnimation)
public:
    QDeclarativePauseAnimationPrivate()
    : QDeclarativeAbstractAnimationPrivate(), pa(0) {}

    void init();

    QPauseAnimation *pa;
};

class QDeclarativeScriptActionPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeScriptAction)
public:
    QDeclarativeScriptActionPrivate();

    void init();

    QDeclarativeScriptString script;
    QString name;
    QDeclarativeScriptString runScriptScript;
    bool hasRunScriptScript;
    bool reversing;

    void execute();

    QAnimationActionProxy<QDeclarativeScriptActionPrivate,
                  &QDeclarativeScriptActionPrivate::execute> proxy;
    QActionAnimation *rsa;
};

class QDeclarativePropertyActionPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePropertyAction)
public:
    QDeclarativePropertyActionPrivate()
    : QDeclarativeAbstractAnimationPrivate(), target(0), spa(0) {}

    void init();

    QObject *target;
    QString propertyName;
    QString properties;
    QList<QObject *> targets;
    QList<QObject *> exclude;

    QDeclarativeNullableValue<QVariant> value;

    QActionAnimation *spa;
};

class QDeclarativeAnimationGroupPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeAnimationGroup)
public:
    QDeclarativeAnimationGroupPrivate()
    : QDeclarativeAbstractAnimationPrivate(), ag(0) {}

    static void append_animation(QDeclarativeListProperty<QDeclarativeAbstractAnimation> *list, QDeclarativeAbstractAnimation *role);
    static void clear_animation(QDeclarativeListProperty<QDeclarativeAbstractAnimation> *list);
    QList<QDeclarativeAbstractAnimation *> animations;
    QAnimationGroup *ag;
};

class QDeclarativePropertyAnimationPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativePropertyAnimation)
public:
    QDeclarativePropertyAnimationPrivate()
    : QDeclarativeAbstractAnimationPrivate(), target(0), fromSourced(false), fromIsDefined(false), toIsDefined(false),
      rangeIsSet(false), defaultToInterpolatorType(0), interpolatorType(0), interpolator(0), va(0), actions(0) {}

    void init();

    QVariant from;
    QVariant to;

    QObject *target;
    QString propertyName;
    QString properties;
    QList<QObject *> targets;
    QList<QObject *> exclude;
    QString defaultProperties;

    bool fromSourced;
    bool fromIsDefined:1;
    bool toIsDefined:1;
    bool rangeIsSet:1;
    bool defaultToInterpolatorType:1;
    int interpolatorType;
    QVariantAnimation::Interpolator interpolator;

    QDeclarativeBulkValueAnimator *va;

    // for animations that don't use the QDeclarativeBulkValueAnimator
    QDeclarativeStateActions *actions;

    static QVariant interpolateVariant(const QVariant &from, const QVariant &to, qreal progress);
    static void convertVariant(QVariant &variant, int type);
};

class QDeclarativeRotationAnimationPrivate : public QDeclarativePropertyAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeRotationAnimation)
public:
    QDeclarativeRotationAnimationPrivate() : direction(QDeclarativeRotationAnimation::Numerical) {}

    QDeclarativeRotationAnimation::RotationDirection direction;
};

class QDeclarativeParentAnimationPrivate : public QDeclarativeAnimationGroupPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeParentAnimation)
public:
    QDeclarativeParentAnimationPrivate()
    : QDeclarativeAnimationGroupPrivate(), target(0), newParent(0),
       via(0), topLevelGroup(0), startAction(0), endAction(0) {}

    QDeclarativeItem *target;
    QDeclarativeItem *newParent;
    QDeclarativeItem *via;

    QSequentialAnimationGroup *topLevelGroup;
    QActionAnimation *startAction;
    QActionAnimation *endAction;

    QPointF computeTransformOrigin(QDeclarativeItem::TransformOrigin origin, qreal width, qreal height) const;
};

class QDeclarativeAnchorAnimationPrivate : public QDeclarativeAbstractAnimationPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeAnchorAnimation)
public:
    QDeclarativeAnchorAnimationPrivate() : rangeIsSet(false), va(0),
        interpolator(QVariantAnimationPrivate::getInterpolator(QMetaType::QReal)) {}

    bool rangeIsSet;
    QDeclarativeBulkValueAnimator *va;
    QVariantAnimation::Interpolator interpolator;
    QList<QDeclarativeItem*> targets;
};

class Q_AUTOTEST_EXPORT QDeclarativeAnimationPropertyUpdater : public QDeclarativeBulkValueUpdater
{
public:
    QDeclarativeStateActions actions;
    int interpolatorType;       //for Number/ColorAnimation
    int prevInterpolatorType;   //for generic
    QVariantAnimation::Interpolator interpolator;
    bool reverse;
    bool fromSourced;
    bool fromDefined;
    bool *wasDeleted;
    QDeclarativeAnimationPropertyUpdater() : prevInterpolatorType(0), wasDeleted(0) {}
    ~QDeclarativeAnimationPropertyUpdater() { if (wasDeleted) *wasDeleted = true; }
    void setValue(qreal v);
};

QT_END_NAMESPACE

#endif // QDECLARATIVEANIMATION_P_H
