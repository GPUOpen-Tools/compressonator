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

#ifndef QDECLARATIVETEXT_P_H
#define QDECLARATIVETEXT_P_H

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

#include "qdeclarativeitem.h"
#include "private/qdeclarativeimplicitsizeitem_p_p.h"
#include "private/qdeclarativetextlayout_p.h"

#include <qdeclarative.h>

#include <QtGui/qtextlayout.h>

QT_BEGIN_NAMESPACE

class QTextLayout;
class QTextDocumentWithImageResources;

class Q_AUTOTEST_EXPORT QDeclarativeTextPrivate : public QDeclarativeImplicitSizeItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeText)
public:
    QDeclarativeTextPrivate();

    ~QDeclarativeTextPrivate();

    void updateSize();
    void updateLayout();
    bool determineHorizontalAlignment();
    bool setHAlign(QDeclarativeText::HAlignment, bool forceAlign = false);
    void mirrorChange();
    QTextDocument *textDocument();

    QString text;
    QFont font;
    QFont sourceFont;
    QColor  color;
    QDeclarativeText::TextStyle style;
    QColor  styleColor;
    QString activeLink;
    QDeclarativeText::HAlignment hAlign;
    QDeclarativeText::VAlignment vAlign;
    QDeclarativeText::TextElideMode elideMode;
    QDeclarativeText::TextFormat format;
    QDeclarativeText::WrapMode wrapMode;
    qreal lineHeight;
    QDeclarativeText::LineHeightMode lineHeightMode;
    int lineCount;
    bool truncated;
    int maximumLineCount;
    int maximumLineCountValid;
    QPointF elidePos;

    static QString elideChar;

    void invalidateImageCache();
    void checkImageCache();
    QPixmap imageCache;

    bool imageCacheDirty:1;
    bool updateOnComponentComplete:1;
    bool richText:1;
    bool singleline:1;
    bool cacheAllTextAsImage:1;
    bool internalWidthUpdate:1;
    bool requireImplicitWidth:1;
    bool hAlignImplicit:1;
    bool rightToLeftText:1;
    bool layoutTextElided:1;

    QRect layedOutTextRect;
    QSize paintedSize;
    qreal naturalWidth;
    virtual qreal implicitWidth() const;
    void ensureDoc();
    QPixmap textDocumentImage(bool drawStyle);
    QTextDocumentWithImageResources *doc;

    QRect setupTextLayout();
    QPixmap textLayoutImage(bool drawStyle);
    void drawTextLayout(QPainter *p, const QPointF &pos, bool drawStyle);
    QDeclarativeTextLayout layout;

    static QPixmap drawOutline(const QPixmap &source, const QPixmap &styleSource);
    static QPixmap drawOutline(const QPixmap &source, const QPixmap &styleSource, int yOffset);

    static inline QDeclarativeTextPrivate *get(QDeclarativeText *t) {
        return t->d_func();
    }
};

QT_END_NAMESPACE
#endif
