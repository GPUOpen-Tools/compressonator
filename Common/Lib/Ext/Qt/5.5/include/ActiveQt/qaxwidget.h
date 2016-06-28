/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the ActiveQt framework of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QAXWIDGET_H
#define QAXWIDGET_H

#include <ActiveQt/qaxbase.h>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class QAxHostWindow;
class QAxAggregated;

class QAxClientSite;
class QAxWidgetPrivate;

class QAxWidget : public QWidget, public QAxBase
{
    Q_OBJECT_FAKE
public:
    QObject* qObject() const { return (QWidget*)this; }
    const char *className() const;

    QAxWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
    QAxWidget(const QString &c, QWidget *parent = 0, Qt::WindowFlags f = 0);
    QAxWidget(IUnknown *iface, QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~QAxWidget();

    void clear();
    bool doVerb(const QString &verb);

    QSize sizeHint() const;
    QSize minimumSizeHint() const;

    virtual QAxAggregated *createAggregate();

protected:
    bool initialize(IUnknown**);
    virtual bool createHostWindow(bool);
    bool createHostWindow(bool, const QByteArray&);

    void changeEvent(QEvent *e);
    void resizeEvent(QResizeEvent *);

    virtual bool translateKeyEvent(int message, int keycode) const;

    void connectNotify(const QMetaMethod &signal);
    const QMetaObject *fallbackMetaObject() const;
private:
    friend class QAxClientSite;
    QAxClientSite *container;

    QAxWidgetPrivate *d;
    const QMetaObject *parentMetaObject() const;
};

template <> inline QAxWidget *qobject_cast<QAxWidget*>(const QObject *o)
{
    void *result = o ? const_cast<QObject *>(o)->qt_metacast("QAxWidget") : 0;
    return (QAxWidget*)(result);
}

template <> inline QAxWidget *qobject_cast<QAxWidget*>(QObject *o)
{
    void *result = o ? o->qt_metacast("QAxWidget") : 0;
    return (QAxWidget*)(result);
}

QT_END_NAMESPACE

#endif // QAXWIDGET_H
