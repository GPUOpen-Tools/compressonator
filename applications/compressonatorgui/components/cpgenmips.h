//=====================================================================
// Copyright 2016-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#include "compressonator.h"
#include "common.h"

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>

#include "qteditorfactory.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"

// As defined by D3DX9

class CGenMips : public QWidget
{
    Q_OBJECT

public:
    CGenMips(const QString title, QWidget* parent);
    ~CGenMips();

    void setMipLevelDisplay(int Width, int Height, bool UsingGPU);

    void SetGUIItems();

private:
    CMP_CFilterParams m_CFilterParams;

    int m_selectedMipLevel;
    int m_levelWidths[MAX_MIPLEVEL_SUPPORTED];

    QtVariantProperty* m_propertyMipLevels;
    QtVariantProperty* m_propertyGamma;
    QtVariantProperty* m_propertyFilterType;
    QtVariantProperty* m_GroupProperty;
    QtVariantProperty* m_propertyDither;
    QtVariantProperty* m_propertyMirrorPixels;
    //QtVariantProperty* m_propertyPerformFiltering;

    QStringList m_mipLevelSizes;

    void addProperty(QtVariantProperty* property, const QString& id);
    void addD3DXProperty(QtVariantProperty* property, const QString& id);

    void updateExpandState();
    void closeEvent(QCloseEvent* event);

    class QtVariantPropertyManager*  m_variantPropertyManager;
    class QtAbstractPropertyBrowser* m_propertyEditor;

    QMap<QtProperty*, QString>        propertyToId;
    QMap<QString, QtVariantProperty*> idToProperty;
    QMap<QString, bool>               idToExpanded;

    // Common for all
    QWidget*      m_newWidget;
    QGridLayout*  m_layout;
    const QString m_title;
    QWidget*      m_parent;

Q_SIGNALS:

    void signalGenerateMipmaps(CMP_CFilterParams m_CFilterParams, const std::vector<QTreeWidgetItem*>& items);

public Q_SLOTS:
    void valueChanged(QtProperty* property, const QVariant& value);
    void onCancel();
    void onGenerate();

public:
    std::vector<QTreeWidgetItem*> m_imageItems;
};
#endif
