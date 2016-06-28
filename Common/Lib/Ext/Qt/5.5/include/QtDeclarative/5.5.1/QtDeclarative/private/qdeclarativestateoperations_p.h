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

#ifndef QDECLARATIVESTATEOPERATIONS_H
#define QDECLARATIVESTATEOPERATIONS_H

#include "private/qdeclarativestate_p.h"

#include <qdeclarativeitem.h>
#include <private/qdeclarativeanchors_p.h>
#include <qdeclarativescriptstring.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeParentChangePrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeParentChange : public QDeclarativeStateOperation, public QDeclarativeActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeParentChange)

    Q_PROPERTY(QDeclarativeItem *target READ object WRITE setObject)
    Q_PROPERTY(QDeclarativeItem *parent READ parent WRITE setParent)
    Q_PROPERTY(QDeclarativeScriptString x READ x WRITE setX)
    Q_PROPERTY(QDeclarativeScriptString y READ y WRITE setY)
    Q_PROPERTY(QDeclarativeScriptString width READ width WRITE setWidth)
    Q_PROPERTY(QDeclarativeScriptString height READ height WRITE setHeight)
    Q_PROPERTY(QDeclarativeScriptString scale READ scale WRITE setScale)
    Q_PROPERTY(QDeclarativeScriptString rotation READ rotation WRITE setRotation)
public:
    QDeclarativeParentChange(QObject *parent=0);
    ~QDeclarativeParentChange();

    QDeclarativeItem *object() const;
    void setObject(QDeclarativeItem *);

    QDeclarativeItem *parent() const;
    void setParent(QDeclarativeItem *);

    QDeclarativeItem *originalParent() const;

    QDeclarativeScriptString x() const;
    void setX(QDeclarativeScriptString x);
    bool xIsSet() const;

    QDeclarativeScriptString y() const;
    void setY(QDeclarativeScriptString y);
    bool yIsSet() const;

    QDeclarativeScriptString width() const;
    void setWidth(QDeclarativeScriptString width);
    bool widthIsSet() const;

    QDeclarativeScriptString height() const;
    void setHeight(QDeclarativeScriptString height);
    bool heightIsSet() const;

    QDeclarativeScriptString scale() const;
    void setScale(QDeclarativeScriptString scale);
    bool scaleIsSet() const;

    QDeclarativeScriptString rotation() const;
    void setRotation(QDeclarativeScriptString rotation);
    bool rotationIsSet() const;

    virtual ActionList actions();

    virtual void saveOriginals();
    //virtual void copyOriginals(QDeclarativeActionEvent*);
    virtual void execute(Reason reason = ActualChange);
    virtual bool isReversable();
    virtual void reverse(Reason reason = ActualChange);
    virtual QString typeName() const;
    virtual bool override(QDeclarativeActionEvent*other);
    virtual void rewind();
    virtual void saveCurrentValues();
};

class QDeclarativeStateChangeScriptPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeStateChangeScript : public QDeclarativeStateOperation, public QDeclarativeActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeStateChangeScript)

    Q_PROPERTY(QDeclarativeScriptString script READ script WRITE setScript)
    Q_PROPERTY(QString name READ name WRITE setName)

public:
    QDeclarativeStateChangeScript(QObject *parent=0);
    ~QDeclarativeStateChangeScript();

    virtual ActionList actions();

    virtual QString typeName() const;

    QDeclarativeScriptString script() const;
    void setScript(const QDeclarativeScriptString &);

    QString name() const;
    void setName(const QString &);

    virtual void execute(Reason reason = ActualChange);
};

class QDeclarativeAnchorChanges;
class QDeclarativeAnchorSetPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeAnchorSet : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeScriptString left READ left WRITE setLeft RESET resetLeft)
    Q_PROPERTY(QDeclarativeScriptString right READ right WRITE setRight RESET resetRight)
    Q_PROPERTY(QDeclarativeScriptString horizontalCenter READ horizontalCenter WRITE setHorizontalCenter RESET resetHorizontalCenter)
    Q_PROPERTY(QDeclarativeScriptString top READ top WRITE setTop RESET resetTop)
    Q_PROPERTY(QDeclarativeScriptString bottom READ bottom WRITE setBottom RESET resetBottom)
    Q_PROPERTY(QDeclarativeScriptString verticalCenter READ verticalCenter WRITE setVerticalCenter RESET resetVerticalCenter)
    Q_PROPERTY(QDeclarativeScriptString baseline READ baseline WRITE setBaseline RESET resetBaseline)
    //Q_PROPERTY(QDeclarativeItem *fill READ fill WRITE setFill RESET resetFill)
    //Q_PROPERTY(QDeclarativeItem *centerIn READ centerIn WRITE setCenterIn RESET resetCenterIn)

    /*Q_PROPERTY(qreal margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(qreal horizontalCenterOffset READ horizontalCenterOffset WRITE setHorizontalCenterOffset NOTIFY horizontalCenterOffsetChanged())
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(qreal verticalCenterOffset READ verticalCenterOffset WRITE setVerticalCenterOffset NOTIFY verticalCenterOffsetChanged())
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged())*/

public:
    QDeclarativeAnchorSet(QObject *parent=0);
    virtual ~QDeclarativeAnchorSet();

    QDeclarativeScriptString left() const;
    void setLeft(const QDeclarativeScriptString &edge);
    void resetLeft();

    QDeclarativeScriptString right() const;
    void setRight(const QDeclarativeScriptString &edge);
    void resetRight();

    QDeclarativeScriptString horizontalCenter() const;
    void setHorizontalCenter(const QDeclarativeScriptString &edge);
    void resetHorizontalCenter();

    QDeclarativeScriptString top() const;
    void setTop(const QDeclarativeScriptString &edge);
    void resetTop();

    QDeclarativeScriptString bottom() const;
    void setBottom(const QDeclarativeScriptString &edge);
    void resetBottom();

    QDeclarativeScriptString verticalCenter() const;
    void setVerticalCenter(const QDeclarativeScriptString &edge);
    void resetVerticalCenter();

    QDeclarativeScriptString baseline() const;
    void setBaseline(const QDeclarativeScriptString &edge);
    void resetBaseline();

    QDeclarativeItem *fill() const;
    void setFill(QDeclarativeItem *);
    void resetFill();

    QDeclarativeItem *centerIn() const;
    void setCenterIn(QDeclarativeItem *);
    void resetCenterIn();

    /*qreal leftMargin() const;
    void setLeftMargin(qreal);

    qreal rightMargin() const;
    void setRightMargin(qreal);

    qreal horizontalCenterOffset() const;
    void setHorizontalCenterOffset(qreal);

    qreal topMargin() const;
    void setTopMargin(qreal);

    qreal bottomMargin() const;
    void setBottomMargin(qreal);

    qreal margins() const;
    void setMargins(qreal);

    qreal verticalCenterOffset() const;
    void setVerticalCenterOffset(qreal);

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);*/

    QDeclarativeAnchors::Anchors usedAnchors() const;

/*Q_SIGNALS:
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();
    void marginsChanged();
    void verticalCenterOffsetChanged();
    void horizontalCenterOffsetChanged();
    void baselineOffsetChanged();*/

private:
    friend class QDeclarativeAnchorChanges;
    Q_DISABLE_COPY(QDeclarativeAnchorSet)
    Q_DECLARE_PRIVATE(QDeclarativeAnchorSet)
};

class QDeclarativeAnchorChangesPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeAnchorChanges : public QDeclarativeStateOperation, public QDeclarativeActionEvent
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(QDeclarativeAnchorChanges)

    Q_PROPERTY(QDeclarativeItem *target READ object WRITE setObject)
    Q_PROPERTY(QDeclarativeAnchorSet *anchors READ anchors CONSTANT)

public:
    QDeclarativeAnchorChanges(QObject *parent=0);
    ~QDeclarativeAnchorChanges();

    virtual ActionList actions();

    QDeclarativeAnchorSet *anchors();

    QDeclarativeItem *object() const;
    void setObject(QDeclarativeItem *);

    virtual void execute(Reason reason = ActualChange);
    virtual bool isReversable();
    virtual void reverse(Reason reason = ActualChange);
    virtual QString typeName() const;
    virtual bool override(QDeclarativeActionEvent*other);
    virtual bool changesBindings();
    virtual void saveOriginals();
    virtual bool needsCopy() { return true; }
    virtual void copyOriginals(QDeclarativeActionEvent*);
    virtual void clearBindings();
    virtual void rewind();
    virtual void saveCurrentValues();

    QList<QDeclarativeAction> additionalActions();
    virtual void saveTargetValues();
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeParentChange)
QML_DECLARE_TYPE(QDeclarativeStateChangeScript)
QML_DECLARE_TYPE(QDeclarativeAnchorSet)
QML_DECLARE_TYPE(QDeclarativeAnchorChanges)

#endif // QDECLARATIVESTATEOPERATIONS_H
