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

#ifndef QDECLARATIVEVIEW_H
#define QDECLARATIVEVIEW_H

#include <QtCore/qdatetime.h>
#include <QtCore/qurl.h>
#include <QtWidgets/qgraphicssceneevent.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qwidget.h>
#include <QtDeclarative/qdeclarativedebug.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QGraphicsObject;
class QDeclarativeEngine;
class QDeclarativeContext;
class QDeclarativeError;

class QDeclarativeViewPrivate;
class Q_DECLARATIVE_EXPORT QDeclarativeView : public QGraphicsView
{
    Q_OBJECT
    Q_PROPERTY(ResizeMode resizeMode READ resizeMode WRITE setResizeMode)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QUrl source READ source WRITE setSource DESIGNABLE true)
    Q_ENUMS(ResizeMode Status)
public:
    explicit QDeclarativeView(QWidget *parent = 0);
    QDeclarativeView(const QUrl &source, QWidget *parent = 0);
    virtual ~QDeclarativeView();

    QUrl source() const;
    void setSource(const QUrl&);

    QDeclarativeEngine* engine() const;
    QDeclarativeContext* rootContext() const;

    QGraphicsObject *rootObject() const;

    enum ResizeMode { SizeViewToRootObject, SizeRootObjectToView };
    ResizeMode resizeMode() const;
    void setResizeMode(ResizeMode);

    enum Status { Null, Ready, Loading, Error };
    Status status() const;

    QList<QDeclarativeError> errors() const;

    QSize sizeHint() const;
    QSize initialSize() const;

Q_SIGNALS:
    void sceneResized(QSize size); // ???
    void statusChanged(QDeclarativeView::Status);

private Q_SLOTS:
    void continueExecute();

protected:
    virtual void resizeEvent(QResizeEvent *);
    virtual void paintEvent(QPaintEvent *event);
    virtual void timerEvent(QTimerEvent*);
    virtual void setRootObject(QObject *obj);
    virtual bool eventFilter(QObject *watched, QEvent *e);

private:
    Q_DISABLE_COPY(QDeclarativeView)
    Q_DECLARE_PRIVATE(QDeclarativeView)
};

QT_END_NAMESPACE

#endif // QDECLARATIVEVIEW_H
