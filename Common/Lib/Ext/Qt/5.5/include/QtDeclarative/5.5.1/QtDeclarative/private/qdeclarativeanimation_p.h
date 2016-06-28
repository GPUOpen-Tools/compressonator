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

#ifndef QDECLARATIVEANIMATION_H
#define QDECLARATIVEANIMATION_H

#include "private/qdeclarativetransition_p.h"
#include "private/qdeclarativestate_p.h"
#include <QtGui/qvector3d.h>

#include <qdeclarativepropertyvaluesource.h>
#include <qdeclarative.h>
#include <qdeclarativescriptstring.h>

#include <QtCore/qvariant.h>
#include <QtCore/qeasingcurve.h>
#include <QtCore/QAbstractAnimation>
#include <QtGui/qcolor.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

#if defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && \
    (defined(Q_CC_CLANG) || (__GNUC__ * 100 + __GNUC_MINOR__) > 406)
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Woverloaded-virtual"
#endif

class QDeclarativeAbstractAnimationPrivate;
class QDeclarativeAnimationGroup;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeAbstractAnimation : public QObject, public QDeclarativePropertyValueSource, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAbstractAnimation)

    Q_INTERFACES(QDeclarativeParserStatus)
    Q_INTERFACES(QDeclarativePropertyValueSource)
    Q_ENUMS(Loops)
    Q_PROPERTY(bool running READ isRunning WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool paused READ isPaused WRITE setPaused NOTIFY pausedChanged)
    Q_PROPERTY(bool alwaysRunToEnd READ alwaysRunToEnd WRITE setAlwaysRunToEnd NOTIFY alwaysRunToEndChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopCountChanged)
    Q_CLASSINFO("DefaultMethod", "start()")

public:
    QDeclarativeAbstractAnimation(QObject *parent=0);
    virtual ~QDeclarativeAbstractAnimation();

    enum Loops { Infinite = -2 };

    bool isRunning() const;
    void setRunning(bool);
    bool isPaused() const;
    void setPaused(bool);
    bool alwaysRunToEnd() const;
    void setAlwaysRunToEnd(bool);

    int loops() const;
    void setLoops(int);

    int currentTime();
    void setCurrentTime(int);

    QDeclarativeAnimationGroup *group() const;
    void setGroup(QDeclarativeAnimationGroup *);

    void setDefaultTarget(const QDeclarativeProperty &);
    void setDisableUserControl();

    void classBegin();
    void componentComplete();

Q_SIGNALS:
    void started();
    void completed();
    void runningChanged(bool);
    void pausedChanged(bool);
    void alwaysRunToEndChanged(bool);
    void loopCountChanged(int);

public Q_SLOTS:
    void restart();
    void start();
    void pause();
    void resume();
    void stop();
    void complete();

protected:
    QDeclarativeAbstractAnimation(QDeclarativeAbstractAnimationPrivate &dd, QObject *parent);

public:
    enum TransitionDirection { Forward, Backward };
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation() = 0;

private Q_SLOTS:
    void timelineComplete();
    void componentFinalized();
private:
    virtual void setTarget(const QDeclarativeProperty &);
    void notifyRunningChanged(bool running);
    friend class QDeclarativeBehavior;


};

class QDeclarativePauseAnimationPrivate;
class Q_AUTOTEST_EXPORT QDeclarativePauseAnimation : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePauseAnimation)

    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)

public:
    QDeclarativePauseAnimation(QObject *parent=0);
    virtual ~QDeclarativePauseAnimation();

    int duration() const;
    void setDuration(int);

Q_SIGNALS:
    void durationChanged(int);

protected:
    virtual QAbstractAnimation *qtAnimation();
};

class QDeclarativeScriptActionPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeScriptAction : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeScriptAction)

    Q_PROPERTY(QDeclarativeScriptString script READ script WRITE setScript)
    Q_PROPERTY(QString scriptName READ stateChangeScriptName WRITE setStateChangeScriptName)

public:
    QDeclarativeScriptAction(QObject *parent=0);
    virtual ~QDeclarativeScriptAction();

    QDeclarativeScriptString script() const;
    void setScript(const QDeclarativeScriptString &);

    QString stateChangeScriptName() const;
    void setStateChangeScriptName(const QString &);

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QDeclarativePropertyActionPrivate;
class QDeclarativePropertyAction : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyAction)

    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(QString properties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> targets READ targets)
    Q_PROPERTY(QDeclarativeListProperty<QObject> exclude READ exclude)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    QDeclarativePropertyAction(QObject *parent=0);
    virtual ~QDeclarativePropertyAction();

    QObject *target() const;
    void setTarget(QObject *);

    QString property() const;
    void setProperty(const QString &);

    QString properties() const;
    void setProperties(const QString &);

    QDeclarativeListProperty<QObject> targets();
    QDeclarativeListProperty<QObject> exclude();

    QVariant value() const;
    void setValue(const QVariant &);

Q_SIGNALS:
    void valueChanged(const QVariant &);
    void propertiesChanged(const QString &);
    void targetChanged();
    void propertyChanged();

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QDeclarativeItem;
class QDeclarativePropertyAnimationPrivate;
class Q_AUTOTEST_EXPORT QDeclarativePropertyAnimation : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyAnimation)

    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QVariant from READ from WRITE setFrom NOTIFY fromChanged)
    Q_PROPERTY(QVariant to READ to WRITE setTo NOTIFY toChanged)
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing NOTIFY easingChanged)
    Q_PROPERTY(QObject *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(QString properties READ properties WRITE setProperties NOTIFY propertiesChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> targets READ targets)
    Q_PROPERTY(QDeclarativeListProperty<QObject> exclude READ exclude)

public:
    QDeclarativePropertyAnimation(QObject *parent=0);
    virtual ~QDeclarativePropertyAnimation();

    virtual int duration() const;
    virtual void setDuration(int);

    QVariant from() const;
    void setFrom(const QVariant &);

    QVariant to() const;
    void setTo(const QVariant &);

    QEasingCurve easing() const;
    void setEasing(const QEasingCurve &);

    QObject *target() const;
    void setTarget(QObject *);

    QString property() const;
    void setProperty(const QString &);

    QString properties() const;
    void setProperties(const QString &);

    QDeclarativeListProperty<QObject> targets();
    QDeclarativeListProperty<QObject> exclude();

protected:
    QDeclarativePropertyAnimation(QDeclarativePropertyAnimationPrivate &dd, QObject *parent);
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();

Q_SIGNALS:
    void durationChanged(int);
    void fromChanged(QVariant);
    void toChanged(QVariant);
    void easingChanged(const QEasingCurve &);
    void propertiesChanged(const QString &);
    void targetChanged();
    void propertyChanged();
};

class Q_AUTOTEST_EXPORT QDeclarativeColorAnimation : public QDeclarativePropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyAnimation)
    Q_PROPERTY(QColor from READ from WRITE setFrom)
    Q_PROPERTY(QColor to READ to WRITE setTo)

public:
    QDeclarativeColorAnimation(QObject *parent=0);
    virtual ~QDeclarativeColorAnimation();

    QColor from() const;
    void setFrom(const QColor &);

    QColor to() const;
    void setTo(const QColor &);
};

class Q_AUTOTEST_EXPORT QDeclarativeNumberAnimation : public QDeclarativePropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyAnimation)

    Q_PROPERTY(qreal from READ from WRITE setFrom)
    Q_PROPERTY(qreal to READ to WRITE setTo)

public:
    QDeclarativeNumberAnimation(QObject *parent=0);
    virtual ~QDeclarativeNumberAnimation();

    qreal from() const;
    void setFrom(qreal);

    qreal to() const;
    void setTo(qreal);

protected:
    QDeclarativeNumberAnimation(QDeclarativePropertyAnimationPrivate &dd, QObject *parent);

private:
    void init();
};

class Q_AUTOTEST_EXPORT QDeclarativeVector3dAnimation : public QDeclarativePropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativePropertyAnimation)

    Q_PROPERTY(QVector3D from READ from WRITE setFrom)
    Q_PROPERTY(QVector3D to READ to WRITE setTo)

public:
    QDeclarativeVector3dAnimation(QObject *parent=0);
    virtual ~QDeclarativeVector3dAnimation();

    QVector3D from() const;
    void setFrom(QVector3D);

    QVector3D to() const;
    void setTo(QVector3D);
};

class QDeclarativeRotationAnimationPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeRotationAnimation : public QDeclarativePropertyAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeRotationAnimation)
    Q_ENUMS(RotationDirection)

    Q_PROPERTY(qreal from READ from WRITE setFrom)
    Q_PROPERTY(qreal to READ to WRITE setTo)
    Q_PROPERTY(RotationDirection direction READ direction WRITE setDirection NOTIFY directionChanged)

public:
    QDeclarativeRotationAnimation(QObject *parent=0);
    virtual ~QDeclarativeRotationAnimation();

    qreal from() const;
    void setFrom(qreal);

    qreal to() const;
    void setTo(qreal);

    enum RotationDirection { Numerical, Shortest, Clockwise, Counterclockwise };
    RotationDirection direction() const;
    void setDirection(RotationDirection direction);

Q_SIGNALS:
    void directionChanged();
};

class QDeclarativeAnimationGroupPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeAnimationGroup : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAnimationGroup)

    Q_CLASSINFO("DefaultProperty", "animations")
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeAbstractAnimation> animations READ animations)

public:
    QDeclarativeAnimationGroup(QObject *parent);
    virtual ~QDeclarativeAnimationGroup();

    QDeclarativeListProperty<QDeclarativeAbstractAnimation> animations();
    friend class QDeclarativeAbstractAnimation;

protected:
    QDeclarativeAnimationGroup(QDeclarativeAnimationGroupPrivate &dd, QObject *parent);
};

class QDeclarativeSequentialAnimation : public QDeclarativeAnimationGroup
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAnimationGroup)

public:
    QDeclarativeSequentialAnimation(QObject *parent=0);
    virtual ~QDeclarativeSequentialAnimation();

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QDeclarativeParallelAnimation : public QDeclarativeAnimationGroup
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAnimationGroup)

public:
    QDeclarativeParallelAnimation(QObject *parent=0);
    virtual ~QDeclarativeParallelAnimation();

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QDeclarativeParentAnimationPrivate;
class QDeclarativeParentAnimation : public QDeclarativeAnimationGroup
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeParentAnimation)

    Q_PROPERTY(QDeclarativeItem *target READ target WRITE setTarget NOTIFY targetChanged)
    Q_PROPERTY(QDeclarativeItem *newParent READ newParent WRITE setNewParent NOTIFY newParentChanged)
    Q_PROPERTY(QDeclarativeItem *via READ via WRITE setVia NOTIFY viaChanged)

public:
    QDeclarativeParentAnimation(QObject *parent=0);
    virtual ~QDeclarativeParentAnimation();

    QDeclarativeItem *target() const;
    void setTarget(QDeclarativeItem *);

    QDeclarativeItem *newParent() const;
    void setNewParent(QDeclarativeItem *);

    QDeclarativeItem *via() const;
    void setVia(QDeclarativeItem *);

Q_SIGNALS:
    void targetChanged();
    void newParentChanged();
    void viaChanged();

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

class QDeclarativeAnchorAnimationPrivate;
class QDeclarativeAnchorAnimation : public QDeclarativeAbstractAnimation
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAnchorAnimation)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeItem> targets READ targets)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QEasingCurve easing READ easing WRITE setEasing NOTIFY easingChanged)

public:
    QDeclarativeAnchorAnimation(QObject *parent=0);
    virtual ~QDeclarativeAnchorAnimation();

    QDeclarativeListProperty<QDeclarativeItem> targets();

    int duration() const;
    void setDuration(int);

    QEasingCurve easing() const;
    void setEasing(const QEasingCurve &);

Q_SIGNALS:
    void durationChanged(int);
    void easingChanged(const QEasingCurve&);

protected:
    virtual void transition(QDeclarativeStateActions &actions,
                            QDeclarativeProperties &modified,
                            TransitionDirection direction);
    virtual QAbstractAnimation *qtAnimation();
};

#if defined(Q_CC_GNU) && !defined(Q_CC_INTEL) && \
    (defined(Q_CC_CLANG) || (__GNUC__ * 100 + __GNUC_MINOR__) > 406)
#  pragma GCC diagnostic pop
#endif

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeAbstractAnimation)
QML_DECLARE_TYPE(QDeclarativePauseAnimation)
QML_DECLARE_TYPE(QDeclarativeScriptAction)
QML_DECLARE_TYPE(QDeclarativePropertyAction)
QML_DECLARE_TYPE(QDeclarativePropertyAnimation)
QML_DECLARE_TYPE(QDeclarativeColorAnimation)
QML_DECLARE_TYPE(QDeclarativeNumberAnimation)
QML_DECLARE_TYPE(QDeclarativeSequentialAnimation)
QML_DECLARE_TYPE(QDeclarativeParallelAnimation)
QML_DECLARE_TYPE(QDeclarativeVector3dAnimation)
QML_DECLARE_TYPE(QDeclarativeRotationAnimation)
QML_DECLARE_TYPE(QDeclarativeParentAnimation)
QML_DECLARE_TYPE(QDeclarativeAnchorAnimation)

#endif // QDECLARATIVEANIMATION_H
