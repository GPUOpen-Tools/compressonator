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

#ifndef QDECLARATIVEPATH_H
#define QDECLARATIVEPATH_H

#include "qdeclarativeitem.h"

#include <qdeclarative.h>

#include <QtCore/QObject>
#include <QtGui/QPainterPath>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class Q_AUTOTEST_EXPORT QDeclarativePathElement : public QObject
{
    Q_OBJECT
public:
    QDeclarativePathElement(QObject *parent=0) : QObject(parent) {}
Q_SIGNALS:
    void changed();
};

class Q_AUTOTEST_EXPORT QDeclarativePathAttribute : public QDeclarativePathElement
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
public:
    QDeclarativePathAttribute(QObject *parent=0) : QDeclarativePathElement(parent), _value(0) {}


    QString name() const;
    void setName(const QString &name);

    qreal value() const;
    void setValue(qreal value);

Q_SIGNALS:
    void nameChanged();
    void valueChanged();

private:
    QString _name;
    qreal _value;
};

class Q_AUTOTEST_EXPORT QDeclarativeCurve : public QDeclarativePathElement
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX NOTIFY xChanged)
    Q_PROPERTY(qreal y READ y WRITE setY NOTIFY yChanged)
public:
    QDeclarativeCurve(QObject *parent=0) : QDeclarativePathElement(parent), _x(0), _y(0) {}

    qreal x() const;
    void setX(qreal x);

    qreal y() const;
    void setY(qreal y);

    virtual void addToPath(QPainterPath &) {}

Q_SIGNALS:
    void xChanged();
    void yChanged();

private:
    qreal _x;
    qreal _y;
};

class Q_AUTOTEST_EXPORT QDeclarativePathLine : public QDeclarativeCurve
{
    Q_OBJECT
public:
    QDeclarativePathLine(QObject *parent=0) : QDeclarativeCurve(parent) {}

    void addToPath(QPainterPath &path);
};

class Q_AUTOTEST_EXPORT QDeclarativePathQuad : public QDeclarativeCurve
{
    Q_OBJECT

    Q_PROPERTY(qreal controlX READ controlX WRITE setControlX NOTIFY controlXChanged)
    Q_PROPERTY(qreal controlY READ controlY WRITE setControlY NOTIFY controlYChanged)
public:
    QDeclarativePathQuad(QObject *parent=0) : QDeclarativeCurve(parent), _controlX(0), _controlY(0) {}

    qreal controlX() const;
    void setControlX(qreal x);

    qreal controlY() const;
    void setControlY(qreal y);

    void addToPath(QPainterPath &path);

Q_SIGNALS:
    void controlXChanged();
    void controlYChanged();

private:
    qreal _controlX;
    qreal _controlY;
};

class Q_AUTOTEST_EXPORT QDeclarativePathCubic : public QDeclarativeCurve
{
    Q_OBJECT

    Q_PROPERTY(qreal control1X READ control1X WRITE setControl1X NOTIFY control1XChanged)
    Q_PROPERTY(qreal control1Y READ control1Y WRITE setControl1Y NOTIFY control1YChanged)
    Q_PROPERTY(qreal control2X READ control2X WRITE setControl2X NOTIFY control2XChanged)
    Q_PROPERTY(qreal control2Y READ control2Y WRITE setControl2Y NOTIFY control2YChanged)
public:
    QDeclarativePathCubic(QObject *parent=0) : QDeclarativeCurve(parent), _control1X(0), _control1Y(0), _control2X(0), _control2Y(0) {}

    qreal control1X() const;
    void setControl1X(qreal x);

    qreal control1Y() const;
    void setControl1Y(qreal y);

    qreal control2X() const;
    void setControl2X(qreal x);

    qreal control2Y() const;
    void setControl2Y(qreal y);

    void addToPath(QPainterPath &path);

Q_SIGNALS:
    void control1XChanged();
    void control1YChanged();
    void control2XChanged();
    void control2YChanged();

private:
    qreal _control1X;
    qreal _control1Y;
    qreal _control2X;
    qreal _control2Y;
};

class Q_AUTOTEST_EXPORT QDeclarativePathPercent : public QDeclarativePathElement
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
public:
    QDeclarativePathPercent(QObject *parent=0) : QDeclarativePathElement(parent) {}

    qreal value() const;
    void setValue(qreal value);

signals:
    void valueChanged();

private:
    qreal _value;
};

class QDeclarativePathPrivate;
class Q_AUTOTEST_EXPORT QDeclarativePath : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QDeclarativeListProperty<QDeclarativePathElement> pathElements READ pathElements)
    Q_PROPERTY(qreal startX READ startX WRITE setStartX NOTIFY startXChanged)
    Q_PROPERTY(qreal startY READ startY WRITE setStartY NOTIFY startYChanged)
    Q_PROPERTY(bool closed READ isClosed NOTIFY changed)
    Q_CLASSINFO("DefaultProperty", "pathElements")
    Q_INTERFACES(QDeclarativeParserStatus)
public:
    QDeclarativePath(QObject *parent=0);
    ~QDeclarativePath();

    QDeclarativeListProperty<QDeclarativePathElement> pathElements();

    qreal startX() const;
    void setStartX(qreal x);

    qreal startY() const;
    void setStartY(qreal y);

    bool isClosed() const;

    QPainterPath path() const;
    QStringList attributes() const;
    qreal attributeAt(const QString &, qreal) const;
    QPointF pointAt(qreal) const;

Q_SIGNALS:
    void changed();
    void startXChanged();
    void startYChanged();

protected:
    virtual void componentComplete();
    virtual void classBegin();

private Q_SLOTS:
    void processPath();

private:
    struct AttributePoint {
        AttributePoint() : percent(0), scale(1), origpercent(0) {}
        AttributePoint(const AttributePoint &other)
            : percent(other.percent), scale(other.scale), origpercent(other.origpercent), values(other.values) {}
        AttributePoint &operator=(const AttributePoint &other) {
            percent = other.percent; scale = other.scale; origpercent = other.origpercent; values = other.values; return *this;
        }
        qreal percent;      //massaged percent along the painter path
        qreal scale;
        qreal origpercent;  //'real' percent along the painter path
        QHash<QString, qreal> values;
    };

    void interpolate(int idx, const QString &name, qreal value);
    void endpoint(const QString &name);
    void createPointCache() const;

private:
    Q_DISABLE_COPY(QDeclarativePath)
    Q_DECLARE_PRIVATE(QDeclarativePath)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativePathElement)
QML_DECLARE_TYPE(QDeclarativePathAttribute)
QML_DECLARE_TYPE(QDeclarativeCurve)
QML_DECLARE_TYPE(QDeclarativePathLine)
QML_DECLARE_TYPE(QDeclarativePathQuad)
QML_DECLARE_TYPE(QDeclarativePathCubic)
QML_DECLARE_TYPE(QDeclarativePathPercent)
QML_DECLARE_TYPE(QDeclarativePath)

#endif // QDECLARATIVEPATH_H
