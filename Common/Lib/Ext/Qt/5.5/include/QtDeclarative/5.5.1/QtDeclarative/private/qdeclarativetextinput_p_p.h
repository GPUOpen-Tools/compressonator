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

#ifndef QDECLARATIVETEXTINPUT_P_H
#define QDECLARATIVETEXTINPUT_P_H

#include "private/qdeclarativetextinput_p.h"

#include "private/qdeclarativeimplicitsizeitem_p_p.h"

#include <qdeclarative.h>

#include <QPointer>

#include <private/qwidgetlinecontrol_p.h>

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#ifndef QT_NO_LINEEDIT

QT_BEGIN_NAMESPACE

class Q_AUTOTEST_EXPORT QDeclarativeTextInputPrivate : public QDeclarativeImplicitSizePaintedItemPrivate
{
    Q_DECLARE_PUBLIC(QDeclarativeTextInput)
public:
    QDeclarativeTextInputPrivate() : control(new QWidgetLineControl),
                 color((QRgb)0), style(QDeclarativeText::Normal),
                 styleColor((QRgb)0), hAlign(QDeclarativeTextInput::AlignLeft),
                 mouseSelectionMode(QDeclarativeTextInput::SelectCharacters), inputMethodHints(Qt::ImhNone),
                 hscroll(0), oldScroll(0), oldValidity(false), focused(false), focusOnPress(true),
                 showInputPanelOnFocus(true), clickCausedFocus(false), cursorVisible(false),
                 autoScroll(true), selectByMouse(false), canPaste(false), canPasteValid(false)
               , hAlignImplicit(true), selectPressed(false)
    {
    }

    ~QDeclarativeTextInputPrivate()
    {
    }

    int xToPos(int x, QTextLine::CursorPosition betweenOrOn = QTextLine::CursorBetweenCharacters) const
    {
        Q_Q(const QDeclarativeTextInput);
        QRect cr = q->boundingRect().toRect();
        x-= cr.x() - hscroll;
        return control->xToPos(x, betweenOrOn);
    }

    void init();
    void startCreatingCursor();
    void focusChanged(bool hasFocus);
    void updateHorizontalScroll();
    bool determineHorizontalAlignment();
    bool setHAlign(QDeclarativeTextInput::HAlignment, bool forceAlign = false);
    void mirrorChange();
    int calculateTextWidth();
    bool sendMouseEventToInputContext(QGraphicsSceneMouseEvent *event, QEvent::Type eventType);
    void updateInputMethodHints();

    QWidgetLineControl* control;

    QFont font;
    QFont sourceFont;
    QColor  color;
    QColor  selectionColor;
    QColor  selectedTextColor;
    QDeclarativeText::TextStyle style;
    QColor  styleColor;
    QDeclarativeTextInput::HAlignment hAlign;
    QDeclarativeTextInput::SelectionMode mouseSelectionMode;
    Qt::InputMethodHints inputMethodHints;
    QPointer<QDeclarativeComponent> cursorComponent;
    QPointer<QDeclarativeItem> cursorItem;
    QPointF pressPos;

    int lastSelectionStart;
    int lastSelectionEnd;
    int oldHeight;
    int oldWidth;
    int hscroll;
    int oldScroll;
    bool oldValidity:1;
    bool focused:1;
    bool focusOnPress:1;
    bool showInputPanelOnFocus:1;
    bool clickCausedFocus:1;
    bool cursorVisible:1;
    bool autoScroll:1;
    bool selectByMouse:1;
    mutable bool canPaste:1;
    mutable bool canPasteValid:1;
    bool hAlignImplicit:1;
    bool selectPressed:1;

    static inline QDeclarativeTextInputPrivate *get(QDeclarativeTextInput *t) {
        return t->d_func();
    }
};

QT_END_NAMESPACE

#endif // QT_NO_LINEEDIT

#endif

