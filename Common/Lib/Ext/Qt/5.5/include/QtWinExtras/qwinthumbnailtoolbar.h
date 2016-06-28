/****************************************************************************
 **
 ** Copyright (C) 2013 Ivan Vizir <define-true-false@yandex.com>
 ** Contact: http://www.qt.io/licensing/
 **
 ** This file is part of the QtWinExtras module of the Qt Toolkit.
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

#ifndef QWINTHUMBNAILTOOLBAR_H
#define QWINTHUMBNAILTOOLBAR_H

#include <QtCore/qobject.h>
#include <QtCore/qscopedpointer.h>
#include <QtWinExtras/qwinextrasglobal.h>

QT_BEGIN_NAMESPACE

class QPixmap;
class QWindow;
class QWinThumbnailToolButton;
class QWinThumbnailToolBarPrivate;

class Q_WINEXTRAS_EXPORT QWinThumbnailToolBar : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count STORED false)
    Q_PROPERTY(QWindow *window READ window WRITE setWindow)
    Q_PROPERTY(bool iconicPixmapNotificationsEnabled READ iconicPixmapNotificationsEnabled WRITE setIconicPixmapNotificationsEnabled)
    Q_PROPERTY(QPixmap iconicThumbnailPixmap READ iconicThumbnailPixmap WRITE setIconicThumbnailPixmap)
    Q_PROPERTY(QPixmap iconicLivePreviewPixmap READ iconicLivePreviewPixmap WRITE setIconicLivePreviewPixmap)

public:
    explicit QWinThumbnailToolBar(QObject *parent = 0);
    ~QWinThumbnailToolBar();

    void setWindow(QWindow *window);
    QWindow *window() const;

    void addButton(QWinThumbnailToolButton *button);
    void removeButton(QWinThumbnailToolButton *button);
    void setButtons(const QList<QWinThumbnailToolButton *> &buttons);
    QList<QWinThumbnailToolButton *> buttons() const;
    int count() const;

    bool iconicPixmapNotificationsEnabled() const;
    void setIconicPixmapNotificationsEnabled(bool enabled);

    QPixmap iconicThumbnailPixmap() const;
    QPixmap iconicLivePreviewPixmap() const;

public Q_SLOTS:
    void clear();
    void setIconicThumbnailPixmap(const QPixmap &);
    void setIconicLivePreviewPixmap(const QPixmap &);

Q_SIGNALS:
    void iconicThumbnailPixmapRequested();
    void iconicLivePreviewPixmapRequested();

private:
    Q_DISABLE_COPY(QWinThumbnailToolBar)
    Q_DECLARE_PRIVATE(QWinThumbnailToolBar)
    QScopedPointer<QWinThumbnailToolBarPrivate> d_ptr;
    friend class QWinThumbnailToolButton;
};

QT_END_NAMESPACE

#endif // QWINTHUMBNAILTOOLBAR_H
