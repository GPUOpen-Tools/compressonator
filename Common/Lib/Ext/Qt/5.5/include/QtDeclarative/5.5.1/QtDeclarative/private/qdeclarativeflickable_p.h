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

#ifndef QDECLARATIVEFLICKABLE_H
#define QDECLARATIVEFLICKABLE_H

#include "qdeclarativeitem.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeFlickablePrivate;
class QDeclarativeFlickableVisibleArea;
class Q_AUTOTEST_EXPORT QDeclarativeFlickable : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(qreal contentWidth READ contentWidth WRITE setContentWidth NOTIFY contentWidthChanged)
    Q_PROPERTY(qreal contentHeight READ contentHeight WRITE setContentHeight NOTIFY contentHeightChanged)
    Q_PROPERTY(qreal contentX READ contentX WRITE setContentX NOTIFY contentXChanged)
    Q_PROPERTY(qreal contentY READ contentY WRITE setContentY NOTIFY contentYChanged)
    Q_PROPERTY(QDeclarativeItem *contentItem READ contentItem CONSTANT)

    Q_PROPERTY(qreal horizontalVelocity READ horizontalVelocity NOTIFY horizontalVelocityChanged)
    Q_PROPERTY(qreal verticalVelocity READ verticalVelocity NOTIFY verticalVelocityChanged)

    Q_PROPERTY(BoundsBehavior boundsBehavior READ boundsBehavior WRITE setBoundsBehavior NOTIFY boundsBehaviorChanged)
    Q_PROPERTY(qreal maximumFlickVelocity READ maximumFlickVelocity WRITE setMaximumFlickVelocity NOTIFY maximumFlickVelocityChanged)
    Q_PROPERTY(qreal flickDeceleration READ flickDeceleration WRITE setFlickDeceleration NOTIFY flickDecelerationChanged)
    Q_PROPERTY(bool moving READ isMoving NOTIFY movingChanged)
    Q_PROPERTY(bool movingHorizontally READ isMovingHorizontally NOTIFY movingHorizontallyChanged)
    Q_PROPERTY(bool movingVertically READ isMovingVertically NOTIFY movingVerticallyChanged)
    Q_PROPERTY(bool flicking READ isFlicking NOTIFY flickingChanged)
    Q_PROPERTY(bool flickingHorizontally READ isFlickingHorizontally NOTIFY flickingHorizontallyChanged)
    Q_PROPERTY(bool flickingVertically READ isFlickingVertically NOTIFY flickingVerticallyChanged)
    Q_PROPERTY(FlickableDirection flickableDirection READ flickableDirection WRITE setFlickableDirection NOTIFY flickableDirectionChanged)

    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY interactiveChanged)
    Q_PROPERTY(int pressDelay READ pressDelay WRITE setPressDelay NOTIFY pressDelayChanged)

    Q_PROPERTY(bool atXEnd READ isAtXEnd NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atYEnd READ isAtYEnd NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atXBeginning READ isAtXBeginning NOTIFY isAtBoundaryChanged)
    Q_PROPERTY(bool atYBeginning READ isAtYBeginning NOTIFY isAtBoundaryChanged)

    Q_PROPERTY(QDeclarativeFlickableVisibleArea *visibleArea READ visibleArea CONSTANT)

    Q_PROPERTY(QDeclarativeListProperty<QObject> flickableData READ flickableData)
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsObject> flickableChildren READ flickableChildren)
    Q_CLASSINFO("DefaultProperty", "flickableData")

    Q_ENUMS(FlickableDirection)
    Q_ENUMS(BoundsBehavior)

public:
    QDeclarativeFlickable(QDeclarativeItem *parent=0);
    ~QDeclarativeFlickable();

    QDeclarativeListProperty<QObject> flickableData();
    QDeclarativeListProperty<QGraphicsObject> flickableChildren();

    enum BoundsBehavior { StopAtBounds, DragOverBounds, DragAndOvershootBounds };
    BoundsBehavior boundsBehavior() const;
    void setBoundsBehavior(BoundsBehavior);

    qreal contentWidth() const;
    void setContentWidth(qreal);

    qreal contentHeight() const;
    void setContentHeight(qreal);

    qreal contentX() const;
    virtual void setContentX(qreal pos);

    qreal contentY() const;
    virtual void setContentY(qreal pos);

    bool isMoving() const;
    bool isMovingHorizontally() const;
    bool isMovingVertically() const;
    bool isFlicking() const;
    bool isFlickingHorizontally() const;
    bool isFlickingVertically() const;

    int pressDelay() const;
    void setPressDelay(int delay);

    qreal maximumFlickVelocity() const;
    void setMaximumFlickVelocity(qreal);

    qreal flickDeceleration() const;
    void setFlickDeceleration(qreal);

    bool isInteractive() const;
    void setInteractive(bool);

    qreal horizontalVelocity() const;
    qreal verticalVelocity() const;

    bool isAtXEnd() const;
    bool isAtXBeginning() const;
    bool isAtYEnd() const;
    bool isAtYBeginning() const;

    QDeclarativeItem *contentItem();

    enum FlickableDirection { AutoFlickDirection=0x00, HorizontalFlick=0x01, VerticalFlick=0x02, HorizontalAndVerticalFlick=0x03 };
    FlickableDirection flickableDirection() const;
    void setFlickableDirection(FlickableDirection);

    Q_INVOKABLE Q_REVISION(1) void resizeContent(qreal w, qreal h, QPointF center);
    Q_INVOKABLE Q_REVISION(1) void returnToBounds();

Q_SIGNALS:
    void contentWidthChanged();
    void contentHeightChanged();
    void contentXChanged();
    void contentYChanged();
    void movingChanged();
    void movingHorizontallyChanged();
    void movingVerticallyChanged();
    void flickingChanged();
    void flickingHorizontallyChanged();
    void flickingVerticallyChanged();
    void horizontalVelocityChanged();
    void verticalVelocityChanged();
    void isAtBoundaryChanged();
    void flickableDirectionChanged();
    void interactiveChanged();
    void boundsBehaviorChanged();
    void maximumFlickVelocityChanged();
    void flickDecelerationChanged();
    void pressDelayChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();

protected:
    virtual bool sceneEventFilter(QGraphicsItem *, QEvent *);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void wheelEvent(QGraphicsSceneWheelEvent *event);
    void timerEvent(QTimerEvent *event);

    QDeclarativeFlickableVisibleArea *visibleArea();

protected Q_SLOTS:
    virtual void ticked();
    void movementStarting();
    void movementEnding();

protected:
    void movementXEnding();
    void movementYEnding();
    virtual qreal minXExtent() const;
    virtual qreal minYExtent() const;
    virtual qreal maxXExtent() const;
    virtual qreal maxYExtent() const;
    qreal vWidth() const;
    qreal vHeight() const;
    virtual void viewportMoved();
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    bool sceneEvent(QEvent *event);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);

    bool xflick() const;
    bool yflick() const;
    void cancelFlick();

protected:
    QDeclarativeFlickable(QDeclarativeFlickablePrivate &dd, QDeclarativeItem *parent);

private:
    Q_DISABLE_COPY(QDeclarativeFlickable)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeFlickable)
    friend class QDeclarativeFlickableVisibleArea;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeFlickable)

#endif
