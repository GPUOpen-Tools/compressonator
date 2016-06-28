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

#ifndef QSCRIPTFUNCTION_P_H
#define QSCRIPTFUNCTION_P_H

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

#include <QtCore/qglobal.h>

#include "qscriptengine.h"

#include "PrototypeFunction.h"

QT_BEGIN_NAMESPACE

namespace QScript
{

class FunctionWrapper : public JSC::PrototypeFunction // ### subclass InternalFunction instead
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        QScriptEngine::FunctionSignature function;
    };

    FunctionWrapper(JSC::ExecState*, int length, const JSC::Identifier&,
                    QScriptEngine::FunctionSignature);
    ~FunctionWrapper();

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    QScriptEngine::FunctionSignature function() const
    { return data->function; }

private:
    virtual JSC::ConstructType getConstructData(JSC::ConstructData&);

    static JSC::JSValue JSC_HOST_CALL proxyCall(JSC::ExecState *, JSC::JSObject *,
                                                JSC::JSValue, const JSC::ArgList &);
    static JSC::JSObject* proxyConstruct(JSC::ExecState *, JSC::JSObject *,
                                         const JSC::ArgList &);

private:
    Data *data;
};

class FunctionWithArgWrapper : public JSC::PrototypeFunction
{
public:
    // work around CELL_SIZE limitation
    struct Data
    {
        QScriptEngine::FunctionWithArgSignature function;
        void *arg;
    };

    FunctionWithArgWrapper(JSC::ExecState*, int length, const JSC::Identifier&,
                           QScriptEngine::FunctionWithArgSignature, void *);
    ~FunctionWithArgWrapper();

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    QScriptEngine::FunctionWithArgSignature function() const
    { return data->function; }

    void *arg() const
    { return data->arg; }

private:
    virtual JSC::ConstructType getConstructData(JSC::ConstructData&);

    static JSC::JSValue JSC_HOST_CALL proxyCall(JSC::ExecState *, JSC::JSObject *,
                                                JSC::JSValue , const JSC::ArgList &);
    static JSC::JSObject* proxyConstruct(JSC::ExecState *, JSC::JSObject *,
                                         const JSC::ArgList &);

private:
    Data *data;
};

} // namespace QScript

QT_END_NAMESPACE

#endif
