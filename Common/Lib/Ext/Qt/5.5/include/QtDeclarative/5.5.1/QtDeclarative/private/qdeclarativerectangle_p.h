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

#ifndef QDECLARATIVERECT_H
#define QDECLARATIVERECT_H

#include "qdeclarativeitem.h"

#include <QtGui/qbrush.h>

#include <private/qtdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativePen : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY penChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY penChanged)
public:
    QDeclarativePen(QObject *parent=0)
        : QObject(parent), _width(1), _color("#000000"), _valid(false)
    {}

    int width() const { return _width; }
    void setWidth(int w);

    QColor color() const { return _color; }
    void setColor(const QColor &c);

    bool isValid() { return _valid; }

Q_SIGNALS:
    void penChanged();

private:
    int _width;
    QColor _color;
    bool _valid;
};

class Q_AUTOTEST_EXPORT QDeclarativeGradientStop : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal position READ position WRITE setPosition)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    QDeclarativeGradientStop(QObject *parent=0) : QObject(parent) {}

    qreal position() const { return m_position; }
    void setPosition(qreal position) { m_position = position; updateGradient(); }

    QColor color() const { return m_color; }
    void setColor(const QColor &color) { m_color = color; updateGradient(); }

private:
    void updateGradient();

private:
    qreal m_position;
    QColor m_color;
};

class Q_AUTOTEST_EXPORT QDeclarativeGradient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QDeclarativeListProperty<QDeclarativeGradientStop> stops READ stops)
    Q_CLASSINFO("DefaultProperty", "stops")

public:
    QDeclarativeGradient(QObject *parent=0) : QObject(parent), m_gradient(0) {}
    ~QDeclarativeGradient() { delete m_gradient; }

    QDeclarativeListProperty<QDeclarativeGradientStop> stops() { return QDeclarativeListProperty<QDeclarativeGradientStop>(this, m_stops); }

    const QGradient *gradient() const;

Q_SIGNALS:
    void updated();

private:
    void doUpdate();

private:
    QList<QDeclarativeGradientStop *> m_stops;
    mutable QGradient *m_gradient;
    friend class QDeclarativeGradientStop;
};

class QDeclarativeRectanglePrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeRectangle : public QDeclarativeItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QDeclarativeGradient *gradient READ gradient WRITE setGradient)
    Q_PROPERTY(QDeclarativePen * border READ border CONSTANT)
    Q_PROPERTY(qreal radius READ radius WRITE setRadius NOTIFY radiusChanged)
public:
    QDeclarativeRectangle(QDeclarativeItem *parent=0);

    QColor color() const;
    void setColor(const QColor &);

    QDeclarativePen *border();

    QDeclarativeGradient *gradient() const;
    void setGradient(QDeclarativeGradient *gradient);

    qreal radius() const;
    void setRadius(qreal radius);

    QRectF boundingRect() const;

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

Q_SIGNALS:
    void colorChanged();
    void radiusChanged();

private Q_SLOTS:
    void doUpdate();

private:
    void generateRoundedRect();
    void generateBorderedRect();
    void drawRect(QPainter &painter);

private:
    Q_DISABLE_COPY(QDeclarativeRectangle)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeRectangle)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePen)
QML_DECLARE_TYPE(QDeclarativeGradientStop)
QML_DECLARE_TYPE(QDeclarativeGradient)
QML_DECLARE_TYPE(QDeclarativeRectangle)

#endif // QDECLARATIVERECT_H
