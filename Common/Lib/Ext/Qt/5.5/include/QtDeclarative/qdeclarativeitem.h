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

#ifndef QDECLARATIVEITEM_H
#define QDECLARATIVEITEM_H

#include <QtDeclarative/qdeclarative.h>
#include <QtDeclarative/qdeclarativecomponent.h>

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtWidgets/qgraphicstransform.h>
#include <QtWidgets/qgraphicswidget.h>
#include <QtGui/qfont.h>
#include <QtWidgets/qaction.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeState;
class QDeclarativeAnchorLine;
class QDeclarativeTransition;
class QDeclarativeKeyEvent;
class QDeclarativeAnchors;
class QDeclarativeItemPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeItem : public QGraphicsObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QDeclarativeItem * parent READ parentItem WRITE setParentItem NOTIFY parentChanged DESIGNABLE false FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeListProperty<QObject> data READ data DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeListProperty<QObject> resources READ resources DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeListProperty<QDeclarativeState> states READ states DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeListProperty<QDeclarativeTransition> transitions READ transitions DESIGNABLE false)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QRectF childrenRect READ childrenRect NOTIFY childrenRectChanged DESIGNABLE false FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchors * anchors READ anchors DESIGNABLE false CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine left READ left CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine right READ right CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine horizontalCenter READ horizontalCenter CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine top READ top CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine bottom READ bottom CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine verticalCenter READ verticalCenter CONSTANT FINAL)
    Q_PRIVATE_PROPERTY(QDeclarativeItem::d_func(), QDeclarativeAnchorLine baseline READ baseline CONSTANT FINAL)
    Q_PROPERTY(qreal baselineOffset READ baselineOffset WRITE setBaselineOffset NOTIFY baselineOffsetChanged)
    Q_PROPERTY(bool clip READ clip WRITE setClip NOTIFY clipChanged) // ### move to QGI/QGO, NOTIFY
    Q_PROPERTY(bool focus READ hasFocus WRITE setFocus NOTIFY focusChanged FINAL)
    Q_PROPERTY(bool activeFocus READ hasActiveFocus NOTIFY activeFocusChanged)
    Q_PROPERTY(QDeclarativeListProperty<QGraphicsTransform> transform READ transform DESIGNABLE false FINAL)
    Q_PROPERTY(TransformOrigin transformOrigin READ transformOrigin WRITE setTransformOrigin NOTIFY transformOriginChanged)
    Q_PROPERTY(QPointF transformOriginPoint READ transformOriginPoint) // transformOriginPoint is read-only for Item
    Q_PROPERTY(bool smooth READ smooth WRITE setSmooth NOTIFY smoothChanged)
    Q_PROPERTY(qreal implicitWidth READ implicitWidth WRITE setImplicitWidth NOTIFY implicitWidthChanged REVISION 1)
    Q_PROPERTY(qreal implicitHeight READ implicitHeight WRITE setImplicitHeight NOTIFY implicitHeightChanged REVISION 1)

    Q_ENUMS(TransformOrigin)
    Q_CLASSINFO("DefaultProperty", "data")

public:
    enum TransformOrigin {
        TopLeft, Top, TopRight,
        Left, Center, Right,
        BottomLeft, Bottom, BottomRight
    };

    QDeclarativeItem(QDeclarativeItem *parent = 0);
    virtual ~QDeclarativeItem();

    QDeclarativeItem *parentItem() const;
    void setParentItem(QDeclarativeItem *parent);

    QRectF childrenRect();

    bool clip() const;
    void setClip(bool);

    qreal baselineOffset() const;
    void setBaselineOffset(qreal);

    QDeclarativeListProperty<QGraphicsTransform> transform();

    qreal width() const;
    void setWidth(qreal);
    void resetWidth();
    qreal implicitWidth() const;

    qreal height() const;
    void setHeight(qreal);
    void resetHeight();
    qreal implicitHeight() const;

    void setSize(const QSizeF &size);

    TransformOrigin transformOrigin() const;
    void setTransformOrigin(TransformOrigin);

    bool smooth() const;
    void setSmooth(bool);

    QRectF boundingRect() const;
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    bool hasActiveFocus() const;
    bool hasFocus() const;
    void setFocus(bool);

    bool keepMouseGrab() const;
    void setKeepMouseGrab(bool);

    Q_INVOKABLE QScriptValue mapFromItem(const QScriptValue &item, qreal x, qreal y) const;
    Q_INVOKABLE QScriptValue mapToItem(const QScriptValue &item, qreal x, qreal y) const;
    Q_INVOKABLE void forceActiveFocus();
    Q_INVOKABLE QDeclarativeItem *childAt(qreal x, qreal y) const;

Q_SIGNALS:
    void childrenRectChanged(const QRectF &);
    void baselineOffsetChanged(qreal);
    void stateChanged(const QString &);
    void focusChanged(bool);
    void activeFocusChanged(bool);
    void parentChanged(QDeclarativeItem *);
    void transformOriginChanged(TransformOrigin);
    void smoothChanged(bool);
    void clipChanged(bool);
    Q_REVISION(1) void implicitWidthChanged();
    Q_REVISION(1) void implicitHeightChanged();

protected:
    bool isComponentComplete() const;
    virtual bool sceneEvent(QEvent *);
    virtual bool event(QEvent *);
    virtual QVariant itemChange(GraphicsItemChange, const QVariant &);

    void setImplicitWidth(qreal);
    bool widthValid() const; // ### better name?
    void setImplicitHeight(qreal);
    bool heightValid() const; // ### better name?

    virtual void classBegin();
    virtual void componentComplete();
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void inputMethodEvent(QInputMethodEvent *);
    virtual QVariant inputMethodQuery(Qt::InputMethodQuery query) const;
    void keyPressPreHandler(QKeyEvent *);
    void keyReleasePreHandler(QKeyEvent *);
    void inputMethodPreHandler(QInputMethodEvent *);

    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

protected:
    QDeclarativeItem(QDeclarativeItemPrivate &dd, QDeclarativeItem *parent = 0);

private:
    Q_DISABLE_COPY(QDeclarativeItem)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeItem)
};

template<typename T>
        T qobject_cast(QGraphicsObject *o)
{
    QObject *obj = o;
    return qobject_cast<T>(obj);
}

// ### move to QGO
template<typename T>
T qobject_cast(QGraphicsItem *item)
{
    if (!item) return 0;
    QObject *o = item->toGraphicsObject();
    return qobject_cast<T>(o);
}

#ifndef QT_NO_DEBUG_STREAM
QDebug Q_DECLARATIVE_EXPORT operator<<(QDebug debug, QDeclarativeItem *item);
#endif

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeItem)
QML_DECLARE_TYPE(QGraphicsObject)
QML_DECLARE_TYPE(QGraphicsTransform)
QML_DECLARE_TYPE(QGraphicsScale)
QML_DECLARE_TYPE(QGraphicsRotation)
QML_DECLARE_TYPE(QGraphicsWidget)
#ifndef QT_NO_ACTION
QML_DECLARE_TYPE(QAction)
#endif

#endif // QDECLARATIVEITEM_H
