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

#ifndef QDECLARATIVEGRIDVIEW_H
#define QDECLARATIVEGRIDVIEW_H

#include "private/qdeclarativeflickable_p.h"
#include "private/qdeclarativeguard_p.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QDeclarativeVisualModel;
class QDeclarativeGridViewAttached;
class QDeclarativeGridViewPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeGridView : public QDeclarativeFlickable
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeGridView)

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QDeclarativeItem *currentItem READ currentItem NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(QDeclarativeComponent *highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)
    Q_PROPERTY(QDeclarativeItem *highlightItem READ highlightItem NOTIFY highlightItemChanged)
    Q_PROPERTY(bool highlightFollowsCurrentItem READ highlightFollowsCurrentItem WRITE setHighlightFollowsCurrentItem)
    Q_PROPERTY(int highlightMoveDuration READ highlightMoveDuration WRITE setHighlightMoveDuration NOTIFY highlightMoveDurationChanged)

    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin NOTIFY preferredHighlightBeginChanged RESET resetPreferredHighlightBegin)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd NOTIFY preferredHighlightEndChanged RESET resetPreferredHighlightEnd)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode NOTIFY highlightRangeModeChanged)

    Q_PROPERTY(Flow flow READ flow WRITE setFlow NOTIFY flowChanged)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged REVISION 1)
    Q_PROPERTY(bool keyNavigationWraps READ isWrapEnabled WRITE setWrapEnabled NOTIFY keyNavigationWrapsChanged)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer NOTIFY cacheBufferChanged)
    Q_PROPERTY(int cellWidth READ cellWidth WRITE setCellWidth NOTIFY cellWidthChanged)
    Q_PROPERTY(int cellHeight READ cellHeight WRITE setCellHeight NOTIFY cellHeightChanged)

    Q_PROPERTY(SnapMode snapMode READ snapMode WRITE setSnapMode NOTIFY snapModeChanged)

    Q_PROPERTY(QDeclarativeComponent *header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QDeclarativeComponent *footer READ footer WRITE setFooter NOTIFY footerChanged)

    Q_ENUMS(HighlightRangeMode)
    Q_ENUMS(SnapMode)
    Q_ENUMS(Flow)
    Q_ENUMS(PositionMode)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    QDeclarativeGridView(QDeclarativeItem *parent=0);
    ~QDeclarativeGridView();

    QVariant model() const;
    int modelCount() const;
    void setModel(const QVariant &);

    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *);

    int currentIndex() const;
    void setCurrentIndex(int idx);

    QDeclarativeItem *currentItem();
    QDeclarativeItem *highlightItem();
    int count() const;

    QDeclarativeComponent *highlight() const;
    void setHighlight(QDeclarativeComponent *highlight);

    bool highlightFollowsCurrentItem() const;
    void setHighlightFollowsCurrentItem(bool);

    int highlightMoveDuration() const;
    void setHighlightMoveDuration(int);

    enum HighlightRangeMode { NoHighlightRange, ApplyRange, StrictlyEnforceRange };
    HighlightRangeMode highlightRangeMode() const;
    void setHighlightRangeMode(HighlightRangeMode mode);

    qreal preferredHighlightBegin() const;
    void setPreferredHighlightBegin(qreal);
    void resetPreferredHighlightBegin();

    qreal preferredHighlightEnd() const;
    void setPreferredHighlightEnd(qreal);
    void resetPreferredHighlightEnd();

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection);
    Qt::LayoutDirection effectiveLayoutDirection() const;

    enum Flow { LeftToRight, TopToBottom };
    Flow flow() const;
    void setFlow(Flow);

    bool isWrapEnabled() const;
    void setWrapEnabled(bool);

    int cacheBuffer() const;
    void setCacheBuffer(int);

    int cellWidth() const;
    void setCellWidth(int);

    int cellHeight() const;
    void setCellHeight(int);

    enum SnapMode { NoSnap, SnapToRow, SnapOneRow };
    SnapMode snapMode() const;
    void setSnapMode(SnapMode mode);

    QDeclarativeComponent *footer() const;
    void setFooter(QDeclarativeComponent *);

    QDeclarativeComponent *header() const;
    void setHeader(QDeclarativeComponent *);

    virtual void setContentX(qreal pos);
    virtual void setContentY(qreal pos);

    enum PositionMode { Beginning, Center, End, Visible, Contain };

    Q_INVOKABLE void positionViewAtIndex(int index, int mode);
    Q_INVOKABLE int indexAt(qreal x, qreal y) const;
    Q_INVOKABLE Q_REVISION(1) void positionViewAtBeginning();
    Q_INVOKABLE Q_REVISION(1) void positionViewAtEnd();

    static QDeclarativeGridViewAttached *qmlAttachedProperties(QObject *);

public Q_SLOTS:
    void moveCurrentIndexUp();
    void moveCurrentIndexDown();
    void moveCurrentIndexLeft();
    void moveCurrentIndexRight();

Q_SIGNALS:
    void countChanged();
    void currentIndexChanged();
    void cellWidthChanged();
    void cellHeightChanged();
    void highlightChanged();
    void highlightItemChanged();
    void preferredHighlightBeginChanged();
    void preferredHighlightEndChanged();
    void highlightRangeModeChanged();
    void highlightMoveDurationChanged();
    void modelChanged();
    void delegateChanged();
    void flowChanged();
    Q_REVISION(1) void layoutDirectionChanged();
    void keyNavigationWrapsChanged();
    void cacheBufferChanged();
    void snapModeChanged();
    void headerChanged();
    void footerChanged();

protected:
    virtual bool event(QEvent *event);
    virtual void viewportMoved();
    virtual qreal minYExtent() const;
    virtual qreal maxYExtent() const;
    virtual qreal minXExtent() const;
    virtual qreal maxXExtent() const;
    virtual void keyPressEvent(QKeyEvent *);
    virtual void componentComplete();

private Q_SLOTS:
    void trackedPositionChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void modelReset();
    void destroyRemoved();
    void createdItem(int index, QDeclarativeItem *item);
    void destroyingItem(QDeclarativeItem *item);
    void animStopped();

private:
    void refill();
};

class QDeclarativeGridViewAttached : public QObject
{
    Q_OBJECT
public:
    QDeclarativeGridViewAttached(QObject *parent)
        : QObject(parent), m_view(0), m_isCurrent(false), m_delayRemove(false) {}
    ~QDeclarativeGridViewAttached() {}

    Q_PROPERTY(QDeclarativeGridView *view READ view NOTIFY viewChanged)
    QDeclarativeGridView *view() { return m_view; }
    void setView(QDeclarativeGridView *view) {
        if (view != m_view) {
            m_view = view;
            emit viewChanged();
        }
    }

    Q_PROPERTY(bool isCurrentItem READ isCurrentItem NOTIFY currentItemChanged)
    bool isCurrentItem() const { return m_isCurrent; }
    void setIsCurrentItem(bool c) {
        if (m_isCurrent != c) {
            m_isCurrent = c;
            emit currentItemChanged();
        }
    }

    Q_PROPERTY(bool delayRemove READ delayRemove WRITE setDelayRemove NOTIFY delayRemoveChanged)
    bool delayRemove() const { return m_delayRemove; }
    void setDelayRemove(bool delay) {
        if (m_delayRemove != delay) {
            m_delayRemove = delay;
            emit delayRemoveChanged();
        }
    }

    void emitAdd() { emit add(); }
    void emitRemove() { emit remove(); }

Q_SIGNALS:
    void currentItemChanged();
    void delayRemoveChanged();
    void add();
    void remove();
    void viewChanged();

public:
    QDeclarativeGuard<QDeclarativeGridView> m_view;
    bool m_isCurrent : 1;
    bool m_delayRemove : 1;
};


QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeGridView)
QML_DECLARE_TYPEINFO(QDeclarativeGridView, QML_HAS_ATTACHED_PROPERTIES)

#endif
