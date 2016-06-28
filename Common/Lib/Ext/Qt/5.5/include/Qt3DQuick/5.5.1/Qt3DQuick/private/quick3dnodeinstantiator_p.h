/****************************************************************************
**
** Copyright (C) 2013 Research In Motion.
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3D_QUICK_NODEINSTANTIATOR_H
#define QT3D_QUICK_NODEINSTANTIATOR_H

#include <Qt3DQuick/qt3dquick_global.h>
#include <Qt3DCore/qnode.h>
#include <QtQml/qqmlcomponent.h>
#include <QtQml/qqmlparserstatus.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {
namespace Quick {

class Quick3DNodeInstantiatorPrivate;

class QT3DQUICKSHARED_EXPORT Quick3DNodeInstantiator : public QNode, public QQmlParserStatus
{
    Q_OBJECT

    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(bool active READ isActive WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool asynchronous READ isAsync WRITE setAsync NOTIFY asynchronousChanged)
    Q_PROPERTY(QVariant model READ model WRITE setModel NOTIFY modelChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate NOTIFY delegateChanged)
    Q_PROPERTY(QObject *object READ object NOTIFY objectChanged)
    Q_CLASSINFO("DefaultProperty", "delegate")

public:
    Quick3DNodeInstantiator(QNode *parent = 0);
    ~Quick3DNodeInstantiator();

    bool isActive() const;
    void setActive(bool newVal);

    bool isAsync() const;
    void setAsync(bool newVal);

    int count() const;

    QQmlComponent *delegate();
    void setDelegate(QQmlComponent *c);

    QVariant model() const;
    void setModel(const QVariant &v);

    QObject *object() const;
    Q_INVOKABLE QObject *objectAt(int index) const;

    void classBegin() Q_DECL_OVERRIDE;
    void componentComplete() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void modelChanged();
    void delegateChanged();
    void countChanged();
    void objectChanged();
    void activeChanged();
    void asynchronousChanged();

    void objectAdded(int index, QObject *object);
    void objectRemoved(int index, QObject *object);

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    QT3D_CLONEABLE(Quick3DNodeInstantiator)
    Q_DISABLE_COPY(Quick3DNodeInstantiator)
    Q_DECLARE_PRIVATE(Quick3DNodeInstantiator)
    Q_PRIVATE_SLOT(d_func(), void _q_createdItem(int, QObject *))
    Q_PRIVATE_SLOT(d_func(), void _q_modelUpdated(const QQmlChangeSet &, bool))
};

} // namespace Quick
} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QUICK_NODEINSTANTIATOR_H
