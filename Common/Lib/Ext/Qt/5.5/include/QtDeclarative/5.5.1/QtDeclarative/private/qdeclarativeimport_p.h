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

#ifndef QDECLARATIVEIMPORT_P_H
#define QDECLARATIVEIMPORT_P_H

#include <QtCore/qurl.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qset.h>
#include <private/qdeclarativedirparser_p.h>
#include <private/qdeclarativescriptparser_p.h>
#include <private/qdeclarativemetatype_p.h>

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

QT_BEGIN_NAMESPACE

class QDeclarativeTypeNameCache;
class QDeclarativeEngine;
class QDir;
class QDeclarativeImportedNamespace;
class QDeclarativeImportsPrivate;
class QDeclarativeImportDatabase;
class QDeclarativeTypeLoader;

class QDeclarativeImports
{
public:
    QDeclarativeImports();
    QDeclarativeImports(QDeclarativeTypeLoader *);
    QDeclarativeImports(const QDeclarativeImports &);
    ~QDeclarativeImports();
    QDeclarativeImports &operator=(const QDeclarativeImports &);

    void setBaseUrl(const QUrl &url);
    QUrl baseUrl() const;

    bool resolveType(const QByteArray& type,
                     QDeclarativeType** type_return, QUrl* url_return,
                     int *version_major, int *version_minor,
                     QDeclarativeImportedNamespace** ns_return,
                     QString *errorString = 0) const;
    bool resolveType(QDeclarativeImportedNamespace*,
                     const QByteArray& type,
                     QDeclarativeType** type_return, QUrl* url_return,
                     int *version_major, int *version_minor) const;

    bool addImport(QDeclarativeImportDatabase *,
                   const QString& uri, const QString& prefix, int vmaj, int vmin,
                   QDeclarativeScriptParser::Import::Type importType,
                   const QDeclarativeDirComponents &qmldircomponentsnetwork,
                   QString *errorString);

    void populateCache(QDeclarativeTypeNameCache *cache, QDeclarativeEngine *) const;

private:
    friend class QDeclarativeImportDatabase;
    QDeclarativeImportsPrivate *d;
};

class QDeclarativeImportDatabase
{
    Q_DECLARE_TR_FUNCTIONS(QDeclarativeImportDatabase)
public:
    QDeclarativeImportDatabase(QDeclarativeEngine *);
    ~QDeclarativeImportDatabase();

    bool importPlugin(const QString &filePath, const QString &uri, QString *errorString);

    QStringList importPathList() const;
    void setImportPathList(const QStringList &paths);
    void addImportPath(const QString& dir);

    QStringList pluginPathList() const;
    void setPluginPathList(const QStringList &paths);
    void addPluginPath(const QString& path);

private:
    friend class QDeclarativeImportsPrivate;
    QString resolvePlugin(const QDir &qmldirPath, const QString &qmldirPluginPath,
                          const QString &baseName, const QStringList &suffixes,
                          const QString &prefix = QString());
    QString resolvePlugin(const QDir &qmldirPath, const QString &qmldirPluginPath,
                          const QString &baseName);


    QStringList filePluginPath;
    QStringList fileImportPath;

    QSet<QString> initializedPlugins;
    QDeclarativeEngine *engine;
};

QT_END_NAMESPACE

#endif // QDECLARATIVEIMPORT_P_H

