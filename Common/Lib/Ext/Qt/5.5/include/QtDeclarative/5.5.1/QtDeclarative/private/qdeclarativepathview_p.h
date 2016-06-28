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

#ifndef QDECLARATIVEPATHVIEW_H
#define QDECLARATIVEPATHVIEW_H

#include "qdeclarativeitem.h"
#include "private/qdeclarativepath_p.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativePathViewPrivate;
class QDeclarativePathViewAttached;
class Q_AUTOTEST_EXPORT QDeclarativePathView : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativePath *path READ path WRITE setPath NOTIFY pathChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(qreal offset READ offset WRITE setOffset NOTIFY offsetChanged)

    Q_PROPERTY(QDeclarativeComponent *highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)
    Q_PROPERTY(QDeclarativeItem *highlightItem READ highlightItem NOTIFY highlightItemChanged)

    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin NOTIFY preferredHighlightBeginChanged)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd NOTIFY preferredHighlightEndChanged)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode NOTIFY highlightRangeModeChanged)
    Q_PROPERTY(int highlightMoveDuration READ highlightMoveDuration WRITE setHighlightMoveDuration NOTIFY highlightMoveDurationChanged)

    Q_PROPERTY(qreal dragMargin READ dragMargin WRITE setDragMargin NOTIFY dragMarginChanged)
    Q_PROPERTY(qreal flickDeceleration READ flickDeceleration WRITE setFlickDeceleration NOTIFY flickDecelerationChanged)
    Q_PROPERTY(bool interactive READ isInteractive WRITE setInteractive NOTIFY interactiveChanged)

    Q_PROPERTY(bool moving READ isMoving NOTIFY movingChanged)
    Q_PROPERTY(bool flicking READ isFlicking NOTIFY flickingChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int pathItemCount READ pathItemCount WRITE setPathItemCount NOTIFY pathItemCountChanged)

    Q_ENUMS(HighlightRangeMode)

public:
    QDeclarativePathView(QDeclarativeItem *parent=0);
    virtual ~QDeclarativePathView();

    QVariant model() const;
    void setModel(const QVariant &);

    QDeclarativePath *path() const;
    void setPath(QDeclarativePath *);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    qreal offset() const;
    void setOffset(qreal offset);

    QDeclarativeComponent *highlight() const;
    void setHighlight(QDeclarativeComponent *highlight);
    QDeclarativeItem *highlightItem();

    enum HighlightRangeMode { NoHighlightRange, ApplyRange, StrictlyEnforceRange };
    HighlightRangeMode highlightRangeMode() const;
    void setHighlightRangeMode(HighlightRangeMode mode);

    qreal preferredHighlightBegin() const;
    void setPreferredHighlightBegin(qreal);

    qreal preferredHighlightEnd() const;
    void setPreferredHighlightEnd(qreal);

    int highlightMoveDuration() const;
    void setHighlightMoveDuration(int);

    qreal dragMargin() const;
    void setDragMargin(qreal margin);

    qreal flickDeceleration() const;
    void setFlickDeceleration(qreal dec);

    bool isInteractive() const;
    void setInteractive(bool);

    bool isMoving() const;
    bool isFlicking() const;

    int count() const;

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int pathItemCount() const;
    void setPathItemCount(int);

    static QDeclarativePathViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void incrementCurrentIndex();
    void decrementCurrentIndex();

Q_SIGNALS:
    void currentIndexChanged();
    void offsetChanged();
    void modelChanged();
    void countChanged();
    void pathChanged();
    void preferredHighlightBeginChanged();
    void preferredHighlightEndChanged();
    void highlightRangeModeChanged();
    void dragMarginChanged();
    void snapPositionChanged();
    void delegateChanged();
    void pathItemCountChanged();
    void flickDecelerationChanged();
    void interactiveChanged();
    void movingChanged();
    void flickingChanged();
    void highlightChanged();
    void highlightItemChanged();
    void highlightMoveDurationChanged();
    void movementStarted();
    void movementEnded();
    void flickStarted();
    void flickEnded();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *);
    bool sendMouseEvent(QGraphicsSceneMouseEvent *event);
    bool sceneEventFilter(QGraphicsItem *, QEvent *);
    bool event(QEvent *event);
    void componentComplete();

private Q_SLOTS:
    void refill();
    void ticked();
    void movementEnding();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int,int,int);
    void modelReset();
    void createdItem(int index, QDeclarativeItem *item);
    void destroyingItem(QDeclarativeItem *item);
    void pathUpdated();

private:
    friend class QDeclarativePathViewAttached;
    Q_DISABLE_COPY(QDeclarativePathView)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativePathView)
};

class QDeclarativeOpenMetaObject;
class QDeclarativePathViewAttached : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativePathView *view READ view CONSTANT)
    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY currentItemChanged)
    Q_PROPERTY(bool onPath READ isOnPath NOTIFY pathChanged)

public:
    QDeclarativePathViewAttached(QObject *parent);
    ~QDeclarativePathViewAttached();

    QDeclarativePathView *view() { return m_view; }

    bool isCurrentItem() const { return m_isCurrent; }
    void setIsCurrentItem(bool c) {
        if (m_isCurrent != c) {
            m_isCurrent = c;
            emit currentItemChanged();
        }
    }

    QVariant value(const QByteArray &name) const;
    void setValue(const QByteArray &name, const QVariant &val);

    bool isOnPath() const { return m_onPath; }
    void setOnPath(bool on) {
        if (on != m_onPath) {
            m_onPath = on;
            emit pathChanged();
        }
    }
    qreal m_percent;

Q_SIGNALS:
    void currentItemChanged();
    void pathChanged();

private:
    friend class QDeclarativePathViewPrivate;
    friend class QDeclarativePathView;
    QDeclarativePathView *m_view;
    QDeclarativeOpenMetaObject *m_metaobject;
    bool m_onPath : 1;
    bool m_isCurrent : 1;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePathView)
QML_DECLARE_TYPEINFO(QDeclarativePathView, QML_HAS_ATTACHED_PROPERTIES)
#endif // QDECLARATIVEPATHVIEW_H
