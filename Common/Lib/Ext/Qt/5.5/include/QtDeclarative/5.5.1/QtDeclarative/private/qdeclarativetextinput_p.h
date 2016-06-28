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

#ifndef QDECLARATIVETEXTINPUT_H
#define QDECLARATIVETEXTINPUT_H

#include "private/qdeclarativetext_p.h"
#include "private/qdeclarativeimplicitsizeitem_p.h"

#include <QGraphicsSceneMouseEvent>
#include <QIntValidator>

#ifndef QT_NO_LINEEDIT

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeTextInputPrivate;
class QValidator;
class Q_AUTOTEST_EXPORT QDeclarativeTextInput : public QDeclarativeImplicitSizePaintedItem
{
    Q_OBJECT
    Q_ENUMS(HAlignment)
    Q_ENUMS(EchoMode)
    Q_ENUMS(SelectionMode)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor selectionColor READ selectionColor WRITE setSelectionColor NOTIFY selectionColorChanged)
    Q_PROPERTY(QColor selectedTextColor READ selectedTextColor WRITE setSelectedTextColor NOTIFY selectedTextColorChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(HAlignment horizontalAlignment READ hAlign WRITE setHAlign RESET resetHAlign NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly NOTIFY readOnlyChanged)
    Q_PROPERTY(bool cursorVisible READ isCursorVisible WRITE setCursorVisible NOTIFY cursorVisibleChanged)
    Q_PROPERTY(int cursorPosition READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(QRect cursorRectangle READ cursorRectangle NOTIFY cursorRectangleChanged)
    Q_PROPERTY(QDeclarativeComponent *cursorDelegate READ cursorDelegate WRITE setCursorDelegate NOTIFY cursorDelegateChanged)
    Q_PROPERTY(int selectionStart READ selectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int selectionEnd READ selectionEnd NOTIFY selectionEndChanged)
    Q_PROPERTY(QString selectedText READ selectedText NOTIFY selectedTextChanged)

    Q_PROPERTY(int maximumLength READ maxLength WRITE setMaxLength NOTIFY maximumLengthChanged)
#ifndef QT_NO_VALIDATOR
    Q_PROPERTY(QValidator* validator READ validator WRITE setValidator NOTIFY validatorChanged)
#endif
    Q_PROPERTY(QString inputMask READ inputMask WRITE setInputMask NOTIFY inputMaskChanged)
    Q_PROPERTY(Qt::InputMethodHints inputMethodHints READ imHints WRITE setIMHints)

    Q_PROPERTY(bool acceptableInput READ hasAcceptableInput NOTIFY acceptableInputChanged)
    Q_PROPERTY(EchoMode echoMode READ echoMode WRITE setEchoMode NOTIFY echoModeChanged)
    Q_PROPERTY(bool activeFocusOnPress READ focusOnPress WRITE setFocusOnPress NOTIFY activeFocusOnPressChanged)
    Q_PROPERTY(QString passwordCharacter READ passwordCharacter WRITE setPasswordCharacter NOTIFY passwordCharacterChanged)
    Q_PROPERTY(QString displayText READ displayText NOTIFY displayTextChanged)
    Q_PROPERTY(bool autoScroll READ autoScroll WRITE setAutoScroll NOTIFY autoScrollChanged)
    Q_PROPERTY(bool selectByMouse READ selectByMouse WRITE setSelectByMouse NOTIFY selectByMouseChanged)
    Q_PROPERTY(SelectionMode mouseSelectionMode READ mouseSelectionMode WRITE setMouseSelectionMode NOTIFY mouseSelectionModeChanged REVISION 1)
    Q_PROPERTY(bool canPaste READ canPaste NOTIFY canPasteChanged REVISION 1)
    Q_PROPERTY(bool inputMethodComposing READ isInputMethodComposing NOTIFY inputMethodComposingChanged REVISION 1)

public:
    QDeclarativeTextInput(QDeclarativeItem* parent=0);
    ~QDeclarativeTextInput();

    enum EchoMode {//To match QLineEdit::EchoMode
        Normal,
        NoEcho,
        Password,
        PasswordEchoOnEdit
    };

    enum HAlignment {
        AlignLeft = Qt::AlignLeft,
        AlignRight = Qt::AlignRight,
        AlignHCenter = Qt::AlignHCenter
    };

    enum SelectionMode {
        SelectCharacters,
        SelectWords
    };

    enum CursorPosition {
        CursorBetweenCharacters,
        CursorOnCharacter
    };

    //Auxilliary functions needed to control the TextInput from QML
    Q_INVOKABLE int positionAt(int x) const;
    Q_INVOKABLE Q_REVISION(1) int positionAt(int x, CursorPosition position) const;
    Q_INVOKABLE QRectF positionToRectangle(int pos) const;
    Q_INVOKABLE void moveCursorSelection(int pos);
    Q_INVOKABLE Q_REVISION(1) void moveCursorSelection(int pos, SelectionMode mode);

    Q_INVOKABLE void openSoftwareInputPanel();
    Q_INVOKABLE void closeSoftwareInputPanel();

    QString text() const;
    void setText(const QString &);

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

    bool isReadOnly() const;
    void setReadOnly(bool);

    bool isCursorVisible() const;
    void setCursorVisible(bool on);

    int cursorPosition() const;
    void setCursorPosition(int cp);

    QRect cursorRectangle() const;

    int selectionStart() const;
    int selectionEnd() const;

    QString selectedText() const;

    int maxLength() const;
    void setMaxLength(int ml);

#ifndef QT_NO_VALIDATOR
    QValidator * validator() const;
    void setValidator(QValidator* v);
#endif
    QString inputMask() const;
    void setInputMask(const QString &im);

    EchoMode echoMode() const;
    void setEchoMode(EchoMode echo);

    QString passwordCharacter() const;
    void setPasswordCharacter(const QString &str);

    QString displayText() const;

    QDeclarativeComponent* cursorDelegate() const;
    void setCursorDelegate(QDeclarativeComponent*);

    bool focusOnPress() const;
    void setFocusOnPress(bool);

    bool autoScroll() const;
    void setAutoScroll(bool);

    bool selectByMouse() const;
    void setSelectByMouse(bool);

    SelectionMode mouseSelectionMode() const;
    void setMouseSelectionMode(SelectionMode mode);

    bool hasAcceptableInput() const;

    void drawContents(QPainter *p,const QRect &r);
    QVariant inputMethodQuery(Qt::InputMethodQuery property) const;

    QRectF boundingRect() const;
    bool canPaste() const;

    bool isInputMethodComposing() const;

    Qt::InputMethodHints imHints() const;
    void setIMHints(Qt::InputMethodHints hints);

Q_SIGNALS:
    void textChanged();
    void cursorPositionChanged();
    void cursorRectangleChanged();
    void selectionStartChanged();
    void selectionEndChanged();
    void selectedTextChanged();
    void accepted();
    void acceptableInputChanged();
    void colorChanged(const QColor &color);
    void selectionColorChanged(const QColor &color);
    void selectedTextColorChanged(const QColor &color);
    void fontChanged(const QFont &font);
    void horizontalAlignmentChanged(HAlignment alignment);
    void readOnlyChanged(bool isReadOnly);
    void cursorVisibleChanged(bool isCursorVisible);
    void cursorDelegateChanged();
    void maximumLengthChanged(int maximumLength);
    void validatorChanged();
    void inputMaskChanged(const QString &inputMask);
    void echoModeChanged(EchoMode echoMode);
    void passwordCharacterChanged();
    void displayTextChanged();
    void activeFocusOnPressChanged(bool activeFocusOnPress);
    void autoScrollChanged(bool autoScroll);
    void selectByMouseChanged(bool selectByMouse);
    Q_REVISION(1) void mouseSelectionModeChanged(SelectionMode mode);
    Q_REVISION(1) void canPasteChanged();
    Q_REVISION(1) void inputMethodComposingChanged();

protected:
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    bool sceneEvent(QEvent *event);
    void keyPressEvent(QKeyEvent* ev);
    void inputMethodEvent(QInputMethodEvent *);
    bool event(QEvent *e);
    void focusInEvent(QFocusEvent *event);

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
    void updateSize(bool needsRedraw = true);
    void q_textChanged();
    void selectionChanged();
    void createCursor();
    void cursorPosChanged();
    void updateCursorRectangle();
    void updateRect(const QRect &r = QRect());
    void q_canPasteChanged();

private:
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeTextInput)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeTextInput)
#ifndef QT_NO_VALIDATOR
QML_DECLARE_TYPE(QValidator)
QML_DECLARE_TYPE(QIntValidator)
QML_DECLARE_TYPE(QDoubleValidator)
QML_DECLARE_TYPE(QRegExpValidator)
#endif

#endif // QT_NO_LINEEDIT

#endif // QDECLARATIVETEXTINPUT_H
