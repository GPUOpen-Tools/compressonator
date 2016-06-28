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

#ifndef QDECLARATIVELISTMODEL_P_P_H
#define QDECLARATIVELISTMODEL_P_P_H

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

#include "private/qdeclarativelistmodel_p.h"
#include "private/qdeclarativeengine_p.h"
#include "private/qdeclarativeopenmetaobject_p.h"
#include "qdeclarative.h"

#include <private/qscriptdeclarativeclass_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeOpenMetaObject;
class QScriptEngine;
class QDeclarativeListModelWorkerAgent;
struct ModelNode;
class FlatListScriptClass;
class FlatNodeData;

class FlatListModel
{
public:
    FlatListModel(QDeclarativeListModel *base);
    ~FlatListModel();

    QVariant data(int index, int role) const;

    QList<int> roles() const;
    QString toString(int role) const;

    int count() const;
    void clear();
    void remove(int index);
    bool insert(int index, const QScriptValue&);
    QScriptValue get(int index) const;
    void set(int index, const QScriptValue&, QList<int> *roles);
    void setProperty(int index, const QString& property, const QVariant& value, QList<int> *roles);
    void move(int from, int to, int count);

private:
    friend class QDeclarativeListModelWorkerAgent;
    friend class QDeclarativeListModel;
    friend class FlatListScriptClass;
    friend class FlatNodeData;

    bool addValue(const QScriptValue &value, QHash<int, QVariant> *row, QList<int> *roles);
    void insertedNode(int index);
    void removedNode(int index);
    void moveNodes(int from, int to, int n);

    QScriptEngine *m_scriptEngine;
    QHash<int, QString> m_roles;
    QHash<QString, int> m_strings;
    QList<QHash<int, QVariant> > m_values;
    QDeclarativeListModel *m_listModel;

    FlatListScriptClass *m_scriptClass;
    QList<FlatNodeData *> m_nodeData;
    QDeclarativeListModelWorkerAgent *m_parentAgent;
};


/*
    Created when get() is called on a FlatListModel. This allows changes to the
    object returned by get() to be tracked, and passed onto the model.
*/
class FlatListScriptClass : public QScriptDeclarativeClass
{
public:
    FlatListScriptClass(FlatListModel *model, QScriptEngine *seng);

    Value property(Object *, const Identifier &);
    void setProperty(Object *, const Identifier &name, const QScriptValue &);
    QScriptClass::QueryFlags queryProperty(Object *, const Identifier &, QScriptClass::QueryFlags flags);
    bool compare(Object *, Object *);

private:
    FlatListModel *m_model;
};

/*
    FlatNodeData and FlatNodeObjectData allow objects returned by get() to still
    point to the correct list index if move(), insert() or remove() are called.
*/
struct FlatNodeObjectData;
class FlatNodeData
{
public:
    FlatNodeData(int i)
        : index(i) {}

    ~FlatNodeData();

    void addData(FlatNodeObjectData *data);
    void removeData(FlatNodeObjectData *data);

    int index;

private:
    QSet<FlatNodeObjectData*> objects;
};

struct FlatNodeObjectData : public QScriptDeclarativeClass::Object
{
    FlatNodeObjectData(FlatNodeData *data) : nodeData(data) {
        nodeData->addData(this);
    }

    ~FlatNodeObjectData() {
        if (nodeData)
            nodeData->removeData(this);
    }

    FlatNodeData *nodeData;
};



class NestedListModel
{
public:
    NestedListModel(QDeclarativeListModel *base);
    ~NestedListModel();

    QHash<int,QVariant> data(int index, const QList<int> &roles, bool *hasNested = 0) const;
    QVariant data(int index, int role) const;

    QList<int> roles() const;
    QString toString(int role) const;

    int count() const;
    void clear();
    void remove(int index);
    bool insert(int index, const QScriptValue&);
    QScriptValue get(int index) const;
    void set(int index, const QScriptValue&, QList<int> *roles);
    void setProperty(int index, const QString& property, const QVariant& value, QList<int> *roles);
    void move(int from, int to, int count);

    QVariant valueForNode(ModelNode *, bool *hasNested = 0) const;
    void checkRoles() const;

    ModelNode *_root;
    bool m_ownsRoot;
    QDeclarativeListModel *m_listModel;

private:
    friend struct ModelNode;
    mutable QStringList roleStrings;
    mutable bool _rolesOk;
};


class ModelNodeMetaObject;
class ModelObject : public QObject
{
    Q_OBJECT
public:
    ModelObject(ModelNode *node, NestedListModel *model, QScriptEngine *seng);
    void setValue(const QByteArray &name, const QVariant &val);
    void setNodeUpdatesEnabled(bool enable);

    NestedListModel *m_model;
    ModelNode *m_node;

private:
    ModelNodeMetaObject *m_meta;
};

class ModelNodeMetaObject : public QDeclarativeOpenMetaObject
{
public:
    ModelNodeMetaObject(QScriptEngine *seng, ModelObject *object);

    bool m_enabled;

protected:
    void propertyWritten(int index);

private:
    QScriptEngine *m_seng;
    ModelObject *m_obj;
};


/*
    A ModelNode is created for each item in a NestedListModel.
*/
struct ModelNode
{
    ModelNode(NestedListModel *model);
    ~ModelNode();

    QList<QVariant> values;
    QHash<QString, ModelNode *> properties;

    void clear();

    QDeclarativeListModel *model(const NestedListModel *model);
    ModelObject *object(const NestedListModel *model);

    bool setObjectValue(const QScriptValue& valuemap, bool writeToCache = true);
    void setListValue(const QScriptValue& valuelist);
    bool setProperty(const QString& prop, const QVariant& val);
    void changedProperty(const QString &name) const;
    void updateListIndexes();
    static void dump(ModelNode *node, int ind);

    QDeclarativeListModel *modelCache;
    ModelObject *objectCache;
    bool isArray;

    NestedListModel *m_model;
    int listIndex;  // only used for top-level nodes within a list
};


QT_END_NAMESPACE

Q_DECLARE_METATYPE(ModelNode *)

#endif // QDECLARATIVELISTMODEL_P_P_H

