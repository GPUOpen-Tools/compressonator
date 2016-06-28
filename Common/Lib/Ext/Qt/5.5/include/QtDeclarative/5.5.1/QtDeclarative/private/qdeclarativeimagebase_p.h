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

#ifndef QDECLARATIVEIMAGEBASE_H
#define QDECLARATIVEIMAGEBASE_H

#include "qdeclarativeimplicitsizeitem_p.h"

QT_BEGIN_NAMESPACE

class QDeclarativeImageBasePrivate;
class Q_AUTOTEST_EXPORT QDeclarativeImageBase : public QDeclarativeImplicitSizeItem
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(bool asynchronous READ asynchronous WRITE setAsynchronous NOTIFY asynchronousChanged)
    Q_PROPERTY(bool cache READ cache WRITE setCache NOTIFY cacheChanged REVISION 1)
    Q_PROPERTY(QSize sourceSize READ sourceSize WRITE setSourceSize RESET resetSourceSize NOTIFY sourceSizeChanged)
    Q_PROPERTY(bool mirror READ mirror WRITE setMirror NOTIFY mirrorChanged REVISION 1)

public:
    QDeclarativeImageBase(QDeclarativeItem *parent=0);
    ~QDeclarativeImageBase();
    enum Status { Null, Ready, Loading, Error };
    Status status() const;
    qreal progress() const;

    QUrl source() const;
    virtual void setSource(const QUrl &url);

    bool asynchronous() const;
    void setAsynchronous(bool);

    bool cache() const;
    void setCache(bool);

    virtual void setSourceSize(const QSize&);
    QSize sourceSize() const;
    void resetSourceSize();

    virtual void setMirror(bool mirror);
    bool mirror() const;

Q_SIGNALS:
    void sourceChanged(const QUrl &);
    void sourceSizeChanged();
    void statusChanged(QDeclarativeImageBase::Status);
    void progressChanged(qreal progress);
    void asynchronousChanged();
    Q_REVISION(1) void cacheChanged();
    Q_REVISION(1) void mirrorChanged();

protected:
    virtual void load();
    virtual void componentComplete();
    virtual void pixmapChange();
    QDeclarativeImageBase(QDeclarativeImageBasePrivate &dd, QDeclarativeItem *parent);

private Q_SLOTS:
    virtual void requestFinished();
    void requestProgress(qint64,qint64);

private:
    Q_DISABLE_COPY(QDeclarativeImageBase)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeImageBase)
};

QT_END_NAMESPACE

#endif // QDECLARATIVEIMAGEBASE_H
