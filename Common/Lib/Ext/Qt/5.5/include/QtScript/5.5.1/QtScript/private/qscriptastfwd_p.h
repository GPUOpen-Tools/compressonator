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

#ifndef QSCRIPTAST_FWD_P_H
#define QSCRIPTAST_FWD_P_H

#include <QtCore/qglobal.h>

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

namespace QScript { namespace AST {

class Visitor;
class Node;
class ExpressionNode;
class Statement;
class ThisExpression;
class IdentifierExpression;
class NullExpression;
class TrueLiteral;
class FalseLiteral;
class NumericLiteral;
class StringLiteral;
class RegExpLiteral;
class ArrayLiteral;
class ObjectLiteral;
class ElementList;
class Elision;
class PropertyNameAndValueList;
class PropertyName;
class IdentifierPropertyName;
class StringLiteralPropertyName;
class NumericLiteralPropertyName;
class ArrayMemberExpression;
class FieldMemberExpression;
class NewMemberExpression;
class NewExpression;
class CallExpression;
class ArgumentList;
class PostIncrementExpression;
class PostDecrementExpression;
class DeleteExpression;
class VoidExpression;
class TypeOfExpression;
class PreIncrementExpression;
class PreDecrementExpression;
class UnaryPlusExpression;
class UnaryMinusExpression;
class TildeExpression;
class NotExpression;
class BinaryExpression;
class ConditionalExpression;
class Expression; // ### rename
class Block;
class StatementList;
class VariableStatement;
class VariableDeclarationList;
class VariableDeclaration;
class EmptyStatement;
class ExpressionStatement;
class IfStatement;
class DoWhileStatement;
class WhileStatement;
class ForStatement;
class LocalForStatement;
class ForEachStatement;
class LocalForEachStatement;
class ContinueStatement;
class BreakStatement;
class ReturnStatement;
class WithStatement;
class SwitchStatement;
class CaseBlock;
class CaseClauses;
class CaseClause;
class DefaultClause;
class LabelledStatement;
class ThrowStatement;
class TryStatement;
class Catch;
class Finally;
class FunctionDeclaration;
class FunctionExpression;
class FormalParameterList;
class FunctionBody;
class Program;
class SourceElements;
class SourceElement;
class FunctionSourceElement;
class StatementSourceElement;
class DebuggerStatement;

} } // namespace AST

QT_END_NAMESPACE

#endif
