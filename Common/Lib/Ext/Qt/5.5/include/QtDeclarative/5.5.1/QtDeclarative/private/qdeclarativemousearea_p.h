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

#ifndef QDECLARATIVEMOUSEAREA_H
#define QDECLARATIVEMOUSEAREA_H

#include "qdeclarativeitem.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_AUTOTEST_EXPORT QDeclarativeDrag : public QObject
{
    Q_OBJECT

    Q_ENUMS(Axis)
    Q_PROPERTY(QGraphicsObject *target READ target WRITE setTarget NOTIFY targetChanged RESET resetTarget)
    Q_PROPERTY(Axis axis READ axis WRITE setAxis NOTIFY axisChanged)
    Q_PROPERTY(qreal minimumX READ xmin WRITE setXmin NOTIFY minimumXChanged)
    Q_PROPERTY(qreal maximumX READ xmax WRITE setXmax NOTIFY maximumXChanged)
    Q_PROPERTY(qreal minimumY READ ymin WRITE setYmin NOTIFY minimumYChanged)
    Q_PROPERTY(qreal maximumY READ ymax WRITE setYmax NOTIFY maximumYChanged)
    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool filterChildren READ filterChildren WRITE setFilterChildren NOTIFY filterChildrenChanged)
    //### consider drag and drop

public:
    QDeclarativeDrag(QObject *parent=0);
    ~QDeclarativeDrag();

    QGraphicsObject *target() const;
    void setTarget(QGraphicsObject *);
    void resetTarget();

    enum Axis { XAxis=0x01, YAxis=0x02, XandYAxis=0x03 };
    Axis axis() const;
    void setAxis(Axis);

    qreal xmin() const;
    void setXmin(qreal);
    qreal xmax() const;
    void setXmax(qreal);
    qreal ymin() const;
    void setYmin(qreal);
    qreal ymax() const;
    void setYmax(qreal);

    bool active() const;
    void setActive(bool);

    bool filterChildren() const;
    void setFilterChildren(bool);

Q_SIGNALS:
    void targetChanged();
    void axisChanged();
    void minimumXChanged();
    void maximumXChanged();
    void minimumYChanged();
    void maximumYChanged();
    void activeChanged();
    void filterChildrenChanged();

private:
    QGraphicsObject *_target;
    Axis _axis;
    qreal _xmin;
    qreal _xmax;
    qreal _ymin;
    qreal _ymax;
    bool _active : 1;
    bool _filterChildren: 1;
    Q_DISABLE_COPY(QDeclarativeDrag)
};

class QDeclarativeMouseEvent;
class QDeclarativeMouseAreaPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeMouseArea : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(qreal mouseX READ mouseX NOTIFY mousePositionChanged)
    Q_PROPERTY(qreal mouseY READ mouseY NOTIFY mousePositionChanged)
    Q_PROPERTY(bool containsMouse READ hovered NOTIFY hoveredChanged)
    Q_PROPERTY(bool pressed READ pressed NOTIFY pressedChanged)
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(Qt::MouseButtons pressedButtons READ pressedButtons NOTIFY pressedChanged)
    Q_PROPERTY(Qt::MouseButtons acceptedButtons READ acceptedButtons WRITE setAcceptedButtons NOTIFY acceptedButtonsChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(QDeclarativeDrag *drag READ drag CONSTANT) //### add flicking to QDeclarativeDrag or add a QDeclarativeFlick ???
    Q_PROPERTY(bool preventStealing READ preventStealing WRITE setPreventStealing NOTIFY preventStealingChanged REVISION 1)

public:
    QDeclarativeMouseArea(QDeclarativeItem *parent=0);
    ~QDeclarativeMouseArea();

    qreal mouseX() const;
    qreal mouseY() const;

    bool isEnabled() const;
    void setEnabled(bool);

    bool hovered() const;
    bool pressed() const;

    Qt::MouseButtons pressedButtons() const;

    Qt::MouseButtons acceptedButtons() const;
    void setAcceptedButtons(Qt::MouseButtons buttons);

    bool hoverEnabled() const;
    void setHoverEnabled(bool h);

    QDeclarativeDrag *drag();

    bool preventStealing() const;
    void setPreventStealing(bool prevent);

Q_SIGNALS:
    void hoveredChanged();
    void pressedChanged();
    void enabledChanged();
    void acceptedButtonsChanged();
    void hoverEnabledChanged();
    void positionChanged(QDeclarativeMouseEvent *mouse);
    void mousePositionChanged(QDeclarativeMouseEvent *mouse);
    Q_REVISION(1) void preventStealingChanged();

    void pressed(QDeclarativeMouseEvent *mouse);
    void pressAndHold(QDeclarativeMouseEvent *mouse);
    void released(QDeclarativeMouseEvent *mouse);
    void clicked(QDeclarativeMouseEvent *mouse);
    void doubleClicked(QDeclarativeMouseEvent *mouse);
    void entered();
    void exited();
    void canceled();

protected:
    void setHovered(bool);
    bool setPressed(bool);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
#ifndef QT_NO_CONTEXTMENU
    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
#endif // QT_NO_CONTEXTMENU
    bool sceneEvent(QEvent *);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);
    bool sceneEventFilter(QGraphicsItem *i, QEvent *e);
    void timerEvent(QTimerEvent *event);

    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant& value);

private:
    void handlePress();
    void handleRelease();

private:
    Q_DISABLE_COPY(QDeclarativeMouseArea)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeMouseArea)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeDrag)
QML_DECLARE_TYPE(QDeclarativeMouseArea)

#endif // QDECLARATIVEMOUSEAREA_H
