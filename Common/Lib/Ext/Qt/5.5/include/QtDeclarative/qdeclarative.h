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

#ifndef QDECLARATIVE_H
#define QDECLARATIVE_H

#include <QtDeclarative/qdeclarativeprivate.h>
#include <QtDeclarative/qdeclarativeparserstatus.h>
#include <QtDeclarative/qdeclarativepropertyvaluesource.h>
#include <QtDeclarative/qdeclarativepropertyvalueinterceptor.h>
#include <QtDeclarative/qdeclarativelist.h>

#include <QtCore/qbytearray.h>
#include <QtCore/qurl.h>
#include <QtCore/qmetaobject.h>

#define QML_DECLARE_TYPE(TYPE) \
    Q_DECLARE_METATYPE(TYPE *) \
    Q_DECLARE_METATYPE(QDeclarativeListProperty<TYPE>)

#define QML_DECLARE_TYPE_HASMETATYPE(TYPE) \
    Q_DECLARE_METATYPE(QDeclarativeListProperty<TYPE>)

#define QML_DECLARE_INTERFACE(INTERFACE) \
    QML_DECLARE_TYPE(INTERFACE)

#define QML_DECLARE_INTERFACE_HASMETATYPE(INTERFACE) \
    QML_DECLARE_TYPE_HASMETATYPE(INTERFACE)

enum { /* TYPEINFO flags */
    QML_HAS_ATTACHED_PROPERTIES = 0x01
};

#define QML_DECLARE_TYPEINFO(TYPE, FLAGS) \
QT_BEGIN_NAMESPACE \
template <> \
class QDeclarativeTypeInfo<TYPE > \
{ \
public: \
    enum { \
        hasAttachedProperties = (((FLAGS) & QML_HAS_ATTACHED_PROPERTIES) == QML_HAS_ATTACHED_PROPERTIES) \
    }; \
}; \
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

template<typename T>
int qmlRegisterType()
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        0, 0,
        QString(),

        0, 0, 0, 0, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        0, 0,

        0,
        0
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

int Q_AUTOTEST_EXPORT qmlRegisterTypeNotAvailable(const char *uri, int versionMajor, int versionMinor, const char *qmlName, const QString& message);

template<typename T>
int qmlRegisterUncreatableType(const char *uri, int versionMajor, int versionMinor, const char *qmlName, const QString& reason)
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        0, 0,
        reason,

        uri, versionMajor, versionMinor, qmlName, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        0, 0,

        0,
        0
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

template<typename T>
int qmlRegisterType(const char *uri, int versionMajor, int versionMinor, const char *qmlName)
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        sizeof(T), QDeclarativePrivate::createInto<T>,
        QString(),

        uri, versionMajor, versionMinor, qmlName, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        0, 0,

        0,
        0
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

template<typename T, int metaObjectRevision>
int qmlRegisterType(const char *uri, int versionMajor, int versionMinor, const char *qmlName)
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        1,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        sizeof(T), QDeclarativePrivate::createInto<T>,
        QString(),

        uri, versionMajor, versionMinor, qmlName, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        0, 0,

        0,
        metaObjectRevision
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

template<typename T, int metaObjectRevision>
int qmlRegisterRevision(const char *uri, int versionMajor, int versionMinor)
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        1,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        sizeof(T), QDeclarativePrivate::createInto<T>,
        QString(),

        uri, versionMajor, versionMinor, 0, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        0, 0,

        0,
        metaObjectRevision
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}


template<typename T, typename E>
int qmlRegisterExtendedType()
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        0, 0,
        QString(),

        0, 0, 0, 0, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        QDeclarativePrivate::createParent<E>, &E::staticMetaObject,

        0,
        0
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

template<typename T, typename E>
int qmlRegisterExtendedType(const char *uri, int versionMajor, int versionMinor,
                            const char *qmlName)
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativeAttachedPropertiesFunc attached = QDeclarativePrivate::attachedPropertiesFunc<E>();
    const QMetaObject * attachedMetaObject = QDeclarativePrivate::attachedPropertiesMetaObject<E>();
    if (!attached) {
        attached = QDeclarativePrivate::attachedPropertiesFunc<T>();
        attachedMetaObject = QDeclarativePrivate::attachedPropertiesMetaObject<T>();
    }

    QDeclarativePrivate::RegisterType type = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        sizeof(T), QDeclarativePrivate::createInto<T>,
        QString(),

        uri, versionMajor, versionMinor, qmlName, &T::staticMetaObject,

        attached,
        attachedMetaObject,

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        QDeclarativePrivate::createParent<E>, &E::staticMetaObject,

        0,
        0
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

template<typename T>
int qmlRegisterInterface(const char *typeName)
{
    QByteArray name(typeName);

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterInterface qmlInterface = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),

        qobject_interface_iid<T *>()
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::InterfaceRegistration, &qmlInterface);
}

template<typename T>
int qmlRegisterCustomType(const char *uri, int versionMajor, int versionMinor,
                          const char *qmlName, QDeclarativeCustomParser *parser)
{
    QByteArray name(T::staticMetaObject.className());

    QByteArray pointerName(name + '*');
    QByteArray listName("QDeclarativeListProperty<" + name + ">");

    QDeclarativePrivate::RegisterType type = {
        0,

        qRegisterMetaType<T *>(pointerName.constData()),
        qRegisterMetaType<QDeclarativeListProperty<T> >(listName.constData()),
        sizeof(T), QDeclarativePrivate::createInto<T>,
        QString(),

        uri, versionMajor, versionMinor, qmlName, &T::staticMetaObject,

        QDeclarativePrivate::attachedPropertiesFunc<T>(),
        QDeclarativePrivate::attachedPropertiesMetaObject<T>(),

        QDeclarativePrivate::StaticCastSelector<T,QDeclarativeParserStatus>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueSource>::cast(),
        QDeclarativePrivate::StaticCastSelector<T,QDeclarativePropertyValueInterceptor>::cast(),

        0, 0,

        parser,
        0
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::TypeRegistration, &type);
}

inline int qmlRegisterType(const QUrl &url, const char *uri, int versionMajor, int versionMinor, const char *qmlName)
{
    QDeclarativePrivate::RegisterComponent type = {
        url,
        uri,
        qmlName,
        versionMajor,
        versionMinor
    };

    return QDeclarativePrivate::qmlregister(QDeclarativePrivate::ComponentRegistration, &type);
}

class QDeclarativeContext;
class QDeclarativeEngine;

namespace QtDeclarative {
    // declared in namespace to avoid symbol conflicts with QtQml
    Q_DECLARATIVE_EXPORT void qmlExecuteDeferred(QObject *);
    Q_DECLARATIVE_EXPORT QDeclarativeContext *qmlContext(const QObject *);
    Q_DECLARATIVE_EXPORT QDeclarativeEngine *qmlEngine(const QObject *);
    Q_DECLARATIVE_EXPORT QObject *qmlAttachedPropertiesObjectById(int, const QObject *,
                                                                  bool create = true);
    Q_DECLARATIVE_EXPORT QObject *qmlAttachedPropertiesObject(int *, const QObject *,
                                                              const QMetaObject *, bool create);
}
using namespace QtDeclarative;

template<typename T>
QObject *qmlAttachedPropertiesObject(const QObject *obj, bool create = true)
{
    static int idx = -1;
    return qmlAttachedPropertiesObject(&idx, obj, &T::staticMetaObject, create);
}

QT_END_NAMESPACE

QML_DECLARE_TYPE(QObject)
Q_DECLARE_METATYPE(QVariant)

#endif // QDECLARATIVE_H
