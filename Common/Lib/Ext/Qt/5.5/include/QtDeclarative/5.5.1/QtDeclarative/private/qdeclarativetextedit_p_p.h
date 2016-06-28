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

#ifndef QDECLARATIVETEXTEDIT_P_H
#define QDECLARATIVETEXTEDIT_P_H

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

#include <qdeclarative.h>

QT_BEGIN_NAMESPACE
class QTextLayout;
class QTextDocument;
class QWidgetTextControl;
class QDeclarativeTextEditPrivate : public QDeclarativeImplicitSizePaintedItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeTextEdit)

public:
    QDeclarativeTextEditPrivate()
      : color("black"), hAlign(QDeclarativeTextEdit::AlignLeft), vAlign(QDeclarativeTextEdit::AlignTop),
      imgDirty(true), dirty(false), richText(false), cursorVisible(false), focusOnPress(true),
      showInputPanelOnFocus(true), clickCausedFocus(false), persistentSelection(true), requireImplicitWidth(false),
      hAlignImplicit(true), rightToLeftText(false), selectByMouse(false), canPaste(false), canPasteValid(false),
      textMargin(0.0), lastSelectionStart(0), lastSelectionEnd(0),
      cursorComponent(0), cursor(0), format(QDeclarativeTextEdit::AutoText), document(0), wrapMode(QDeclarativeTextEdit::NoWrap),
      mouseSelectionMode(QDeclarativeTextEdit::SelectCharacters), lineCount(0),
      yoff(0)
    {
    }

    void init();

    void updateDefaultTextOption();
    void relayoutDocument();
    void updateSelection();
    bool determineHorizontalAlignment();
    bool setHAlign(QDeclarativeTextEdit::HAlignment, bool forceAlign = false);
    void mirrorChange();
    qreal implicitWidth() const;
    void focusChanged(bool);

    QString text;
    QFont font;
    QFont sourceFont;
    QColor  color;
    QColor  selectionColor;
    QColor  selectedTextColor;
    QString style;
    QColor  styleColor;
    QPixmap imgCache;
    QPixmap imgStyleCache;
    QDeclarativeTextEdit::HAlignment hAlign;
    QDeclarativeTextEdit::VAlignment vAlign;
    bool imgDirty : 1;
    bool dirty : 1;
    bool richText : 1;
    bool cursorVisible : 1;
    bool focusOnPress : 1;
    bool showInputPanelOnFocus : 1;
    bool clickCausedFocus : 1;
    bool persistentSelection : 1;
    bool requireImplicitWidth:1;
    bool hAlignImplicit:1;
    bool rightToLeftText:1;
    bool selectByMouse : 1;
    mutable bool canPaste : 1;
    mutable bool canPasteValid : 1;
    qreal textMargin;
    int lastSelectionStart;
    int lastSelectionEnd;
    QDeclarativeComponent* cursorComponent;
    QDeclarativeItem* cursor;
    QDeclarativeTextEdit::TextFormat format;
    QTextDocument *document;
    QWidgetTextControl *control;
    QDeclarativeTextEdit::WrapMode wrapMode;
    QDeclarativeTextEdit::SelectionMode mouseSelectionMode;
    int lineCount;
    int yoff;
    QSize paintedSize;
};

QT_END_NAMESPACE
#endif
