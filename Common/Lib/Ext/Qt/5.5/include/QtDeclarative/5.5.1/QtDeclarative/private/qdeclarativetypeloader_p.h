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

#ifndef QDECLARATIVETYPELOADER_P_H
#define QDECLARATIVETYPELOADER_P_H

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

#include <QtCore/qobject.h>
#include <QtNetwork/qnetworkreply.h>
#include <QtDeclarative/qdeclarativeerror.h>
#include <QtDeclarative/qdeclarativeengine.h>
#include <private/qdeclarativescriptparser_p.h>
#include <private/qdeclarativedirparser_p.h>
#include <private/qdeclarativeimport_p.h>

QT_BEGIN_NAMESPACE

class QDeclarativeScriptData;
class QDeclarativeQmldirData;
class QDeclarativeTypeLoader;
class QDeclarativeCompiledData;
class QDeclarativeComponentPrivate;
class QDeclarativeTypeData;
class QDeclarativeDataLoader;

class Q_AUTOTEST_EXPORT QDeclarativeDataBlob : public QDeclarativeRefCount
{
public:
    enum Status {
        Null,                    // Prior to QDeclarativeDataLoader::load()
        Loading,                 // Prior to data being received and dataReceived() being called
        WaitingForDependencies,  // While there are outstanding addDependency()s
        Complete,                // Finished
        Error                    // Error
    };

    enum Type {
        QmlFile,
        JavaScriptFile,
        QmldirFile
    };

    QDeclarativeDataBlob(const QUrl &, Type);
    virtual ~QDeclarativeDataBlob();

    Type type() const;

    Status status() const;
    bool isNull() const;
    bool isLoading() const;
    bool isWaiting() const;
    bool isComplete() const;
    bool isError() const;
    bool isCompleteOrError() const;

    qreal progress() const;

    QUrl url() const;
    QUrl finalUrl() const;

    QList<QDeclarativeError> errors() const;

    void setError(const QDeclarativeError &);
    void setError(const QList<QDeclarativeError> &errors);

    void addDependency(QDeclarativeDataBlob *);

protected:
    virtual void dataReceived(const QByteArray &) = 0;

    virtual void done();
    virtual void networkError(QNetworkReply::NetworkError);

    virtual void dependencyError(QDeclarativeDataBlob *);
    virtual void dependencyComplete(QDeclarativeDataBlob *);
    virtual void allDependenciesDone();

    virtual void downloadProgressChanged(qreal);

private:
    friend class QDeclarativeDataLoader;
    void tryDone();
    void cancelAllWaitingFor();
    void notifyAllWaitingOnMe();
    void notifyComplete(QDeclarativeDataBlob *);

    Type m_type;
    Status m_status;
    qreal m_progress;

    QUrl m_url;
    QUrl m_finalUrl;

    // List of QDeclarativeDataBlob's that are waiting for me to complete.
    QList<QDeclarativeDataBlob *> m_waitingOnMe;

    // List of QDeclarativeDataBlob's that I am waiting for to complete.
    QList<QDeclarativeDataBlob *> m_waitingFor;

    // Manager that is currently fetching data for me
    QDeclarativeDataLoader *m_manager;
    int m_redirectCount:30;
    bool m_inCallback:1;
    bool m_isDone:1;

    QList<QDeclarativeError> m_errors;
};

class Q_AUTOTEST_EXPORT QDeclarativeDataLoader : public QObject
{
    Q_OBJECT
public:
    QDeclarativeDataLoader(QDeclarativeEngine *);
    ~QDeclarativeDataLoader();

    void load(QDeclarativeDataBlob *);
    void loadWithStaticData(QDeclarativeDataBlob *, const QByteArray &);

    QDeclarativeEngine *engine() const;

private slots:
    void networkReplyFinished();
    void networkReplyProgress(qint64,qint64);

private:
    void setData(QDeclarativeDataBlob *, const QByteArray &);
    void networkReplyFinished(QNetworkReply *);
    void networkReplyProgress(QNetworkReply *, qint64, qint64);

    QDeclarativeEngine *m_engine;
    typedef QHash<QNetworkReply *, QDeclarativeDataBlob *> NetworkReplies;
    NetworkReplies m_networkReplies;
};


class Q_AUTOTEST_EXPORT QDeclarativeTypeLoader : public QDeclarativeDataLoader
{
    Q_OBJECT
public:
    QDeclarativeTypeLoader(QDeclarativeEngine *);
    ~QDeclarativeTypeLoader();

    enum Option {
        None,
        PreserveParser
    };
    Q_DECLARE_FLAGS(Options, Option)

    QDeclarativeTypeData *get(const QUrl &url);
    QDeclarativeTypeData *get(const QByteArray &, const QUrl &url, Options = None);
    void clearCache();

    QDeclarativeScriptData *getScript(const QUrl &);
    QDeclarativeQmldirData *getQmldir(const QUrl &);

    QString absoluteFilePath(const QString &path);
    const QDeclarativeDirParser *qmlDirParser(const QString &absoluteFilePath);

private:
    typedef QHash<QUrl, QDeclarativeTypeData *> TypeCache;
    typedef QHash<QUrl, QDeclarativeScriptData *> ScriptCache;
    typedef QHash<QUrl, QDeclarativeQmldirData *> QmldirCache;
    typedef QSet<QString> StringSet;
    typedef QHash<QString, StringSet*> ImportDirCache;
    typedef QHash<QString, QDeclarativeDirParser*> ImportQmlDirCache;

    TypeCache m_typeCache;
    ScriptCache m_scriptCache;
    QmldirCache m_qmldirCache;
    ImportDirCache m_importDirCache;
    ImportQmlDirCache m_importQmlDirCache;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QDeclarativeTypeLoader::Options)

class Q_AUTOTEST_EXPORT QDeclarativeTypeData : public QDeclarativeDataBlob
{
public:
    struct TypeReference
    {
        TypeReference() : type(0), majorVersion(0), minorVersion(0), typeData(0) {}

        QDeclarativeParser::Location location;
        QDeclarativeType *type;
        int majorVersion;
        int minorVersion;
        QDeclarativeTypeData *typeData;
    };

    struct ScriptReference
    {
        ScriptReference() : script(0) {}

        QDeclarativeParser::Location location;
        QString qualifier;
        QDeclarativeScriptData *script;
    };

    QDeclarativeTypeData(const QUrl &, QDeclarativeTypeLoader::Options, QDeclarativeTypeLoader *);
    ~QDeclarativeTypeData();

    QDeclarativeTypeLoader *typeLoader() const;

    const QDeclarativeImports &imports() const;
    const QDeclarativeScriptParser &parser() const;

    const QList<TypeReference> &resolvedTypes() const;
    const QList<ScriptReference> &resolvedScripts() const;

    QDeclarativeCompiledData *compiledData() const;

    // Used by QDeclarativeComponent to get notifications
    struct TypeDataCallback {
        ~TypeDataCallback() {}
        virtual void typeDataProgress(QDeclarativeTypeData *, qreal) {}
        virtual void typeDataReady(QDeclarativeTypeData *) {}
    };
    void registerCallback(TypeDataCallback *);
    void unregisterCallback(TypeDataCallback *);

protected:
    virtual void done();
    virtual void dataReceived(const QByteArray &);
    virtual void allDependenciesDone();
    virtual void downloadProgressChanged(qreal);

private:
    void resolveTypes();
    void compile();

    QDeclarativeTypeLoader::Options m_options;

    QDeclarativeQmldirData *qmldirForUrl(const QUrl &);

    QDeclarativeScriptParser scriptParser;
    QDeclarativeImports m_imports;

    QList<ScriptReference> m_scripts;
    QList<QDeclarativeQmldirData *> m_qmldirs;

    QList<TypeReference> m_types;
    bool m_typesResolved:1;

    QDeclarativeCompiledData *m_compiledData;

    QList<TypeDataCallback *> m_callbacks;

    QDeclarativeTypeLoader *m_typeLoader;
};

class Q_AUTOTEST_EXPORT QDeclarativeScriptData : public QDeclarativeDataBlob
{
public:
    QDeclarativeScriptData(const QUrl &);

    QDeclarativeParser::Object::ScriptBlock::Pragmas pragmas() const;
    QString scriptSource() const;

protected:
    virtual void dataReceived(const QByteArray &);

private:
    QDeclarativeParser::Object::ScriptBlock::Pragmas m_pragmas;
    QString m_source;
};

class Q_AUTOTEST_EXPORT QDeclarativeQmldirData : public QDeclarativeDataBlob
{
public:
    QDeclarativeQmldirData(const QUrl &);

    const QDeclarativeDirComponents &dirComponents() const;

protected:
    virtual void dataReceived(const QByteArray &);

private:
    QDeclarativeDirComponents m_components;

};

QT_END_NAMESPACE

#endif // QDECLARATIVETYPELOADER_P_H
