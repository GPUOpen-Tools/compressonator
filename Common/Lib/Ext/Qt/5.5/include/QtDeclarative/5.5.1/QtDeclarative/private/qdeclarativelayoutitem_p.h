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

#ifndef QDECLARATIVEGRAPHICSLAYOUTITEM_H
#define QDECLARATIVEGRAPHICSLAYOUTITEM_H
#include "qdeclarativeitem.h"

#include <QGraphicsLayoutItem>
#include <QSizeF>

QT_BEGIN_NAMESPACE

QT_MODULE(Declarative)

class QDeclarativeLayoutItem : public QDeclarativeItem, public QGraphicsLayoutItem
{
    Q_OBJECT
    Q_INTERFACES(QGraphicsLayoutItem)
    Q_PROPERTY(QSizeF maximumSize READ maximumSize WRITE setMaximumSize NOTIFY maximumSizeChanged)
    Q_PROPERTY(QSizeF minimumSize READ minimumSize WRITE setMinimumSize NOTIFY minimumSizeChanged)
    Q_PROPERTY(QSizeF preferredSize READ preferredSize WRITE setPreferredSize NOTIFY preferredSizeChanged)
public:
    QDeclarativeLayoutItem(QDeclarativeItem* parent=0);

    QSizeF maximumSize() const { return m_maximumSize; }
    void setMaximumSize(const QSizeF &s) { if(s==m_maximumSize) return; m_maximumSize = s; emit maximumSizeChanged(); }

    QSizeF minimumSize() const { return m_minimumSize; }
    void setMinimumSize(const QSizeF &s) { if(s==m_minimumSize) return; m_minimumSize = s; emit minimumSizeChanged(); }

    QSizeF preferredSize() const { return m_preferredSize; }
    void setPreferredSize(const QSizeF &s) { if(s==m_preferredSize) return; m_preferredSize = s; emit preferredSizeChanged(); }

    virtual void setGeometry(const QRectF & rect);
protected:
    virtual QSizeF sizeHint(Qt::SizeHint which, const QSizeF &constraint = QSizeF()) const;

Q_SIGNALS:
    void maximumSizeChanged();
    void minimumSizeChanged();
    void preferredSizeChanged();

private:
    QSizeF m_maximumSize;
    QSizeF m_minimumSize;
    QSizeF m_preferredSize;
};

QT_END_NAMESPACE

QML_DECLARE_TYPE(QDeclarativeLayoutItem)

#endif
