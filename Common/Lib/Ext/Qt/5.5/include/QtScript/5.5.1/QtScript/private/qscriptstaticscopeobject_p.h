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

#ifndef QSCRIPTSTATICSCOPEOBJECT_P_H
#define QSCRIPTSTATICSCOPEOBJECT_P_H

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

#include <QtCore/qobjectdefs.h>

#include "JSVariableObject.h"

QT_BEGIN_NAMESPACE

class QScriptStaticScopeObject : public JSC::JSVariableObject {
public:
    struct PropertyInfo {
        PropertyInfo(const JSC::Identifier& i, JSC::JSValue v, unsigned a)
            : identifier(i), value(v), attributes(a)
            { }
        PropertyInfo() {}

        JSC::Identifier identifier;
        JSC::JSValue value;
        unsigned attributes;
    };

    QScriptStaticScopeObject(WTF::NonNullPassRefPtr<JSC::Structure> structure,
                            int propertyCount, const PropertyInfo*);
    QScriptStaticScopeObject(WTF::NonNullPassRefPtr<JSC::Structure> structure);
    virtual ~QScriptStaticScopeObject();

    virtual bool isDynamicScope() const { return false; }

    virtual bool getOwnPropertySlot(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertySlot&);
    virtual bool getOwnPropertyDescriptor(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::PropertyDescriptor&);

    virtual void putWithAttributes(JSC::ExecState *exec, const JSC::Identifier &propertyName, JSC::JSValue value, unsigned attributes);
    virtual void put(JSC::ExecState*, const JSC::Identifier& propertyName, JSC::JSValue value, JSC::PutPropertySlot&);

    virtual bool deleteProperty(JSC::ExecState*, const JSC::Identifier& propertyName);

    virtual void markChildren(JSC::MarkStack&);

    virtual const JSC::ClassInfo* classInfo() const { return &info; }
    static const JSC::ClassInfo info;

    static WTF::PassRefPtr<JSC::Structure> createStructure(JSC::JSValue proto) {
        return JSC::Structure::create(proto, JSC::TypeInfo(JSC::ObjectType, StructureFlags));
    }

protected:
    static const unsigned StructureFlags = JSC::OverridesGetOwnPropertySlot | JSC::NeedsThisConversion | JSC::OverridesMarkChildren | JSC::OverridesGetPropertyNames | JSC::JSVariableObject::StructureFlags;

    struct Data : public JSVariableObjectData {
        Data(bool canGrow_)
            : JSVariableObjectData(&symbolTable, /*registers=*/0),
            canGrow(canGrow_), registerArraySize(0)
        { }
        bool canGrow;
        int registerArraySize;
        JSC::SymbolTable symbolTable;
    };

    Data* d_ptr() const { return static_cast<Data*>(JSVariableObject::d); }

private:
    void addSymbolTableProperty(const JSC::Identifier&, JSC::JSValue, unsigned attributes);
    int growRegisterArray(int);
};

QT_END_NAMESPACE

#endif
