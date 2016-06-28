/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
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
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_QNODEVISITOR_H
#define QT3D_QNODEVISITOR_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/qnode.h>
#include <Qt3DCore/qentity.h>

QT_BEGIN_NAMESPACE

namespace Qt3D
{

class QT3DCORESHARED_EXPORT QNodeVisitor
{
public:
    QNodeVisitor();
    virtual ~QNodeVisitor();

    template<typename NodeVisitorFunc>
    void traverse(QNode *rootNode_, NodeVisitorFunc fN)
    {
        startTraversing(rootNode_, createFunctor(fN));
    }

    template<typename Obj, typename NodeVisitorFunc>
    void traverse(QNode *rootNode_, Obj *instance, NodeVisitorFunc fN)
    {
        startTraversing(rootNode_, createFunctor(instance, fN));
    }

    template<typename NodeVisitorFunc, typename EntityVisitorFunc>
    void traverse(QNode *rootNode_, NodeVisitorFunc fN, EntityVisitorFunc fE)
    {
        startTraversing(rootNode_, createFunctor(fN), createFunctor(fE));
    }

    template<typename Obj, typename NodeVisitorFunc, typename EntityVisitorFunc>
    void traverse(QNode *rootNode_, Obj *instance, NodeVisitorFunc fN, EntityVisitorFunc fE)
    {
        startTraversing(rootNode_, createFunctor(instance, fN), createFunctor(instance, fE));
    }

    QNode *rootNode() const;
    QNode *currentNode() const;
    QNodeList path() const;

private:
    QNodeList m_path;

    template<typename NodeVisitorFunctor>
    void startTraversing(QNode *rootNode_, NodeVisitorFunctor fN)
    {
        m_path = QNodeList() << rootNode_;
        if (rootNode_)
            visitNode(rootNode_, fN);
    }

    template<typename NodeVisitorFunctor, typename EntityVisitorFunctor>
    void startTraversing(QNode *rootNode_, NodeVisitorFunctor fN, EntityVisitorFunctor fE)
    {
        m_path = QNodeList() << rootNode_;
        QEntity* rootEntity = qobject_cast<QEntity *>(rootNode_);

        if (rootEntity)
            visitEntity(rootEntity, fN, fE);
        else if (rootNode_)
            visitNode(rootNode_, fN, fE);
    }

    template<typename NodeVisitorFunctor>
    void visitNode(QNode *nd, NodeVisitorFunctor &fN)
    {
        fN(nd);
        traverseChildren(fN);
    }

    template<typename NodeVisitorFunctor, typename EntityVisitorFunctor>
    void visitNode(QNode *nd, NodeVisitorFunctor &fN, EntityVisitorFunctor &fE)
    {
        fN(nd);
        traverseChildren(fN, fE);
    }

    template<typename NodeVisitorFunctor, typename EntityVisitorFunctor>
    void visitEntity(QEntity *ent, NodeVisitorFunctor &fN, EntityVisitorFunctor &fE)
    {
        fE(ent);
        visitNode(ent, fN, fE);
    }

    template<typename NodeVisitorFunctor, typename EntityVisitorFunctor>
    void traverseChildren(NodeVisitorFunctor &fN, EntityVisitorFunctor &fE)
    {
        Q_FOREACH (QObject *n, currentNode()->children()) {
            QNode *node = qobject_cast<QNode *>(n);
            if (node != Q_NULLPTR)
                outerVisitNode(node, fN, fE);
        } // of children iteration
    }

    template<typename NodeVisitorFunctor>
    void traverseChildren(NodeVisitorFunctor &fN)
    {
        Q_FOREACH (QObject *n, currentNode()->children()) {
            QNode *node = qobject_cast<QNode *>(n);
            if (node != Q_NULLPTR)
                outerVisitNode(node, fN);
        } // of children iteration
    }

    template<typename NodeVisitorFunctor, typename EntityVisitorFunctor>
    void outerVisitNode(QNode *n, NodeVisitorFunctor &fN, EntityVisitorFunctor &fE)
    {
        m_path.append(n);
        QEntity* e = qobject_cast<QEntity *>(n);
        if (e) {
            visitEntity(e, fN, fE);
        } else {
            visitNode(n, fN, fE);
        }
        m_path.pop_back();
    }

    template<typename NodeVisitorFunctor>
    void outerVisitNode(QNode *n, NodeVisitorFunctor &fN)
    {
        m_path.append(n);
        visitNode(n, fN);
        m_path.pop_back();
    }

    template <typename NodeType>
    class FunctionFunctor {
    public:
        typedef void (*functionPtr)(NodeType);

        FunctionFunctor(functionPtr fPtr)
            : m_functionPointer(fPtr)
        {}

        void operator()(NodeType node)
        {
            (*m_functionPointer)(node);
        }

    private:
        functionPtr m_functionPointer;
    };

    template <typename C, typename NodeType>
    class MemberFunctionFunctor {
    public:
        typedef void (C::*functionPtr)(NodeType);

        MemberFunctionFunctor(C* instance, functionPtr fPtr)
            : m_instance(instance)
            , m_functionPointer(fPtr)
        {}

        void operator()(NodeType node)
        {
            (*m_instance.*m_functionPointer)(node);
        }

    private:
        C *m_instance;
        functionPtr m_functionPointer;
    };

    template <typename C, typename NodeType>
    class ConstMemberFunctionFunctor {
    public:
        typedef void (C::*functionPtr)(NodeType) const;

        ConstMemberFunctionFunctor(C* instance, functionPtr fPtr)
            : m_instance(instance)
            , m_functionPointer(fPtr)
        {}

        void operator()(NodeType node) const
        {
            (*m_instance.*m_functionPointer)(node);
        }

    private:
        C *m_instance;
        functionPtr m_functionPointer;
    };

    template <typename T>
    const T& createFunctor(const T& t)
    {
        return t;
    }

    template <typename NodeType>
    FunctionFunctor<NodeType> createFunctor(void (*fPtr)(NodeType))
    {
        return FunctionFunctor<NodeType>(fPtr);
    }

    template <typename C, typename NodeType>
    MemberFunctionFunctor<C, NodeType> createFunctor(C *instance, void (C::*fPtr)(NodeType))
    {
        return MemberFunctionFunctor<C, NodeType>(instance, fPtr);
    }

    template <typename C, typename NodeType>
    ConstMemberFunctionFunctor<C, NodeType> createFunctor(C *instance, void (C::*fPtr)(NodeType) const)
    {
        return ConstMemberFunctionFunctor<C, NodeType>(instance, fPtr);
    }
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QNODEVISITOR_H
