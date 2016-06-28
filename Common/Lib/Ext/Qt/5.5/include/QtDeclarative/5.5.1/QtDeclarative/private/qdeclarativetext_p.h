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

#ifndef QDECLARATIVETEXT_H
#define QDECLARATIVETEXT_H

#include <QtGui/qtextoption.h>
#include "qdeclarativeimplicitsizeitem_p.h"

#include <private/qtdeclarativeglobal_p.h>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)
class QDeclarativeTextPrivate;
class Q_DECLARATIVE_PRIVATE_EXPORT QDeclarativeText : public QDeclarativeImplicitSizeItem
{
    Q_OBJECT
    Q_ENUMS(HAlignment)
    Q_ENUMS(VAlignment)
    Q_ENUMS(TextStyle)
    Q_ENUMS(TextFormat)
    Q_ENUMS(TextElideMode)
    Q_ENUMS(WrapMode)
    Q_ENUMS(LineHeightMode)

    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(TextStyle style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QColor styleColor READ styleColor WRITE setStyleColor NOTIFY styleColorChanged)
    Q_PROPERTY(HAlignment horizontalAlignment READ hAlign WRITE setHAlign RESET resetHAlign NOTIFY horizontalAlignmentChanged)
    Q_PROPERTY(VAlignment verticalAlignment READ vAlign WRITE setVAlign NOTIFY verticalAlignmentChanged)
    Q_PROPERTY(WrapMode wrapMode READ wrapMode WRITE setWrapMode NOTIFY wrapModeChanged)
    Q_PROPERTY(int lineCount READ lineCount NOTIFY lineCountChanged REVISION 1)
    Q_PROPERTY(bool truncated READ truncated NOTIFY truncatedChanged REVISION 1)
    Q_PROPERTY(int maximumLineCount READ maximumLineCount WRITE setMaximumLineCount NOTIFY maximumLineCountChanged RESET resetMaximumLineCount REVISION 1)

    Q_PROPERTY(TextFormat textFormat READ textFormat WRITE setTextFormat NOTIFY textFormatChanged)
    Q_PROPERTY(TextElideMode elide READ elideMode WRITE setElideMode NOTIFY elideModeChanged) //### elideMode?
    Q_PROPERTY(qreal paintedWidth READ paintedWidth NOTIFY paintedSizeChanged)
    Q_PROPERTY(qreal paintedHeight READ paintedHeight NOTIFY paintedSizeChanged)
    Q_PROPERTY(qreal lineHeight READ lineHeight WRITE setLineHeight NOTIFY lineHeightChanged REVISION 1)
    Q_PROPERTY(LineHeightMode lineHeightMode READ lineHeightMode WRITE setLineHeightMode NOTIFY lineHeightModeChanged REVISION 1)

public:
    QDeclarativeText(QDeclarativeItem *parent=0);
    ~QDeclarativeText();

    enum HAlignment { AlignLeft = Qt::AlignLeft,
                       AlignRight = Qt::AlignRight,
                       AlignHCenter = Qt::AlignHCenter,
                       AlignJustify = Qt::AlignJustify }; // ### VERSIONING: Only in QtQuick 1.1
    enum VAlignment { AlignTop = Qt::AlignTop,
                       AlignBottom = Qt::AlignBottom,
                       AlignVCenter = Qt::AlignVCenter };
    enum TextStyle { Normal,
                      Outline,
                      Raised,
                      Sunken };
    enum TextFormat { PlainText = Qt::PlainText,
                       RichText = Qt::RichText,
                       AutoText = Qt::AutoText,
                       StyledText = 4 };
    enum TextElideMode { ElideLeft = Qt::ElideLeft,
                          ElideRight = Qt::ElideRight,
                          ElideMiddle = Qt::ElideMiddle,
                          ElideNone = Qt::ElideNone };

    enum WrapMode { NoWrap = QTextOption::NoWrap,
                    WordWrap = QTextOption::WordWrap,
                    WrapAnywhere = QTextOption::WrapAnywhere,
                    WrapAtWordBoundaryOrAnywhere = QTextOption::WrapAtWordBoundaryOrAnywhere, // COMPAT
                    Wrap = QTextOption::WrapAtWordBoundaryOrAnywhere
                  };

    enum LineHeightMode { ProportionalHeight, FixedHeight };

    QString text() const;
    void setText(const QString &);

    QFont font() const;
    void setFont(const QFont &font);

    QColor color() const;
    void setColor(const QColor &c);

    TextStyle style() const;
    void setStyle(TextStyle style);

    QColor styleColor() const;
    void setStyleColor(const QColor &c);

    HAlignment hAlign() const;
    void setHAlign(HAlignment align);
    void resetHAlign();
    HAlignment effectiveHAlign() const;

    VAlignment vAlign() const;
    void setVAlign(VAlignment align);

    WrapMode wrapMode() const;
    void setWrapMode(WrapMode w);

    int lineCount() const;
    bool truncated() const;

    int maximumLineCount() const;
    void setMaximumLineCount(int lines);
    void resetMaximumLineCount();

    TextFormat textFormat() const;
    void setTextFormat(TextFormat format);

    TextElideMode elideMode() const;
    void setElideMode(TextElideMode);

    qreal lineHeight() const;
    void setLineHeight(qreal lineHeight);

    LineHeightMode lineHeightMode() const;
    void setLineHeightMode(LineHeightMode);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    virtual void componentComplete();

    int resourcesLoading() const; // mainly for testing

    qreal paintedWidth() const;
    qreal paintedHeight() const;

    QRectF boundingRect() const;

Q_SIGNALS:
    void textChanged(const QString &text);
    void linkActivated(const QString &link);
    void fontChanged(const QFont &font);
    void colorChanged(const QColor &color);
    void styleChanged(TextStyle style);
    void styleColorChanged(const QColor &color);
    void horizontalAlignmentChanged(HAlignment alignment);
    void verticalAlignmentChanged(VAlignment alignment);
    void wrapModeChanged();
    Q_REVISION(1) void lineCountChanged();
    Q_REVISION(1) void truncatedChanged();
    Q_REVISION(1) void maximumLineCountChanged();
    void textFormatChanged(TextFormat textFormat);
    void elideModeChanged(TextElideMode mode);
    void paintedSizeChanged();
    Q_REVISION(1) void lineHeightChanged(qreal lineHeight);
    Q_REVISION(1) void lineHeightModeChanged(LineHeightMode mode);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void geometryChanged(const QRectF &newGeometry,
                                 const QRectF &oldGeometry);

private:
    Q_DISABLE_COPY(QDeclarativeText)
    Q_DECLARE_PRIVATE_D(QGraphicsItem::d_ptr.data(), QDeclarativeText)
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeText)

#endif
