/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtScript module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL-ONLY$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** If you have questions regarding the use of this file, please contact
** us via http://www.qt.io/contact-us/.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QSCRIPTEXTENSIONINTERFACE_H
#define QSCRIPTEXTENSIONINTERFACE_H

#include <QtCore/qfactoryinterface.h>

#include <QtCore/qobject.h>
#include <QtScript/qtscriptglobal.h>

QT_BEGIN_NAMESPACE


class QScriptEngine;

struct Q_SCRIPT_EXPORT QScriptExtensionInterface
    : public QFactoryInterface
{
    virtual void initialize(const QString &key, QScriptEngine *engine) = 0;
};

#define QScriptExtensionInterface_iid "org.qt-project.Qt.QScriptExtensionInterface"

Q_DECLARE_INTERFACE(QScriptExtensionInterface, QScriptExtensionInterface_iid)

QT_END_NAMESPACE

#endif // QSCRIPTEXTENSIONINTERFACE_H
