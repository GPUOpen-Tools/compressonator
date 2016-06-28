/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the ActiveQt framework of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAXTYPES_H
#define QAXTYPES_H

#include <ActiveQt/qaxtypefunctions.h>

QT_BEGIN_NAMESPACE

#ifdef QAX_SERVER
#   define QVariantToVARIANTFunc QVariantToVARIANT_server
#   define VARIANTToQVariantFunc VARIANTToQVariant_server
#else
#   define QVariantToVARIANTFunc QVariantToVARIANT_container
#   define VARIANTToQVariantFunc VARIANTToQVariant_container
#endif

extern bool QVariantToVARIANTFunc(const QVariant &var, VARIANT &arg, const QByteArray &typeName = QByteArray(), bool out = false);
extern QVariant VARIANTToQVariantFunc(const VARIANT &arg, const QByteArray &typeName, uint type = 0);

inline bool QVariantToVARIANT(const QVariant &var, VARIANT &arg, const QByteArray &typeName = QByteArray(), bool out = false)
{
    return QVariantToVARIANTFunc(var, arg, typeName, out);
}

inline QVariant VARIANTToQVariant(const VARIANT &arg, const QByteArray &typeName, uint type = 0)
{
    return VARIANTToQVariantFunc(arg, typeName, type);
}

#undef QVariantToVARIANTFunc
#undef VARIANTToQVariantFunc

QT_END_NAMESPACE

#endif // QAXTYPES_H
