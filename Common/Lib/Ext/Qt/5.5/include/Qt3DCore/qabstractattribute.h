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

#ifndef QT3D_QABSTRACTATTRIBUTE_H
#define QT3D_QABSTRACTATTRIBUTE_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DCore/QNode>
#include <QtCore/QSharedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractBuffer;
class QAbstractAttributePrivate;

typedef QSharedPointer<QAbstractBuffer> QAbstractBufferPtr;

class QT3DCORESHARED_EXPORT QAbstractAttribute : public QNode
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QAbstractBuffer *buffer READ buffer WRITE setBuffer NOTIFY bufferChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(DataType dataType READ dataType WRITE setDataType NOTIFY dataTypeChanged)
    Q_PROPERTY(uint dataSize READ dataSize WRITE setDataSize NOTIFY dataSizeChanged)
    Q_PROPERTY(uint count READ count WRITE setCount NOTIFY countChanged)
    Q_PROPERTY(uint byteStride READ byteStride WRITE setByteStride NOTIFY byteStrideChanged)
    Q_PROPERTY(uint byteOffset READ byteOffset WRITE setByteOffset NOTIFY byteOffsetChanged)
    Q_PROPERTY(uint divisor READ divisor WRITE setDivisor NOTIFY divisorChanged)
    Q_PROPERTY(AttributeType attributeType READ attributeType WRITE setAttributeType NOTIFY attributeTypeChanged)

public:
    enum AttributeType {
        VertexAttribute,
        IndexAttribute
    };

    Q_ENUM(AttributeType)

    enum DataType {
        Byte = 0,
        UnsignedByte,
        Short,
        UnsignedShort,
        Int,
        UnsignedInt,
        HalfFloat,
        Float,
        Double
    };
    Q_ENUM(DataType)

    explicit QAbstractAttribute(QNode *parent = 0);
    ~QAbstractAttribute();
    QAbstractAttribute(QAbstractBuffer *buf, DataType dataType, uint dataSize, uint count, uint offset = 0, uint stride = 0, QNode *parent = 0);

    QAbstractBuffer *buffer() const;
    QString name() const;
    DataType dataType() const;
    uint dataSize() const;
    uint count() const;
    uint byteStride() const;
    uint byteOffset() const;
    uint divisor() const;
    AttributeType attributeType() const;

    virtual QVector<QVector4D> asVector4D() const = 0;
    virtual QVector<QVector3D> asVector3D() const = 0;
    virtual QVector<QVector2D> asVector2D() const = 0;
    virtual void dump(int count) = 0;

    void setBuffer(QAbstractBuffer *buffer);
    void setName(const QString &name);
    void setDataType(DataType type);
    void setDataSize(uint size);
    void setCount(uint count);
    void setByteStride(uint byteStride);
    void setByteOffset(uint byteOffset);
    void setDivisor(uint divisor);
    void setAttributeType(AttributeType attributeType);

Q_SIGNALS:
    void bufferChanged();
    void nameChanged();
    void dataTypeChanged();
    void dataSizeChanged();
    void countChanged();
    void byteStrideChanged();
    void byteOffsetChanged();
    void divisorChanged();
    void attributeTypeChanged();

protected:
    QAbstractAttribute(QAbstractAttributePrivate &dd, QNode *parent = 0);
    QAbstractAttribute(QAbstractAttributePrivate &dd, QAbstractBuffer *buf, const QString &name, DataType dataType, uint dataSize, uint count, uint offset = 0, uint stride = 0, QNode *parent = 0);

    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QAbstractAttribute)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QABSTRACTATTRIBUTE_H
