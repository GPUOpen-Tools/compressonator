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
#ifndef QDECLARATIVESCRIPTPARSER_P_H
#define QDECLARATIVESCRIPTPARSER_P_H

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

#include "qdeclarativeerror.h"
#include "private/qdeclarativeparser_p.h"

#include <QtCore/QList>
#include <QtCore/QUrl>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QByteArray;

class QDeclarativeScriptParserJsASTData;
class QDeclarativeScriptParser
{
public:
    class Import
    {
    public:
        Import() : type(Library) {}

        enum Type { Library, File, Script, Implicit }; //Implicit is only used internally
        Type type;

        QString uri;
        QString qualifier;
        QString version;

        QDeclarativeParser::LocationSpan location;
    };

    class TypeReference
    {
    public:
        TypeReference(int typeId, const QString &typeName) : id(typeId), name(typeName) {}

        int id;
        // type as it has been referenced in Qml
        QString name;
        // objects in parse tree referencing the type
        QList<QDeclarativeParser::Object*> refObjects;
    };

    QDeclarativeScriptParser();
    ~QDeclarativeScriptParser();

    bool parse(const QByteArray &data, const QUrl &url = QUrl());

    QList<TypeReference*> referencedTypes() const;

    QDeclarativeParser::Object *tree() const;
    QList<Import> imports() const;

    void clear();

    QList<QDeclarativeError> errors() const;

    class JavaScriptMetaData {
    public:
        JavaScriptMetaData()
        : pragmas(QDeclarativeParser::Object::ScriptBlock::None) {}

        QDeclarativeParser::Object::ScriptBlock::Pragmas pragmas;
        QList<Import> imports;
    };

    static QDeclarativeParser::Object::ScriptBlock::Pragmas extractPragmas(QString &);
    static JavaScriptMetaData extractMetaData(QString &);


// ### private:
    TypeReference *findOrCreateType(const QString &name);
    void setTree(QDeclarativeParser::Object *tree);

    void setScriptFile(const QString &filename) {_scriptFile = filename; }
    QString scriptFile() const { return _scriptFile; }

// ### private:
    QList<QDeclarativeError> _errors;

    QDeclarativeParser::Object *root;
    QList<Import> _imports;
    QList<TypeReference*> _refTypes;
    QString _scriptFile;
    QDeclarativeScriptParserJsASTData *data;
};

QT_END_NAMESPACE

#endif // QDECLARATIVESCRIPTPARSER_P_H
