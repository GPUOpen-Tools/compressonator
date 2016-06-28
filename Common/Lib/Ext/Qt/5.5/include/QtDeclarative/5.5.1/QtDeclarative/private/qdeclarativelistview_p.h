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

#ifndef QDECLARATIVELISTVIEW_H
#define QDECLARATIVELISTVIEW_H

#include "private/qdeclarativeflickable_p.h"
#include "private/qdeclarativeguard_p.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class Q_AUTOTEST_EXPORT QDeclarativeViewSection : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString property READ property WRITE setProperty NOTIFY propertyChanged)
    Q_PROPERTY(SectionCriteria criteria READ criteria WRITE setCriteria NOTIFY criteriaChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_ENUMS(SectionCriteria)
public:
    QDeclarativeViewSection(QObject *parent=0) : QObject(parent), m_criteria(FullString), m_delegate(0) {}

    QString property() const { return m_property; }
    void setProperty(const QString &);

    enum SectionCriteria { FullString, FirstCharacter };
    SectionCriteria criteria() const { return m_criteria; }
    void setCriteria(SectionCriteria);

    QDeclarativeComponent *delegate() const { return m_delegate; }
    void setDelegate(QDeclarativeComponent *delegate);

    QString sectionString(const QString &value);

Q_SIGNALS:
    void propertyChanged();
    void criteriaChanged();
    void delegateChanged();

private:
    QString m_property;
    SectionCriteria m_criteria;
    QDeclarativeComponent *m_delegate;
};


class QDeclarativeVisualModel;
class QDeclarativeListViewAttached;
class QDeclarativeListViewPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeListView : public QDeclarativeFlickable
{
    Q_OBJECT
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeListView)

    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(QDeclarativeItem *currentItem READ currentItem NOTIFY currentIndexChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(QDeclarativeComponent *highlight READ highlight WRITE setHighlight NOTIFY highlightChanged)
    Q_PROPERTY(QDeclarativeItem *highlightItem READ highlightItem NOTIFY highlightItemChanged)
    Q_PROPERTY(bool highlightFollowsCurrentItem READ highlightFollowsCurrentItem WRITE setHighlightFollowsCurrentItem NOTIFY highlightFollowsCurrentItemChanged)
    Q_PROPERTY(qreal highlightMoveSpeed READ highlightMoveSpeed WRITE setHighlightMoveSpeed NOTIFY highlightMoveSpeedChanged)
    Q_PROPERTY(int highlightMoveDuration READ highlightMoveDuration WRITE setHighlightMoveDuration NOTIFY highlightMoveDurationChanged)
    Q_PROPERTY(qreal highlightResizeSpeed READ highlightResizeSpeed WRITE setHighlightResizeSpeed NOTIFY highlightResizeSpeedChanged)
    Q_PROPERTY(int highlightResizeDuration READ highlightResizeDuration WRITE setHighlightResizeDuration NOTIFY highlightResizeDurationChanged)

    Q_PROPERTY(qreal preferredHighlightBegin READ preferredHighlightBegin WRITE setPreferredHighlightBegin NOTIFY preferredHighlightBeginChanged RESET resetPreferredHighlightBegin)
    Q_PROPERTY(qreal preferredHighlightEnd READ preferredHighlightEnd WRITE setPreferredHighlightEnd NOTIFY preferredHighlightEndChanged RESET resetPreferredHighlightEnd)
    Q_PROPERTY(HighlightRangeMode highlightRangeMode READ highlightRangeMode WRITE setHighlightRangeMode NOTIFY highlightRangeModeChanged)

    Q_PROPERTY(qreal spacing READ spacing WRITE setSpacing NOTIFY spacingChanged)
    Q_PROPERTY(Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(Qt::LayoutDirection layoutDirection READ layoutDirection WRITE setLayoutDirection NOTIFY layoutDirectionChanged REVISION 1)
    Q_PROPERTY(bool keyNavigationWraps READ isWrapEnabled WRITE setWrapEnabled NOTIFY keyNavigationWrapsChanged)
    Q_PROPERTY(int cacheBuffer READ cacheBuffer WRITE setCacheBuffer NOTIFY cacheBufferChanged)
    Q_PROPERTY(QDeclarativeViewSection *section READ sectionCriteria CONSTANT)
    Q_PROPERTY(QString currentSection READ currentSection NOTIFY currentSectionChanged)

    Q_PROPERTY(SnapMode snapMode READ snapMode WRITE setSnapMode NOTIFY snapModeChanged)

    Q_PROPERTY(QDeclarativeComponent *header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QDeclarativeComponent *footer READ footer WRITE setFooter NOTIFY footerChanged)

    Q_ENUMS(HighlightRangeMode)
    Q_ENUMS(Orientation)
    Q_ENUMS(SnapMode)
    Q_ENUMS(PositionMode)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    QDeclarativeListView(QDeclarativeItem *parent=0);
    ~QDeclarativeListView();

    QVariant model() const;
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

    enum HighlightRangeMode { NoHighlightRange, ApplyRange, StrictlyEnforceRange };
    HighlightRangeMode highlightRangeMode() const;
    void setHighlightRangeMode(HighlightRangeMode mode);

    qreal preferredHighlightBegin() const;
    void setPreferredHighlightBegin(qreal);
    void resetPreferredHighlightBegin();

    qreal preferredHighlightEnd() const;
    void setPreferredHighlightEnd(qreal);
    void resetPreferredHighlightEnd();

    qreal spacing() const;
    void setSpacing(qreal spacing);

    enum Orientation { Horizontal = Qt::Horizontal, Vertical = Qt::Vertical };
    Orientation orientation() const;
    void setOrientation(Orientation);

    Qt::LayoutDirection layoutDirection() const;
    void setLayoutDirection(Qt::LayoutDirection);
    Qt::LayoutDirection effectiveLayoutDirection() const;

    bool isWrapEnabled() const;
    void setWrapEnabled(bool);

    int cacheBuffer() const;
    void setCacheBuffer(int);

    QDeclarativeViewSection *sectionCriteria();
    QString currentSection() const;

    qreal highlightMoveSpeed() const;
    void setHighlightMoveSpeed(qreal);

    int highlightMoveDuration() const;
    void setHighlightMoveDuration(int);

    qreal highlightResizeSpeed() const;
    void setHighlightResizeSpeed(qreal);

    int highlightResizeDuration() const;
    void setHighlightResizeDuration(int);

    enum SnapMode { NoSnap, SnapToItem, SnapOneItem };
    SnapMode snapMode() const;
    void setSnapMode(SnapMode mode);

    QDeclarativeComponent *footer() const;
    void setFooter(QDeclarativeComponent *);

    QDeclarativeComponent *header() const;
    void setHeader(QDeclarativeComponent *);

    virtual void setContentX(qreal pos);
    virtual void setContentY(qreal pos);

    static QDeclarativeListViewAttached *qmlAttachedProperties(QObject *);

    enum PositionMode { Beginning, Center, End, Visible, Contain };

    Q_INVOKABLE void positionViewAtIndex(int index, int mode);
    Q_INVOKABLE int indexAt(qreal x, qreal y) const;
    Q_INVOKABLE Q_REVISION(1) void positionViewAtBeginning();
    Q_INVOKABLE Q_REVISION(1) void positionViewAtEnd();

public Q_SLOTS:
    void incrementCurrentIndex();
    void decrementCurrentIndex();

Q_SIGNALS:
    void countChanged();
    void spacingChanged();
    void orientationChanged();
    Q_REVISION(1) void layoutDirectionChanged();
    void currentIndexChanged();
    void currentSectionChanged();
    void highlightMoveSpeedChanged();
    void highlightMoveDurationChanged();
    void highlightResizeSpeedChanged();
    void highlightResizeDurationChanged();
    void highlightChanged();
    void highlightItemChanged();
    void modelChanged();
    void delegateChanged();
    void highlightFollowsCurrentItemChanged();
    void preferredHighlightBeginChanged();
    void preferredHighlightEndChanged();
    void highlightRangeModeChanged();
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
    virtual void geometryChanged(const QRectF &newGeometry,const QRectF &oldGeometry);
    virtual void componentComplete();

private Q_SLOTS:
    void updateSections();
    void refill();
    void trackedPositionChanged();
    void itemsInserted(int index, int count);
    void itemsRemoved(int index, int count);
    void itemsMoved(int from, int to, int count);
    void itemsChanged(int index, int count);
    void modelReset();
    void destroyRemoved();
    void createdItem(int index, QDeclarativeItem *item);
    void destroyingItem(QDeclarativeItem *item);
    void animStopped();
};

class QDeclarativeListViewAttached : public QObject
{
    Q_OBJECT
public:
    QDeclarativeListViewAttached(QObject *parent)
        : QObject(parent), m_view(0), m_isCurrent(false), m_delayRemove(false) {}
    ~QDeclarativeListViewAttached() {}

    Q_PROPERTY(QDeclarativeListView *view READ view NOTIFY viewChanged)
    QDeclarativeListView *view() { return m_view; }
    void setView(QDeclarativeListView *view) {
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

    Q_PROPERTY(QString previousSection READ prevSection NOTIFY prevSectionChanged)
    QString prevSection() const { return m_prevSection; }
    void setPrevSection(const QString &sect) {
        if (m_prevSection != sect) {
            m_prevSection = sect;
            emit prevSectionChanged();
        }
    }

    Q_PROPERTY(QString nextSection READ nextSection NOTIFY nextSectionChanged)
    QString nextSection() const { return m_nextSection; }
    void setNextSection(const QString &sect) {
        if (m_nextSection != sect) {
            m_nextSection = sect;
            emit nextSectionChanged();
        }
    }

    Q_PROPERTY(QString section READ section NOTIFY sectionChanged)
    QString section() const { return m_section; }
    void setSection(const QString &sect) {
        if (m_section != sect) {
            m_section = sect;
            emit sectionChanged();
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
    void sectionChanged();
    void prevSectionChanged();
    void nextSectionChanged();
    void delayRemoveChanged();
    void add();
    void remove();
    void viewChanged();

public:
    QDeclarativeGuard<QDeclarativeListView> m_view;
    mutable QString m_section;
    QString m_prevSection;
    QString m_nextSection;
    bool m_isCurrent : 1;
    bool m_delayRemove : 1;
};


QT_END_NAMESPACE

QML_DECLARE_TYPEINFO(QDeclarativeListView, QML_HAS_ATTACHED_PROPERTIES)
QML_DECLARE_TYPE(QDeclarativeListView)
QML_DECLARE_TYPE(QDeclarativeViewSection)

#endif
