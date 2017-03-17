//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
/// \version 2.20
//
//=====================================================================

#ifndef _GENMIPS_H
#define _GENMIPS_H

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"


class CData : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString m_str READ get_str WRITE set_str)    // NOTIFY objectNameChanged

public:
    QString get_str()               { return m_str; }
    void set_str(QString in_str)    { m_str = in_str; }

    Qt::FocusPolicy focusPolicy() const;
    void setFocusPolicy(Qt::FocusPolicy policy);


    // Options
    bool    get_bDither()               { return m_bDither;             }
    bool    get_bMirrorPixels()         { return m_bMirrorPixels;       }
    bool    get_bPerformFiltering()     { return m_bPerformFiltering;   }
    void    set_bDither(bool Dither)               { m_bDither = Dither; }
    void    set_bMirrorPixels(bool MirrorPixels)         { m_bMirrorPixels = MirrorPixels; }
    void    set_bPerformFiltering(bool PerformFiltering)     { m_bPerformFiltering = PerformFiltering; }


private:
    // Options
    bool            m_bDither;
    bool            m_bMirrorPixels;
    bool            m_bPerformFiltering;
    QStringList     m_MipLevelSizes;
    QString         m_str;
};

class CGenMips : public QWidget
{
    Q_OBJECT

public:
    
    CGenMips(const QString title, QWidget *parent);
    ~CGenMips();

    void setMipLevelDisplay(int Width, int Height);

    void SetDefaults();
    void evalProperties();


private:
    
    int m_ImageSize_W;
    int m_ImageSize_H;
    int m_MipLevels;
    QtVariantProperty   *m_property;
    QStringList         m_MipLevelSizes;

    void addProperty(QtVariantProperty *property, const QString &id);
    void updateExpandState();
    void closeEvent(QCloseEvent * event);

    class QtVariantPropertyManager      *variantManager;
    class QtAbstractPropertyBrowser     *m_propertyEditor;
    
    QMap<QtProperty *, QString> propertyToId;
    QMap<QString, QtVariantProperty *> idToProperty;
    QMap<QString, bool> idToExpanded;
    
    // Common for all
    QWidget                     *m_newWidget;
    QGridLayout                 *m_layout;
    const QString               m_title;
    QWidget                    *m_parent;


Q_SIGNALS:
    
    void generateMIPMap(int levels, QTreeWidgetItem *item);

public Q_SLOTS:
    void valueChanged(QtProperty *property, const QVariant &value);
    void onCancel();
    void onGenerate();

public:
    CData   *m_cdata;
    QTreeWidgetItem *m_mipsitem;
};

// External R&D - Info pages
// https://en.wikipedia.org/wiki/Mipmap
// http://www.number-none.com/product/Mipmapping,%20Part%201/index.html
// http://www.number-none.com/product/Mipmapping,%20Part%202/index.html


#endif