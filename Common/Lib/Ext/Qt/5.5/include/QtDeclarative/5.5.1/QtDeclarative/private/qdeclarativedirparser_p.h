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

#ifndef QDECLARATIVEDIRPARSER_P_H
#define QDECLARATIVEDIRPARSER_P_H

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

#include <QtCore/QUrl>
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

class QDeclarativeError;
class QDeclarativeDirParser
{
    Q_DISABLE_COPY(QDeclarativeDirParser)

public:
    QDeclarativeDirParser();
    ~QDeclarativeDirParser();

    QUrl url() const;
    void setUrl(const QUrl &url);

    QString source() const;
    void setSource(const QString &source);

    QString fileSource() const;
    void setFileSource(const QString &filePath);

    bool isParsed() const;
    bool parse();

    bool hasError() const;
    QList<QDeclarativeError> errors(const QString &uri) const;

    struct Plugin
    {
        Plugin() {}

        Plugin(const QString &name, const QString &path)
            : name(name), path(path) {}

        QString name;
        QString path;
    };

    struct Component
    {
        Component()
            : majorVersion(0), minorVersion(0), internal(false) {}

        Component(const QString &typeName, const QString &fileName, int majorVersion, int minorVersion)
            : typeName(typeName), fileName(fileName), majorVersion(majorVersion), minorVersion(minorVersion),
            internal(false) {}

        QString typeName;
        QString fileName;
        int majorVersion;
        int minorVersion;
        bool internal;
    };

    QList<Component> components() const;
    QList<Plugin> plugins() const;

#ifdef QT_CREATOR
    struct TypeInfo
    {
        TypeInfo() {}
        TypeInfo(const QString &fileName)
            : fileName(fileName) {}

        QString fileName;
    };

    QList<TypeInfo> typeInfos() const;
#endif

private:
    void reportError(int line, int column, const QString &message);

private:
    QList<QDeclarativeError> _errors;
    QUrl _url;
    QString _source;
    QString _filePathSouce;
    QList<Component> _components;
    QList<Plugin> _plugins;
#ifdef QT_CREATOR
    QList<TypeInfo> _typeInfos;
#endif
    unsigned _isParsed: 1;
};

typedef QList<QDeclarativeDirParser::Component> QDeclarativeDirComponents;


QT_END_NAMESPACE

#endif // QDECLARATIVEDIRPARSER_P_H
