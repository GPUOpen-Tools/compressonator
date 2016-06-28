/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the ActiveQt framework of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAXFACTORY_H
#define QAXFACTORY_H

#include <QtCore/qhash.h>
#include <QtCore/quuid.h>
#include <QtCore/qfactoryinterface.h>
#include <QtCore/qmetaobject.h>
#include <QtCore/qstringlist.h>

struct IUnknown;
struct IDispatch;

QT_BEGIN_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Woverloaded-virtual") // gcc complains about QObject::metaObject() being hidden.

class QWidget;
class QSettings;

class QAxFactory : public QObject
{
public:
    QAxFactory(const QUuid &libId, const QUuid &appId);
    virtual ~QAxFactory();

    virtual QStringList featureList() const = 0;

    virtual QObject *createObject(const QString &key) = 0;
    virtual const QMetaObject *metaObject(const QString &key) const = 0;
    virtual bool createObjectWrapper(QObject *object, IDispatch **wrapper);

    virtual QUuid classID(const QString &key) const;
    virtual QUuid interfaceID(const QString &key) const;
    virtual QUuid eventsID(const QString &key) const;

    virtual QUuid typeLibID() const;
    virtual QUuid appID() const;

    virtual void registerClass(const QString &key, QSettings *) const;
    virtual void unregisterClass(const QString &key, QSettings *) const;

    virtual bool validateLicenseKey(const QString &key, const QString &licenseKey) const;

    virtual QString exposeToSuperClass(const QString &key) const;
    virtual bool stayTopLevel(const QString &key) const;
    virtual bool hasStockEvents(const QString &key) const;
    virtual bool isService() const;

    enum ServerType {
        SingleInstance,
        MultipleInstances
    };

    static bool isServer();
    static QString serverDirPath();
    static QString serverFilePath();
    static bool startServer(ServerType type = MultipleInstances);
    static bool stopServer();

    static bool registerActiveObject(QObject *object);

private:
    QUuid typelib;
    QUuid app;
};

extern QAxFactory *qAxFactory();

extern bool qax_startServer(QAxFactory::ServerType);

inline bool QAxFactory::startServer(ServerType type)
{
    // implementation in qaxservermain.cpp
    return qax_startServer(type);
}

extern bool qax_stopServer();

inline bool QAxFactory::stopServer()
{
    // implementation in qaxservermain.cpp
    return qax_stopServer();
}

#define QAXFACTORY_EXPORT(IMPL, TYPELIB, APPID) \
    QT_BEGIN_NAMESPACE \
    QAxFactory *qax_instantiate()               \
    {                                                   \
        IMPL *impl = new IMPL(QUuid(TYPELIB), QUuid(APPID));    \
        return impl;                                    \
    } \
    QT_END_NAMESPACE

#define QAXFACTORY_DEFAULT(Class, IIDClass, IIDInterface, IIDEvents, IIDTypeLib, IIDApp) \
    QT_BEGIN_NAMESPACE \
    class QAxDefaultFactory : public QAxFactory \
    { \
    public: \
        QAxDefaultFactory(const QUuid &app, const QUuid &lib) \
        : QAxFactory(app, lib), className(QLatin1String(#Class)) {} \
        QStringList featureList() const \
        { \
            QStringList list; \
            list << className; \
            return list; \
        } \
        const QMetaObject *metaObject(const QString &key) const \
        { \
            if (key == className) \
            return &Class::staticMetaObject; \
            return 0; \
        } \
        QObject *createObject(const QString &key) \
        { \
            if (key == className) \
                return new Class(0); \
            return 0; \
        } \
        QUuid classID(const QString &key) const \
        { \
            if (key == className) \
                return QUuid(IIDClass); \
            return QUuid(); \
        } \
        QUuid interfaceID(const QString &key) const \
        { \
            if (key == className) \
                return QUuid(IIDInterface); \
            return QUuid(); \
        } \
        QUuid eventsID(const QString &key) const \
        { \
            if (key == className) \
                return QUuid(IIDEvents); \
            return QUuid(); \
        } \
    private: \
        QString className; \
    }; \
    QT_END_NAMESPACE \
    QAXFACTORY_EXPORT(QAxDefaultFactory, IIDTypeLib, IIDApp) \

template<class T>
class QAxClass : public QAxFactory
{
public:
    QAxClass(const QString &libId, const QString &appId)
    : QAxFactory(libId, appId)
    {}

    const QMetaObject *metaObject(const QString &) const { return &T::staticMetaObject; }
    QStringList featureList() const { return QStringList(QString(T::staticMetaObject.className())); }
    QObject *createObject(const QString &key)
    {
        const QMetaObject &mo = T::staticMetaObject;
        if (key != QLatin1String(mo.className()))
            return 0;
        if (!qstrcmp(mo.classInfo(mo.indexOfClassInfo("Creatable")).value(), "no"))
            return 0;
        return new T(0);
    }
};

#define QAXFACTORY_BEGIN(IDTypeLib, IDApp) \
    QT_BEGIN_NAMESPACE \
    class QAxFactoryList : public QAxFactory \
    { \
        QStringList factoryKeys; \
        QHash<QString, QAxFactory*> factories; \
        QHash<QString, bool> creatable; \
    public: \
        QAxFactoryList() \
        : QAxFactory(IDTypeLib, IDApp) \
        { \
            QAxFactory *factory = 0; \
            QStringList keys; \
            QStringList::Iterator it; \

#define QAXCLASS(Class) \
            factory = new QAxClass<Class>(typeLibID().toString(), appID().toString()); \
            qRegisterMetaType<Class*>(#Class"*"); \
            keys = factory->featureList(); \
            foreach (const QString &key, keys) { \
                factoryKeys += key; \
                factories.insert(key, factory); \
                creatable.insert(key, true); \
            }\

#define QAXTYPE(Class) \
            factory = new QAxClass<Class>(typeLibID().toString(), appID().toString()); \
            qRegisterMetaType<Class*>(#Class"*"); \
            keys = factory->featureList(); \
            foreach (const QString &key, keys) { \
                factoryKeys += key; \
                factories.insert(key, factory); \
                creatable.insert(key, false); \
            }\

#define QAXFACTORY_END() \
        } \
        ~QAxFactoryList() { qDeleteAll(factories); } \
        QStringList featureList() const {  return factoryKeys; } \
        const QMetaObject *metaObject(const QString&key) const { \
            QAxFactory *f = factories[key]; \
            return f ? f->metaObject(key) : 0; \
        } \
        QObject *createObject(const QString &key) { \
            if (!creatable.value(key)) \
                return 0; \
            QAxFactory *f = factories[key]; \
            return f ? f->createObject(key) : 0; \
        } \
        QUuid classID(const QString &key) { \
            QAxFactory *f = factories.value(key); \
            return f ? f->classID(key) : QUuid(); \
        } \
        QUuid interfaceID(const QString &key) { \
            QAxFactory *f = factories.value(key); \
            return f ? f->interfaceID(key) : QUuid(); \
        } \
        QUuid eventsID(const QString &key) { \
            QAxFactory *f = factories.value(key); \
            return f ? f->eventsID(key) : QUuid(); \
        } \
        void registerClass(const QString &key, QSettings *s) const { \
            QAxFactory *f = factories.value(key); \
            if (f) f->registerClass(key, s); \
        } \
        void unregisterClass(const QString &key, QSettings *s) const { \
            QAxFactory *f = factories.value(key); \
            if (f) f->unregisterClass(key, s); \
        } \
        QString exposeToSuperClass(const QString &key) const { \
            QAxFactory *f = factories.value(key); \
            return f ? f->exposeToSuperClass(key) : QString(); \
        } \
        bool stayTopLevel(const QString &key) const { \
            QAxFactory *f = factories.value(key); \
            return f ? f->stayTopLevel(key) : false; \
        } \
        bool hasStockEvents(const QString &key) const { \
            QAxFactory *f = factories.value(key); \
            return f ? f->hasStockEvents(key) : false; \
        } \
    }; \
    QAxFactory *qax_instantiate()               \
    {                                                   \
        QAxFactoryList *impl = new QAxFactoryList();    \
        return impl;                                    \
    } \
    QT_END_NAMESPACE

QT_WARNING_POP
QT_END_NAMESPACE

#ifndef Q_COM_METATYPE_DECLARED
#define Q_COM_METATYPE_DECLARED

Q_DECLARE_OPAQUE_POINTER(IUnknown*)
Q_DECLARE_OPAQUE_POINTER(IDispatch*)

Q_DECLARE_METATYPE(IUnknown*)
Q_DECLARE_METATYPE(IDispatch*)

#endif

#endif // QAXFACTORY_H
