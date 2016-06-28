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

#ifndef QDECLARATIVETEXTEDIT_H
#define QDECLARATIVETEXTEDIT_H

#include "private/qdeclarativetext_p.h"
#include "private/qdeclarativeimplicitsizeitem_p.h"

#include <QtGui/qtextdocument.h>
#include <QtGui/qtextoption.h>
#include <QtGui/qtextcursor.h>
#include <QtGui/qtextformat.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)


class QDeclarativeTextEditPrivate;
class Q_AUTOTEST_EXPORT QDeclarativeTextEdit : public QDeclarativeImplicitSizePaintedItem
{
    Q_OBJECT
    Q_ENUMS(VAlignment)
    Q_ENUMS(HAlignment)
    Q_ENUMS(TextFormat)
    Q_ENUMS(WrapMode)
    Q_ENUMS(SelectionMode)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY selectionColorChanged)
    Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor NOTIFY selectedTextColorChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(HAlignment horizontalAlignment READ hAlign WRITE setHAlign RESET resetHAlign NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VAlignment verticalAlignment READ vAlign WRITE setVAlign NOTIFY verticalAlignmentChanged)
    Q_PROPERTY(WrapMode wrapMode READ wrapMode WRITE setWrapMode NOTIFY wrapModeChanged)
    Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged REVISION 1)
    Q_PROPERTY(qreal paintedWidth READ paintedWidth NOTIFY paintedSizeChanged)
    Q_PROPERTY(qreal paintedHeight READ paintedHeight NOTIFY paintedSizeChanged)
    Q_PROPERTY(TextFormat textFormat READ textFormat WRITE setTextFormat NOTIFY textFormatChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool cursorVisible READ isCursorVisible WRITE setCursorVisible NOTIFY cursorVisibleChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QRect cursorRectangle READ cursorRectangle NOTIFY cursorRectangleChanged)
    Q_PROPERTY(QDeclarativeComponent* cursorDelegate READ cursorDelegate WRITE setCursorDelegate NOTIFY cursorDelegateChanged)
    Q_PROPERTY(int selectionStart READ selectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectionChanged)
    Q_PROPERTY(bool activeFocusOnPress READ focusOnPress WRITE setFocusOnPress NOTIFY activeFocusOnPressChanged)
    Q_PROPERTY(bool persistentSelection READ persistentSelection WRITE setPersistentSelection NOTIFY persistentSelectionChanged)
    Q_PROPERTY(qreal textMargin READ textMargin WRITE setTextMargin NOTIFY textMarginChanged)
    Q_PROPERTY(Qt::InputMethodHints inputMethodHints READ inputMethodHints WRITE setInputMethodHints)
    Q_PROPERTY(bool selectByMouse READ selectByMouse WRITE setSelectByMouse NOTIFY selectByMouseChanged)
    Q_PROPERTY(SelectionMode mouseSelectionMode READ mouseSelectionMode WRITE setMouseSelectionMode NOTIFY mouseSelectionModeChanged REVISION 1)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged REVISION 1)
    Q_PROPERTY(bool inputMethodComposing READ isInputMethodComposing NOTIFY inputMethodComposingChanged REVISION 1)

public:
    QDeclarativeTextEdit(QDeclarativeItem *parent=0);

    enum HAlignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter,
        AlignJustify = Qt::AlignJustify // ### VERSIONING: Only in QtQuick 1.1
    };

    enum VAlignment {
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignVCenter = Qt::AlignVCenter
    };

    enum TextFormat {
        PlainText = Qt::PlainText,
        RichText = Qt::RichText,
        AutoText = Qt::AutoText
    };

    enum WrapMode { NoWrap = QTextOption::NoWrap,
                    WordWrap = QTextOption::WordWrap,
                    WrapAnywhere = QTextOption::WrapAnywhere,
                    WrapAtWordBoundaryOrAnywhere = QTextOption::WrapAtWordBoundaryOrAnywhere, // COMPAT
                    Wrap = QTextOption::WrapAtWordBoundaryOrAnywhere
                  };

    enum SelectionMode {
        SelectCharacters,
        SelectWords
    };

    Q_INVOKABLE void openSoftwareInputPanel();
    Q_INVOKABLE void closeSoftwareInputPanel();

    QString text() const;
    void setText(const QString &);

    TextFormat textFormat() const;
    void setTextFormat(TextFormat format);

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &c);

    QColor selectionColor() const;
    void setSelectionColor(const QColor &c);

    QColor selectedTextColor() const;
    void setSelectedTextColor(const QColor &c);

    HAlignment hAlign() const;
    void setHAlign(HAlignment align);
    void resetHAlign();
    HAlignment effectiveHAlign() const;

    VAlignment vAlign() const;
    void setVAlign(VAlignment align);

    WrapMode wrapMode() const;
    void setWrapMode(WrapMode w);

    int lineCount() const;

    bool isCursorVisible() const;
    void setCursorVisible(bool on);

    int cursorPosition() const;
    void setCursorPosition(int pos);

    QDeclarativeComponent* cursorDelegate() const;
    void setCursorDelegate(QDeclarativeComponent*);

    int selectionStart() const;
    int selectionEnd() const;

    QString selectedText() const;

    bool focusOnPress() const;
    void setFocusOnPress(bool on);

    bool persistentSelection() const;
    void setPersistentSelection(bool on);

    qreal textMargin() const;
    void setTextMargin(qreal margin);

    bool selectByMouse() const;
    void setSelectByMouse(bool);

    SelectionMode mouseSelectionMode() const;
    void setMouseSelectionMode(SelectionMode mode);

    bool canPaste() const;

    virtual void componentComplete();

    /* FROM EDIT */
    void setReadOnly(bool);
    bool isReadOnly() const;

    void setTextInteractionFlags(Qt::TextInteractionFlags flags);
    Qt::TextInteractionFlags textInteractionFlags() const;

    QRect cursorRectangle() const;

    QVariant inputMethodQuery(Qt::InputMethodQuery property) const;
    Q_INVOKABLE QVariant inputMethodQuery(Qt::InputMethodQuery property, const QVariant &argument) const;

    qreal paintedWidth() const;
    qreal paintedHeight() const;

    Q_INVOKABLE QRectF positionToRectangle(int) const;
    Q_INVOKABLE int positionAt(int x, int y) const;
    Q_INVOKABLE void moveCursorSelection(int pos);
    Q_INVOKABLE Q_REVISION(1) void moveCursorSelection(int pos, SelectionMode mode);

    QRectF boundingRect() const;

    bool isInputMethodComposing() const;

Q_SIGNALS:
    void textChanged(const QString &);
    void paintedSizeChanged();
    void cursorPositionChanged();
    void cursorRectangleChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void selectionChanged();
    void colorChanged(const QColor &color);
    void selectionColorChanged(const QColor &color);
    void selectedTextColorChanged(const QColor &color);
    void fontChanged(const QFont &font);
    void horizontalAlignmentChanged(HAlignment alignment);
    void verticalAlignmentChanged(VAlignment alignment);
    void wrapModeChanged();
    void lineCountChanged();
    void textFormatChanged(TextFormat textFormat);
    void readOnlyChanged(bool isReadOnly);
    void cursorVisibleChanged(bool isCursorVisible);
    void cursorDelegateChanged();
    void activeFocusOnPressChanged(bool activeFocusOnPressed);
    void persistentSelectionChanged(bool isPersistentSelection);
    void textMarginChanged(qreal textMargin);
    void selectByMouseChanged(bool selectByMouse);
    Q_REVISION(1) void mouseSelectionModeChanged(SelectionMode mode);
    Q_REVISION(1) void linkActivated(const QString &link);
    Q_REVISION(1) void canPasteChanged();
    Q_REVISION(1) void inputMethodComposingChanged();

public Q_SLOTS:
    void selectAll();
    void selectWord();
    void select(int start, int end);
    Q_REVISION(1) void deselect();
    Q_REVISION(1) bool isRightToLeft(int start, int end);
#ifndef QT_NO_CLIPBOARD
    void cut();
    void copy();
    void paste();
#endif

private Q_SLOTS:
    void updateImgCache(const QRectF &rect);
    void q_textChanged();
    void updateSelectionMarkers();
    void moveCursorDelegate();
    void loadCursorDelegate();
    void q_canPasteChanged();

private:
    void updateSize();
    void updateTotalLines();

protected:
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

    bool event(QEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void focusInEvent(QFocusEvent *event);

    // mouse filter?
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    void inputMethodEvent(QInputMethodEvent *e);

    void drawContents(QPainter *, const QRect &);
private:
    Q_DISABLE_COPY(QDeclarativeTextEdit)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeTextEdit)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeTextEdit)

#endif
