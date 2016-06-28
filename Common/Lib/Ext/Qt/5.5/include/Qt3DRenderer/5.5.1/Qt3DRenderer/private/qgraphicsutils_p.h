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

#ifndef QT3D_RENDER_QGRAPHICSUTILS_P_H
#define QT3D_RENDER_QGRAPHICSUTILS_P_H

#include <Qt3DRenderer/qt3drenderer_global.h>
#include <Qt3DRenderer/private/shadervariables_p.h>
#include <QMatrix2x2>
#include <QMatrix3x3>
#include <QMatrix4x4>
#include <QGenericMatrix>
#include <QVector2D>
#include <QVarLengthArray>
#include <QColor>

QT_BEGIN_NAMESPACE

namespace Qt3D {

namespace Render {

namespace {

const int QMatrix2x2Type = qMetaTypeId<QMatrix2x2>();
const int QMatrix2x3Type = qMetaTypeId<QMatrix2x3>();
const int QMatrix2x4Type = qMetaTypeId<QMatrix2x4>();
const int QMatrix3x2Type = qMetaTypeId<QMatrix3x2>();
const int QMatrix3x3Type = qMetaTypeId<QMatrix3x3>();
const int QMatrix3x4Type = qMetaTypeId<QMatrix3x4>();
const int QMatrix4x2Type = qMetaTypeId<QMatrix4x2>();
const int QMatrix4x3Type = qMetaTypeId<QMatrix4x3>();

}

class QGraphicsUtils
{

public:

    template<typename T>
    static const char *bytesFromVariant(const QVariant &v)
    {
        uint byteSize = sizeof(T);
        // Max 16 float that we may want as doubles
        // 64 should be best for most cases
        static QVarLengthArray<char, 64> array(16 * byteSize);
        memset(array.data(), 0, array.size());

        switch (static_cast<QMetaType::Type>(v.type())) {

        // 1 byte
        case QMetaType::Bool: {
            T data = v.value<bool>();
            memcpy(array.data(), &data, byteSize);
            break;
        }
        case QMetaType::Char: {
            T data = v.value<char>();
            memcpy(array.data(), &data, byteSize);
            break;
        }

            // 4 bytes
        case QMetaType::Float: {
            T data = v.value<float>();
            memcpy(array.data(), &data, byteSize);
            break;
        }
        case QMetaType::Int: {
            T data = v.value<int>();
            memcpy(array.data(), &data, byteSize);
            break;

        }
        case QMetaType::UInt: {
            qDebug() << "UINT";
            T data = v.value<uint>();
            memcpy(array.data(), &data, byteSize);
            break;
        }

            // 8 bytes
        case QMetaType::Double: {
            T data = v.value<double>();
            memcpy(array.data(), &data, byteSize);
            break;
        }

            // 2 floats
        case QMetaType::QPointF: {
            QPointF vv = v.value<QPointF>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            break;
        }
        case QMetaType::QSizeF: {
            QSizeF vv = v.value<QSizeF>();
            T data = vv.width();
            memcpy(array.data(), &data, byteSize);
            data = vv.height();
            memcpy(array.data() + byteSize, &data, byteSize);
            break;
        }

        case QMetaType::QVector2D: {
            QVector2D vv = v.value<QVector2D>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            break;
        }

            // 2 ints
        case QMetaType::QPoint: {
            QPointF vv = v.value<QPoint>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            break;
        }

        case QMetaType::QSize: {
            QSize vv = v.value<QSize>();
            T data = vv.width();
            memcpy(array.data(), &data, byteSize);
            data = vv.height();
            memcpy(array.data() + byteSize, &data, byteSize);
            break;
        }

            // 3 floats
        case QMetaType::QVector3D: {
            QVector3D vv = v.value<QVector3D>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            data = vv.z();
            memcpy(array.data() + 2 * byteSize, &data, byteSize);
            break;
        }

            // 4 floats
        case QMetaType::QVector4D: {
            QVector4D vv = v.value<QVector4D>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            data = vv.z();
            memcpy(array.data() + 2 * byteSize, &data, byteSize);
            data = vv.w();
            memcpy(array.data() + 3 * byteSize, &data, byteSize);
            break;
        }

        case QMetaType::QQuaternion: {

            break;
        }

        case QMetaType::QRectF: {
            QRectF vv = v.value<QRectF>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            data = vv.width();
            memcpy(array.data() + 2 * byteSize, &data, byteSize);
            data = vv.height();
            memcpy(array.data() + 3 * byteSize, &data, byteSize);
            break;
        }

        case QMetaType::QColor: {
            QColor vv = v.value<QColor>();
            T data = vv.redF();
            memcpy(array.data(), &data, byteSize);
            data = vv.greenF();
            memcpy(array.data() + byteSize, &data, byteSize);
            data = vv.blueF();
            memcpy(array.data() + 2 * byteSize, &data, byteSize);
            data = vv.alphaF();
            memcpy(array.data() + 3 * byteSize, &data, byteSize);
            break;
        }

            // 4 ints
        case QMetaType::QRect: {
            QRectF vv = v.value<QRect>();
            T data = vv.x();
            memcpy(array.data(), &data, byteSize);
            data = vv.y();
            memcpy(array.data() + byteSize, &data, byteSize);
            data = vv.width();
            memcpy(array.data() + 2 * byteSize, &data, byteSize);
            data = vv.height();
            memcpy(array.data() + 3 * byteSize, &data, byteSize);
            break;
        }

            // 16 floats
        case QMetaType::QMatrix4x4: {
            QMatrix4x4 mat = v.value<QMatrix4x4>();
            float *data = mat.data();
            for (int i = 0; i < 16; i++) {
                T d = data[i];
                memcpy(array.data() + i * byteSize, &d, byteSize);
            }
            break;
        }

        default: {

            float *data = Q_NULLPTR;
            if (v.userType() == QMatrix3x3Type) {
                QMatrix3x3 mat = v.value<QMatrix3x3>();
                data = mat.data();
                for (int i = 0; i < 9; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix2x2Type) {
                QMatrix2x2 mat = v.value<QMatrix2x2>();
                data = mat.data();
                for (int i = 0; i < 4; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix2x3Type) {
                QMatrix2x3 mat = v.value<QMatrix2x3>();
                data = mat.data();
                for (int i = 0; i < 6; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix3x2Type) {
                QMatrix3x2 mat = v.value<QMatrix3x2>();
                data = mat.data();
                for (int i = 0; i < 6; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix2x4Type) {
                QMatrix2x4 mat = v.value<QMatrix2x4>();
                data = mat.data();
                for (int i = 0; i < 8; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix4x2Type) {
                QMatrix4x2 mat = v.value<QMatrix4x2>();
                data = mat.data();
                for (int i = 0; i < 8; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix3x4Type) {
                QMatrix3x4 mat = v.value<QMatrix3x4>();
                data = mat.data();
                for (int i = 0; i < 12; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else if (v.userType() == QMatrix4x3Type) {
                QMatrix4x3 mat = v.value<QMatrix4x3>();
                data = mat.data();
                for (int i = 0; i < 12; i++) {
                    T d = data[i];
                    memcpy(array.data() + i * byteSize, &d, byteSize);
                }
            }
            else
                qWarning() << Q_FUNC_INFO << "QVariant type conversion not handled for " << v.type();
            break;
        }

        }
        return array.constData();
    }


    template<typename T>
    static const T *valueArrayFromVariant(const QVariant &v, int count, int tupleSize)
    {
        uint byteSize = sizeof(T);
        uint offset = byteSize * tupleSize;
        static QVarLengthArray<char, 1024> uniformValuesArray(1024);
        uniformValuesArray.resize(count * offset);
        char *data = uniformValuesArray.data();
        memset(data, 0, uniformValuesArray.size());

        QVariantList vList = v.toList();
        // Handles list of QVariant: usually arrays of float
        if (!vList.isEmpty()) {
            for (int i = 0; i < vList.length() && uint(i) * offset < uint(uniformValuesArray.size()); i++) {
                const char *subBuffer = QGraphicsUtils::bytesFromVariant<T>(vList.at(i));
                memcpy(data + i * offset, subBuffer, offset);
            }
        }
        else {
            memcpy(data, QGraphicsUtils::bytesFromVariant<T>(v), offset);
        }
        return reinterpret_cast<const T *>(uniformValuesArray.constData());
    }

    template<typename T>
    static void fillDataArray(void *buffer, const T *data, const ShaderUniform &description, int tupleSize)
    {
        uint offset = description.m_offset / sizeof(T);
        uint stride = description.m_arrayStride / sizeof(T);
        T *bufferData = (T*)buffer;

        for (int i = 0; i < description.m_size; ++i) {
            for (int j = 0; j < tupleSize; j++) {
                int idx = i * tupleSize + j;
                bufferData[offset + j] = data[idx];
            }
            offset += stride;
        }
    }

    template<typename T>
    static void fillDataMatrixArray(void *buffer, const T *data, const ShaderUniform &description, int cols, int rows)
    {
        uint offset = description.m_offset / sizeof(T);
        uint arrayStride = description.m_arrayStride / sizeof(T);
        uint matrixStride = description.m_matrixStride / sizeof(T);
        T *bufferData = (T*)buffer;

        for (int i = 0; i < description.m_size; ++i) {
            for (int col = 0; col < cols; ++col) {
                for (int row = 0; row < rows; ++row) {
                    int idx = i * cols * rows + rows * col + row;
                    bufferData[offset + row] = data[idx];
                }
                offset += matrixStride;
            }
            offset += arrayStride;
        }
    }

};

} // Render

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_RENDER_QGRAPHICSUTILS_P_H
