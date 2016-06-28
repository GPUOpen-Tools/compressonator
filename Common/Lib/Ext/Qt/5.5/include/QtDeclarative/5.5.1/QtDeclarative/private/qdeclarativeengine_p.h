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

#ifndef QDECLARATIVEENGINE_P_H
#define QDECLARATIVEENGINE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qdeclarativeengine.h"

#include "private/qdeclarativetypeloader_p.h"
#include "private/qdeclarativeimport_p.h"
#include "private/qpodvector_p.h"
#include "qdeclarative.h"
#include "private/qdeclarativevaluetype_p.h"
#include "qdeclarativecontext.h"
#include "private/qdeclarativecontext_p.h"
#include "qdeclarativeexpression.h"
#include "qdeclarativeimageprovider.h"
#include "private/qdeclarativeproperty_p.h"
#include "private/qdeclarativepropertycache_p.h"
#include "private/qdeclarativeobjectscriptclass_p.h"
#include "private/qdeclarativecontextscriptclass_p.h"
#include "private/qdeclarativevaluetypescriptclass_p.h"
#include "private/qdeclarativemetatype_p.h"
#include "private/qdeclarativedirparser_p.h"

#include <QtScript/QScriptClass>
#include <QtScript/QScriptValue>
#include <QtScript/QScriptString>
#include <QtCore/qstring.h>
#include <QtCore/qlist.h>
#include <QtCore/qpair.h>
#include <QtCore/qstack.h>
#include <QtCore/qmutex.h>
#include <QtScript/qscriptengine.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeContext;
class QDeclarativeEngine;
class QDeclarativeContextPrivate;
class QDeclarativeExpression;
class QDeclarativeContextScriptClass;
class QDeclarativeImportDatabase;
class QDeclarativeObjectScriptClass;
class QDeclarativeTypeNameScriptClass;
class QDeclarativeValueTypeScriptClass;
class QNetworkReply;
class QNetworkAccessManager;
class QDeclarativeNetworkAccessManagerFactory;
class QDeclarativeAbstractBinding;
class QScriptDeclarativeClass;
class QDeclarativeTypeNameScriptClass;
class QDeclarativeTypeNameCache;
class QDeclarativeComponentAttached;
class QDeclarativeListScriptClass;
class QDeclarativeCleanup;
class QDeclarativeDelayedError;
class QDeclarativeWorkerScriptEngine;
class QDeclarativeGlobalScriptClass;
class QDir;

class QDeclarativeScriptEngine : public QScriptEngine
{
public:
    QDeclarativeScriptEngine(QDeclarativeEnginePrivate *priv);
    virtual ~QDeclarativeScriptEngine();

    QUrl resolvedUrl(QScriptContext *context, const QUrl& url); // resolved against p's context, or baseUrl if no p
    static QScriptValue resolvedUrl(QScriptContext *ctxt, QScriptEngine *engine);

    static QDeclarativeScriptEngine *get(QScriptEngine* e) { return static_cast<QDeclarativeScriptEngine*>(e); }

    QDeclarativeEnginePrivate *p;

    // User by SQL API
    QScriptClass *sqlQueryClass;
    QString offlineStoragePath;

    // Used by DOM Core 3 API
    QScriptClass *namedNodeMapClass;
    QScriptClass *nodeListClass;

    QUrl baseUrl;

    virtual QNetworkAccessManager *networkAccessManager();
};

class Q_AUTOTEST_EXPORT QDeclarativeEnginePrivate : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeEngine)
public:
    QDeclarativeEnginePrivate(QDeclarativeEngine *);
    ~QDeclarativeEnginePrivate();

    void init();

    struct CapturedProperty {
        CapturedProperty(QObject *o, int c, int n)
            : object(o), coreIndex(c), notifier(0), notifyIndex(n) {}
        CapturedProperty(QDeclarativeNotifier *n)
            : object(0), coreIndex(-1), notifier(n), notifyIndex(-1) {}

        QObject *object;
        int coreIndex;
        QDeclarativeNotifier *notifier;
        int notifyIndex;
    };
    bool captureProperties;
    QPODVector<CapturedProperty, 16> capturedProperties;

    QDeclarativeContext *rootContext;
    bool isDebugging;

    bool outputWarningsToStdErr;

    QDeclarativeContextScriptClass *contextClass;
    QDeclarativeContextData *sharedContext;
    QObject *sharedScope;
    QDeclarativeObjectScriptClass *objectClass;
    QDeclarativeValueTypeScriptClass *valueTypeClass;
    QDeclarativeTypeNameScriptClass *typeNameClass;
    QDeclarativeListScriptClass *listClass;
    // Global script class
    QDeclarativeGlobalScriptClass *globalClass;

    // Registered cleanup handlers
    QDeclarativeCleanup *cleanup;

    // Bindings that have had errors during startup
    QDeclarativeDelayedError *erroredBindings;
    int inProgressCreations;

    QDeclarativeScriptEngine scriptEngine;

    QDeclarativeWorkerScriptEngine *getWorkerScriptEngine();
    QDeclarativeWorkerScriptEngine *workerScriptEngine;

    QUrl baseUrl;

    template<class T>
    struct SimpleList {
        SimpleList()
            : count(0), values(0) {}
        SimpleList(int r)
            : count(0), values(new T*[r]) {}

        int count;
        T **values;

        void append(T *v) {
            values[count++] = v;
        }

        T *at(int idx) const {
            return values[idx];
        }

        void clear() {
            delete [] values;
        }
    };

    static void clear(SimpleList<QDeclarativeAbstractBinding> &);
    static void clear(SimpleList<QDeclarativeParserStatus> &);

    QList<SimpleList<QDeclarativeAbstractBinding> > bindValues;
    QList<SimpleList<QDeclarativeParserStatus> > parserStatus;
    QList<QPair<QDeclarativeGuard<QObject>,int> > finalizedParserStatus;
    QDeclarativeComponentAttached *componentAttached;

    void registerFinalizedParserStatusObject(QObject *obj, int index) {
        finalizedParserStatus.append(qMakePair(QDeclarativeGuard<QObject>(obj), index));
    }

    bool inBeginCreate;

    QNetworkAccessManager *createNetworkAccessManager(QObject *parent) const;
    QNetworkAccessManager *getNetworkAccessManager() const;
    mutable QNetworkAccessManager *networkAccessManager;
    mutable QDeclarativeNetworkAccessManagerFactory *networkAccessManagerFactory;

    QHash<QString,QSharedPointer<QDeclarativeImageProvider> > imageProviders;
    QDeclarativeImageProvider::ImageType getImageProviderType(const QUrl &url);
    QImage getImageFromProvider(const QUrl &url, QSize *size, const QSize& req_size);
    QPixmap getPixmapFromProvider(const QUrl &url, QSize *size, const QSize& req_size);

    mutable QMutex mutex;

    QDeclarativeTypeLoader typeLoader;
    QDeclarativeImportDatabase importDatabase;

    QString offlineStoragePath;

    mutable quint32 uniqueId;
    quint32 getUniqueId() const {
        return uniqueId++;
    }

    QDeclarativeValueTypeFactory valueTypes;

    QHash<const QMetaObject *, QDeclarativePropertyCache *> propertyCache;
    QHash<QPair<QDeclarativeType *, int>, QDeclarativePropertyCache *> typePropertyCache;
    inline QDeclarativePropertyCache *cache(QObject *obj);
    inline QDeclarativePropertyCache *cache(const QMetaObject *);
    inline QDeclarativePropertyCache *cache(QDeclarativeType *, int, QDeclarativeError &error);
    QDeclarativePropertyCache *createCache(const QMetaObject *);
    QDeclarativePropertyCache *createCache(QDeclarativeType *, int, QDeclarativeError &error);

    void registerCompositeType(QDeclarativeCompiledData *);

    bool isQObject(int);
    QObject *toQObject(const QVariant &, bool *ok = 0) const;
    QDeclarativeMetaType::TypeCategory typeCategory(int) const;
    bool isList(int) const;
    int listType(int) const;
    const QMetaObject *rawMetaObjectForType(int) const;
    const QMetaObject *metaObjectForType(int) const;
    QHash<int, int> m_qmlLists;
    QHash<int, QDeclarativeCompiledData *> m_compositeTypes;

    QHash<QString, QScriptValue> m_sharedScriptImports;

    QScriptValue scriptValueFromVariant(const QVariant &);
    QVariant scriptValueToVariant(const QScriptValue &, int hint = QVariant::Invalid);

    void sendQuit();
    void warning(const QDeclarativeError &);
    void warning(const QList<QDeclarativeError> &);
    static void warning(QDeclarativeEngine *, const QDeclarativeError &);
    static void warning(QDeclarativeEngine *, const QList<QDeclarativeError> &);
    static void warning(QDeclarativeEnginePrivate *, const QDeclarativeError &);
    static void warning(QDeclarativeEnginePrivate *, const QList<QDeclarativeError> &);

    static QScriptValue qmlScriptObject(QObject*, QDeclarativeEngine*);

    static QScriptValue createComponent(QScriptContext*, QScriptEngine*);
    static QScriptValue createQmlObject(QScriptContext*, QScriptEngine*);
    static QScriptValue isQtObject(QScriptContext*, QScriptEngine*);
    static QScriptValue vector3d(QScriptContext*, QScriptEngine*);
    static QScriptValue rgba(QScriptContext*, QScriptEngine*);
    static QScriptValue hsla(QScriptContext*, QScriptEngine*);
    static QScriptValue point(QScriptContext*, QScriptEngine*);
    static QScriptValue size(QScriptContext*, QScriptEngine*);
    static QScriptValue rect(QScriptContext*, QScriptEngine*);

    static QScriptValue lighter(QScriptContext*, QScriptEngine*);
    static QScriptValue darker(QScriptContext*, QScriptEngine*);
    static QScriptValue tint(QScriptContext*, QScriptEngine*);

    static QScriptValue desktopOpenUrl(QScriptContext*, QScriptEngine*);
    static QScriptValue fontFamilies(QScriptContext*, QScriptEngine*);
    static QScriptValue md5(QScriptContext*, QScriptEngine*);
    static QScriptValue btoa(QScriptContext*, QScriptEngine*);
    static QScriptValue atob(QScriptContext*, QScriptEngine*);
    static QScriptValue consoleLog(QScriptContext*, QScriptEngine*);
    static QScriptValue quit(QScriptContext*, QScriptEngine*);

#ifndef QT_NO_DATESTRING
    static QScriptValue formatDate(QScriptContext*, QScriptEngine*);
    static QScriptValue formatTime(QScriptContext*, QScriptEngine*);
    static QScriptValue formatDateTime(QScriptContext*, QScriptEngine*);
#endif
    static QScriptEngine *getScriptEngine(QDeclarativeEngine *e) { return &e->d_func()->scriptEngine; }
    static QDeclarativeEngine *getEngine(QScriptEngine *e) { return static_cast<QDeclarativeScriptEngine*>(e)->p->q_func(); }
    static QDeclarativeEnginePrivate *get(QDeclarativeEngine *e) { return e->d_func(); }
    static QDeclarativeEnginePrivate *get(QDeclarativeContext *c) { return (c && c->engine()) ? QDeclarativeEnginePrivate::get(c->engine()) : 0; }
    static QDeclarativeEnginePrivate *get(QDeclarativeContextData *c) { return (c && c->engine) ? QDeclarativeEnginePrivate::get(c->engine) : 0; }
    static QDeclarativeEnginePrivate *get(QScriptEngine *e) { return static_cast<QDeclarativeScriptEngine*>(e)->p; }
    static QDeclarativeEngine *get(QDeclarativeEnginePrivate *p) { return p->q_func(); }
    QDeclarativeContextData *getContext(QScriptContext *);
    QUrl getUrl(QScriptContext *);

    static QString urlToLocalFileOrQrc(const QUrl& url);

    static void defineModule();
    static void defineModuleCompat();

    static bool qml_debugging_enabled;
};

/*!
Returns a QDeclarativePropertyCache for \a obj if one is available.

If \a obj is null, being deleted or contains a dynamic meta object 0
is returned.

The returned cache is not referenced, so if it is to be stored, call addref().
*/
QDeclarativePropertyCache *QDeclarativeEnginePrivate::cache(QObject *obj)
{
    if (!obj || QObjectPrivate::get(obj)->metaObject || QObjectPrivate::get(obj)->wasDeleted)
        return 0;

    const QMetaObject *mo = obj->metaObject();
    QDeclarativePropertyCache *rv = propertyCache.value(mo);
    if (!rv) rv = createCache(mo);
    return rv;
}

/*!
Returns a QDeclarativePropertyCache for \a metaObject.

As the cache is persisted for the life of the engine, \a metaObject must be
a static "compile time" meta-object, or a meta-object that is otherwise known to
exist for the lifetime of the QDeclarativeEngine.

The returned cache is not referenced, so if it is to be stored, call addref().
*/
QDeclarativePropertyCache *QDeclarativeEnginePrivate::cache(const QMetaObject *metaObject)
{
    Q_ASSERT(metaObject);

    QDeclarativePropertyCache *rv = propertyCache.value(metaObject);
    if (!rv) rv = createCache(metaObject);
    return rv;
}

/*!
Returns a QDeclarativePropertyCache for \a type with \a minorVersion.

The returned cache is not referenced, so if it is to be stored, call addref().
*/
QDeclarativePropertyCache *QDeclarativeEnginePrivate::cache(QDeclarativeType *type, int minorVersion, QDeclarativeError &error)
{
    Q_ASSERT(type);

    if (minorVersion == -1 || !type->containsRevisionedAttributes())
        return cache(type->metaObject());

    QDeclarativePropertyCache *rv = typePropertyCache.value(qMakePair(type, minorVersion));
    if (!rv) rv = createCache(type, minorVersion, error);
    return rv;
}

QT_END_NAMESPACE

#endif // QDECLARATIVEENGINE_P_H
