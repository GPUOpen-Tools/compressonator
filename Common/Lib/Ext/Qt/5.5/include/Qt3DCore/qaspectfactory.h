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

#ifndef QT3D_QASPECTFACTORY_H
#define QT3D_QASPECTFACTORY_H

#include <QSharedDataPointer>
#include <QStringList>

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

class QObject;

namespace Qt3D {

class QAbstractAspect;
class QAspectFactoryPrivate;

class QT3DCORESHARED_EXPORT QAspectFactory
{
public:
    typedef QAbstractAspect *(*CreateFunction)(QObject *);

    template<class AspectType>
    static QAbstractAspect *functionHelper(QObject *parent)
    {
        return new AspectType(parent);
    }

    QAspectFactory();
    QAspectFactory(const QAspectFactory &other);
    ~QAspectFactory();
    QAspectFactory &operator=(const QAspectFactory &other);
#ifdef Q_COMPILER_RVALUE_REFS
    QAspectFactory &operator=(QAspectFactory &&other) Q_DECL_NOTHROW
    {
        d.swap(other.d);
        return *this;
    }
#endif

    inline void swap(QAspectFactory &other) Q_DECL_NOTHROW { d.swap(other.d); }

    void addFactory(const QString &name, CreateFunction factory);
    QStringList availableFactories() const;

    QAbstractAspect *createAspect(const QString &aspect, QObject *parent = 0) const;

private:
    QSharedDataPointer<QAspectFactoryPrivate> d;
};

} // namespace Qt3D

QT_END_NAMESPACE

#define QT3D_REGISTER_NAMESPACED_ASPECT(name, AspectNamespace, AspectType) \
    QT_BEGIN_NAMESPACE \
    namespace Qt3D { \
        QT3DCORESHARED_EXPORT void qt3d_QAspectFactoryPrivate_addDefaultFactory(const QString &, QAspectFactory::CreateFunction); \
    } \
    QT_END_NAMESPACE \
    namespace { \
    void qt3d_ ## AspectType ## _registerFunction() \
    { \
        using namespace QT_PREPEND_NAMESPACE(Qt3D); \
        qt3d_QAspectFactoryPrivate_addDefaultFactory(QStringLiteral(name), &QAspectFactory::functionHelper<AspectNamespace::AspectType>); \
    } \
    \
    Q_CONSTRUCTOR_FUNCTION(qt3d_ ## AspectType ## _registerFunction) \
    }

#define QT3D_REGISTER_ASPECT(name, AspectType) \
    QT3D_REGISTER_NAMESPACED_ASPECT(name, , AspectType)

#endif // QT3D_QASPECTFACTORY_H
