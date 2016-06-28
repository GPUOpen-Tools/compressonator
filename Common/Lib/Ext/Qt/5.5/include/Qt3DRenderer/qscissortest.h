/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Copyright (C) 2015 The Qt Company Ltd and/or its subsidiary(-ies).
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

#ifndef QT3D_QSCISSORTEST_H
#define QT3D_QSCISSORTEST_H

#include <Qt3DRenderer/qrenderstate.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QScissorTestPrivate;

class QT3DRENDERERSHARED_EXPORT QScissorTest  : public QRenderState
{
    Q_OBJECT
    Q_PROPERTY(int left READ left WRITE setLeft NOTIFY leftChanged)
    Q_PROPERTY(int bottom READ bottom WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(int width READ width WRITE setWidth NOTIFY widthChanged)
    Q_PROPERTY(int height READ height WRITE setHeight NOTIFY heightChanged)

public:
    explicit QScissorTest(QNode *parent = 0);
    ~QScissorTest();

    int left() const;
    void setLeft(int left);

    int bottom() const;
    void setBottom(int bottom);

    int width() const;
    void setWidth(int width);

    int height() const;
    void setHeight(int height);

Q_SIGNALS:
    void leftChanged();
    void bottomChanged();
    void widthChanged();
    void heightChanged();

protected:
    void copy(const QNode *ref) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(QScissorTest)
    QT3D_CLONEABLE(QScissorTest)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_QSCISSORTEST_H
