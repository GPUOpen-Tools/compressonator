/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtLocation module of the Qt Toolkit.
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

#ifndef QPLACE_P_H
#define QPLACE_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <QSharedData>
#include <QUrl>

#include "qplace.h"
#include "qgeoaddress.h"
#include "qgeorectangle.h"
#include "qgeocoordinate.h"
#include "qplacesupplier.h"
#include <QtLocation/QPlaceIcon>

QT_BEGIN_NAMESPACE

class QPlacePrivate : public QSharedData
{
public:
    QPlacePrivate();
    QPlacePrivate(const QPlacePrivate &other);
    ~QPlacePrivate();

    QPlacePrivate &operator= (const QPlacePrivate &other);

    bool operator==(const QPlacePrivate &other) const;

    bool isEmpty() const;

    QList<QPlaceCategory> categories;
    QGeoLocation location;
    QPlaceRatings ratings;
    QPlaceSupplier supplier;
    QString name;
    QString placeId;
    QString attribution;

    QMap<QPlaceContent::Type, QPlaceContent::Collection> contentCollections;
    QMap<QPlaceContent::Type, int> contentCounts;

    QMap<QString, QPlaceAttribute> extendedAttributes;
    QMap<QString, QList<QPlaceContactDetail> > contacts;

    QLocation::Visibility visibility;
    QPlaceIcon icon;
    bool detailsFetched;
};

QT_END_NAMESPACE

#endif

