/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTSYNTAXCHECKER_H
#define QSCRIPTSYNTAXCHECKER_H

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

#include <QtCore/qstring.h>

#include "qscriptgrammar_p.h"

#include <stdlib.h>

QT_BEGIN_NAMESPACE

namespace QScript {

class Lexer;

class SyntaxChecker: protected QScriptGrammar
{
public:
    enum State {
        Error,
        Intermediate,
        Valid,
    };

    struct Result {
        Result(State s, int ln, int col, const QString &msg)
            : state(s), errorLineNumber(ln), errorColumnNumber(col),
              errorMessage(msg) {}
        State state;
        int errorLineNumber;
        int errorColumnNumber;
        QString errorMessage;
    };

    SyntaxChecker();
    ~SyntaxChecker();

    Result checkSyntax(const QString &code);

protected:
    bool automatic(QScript::Lexer *lexer, int token) const;
    inline void reallocateStack();

protected:
    int tos;
    int stack_size;
    int *state_stack;
};

inline void SyntaxChecker::reallocateStack()
{
    if (! stack_size)
        stack_size = 128;
    else
        stack_size <<= 1;

    state_stack = reinterpret_cast<int*> (realloc(state_stack, stack_size * sizeof(int)));
}

} // namespace QScript

QT_END_NAMESPACE

#endif
