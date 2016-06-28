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

#ifndef QDECLARATIVEPROPERTY_H
#define QDECLARATIVEPROPERTY_H

#include <QtDeclarative/qtdeclarativeglobal.h>
#include <QtCore/qmetaobject.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QObject;
class QVariant;
class QDeclarativeContext;
class QDeclarativeEngine;

class QDeclarativePropertyPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeProperty
{
public:
    enum PropertyTypeCategory {
        InvalidCategory,
        List,
        Object,
        Normal
    };

    enum Type {
        Invalid,
        Property,
        SignalProperty
    };

    QDeclarativeProperty();
    ~QDeclarativeProperty();

    QDeclarativeProperty(QObject *);
    QDeclarativeProperty(QObject *, QDeclarativeContext *);
    QDeclarativeProperty(QObject *, QDeclarativeEngine *);

    QDeclarativeProperty(QObject *, const QString &);
    QDeclarativeProperty(QObject *, const QString &, QDeclarativeContext *);
    QDeclarativeProperty(QObject *, const QString &, QDeclarativeEngine *);

    QDeclarativeProperty(const QDeclarativeProperty &);
    QDeclarativeProperty &operator=(const QDeclarativeProperty &);

    bool operator==(const QDeclarativeProperty &) const;

    Type type() const;
    bool isValid() const;
    bool isProperty() const;
    bool isSignalProperty() const;

    int propertyType() const;
    PropertyTypeCategory propertyTypeCategory() const;
    const char *propertyTypeName() const;

    QString name() const;

    QVariant read() const;
    static QVariant read(QObject *, const QString &);
    static QVariant read(QObject *, const QString &, QDeclarativeContext *);
    static QVariant read(QObject *, const QString &, QDeclarativeEngine *);

    bool write(const QVariant &) const;
    static bool write(QObject *, const QString &, const QVariant &);
    static bool write(QObject *, const QString &, const QVariant &, QDeclarativeContext *);
    static bool write(QObject *, const QString &, const QVariant &, QDeclarativeEngine *);

    bool reset() const;

    bool hasNotifySignal() const;
    bool needsNotifySignal() const;
    bool connectNotifySignal(QObject *dest, const char *slot) const;
    bool connectNotifySignal(QObject *dest, int method) const;

    bool isWritable() const;
    bool isDesignable() const;
    bool isResettable() const;
    QObject *object() const;

    int index() const;
    QMetaProperty property() const;
    QMetaMethod method() const;

private:
    friend class QDeclarativePropertyPrivate;
    QDeclarativePropertyPrivate *d;
};
typedef QList<QDeclarativeProperty> QDeclarativeProperties;

inline uint qHash (const QDeclarativeProperty &key)
{
    return qHash(key.object()) + qHash(key.name());
}

QT_END_NAMESPACE

#endif // QDECLARATIVEPROPERTY_H
