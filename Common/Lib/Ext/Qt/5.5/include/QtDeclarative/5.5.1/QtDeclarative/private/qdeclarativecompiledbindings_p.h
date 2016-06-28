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

#ifndef QDECLARATIVEBINDINGOPTIMIZATIONS_P_H
#define QDECLARATIVEBINDINGOPTIMIZATIONS_P_H

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

#include "private/qdeclarativeexpression_p.h"
#include "private/qdeclarativebinding_p.h"

QT_BEGIN_NAMESPACE

struct QDeclarativeBindingCompilerPrivate;
class QDeclarativeBindingCompiler
{
public:
    QDeclarativeBindingCompiler();
    ~QDeclarativeBindingCompiler();

    // Returns true if bindings were compiled
    bool isValid() const;

    struct Expression
    {
        QDeclarativeParser::Object *component;
        QDeclarativeParser::Object *context;
        QDeclarativeParser::Property *property;
        QDeclarativeParser::Variant expression;
        QHash<QString, QDeclarativeParser::Object *> ids;
        QDeclarativeImports imports;
    };

    // -1 on failure, otherwise the binding index to use
    int compile(const Expression &, QDeclarativeEnginePrivate *);

    // Returns the compiled program
    QByteArray program() const;

    static void dump(const QByteArray &);
private:
    QDeclarativeBindingCompilerPrivate *d;
};

class QDeclarativeCompiledBindingsPrivate;
class QDeclarativeCompiledBindings : public QObject, public QDeclarativeAbstractExpression, public QDeclarativeRefCount
{
public:
    QDeclarativeCompiledBindings(const char *program, QDeclarativeContextData *context, QDeclarativeRefCount *);
    virtual ~QDeclarativeCompiledBindings();

    QDeclarativeAbstractBinding *configBinding(int index, QObject *target, QObject *scope, int property);

protected:
    int qt_metacall(QMetaObject::Call, int, void **);

private:
    Q_DISABLE_COPY(QDeclarativeCompiledBindings)
    Q_DECLARE_PRIVATE(QDeclarativeCompiledBindings)
};

QT_END_NAMESPACE

#endif // QDECLARATIVEBINDINGOPTIMIZATIONS_P_H

