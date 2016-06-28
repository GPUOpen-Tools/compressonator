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

#ifndef QDECLARATIVELAYOUTS_P_H
#define QDECLARATIVELAYOUTS_P_H

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

#include "private/qdeclarativepositioners_p.h"

#include "private/qdeclarativeimplicitsizeitem_p_p.h"

#include <qdeclarativestate_p.h>
#include <qdeclarativetransitionmanager_p_p.h>
#include <qdeclarativestateoperations_p.h>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QDebug>

QT_BEGIN_NAMESPACE
class QDeclarativeBasePositionerPrivate : public QDeclarativeImplicitSizeItemPrivate, public QDeclarativeItemChangeListener
{
    Q_DECLARE_PUBLIC(QDeclarativeBasePositioner)

public:
    QDeclarativeBasePositionerPrivate()
        : spacing(0), type(QDeclarativeBasePositioner::None)
        , moveTransition(0), addTransition(0), queuedPositioning(false)
        , doingPositioning(false), anchorConflict(false), layoutDirection(Qt::LeftToRight)
    {
    }

    void init(QDeclarativeBasePositioner::PositionerType at)
    {
        type = at;
    }

    int spacing;

    QDeclarativeBasePositioner::PositionerType type;
    QDeclarativeTransition *moveTransition;
    QDeclarativeTransition *addTransition;
    QDeclarativeStateOperation::ActionList addActions;
    QDeclarativeStateOperation::ActionList moveActions;
    QDeclarativeTransitionManager addTransitionManager;
    QDeclarativeTransitionManager moveTransitionManager;

    void watchChanges(QGraphicsObject *other);
    void unwatchChanges(QGraphicsObject* other);
    bool queuedPositioning : 1;
    bool doingPositioning : 1;
    bool anchorConflict : 1;

    Qt::LayoutDirection layoutDirection;


    void schedulePositioning()
    {
        Q_Q(QDeclarativeBasePositioner);
        if(!queuedPositioning){
            QTimer::singleShot(0,q,SLOT(prePositioning()));
            queuedPositioning = true;
        }
    }

    void mirrorChange() {
        Q_Q(QDeclarativeBasePositioner);
        if (type != QDeclarativeBasePositioner::Vertical)
            q->prePositioning();
    }
    bool isLeftToRight() const {
        if (type == QDeclarativeBasePositioner::Vertical)
            return true;
        else
            return effectiveLayoutMirror ? layoutDirection == Qt::RightToLeft : layoutDirection == Qt::LeftToRight;
    }

    virtual void itemSiblingOrderChanged(QDeclarativeItem* other)
    {
        Q_UNUSED(other);
        //Delay is due to many children often being reordered at once
        //And we only want to reposition them all once
        schedulePositioning();
    }

    void itemGeometryChanged(QDeclarativeItem *, const QRectF &newGeometry, const QRectF &oldGeometry)
    {
        Q_Q(QDeclarativeBasePositioner);
        if (newGeometry.size() != oldGeometry.size())
            q->prePositioning();
    }

    virtual void itemVisibilityChanged(QDeclarativeItem *)
    {
        schedulePositioning();
    }
    virtual void itemOpacityChanged(QDeclarativeItem *)
    {
        Q_Q(QDeclarativeBasePositioner);
        q->prePositioning();
    }

    void itemDestroyed(QDeclarativeItem *item)
    {
        Q_Q(QDeclarativeBasePositioner);
        q->positionedItems.removeOne(QDeclarativeBasePositioner::PositionedItem(item));
    }

    static Qt::LayoutDirection getLayoutDirection(const QDeclarativeBasePositioner *positioner)
    {
        return positioner->d_func()->layoutDirection;
    }

    static Qt::LayoutDirection getEffectiveLayoutDirection(const QDeclarativeBasePositioner *positioner)
    {
        if (positioner->d_func()->effectiveLayoutMirror)
            return positioner->d_func()->layoutDirection == Qt::RightToLeft ? Qt::LeftToRight : Qt::RightToLeft;
        else
            return positioner->d_func()->layoutDirection;
    }
};

QT_END_NAMESPACE
#endif
