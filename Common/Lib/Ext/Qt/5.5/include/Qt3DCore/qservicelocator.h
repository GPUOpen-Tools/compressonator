/****************************************************************************
**
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

#ifndef QT3D_QSERVICELOCATOR_H
#define QT3D_QSERVICELOCATOR_H

#include <Qt3DCore/qt3dcore_global.h>
#include <QScopedPointer>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QAbstractServiceProviderPrivate;

class QT3DCORESHARED_EXPORT QAbstractServiceProvider
{
public:
    virtual ~QAbstractServiceProvider();

    int type() const;
    QString description() const;

protected:
    QAbstractServiceProvider(int type, const QString &description);
    QAbstractServiceProvider(QAbstractServiceProviderPrivate &dd);
    QScopedPointer<QAbstractServiceProviderPrivate> d_ptr;

private:
    Q_DISABLE_COPY(QAbstractServiceProvider)
    Q_DECLARE_PRIVATE(QAbstractServiceProvider)
};

class QAbstractFrameAdvanceService;
class QOpenGLInformationService;
class QSystemInformationService;
class QServiceLocatorPrivate;

class QT3DCORESHARED_EXPORT QServiceLocator
{
public:
    QServiceLocator();
    ~QServiceLocator();

    enum ServiceType {
        SystemInformation,
        OpenGLInformation,
        CollisionService,
        FrameAdvanceService,
#if !defined(Q_QDOC)
        DefaultServiceCount, // Add additional default services before here
#endif
        UserService = 256
    };

    void registerServiceProvider(int serviceType, QAbstractServiceProvider *provider);
    void unregisterServiceProvider(int serviceType);

    int serviceCount() const;

    template<class T>
    T *service(int serviceType)
    {
        QAbstractServiceProvider *service_ = _q_getServiceHelper(serviceType);
        return static_cast<T *>(service_);
    }

    // Convenience accessors for Qt3D provided services
    QSystemInformationService *systemInformation();
    QOpenGLInformationService *openGLInformation();
    QAbstractFrameAdvanceService *frameAdvanceService();

private:
    Q_DISABLE_COPY(QServiceLocator)
    Q_DECLARE_PRIVATE(QServiceLocator)
    QScopedPointer<QServiceLocatorPrivate> d_ptr;

    QAbstractServiceProvider *_q_getServiceHelper(int type);
};

}

QT_END_NAMESPACE

#endif // QT3D_QSERVICELOCATOR_H
