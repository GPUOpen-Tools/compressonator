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

#ifndef QT3D_QITEMMODELBUFFER_H
#define QT3D_QITEMMODELBUFFER_H

#include <QObject>
#include <Qt3DRenderer/qt3drenderer_global.h>

#include <Qt3DRenderer/qbuffer.h>
#include <Qt3DRenderer/qattribute.h>

#include <QAbstractItemModel>
#include <QMap>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QT3DRENDERERSHARED_EXPORT QItemModelBuffer : public QObject
{
    Q_OBJECT
public:
    QItemModelBuffer();

    void setModel(QAbstractItemModel* model);
    void setRoot(const QModelIndex& rootIndex);

    void mapRoleName(QByteArray roleName, int type);
    void mapRoleName(QByteArray roleName, QString attributeName, int type);

    QBuffer *buffer();

    QStringList attributeNames() const;
    QAttribute *attributeByName(QString nm) const;

private Q_SLOTS:

    void onModelReset();

    void onModelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
private:
    QAbstractItemModel* m_model;
    QModelIndex m_rootIndex;

    struct RoleMapping {
        RoleMapping(QByteArray role, QString attr, int ty);

        QByteArray roleName;
        int cachedRole;
        QString attribute;
        int type;
        int byteSize;
    };

    QList<RoleMapping> m_mappings;

    QBuffer *m_buffer;
    QMap<QString, QAttribute *> m_attributes;
    int m_itemStride;

    QByteArray computeBufferData();

    void writeDataForIndex(const QModelIndex &index, int mappingCount, char *bufferPtr);
    bool validateRoles();
};

} // namespace Qt3D

QT_END_NAMESPACE

#endif // QT3D_QITEMMODELBUFFER_H
