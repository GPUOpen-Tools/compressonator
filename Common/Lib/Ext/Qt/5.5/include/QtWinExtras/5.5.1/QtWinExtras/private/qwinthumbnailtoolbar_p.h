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

#ifndef QWINTHUMBNAILTOOLBAR_P_H
#define QWINTHUMBNAILTOOLBAR_P_H

#include "qwinthumbnailtoolbar.h"

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtGui/QIcon>
#include <QtGui/QPixmap>
#include <QtCore/QAbstractNativeEventFilter>

#include "winshobjidl_p.h"

QT_BEGIN_NAMESPACE

class QWinThumbnailToolBarPrivate : public QObject, QAbstractNativeEventFilter
{
public:
    class IconicPixmapCache
    {
    public:
        IconicPixmapCache() : m_bitmap(0) {}
        ~IconicPixmapCache() { deleteBitmap(); }

        operator bool() const { return !m_pixmap.isNull(); }

        QPixmap pixmap() const { return m_pixmap; }
        bool setPixmap(const QPixmap &p);

        HBITMAP bitmap(const QSize &maxSize);

    private:
        void deleteBitmap();

        QPixmap m_pixmap;
        QSize m_size;
        HBITMAP m_bitmap;
    };

    QWinThumbnailToolBarPrivate();
    ~QWinThumbnailToolBarPrivate();
    void initToolbar();
    void clearToolbar();
    void _q_updateToolbar();
    void _q_scheduleUpdate();
    bool eventFilter(QObject *, QEvent *) Q_DECL_OVERRIDE;

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE;

    static void initButtons(THUMBBUTTON *buttons);
    static int makeNativeButtonFlags(const QWinThumbnailToolButton *button);
    static int makeButtonMask(const QWinThumbnailToolButton *button);
    static QString msgComFailed(const char *function, HRESULT hresult);

    bool updateScheduled;
    QList<QWinThumbnailToolButton *> buttonList;
    QWindow *window;
    ITaskbarList4 * const pTbList;

    IconicPixmapCache iconicThumbnail;
    IconicPixmapCache iconicLivePreview;

private:
    bool hasHandle() const;
    HWND handle() const;
    void updateIconicPixmapsEnabled(bool invalidate);
    void updateIconicThumbnail(const MSG *message);
    void updateIconicLivePreview(const MSG *message);

    QWinThumbnailToolBar *q_ptr;
    Q_DECLARE_PUBLIC(QWinThumbnailToolBar)
    bool withinIconicThumbnailRequest;
    bool withinIconicLivePreviewRequest;
};

QT_END_NAMESPACE

#endif // QWINTHUMBNAILTOOLBAR_P_H
