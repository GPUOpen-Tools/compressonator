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

#ifndef QT3D_QPARAMETER_H
#define QT3D_QPARAMETER_H

#include <QVariant>
#include <Qt3DCore/qnode.h>
#include <Qt3DRenderer/qt3drenderer_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QParameterPrivate;
class QAbstractTextureProvider;

class QT3DRENDERERSHARED_EXPORT QParameter : public QNode
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QVariant value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit QParameter(QNode *parent = 0);
    ~QParameter();
    QParameter(const QString& name, const QVariant& value, QNode* parent = 0);
    QParameter(const QString &name, QAbstractTextureProvider *texture, QNode *parent = 0);

    void setName(const QString &name);
    QString name() const;

    /**
     * @brief setDefaultValue - for non-texture uniform parameters
     * @param dv
     */
    void setValue(const QVariant& dv);
    QVariant value() const;

Q_SIGNALS:
    void valueChanged();
    void nameChanged();

protected:
    QParameter(QParameterPrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QParameter)
    QT3D_CLONEABLE(QParameter)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_PARAMETER_H
