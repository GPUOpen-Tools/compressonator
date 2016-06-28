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

#ifndef QDECLARATIVEENGINE_H
#define QDECLARATIVEENGINE_H

#include <QtCore/qurl.h>
#include <QtCore/qobject.h>
#include <QtCore/qmap.h>
#include <QtScript/qscriptvalue.h>
#include <QtDeclarative/qdeclarativeerror.h>
#include <QtDeclarative/qdeclarativedebug.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeComponent;
class QDeclarativeEnginePrivate;
class QDeclarativeImportsPrivate;
class QDeclarativeExpression;
class QDeclarativeContext;
class QDeclarativeType;
class QUrl;
class QScriptEngine;
class QScriptContext;
class QDeclarativeImageProvider;
class QNetworkAccessManager;
class QDeclarativeNetworkAccessManagerFactory;
class Q_DECLARATIVE_EXPORT QDeclarativeEngine : public QObject
{
    Q_PROPERTY(QString offlineStoragePath READ offlineStoragePath WRITE setOfflineStoragePath)
    Q_OBJECT
public:
    QDeclarativeEngine(QObject *p = 0);
    virtual ~QDeclarativeEngine();

    QDeclarativeContext *rootContext() const;

    void clearComponentCache();

    QStringList importPathList() const;
    void setImportPathList(const QStringList &paths);
    void addImportPath(const QString& dir);

    QStringList pluginPathList() const;
    void setPluginPathList(const QStringList &paths);
    void addPluginPath(const QString& dir);

    bool importPlugin(const QString &filePath, const QString &uri, QString *errorString);

    void setNetworkAccessManagerFactory(QDeclarativeNetworkAccessManagerFactory *);
    QDeclarativeNetworkAccessManagerFactory *networkAccessManagerFactory() const;

    QNetworkAccessManager *networkAccessManager() const;

    void addImageProvider(const QString &id, QDeclarativeImageProvider *);
    QDeclarativeImageProvider *imageProvider(const QString &id) const;
    void removeImageProvider(const QString &id);

    void setOfflineStoragePath(const QString& dir);
    QString offlineStoragePath() const;

    QUrl baseUrl() const;
    void setBaseUrl(const QUrl &);

    bool outputWarningsToStandardError() const;
    void setOutputWarningsToStandardError(bool);

    static QDeclarativeContext *contextForObject(const QObject *);
    static void setContextForObject(QObject *, QDeclarativeContext *);

    enum ObjectOwnership { CppOwnership, JavaScriptOwnership };
    static void setObjectOwnership(QObject *, ObjectOwnership);
    static ObjectOwnership objectOwnership(QObject *);

Q_SIGNALS:
    void quit();
    void warnings(const QList<QDeclarativeError> &warnings);

private:
    Q_DISABLE_COPY(QDeclarativeEngine)
    Q_DECLARE_PRIVATE(QDeclarativeEngine)
};

QT_END_NAMESPACE

#endif // QDECLARATIVEENGINE_H
