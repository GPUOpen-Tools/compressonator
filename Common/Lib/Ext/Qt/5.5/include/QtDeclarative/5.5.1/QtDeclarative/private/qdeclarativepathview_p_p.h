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

#ifndef QDECLARATIVEPATHVIEW_P_H
#define QDECLARATIVEPATHVIEW_P_H

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

#include "private/qdeclarativepathview_p.h"

#include "private/qdeclarativeitem_p.h"
#include "private/qdeclarativevisualitemmodel_p.h"

#include <qdeclarative.h>
#include <qdeclarativeanimation_p_p.h>
#include <private/qdeclarativeguard_p.h>

#include <qdatetime.h>

QT_BEGIN_NAMESPACE

class QDeclarativeOpenMetaObjectType;
class QDeclarativePathViewAttached;
class QDeclarativePathViewPrivate : public QDeclarativeItemPrivate, public QDeclarativeItemChangeListener
{
    Q_DECLARE_PUBLIC(QDeclarativePathView)

public:
    QDeclarativePathViewPrivate()
      : path(0), currentIndex(0), currentItemOffset(0.0), startPc(0), lastDist(0)
        , lastElapsed(0), offset(0.0), offsetAdj(0.0), mappedRange(1.0)
        , stealMouse(false), ownModel(false), interactive(true), haveHighlightRange(true)
        , autoHighlight(true), highlightUp(false), layoutScheduled(false)
        , moving(false), flicking(false)
        , dragMargin(0), deceleration(100)
        , moveOffset(this, &QDeclarativePathViewPrivate::setAdjustedOffset)
        , firstIndex(-1), pathItems(-1), requestedIndex(-1)
        , moveReason(Other), moveDirection(Shortest), attType(0), highlightComponent(0), highlightItem(0)
        , moveHighlight(this, &QDeclarativePathViewPrivate::setHighlightPosition)
        , highlightPosition(0)
        , highlightRangeStart(0), highlightRangeEnd(0)
        , highlightRangeMode(QDeclarativePathView::StrictlyEnforceRange)
        , highlightMoveDuration(300), modelCount(0)
    {
    }

    void init();

    void itemGeometryChanged(QDeclarativeItem *item, const QRectF &newGeometry, const QRectF &oldGeometry) {
        if ((newGeometry.size() != oldGeometry.size())
            && (!highlightItem || item != highlightItem)) {
            if (QDeclarativePathViewAttached *att = attached(item))
                att->m_percent = -1;
            scheduleLayout();
        }
    }

    void scheduleLayout() {
        Q_Q(QDeclarativePathView);
        if (!layoutScheduled) {
            layoutScheduled = true;
            QCoreApplication::postEvent(q, new QEvent(QEvent::User), Qt::HighEventPriority);
        }
    }

    QDeclarativeItem *getItem(int modelIndex);
    void releaseItem(QDeclarativeItem *item);
    QDeclarativePathViewAttached *attached(QDeclarativeItem *item);
    void clear();
    void updateMappedRange();
    qreal positionOfIndex(qreal index) const;
    void createHighlight();
    void updateHighlight();
    void setHighlightPosition(qreal pos);
    bool isValid() const {
        return model && model->count() > 0 && model->isValid() && path;
    }

    void handleMousePressEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void handleMouseReleaseEvent(QGraphicsSceneMouseEvent *);

    int calcCurrentIndex();
    void updateCurrent();
    static void fixOffsetCallback(void*);
    void fixOffset();
    void setOffset(qreal offset);
    void setAdjustedOffset(qreal offset);
    void regenerate();
    void updateItem(QDeclarativeItem *, qreal);
    void snapToCurrent();
    QPointF pointNear(const QPointF &point, qreal *nearPercent=0) const;

    QDeclarativePath *path;
    int currentIndex;
    QDeclarativeGuard<QDeclarativeItem> currentItem;
    qreal currentItemOffset;
    qreal startPc;
    QPointF startPoint;
    qreal lastDist;
    int lastElapsed;
    qreal offset;
    qreal offsetAdj;
    qreal mappedRange;
    bool stealMouse : 1;
    bool ownModel : 1;
    bool interactive : 1;
    bool haveHighlightRange : 1;
    bool autoHighlight : 1;
    bool highlightUp : 1;
    bool layoutScheduled : 1;
    bool moving : 1;
    bool flicking : 1;
    QElapsedTimer lastPosTime;
    QPointF lastPos;
    qreal dragMargin;
    qreal deceleration;
    QDeclarativeTimeLine tl;
    QDeclarativeTimeLineValueProxy<QDeclarativePathViewPrivate> moveOffset;
    int firstIndex;
    int pathItems;
    int requestedIndex;
    QList<QDeclarativeItem *> items;
    QList<QDeclarativeItem *> itemCache;
    QDeclarativeGuard<QDeclarativeVisualModel> model;
    QVariant modelVariant;
    enum MovementReason { Other, SetIndex, Mouse };
    MovementReason moveReason;
    enum MovementDirection { Shortest, Negative, Positive };
    MovementDirection moveDirection;
    QDeclarativeOpenMetaObjectType *attType;
    QDeclarativeComponent *highlightComponent;
    QDeclarativeItem *highlightItem;
    QDeclarativeTimeLineValueProxy<QDeclarativePathViewPrivate> moveHighlight;
    qreal highlightPosition;
    qreal highlightRangeStart;
    qreal highlightRangeEnd;
    QDeclarativePathView::HighlightRangeMode highlightRangeMode;
    int highlightMoveDuration;
    int modelCount;
};

QT_END_NAMESPACE

#endif
