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

#ifndef QDECLARATIVEANCHORS_H
#define QDECLARATIVEANCHORS_H

#include "qdeclarativeitem.h"

#include <qdeclarative.h>

#include <QtCore/QObject>

#include <private/qtdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeAnchorsPrivate;
class QDeclarativeAnchorLine;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeAnchors : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeAnchorLine left READ left WRITE setLeft RESET resetLeft NOTIFY leftChanged)
    Q_PROPERTY(QDeclarativeAnchorLine right READ right WRITE setRight RESET resetRight NOTIFY rightChanged)
    Q_PROPERTY(QDeclarativeAnchorLine horizontalCenter READ horizontalCenter WRITE setHorizontalCenter RESET resetHorizontalCenter NOTIFY horizontalCenterChanged)
    Q_PROPERTY(QDeclarativeAnchorLine top READ top WRITE setTop RESET resetTop NOTIFY topChanged)
    Q_PROPERTY(QDeclarativeAnchorLine bottom READ bottom WRITE setBottom RESET resetBottom NOTIFY bottomChanged)
    Q_PROPERTY(QDeclarativeAnchorLine verticalCenter READ verticalCenter WRITE setVerticalCenter RESET resetVerticalCenter NOTIFY verticalCenterChanged)
    Q_PROPERTY(QDeclarativeAnchorLine baseline READ baseline WRITE setBaseline RESET resetBaseline NOTIFY baselineChanged)
    Q_PROPERTY(qreal margins READ margins WRITE setMargins NOTIFY marginsChanged)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)
    Q_PROPERTY(qreal horizontalCenterOffset READ horizontalCenterOffset WRITE setHorizontalCenterOffset NOTIFY horizontalCenterOffsetChanged)
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMargin NOTIFY topMarginChanged)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMargin NOTIFY bottomMarginChanged)
    Q_PROPERTY(qreal verticalCenterOffset READ verticalCenterOffset WRITE setVerticalCenterOffset NOTIFY verticalCenterOffsetChanged)
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged)
    Q_PROPERTY(QGraphicsObject *fill READ fill WRITE setFill RESET resetFill NOTIFY fillChanged)
    Q_PROPERTY(QGraphicsObject *centerIn READ centerIn WRITE setCenterIn RESET resetCenterIn NOTIFY centerInChanged)

public:
    QDeclarativeAnchors(QObject *parent=0);
    QDeclarativeAnchors(QGraphicsObject *item, QObject *parent=0);
    virtual ~QDeclarativeAnchors();

    enum Anchor {
        LeftAnchor = 0x01,
        RightAnchor = 0x02,
        TopAnchor = 0x04,
        BottomAnchor = 0x08,
        HCenterAnchor = 0x10,
        VCenterAnchor = 0x20,
        BaselineAnchor = 0x40,
        Horizontal_Mask = LeftAnchor | RightAnchor | HCenterAnchor,
        Vertical_Mask = TopAnchor | BottomAnchor | VCenterAnchor | BaselineAnchor
    };
    Q_DECLARE_FLAGS(Anchors, Anchor)

    QDeclarativeAnchorLine left() const;
    void setLeft(const QDeclarativeAnchorLine &edge);
    void resetLeft();

    QDeclarativeAnchorLine right() const;
    void setRight(const QDeclarativeAnchorLine &edge);
    void resetRight();

    QDeclarativeAnchorLine horizontalCenter() const;
    void setHorizontalCenter(const QDeclarativeAnchorLine &edge);
    void resetHorizontalCenter();

    QDeclarativeAnchorLine top() const;
    void setTop(const QDeclarativeAnchorLine &edge);
    void resetTop();

    QDeclarativeAnchorLine bottom() const;
    void setBottom(const QDeclarativeAnchorLine &edge);
    void resetBottom();

    QDeclarativeAnchorLine verticalCenter() const;
    void setVerticalCenter(const QDeclarativeAnchorLine &edge);
    void resetVerticalCenter();

    QDeclarativeAnchorLine baseline() const;
    void setBaseline(const QDeclarativeAnchorLine &edge);
    void resetBaseline();

    qreal leftMargin() const;
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
    void setBaselineOffset(qreal);

    QGraphicsObject *fill() const;
    void setFill(QGraphicsObject *);
    void resetFill();

    QGraphicsObject *centerIn() const;
    void setCenterIn(QGraphicsObject *);
    void resetCenterIn();

    Anchors usedAnchors() const;

    void classBegin();
    void componentComplete();

    bool mirrored();

Q_SIGNALS:
    void leftChanged();
    void rightChanged();
    void topChanged();
    void bottomChanged();
    void verticalCenterChanged();
    void horizontalCenterChanged();
    void baselineChanged();
    void fillChanged();
    void centerInChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();
    void marginsChanged();
    void verticalCenterOffsetChanged();
    void horizontalCenterOffsetChanged();
    void baselineOffsetChanged();

private:
    friend class QDeclarativeItem;
    friend class QDeclarativeItemPrivate;
    friend class QDeclarativeGraphicsWidget;
    Q_DISABLE_COPY(QDeclarativeAnchors)
    Q_DECLARE_PRIVATE(QDeclarativeAnchors)
    Q_PRIVATE_SLOT(d_func(), void _q_widgetGeometryChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_widgetDestroyed(QObject *obj))
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeAnchors::Anchors)

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeAnchors)

#endif
