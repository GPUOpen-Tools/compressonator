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

#ifndef QT3D_QUICK_QUICK3DNODE_H
#define QT3D_QUICK_QUICK3DNODE_H

#include <QQmlListProperty>
#include <Qt3DCore/qnode.h>
#include <Qt3DQuick/qt3dquick_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Quick {

class QT3DQUICKSHARED_EXPORT Quick3DNode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QObject> data READ data)
    Q_PROPERTY(QQmlListProperty<Qt3D::QNode> childNodes READ childNodes)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    explicit Quick3DNode(QObject *parent = 0);

    QQmlListProperty<QObject> data();
    QQmlListProperty<Qt3D::QNode> childNodes();

    inline QNode *parentNode() const { return qobject_cast<QNode*>(parent()); }

private:
    static void appendData(QQmlListProperty<QObject> *list, QObject *obj);
    static QObject *dataAt(QQmlListProperty<QObject> *list, int index);
    static int dataCount(QQmlListProperty<QObject> *list);
    static void clearData(QQmlListProperty<QObject> *list);

    static void appendChild(QQmlListProperty<Qt3D::QNode> *list, Qt3D::QNode *obj);
    static QNode *childAt(QQmlListProperty<Qt3D::QNode> *list, int index);
    static int childCount(QQmlListProperty<Qt3D::QNode> *list);
    static void clearChildren(QQmlListProperty<Qt3D::QNode> *list);

private Q_SLOTS:
    void childAppended(int idx, QObject *child);
    void childRemoved(int idx, QObject *child);
};

} // Quick;

} // Qt3D

QT_END_NAMESPACE

#endif // QUICK3DNODE_H
