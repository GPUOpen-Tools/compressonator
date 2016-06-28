/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
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
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QDECLARATIVELOADER_H
#define QDECLARATIVELOADER_H

#include "qdeclarativeimplicitsizeitem_p.h"

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeLoaderPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeLoader : public QDeclarativeImplicitSizeItem
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QDeclarativeComponent *sourceComponent READ sourceComponent WRITE setSourceComponent RESET resetSourceComponent NOTIFY sourceChanged)
    Q_PROPERTY(QGraphicsObject *item READ item NOTIFY itemChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

public:
    QDeclarativeLoader(QDeclarativeItem *parent=0);
    virtual ~QDeclarativeLoader();

    QUrl source() const;
    void setSource(const QUrl &);

    QDeclarativeComponent *sourceComponent() const;
    void setSourceComponent(QDeclarativeComponent *);
    void resetSourceComponent();

    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    QGraphicsObject *item() const;

Q_SIGNALS:
    void itemChanged();
    void sourceChanged();
    void statusChanged();
    void progressChanged();
    void loaded();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    bool eventFilter(QObject *watched, QEvent *e);
    void componentComplete();

private:
    Q_DISABLE_COPY(QDeclarativeLoader)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeLoader)
    Q_PRIVATE_SLOT(d_func(), void _q_sourceLoaded())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeLoader)

#endif // QDECLARATIVELOADER_H
