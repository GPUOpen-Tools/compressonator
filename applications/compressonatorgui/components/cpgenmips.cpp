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

#include "cpgenmips.h"

#define LOWEST_MIP_LEVELS "Lowest Mip-Level (Width x Height)"

CGenMips::CGenMips(const QString title, QWidget* parent)
    : QWidget(parent)
    , m_title(title)
    , m_parent(parent) {
    setWindowTitle(title);
    setWindowFlags(Qt::Dialog);

    Qt::WindowFlags flags    = windowFlags();
    Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
    flags                    = flags & (~helpFlag);
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);

    resize(300, 220);

    m_CFilterParams.nFilterType         = 0; // CMP CPU version
    m_CFilterParams.dwMipFilterOptions  = 0;
    m_CFilterParams.nMinSize            = 0;
    m_CFilterParams.fGammaCorrection    = 1.0;

    m_variantPropertyManager = new QtVariantPropertyManager(this);
    connect(m_variantPropertyManager, SIGNAL(valueChanged(QtProperty*, const QVariant&)), this, SLOT(valueChanged(QtProperty*, const QVariant&)));
    QtVariantEditorFactory* variantFactory = new QtVariantEditorFactory(this);

    m_propertyEditor = new QtGroupBoxPropertyBrowser(this);
    m_propertyEditor->setFactoryForManager(m_variantPropertyManager, variantFactory);

    SetGUIItems();

    m_ImageSize_W = 0;
    m_ImageSize_H = 0;
    m_MipLevels   = 0;
    m_minsize[0]  = 0;

    // evalProperties(); use this to debug set properties of a class definition , in this case its CData
}

void CGenMips::setMipLevelDisplay(int Width, int Height, bool UsingGPU = false) {
    m_ImageSize_W = Width;
    m_ImageSize_H = Height;
    m_MipLevelSizes.clear();

    QString level;
    level.append(QString::number(Width));
    level.append("x");
    level.append(QString::number(Height));

    m_MipLevelSizes << level;

    m_minsize[0] = Width;
    m_MipLevels  = 1;

    do {
        Width  = (Width > 1) ? (Width >> 1) : 1;
        Height = (Height > 1) ? (Height >> 1) : 1;

        if (UsingGPU) {
            int non4DivW = (Width  % 4); 
            int non4DivH = (Height % 4);
            if (non4DivW || non4DivH)
                break;
        }

        if (m_MipLevels > MAX_MIPLEVEL_SUPPORTED)
            break;

        m_minsize[m_MipLevels] = Width;

        QString level;
        level.append(QString::number(Width));
        level.append("x");
        level.append(QString::number(Height));

        m_MipLevelSizes << level;
        m_MipLevels++;

    } while (Width > 1 && Height > 1);

    QStringList reversedList = m_MipLevelSizes;
    const int   levelSize    = m_MipLevelSizes.size();
    const int   maxSwap      = m_MipLevelSizes.size() / 2;
    for (int i = 0; i < maxSwap; ++i) {
        qSwap(reversedList[i], reversedList[levelSize - 1 - i]);
    }
    m_MipLevelSizes = reversedList;

    m_propertyMipLevels->setAttribute("enumNames", m_MipLevelSizes);
    m_propertyMipLevels->setValue(0);
    m_MipLevels = 0;
}

void CGenMips::onGenerate() {
    int levelSize   = (m_MipLevelSizes.size() - 1) - m_MipLevels;
    if ((levelSize < 0) || (levelSize >= MAX_MIPLEVEL_SUPPORTED))
        levelSize = 0;
    m_CFilterParams.nMinSize = m_minsize[levelSize];
    emit generateMIPMap(m_CFilterParams, this->m_mipsitem);
    hide();
}

CGenMips::~CGenMips() {
}

void CGenMips::closeEvent(QCloseEvent* event) {
    hide();
    event->ignore();
}

void CGenMips::onCancel() {
    hide();
}

void CGenMips::updateExpandState() {
    QList<QtBrowserItem*>         list = m_propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem*> it(list);
    while (it.hasNext()) {
        QtBrowserItem* item              = it.next();
        QtProperty*    prop              = item->property();
        idToExpanded[propertyToId[prop]] = false;  //  propertyEditor->isExpanded(item);
    }
}

void CGenMips::addProperty(QtVariantProperty* property, const QString& id) {
    propertyToId[property] = id;
    idToProperty[id]       = property;
    m_propertyEditor->addProperty(property);
}

void CGenMips::addD3DXProperty(QtVariantProperty* property, const QString& id) {
    propertyToId[property] = id;
    idToProperty[id]       = property;
}


// Info how to evaluate class meta object props
//     QObject* wid; <= set this to equal a class var pointer!
//     const QMetaObject* metaobject = wid->metaObject();
//     int                count      = metaobject->propertyCount();
//     for (int i = 0; i < count; ++i)
//     {
//         QMetaProperty metaproperty = metaobject->property(i);
//         const char*   name         = metaproperty.name();
//         QVariant      value        = wid->property(name);
//     }


void CGenMips::valueChanged(QtProperty* property, const QVariant& value) {
    if (!propertyToId.contains(property))
        return;
    QString id = propertyToId[property];

    if (id == QLatin1String(LOWEST_MIP_LEVELS)) {
        m_MipLevels = value.toInt();
    } else if (id == QLatin1String("FilterType")) {
        int filtertype = value.value<int>();
        switch (filtertype) {
        case 0:
            m_CFilterParams.nFilterType         = 0;
            m_CFilterParams.dwMipFilterOptions  = 0;
            m_GroupProperty->setEnabled(false);
            m_propertyGamma->setEnabled(true);
            break;
        default:
            m_GroupProperty->setEnabled(true);
            m_propertyGamma->setEnabled(false); // Not implemented in D3DX options
            m_CFilterParams.nFilterType = 1;    // Using D3DX options 
            int dxFilter                = CMP_D3DX_FILTER_POINT;  // Default

            // CMP_D3DX_FILTER_NONE is skipped as it cause the mip map image to be enlarged and offset!

            switch (filtertype) {
                case 1:
                    dxFilter = CMP_D3DX_FILTER_POINT;
                    break;
                case 2:
                    dxFilter = CMP_D3DX_FILTER_LINEAR;
                    break;
                case 3:
                    dxFilter = CMP_D3DX_FILTER_TRIANGLE;
                    break;
                case 4:
                    dxFilter = CMP_D3DX_FILTER_BOX;
                    break;
            }

            m_CFilterParams.dwMipFilterOptions =
                m_CFilterParams.dwMipFilterOptions & 0xFFFFFFE0 | dxFilter;
            break;
        }
    } else if (id == QLatin1String("Gamma")) {
        m_CFilterParams.fGammaCorrection = value.toDouble();
    } else if (id == QLatin1String("Dither")) {
        m_CFilterParams.dwMipFilterOptions ^= CMP_D3DX_FILTER_DITHER;
    } else if (id == QLatin1String("MirrorPixels")) {
        m_CFilterParams.dwMipFilterOptions ^= CMP_D3DX_FILTER_MIRROR;
    }
    // Enable when support for sRGB mipmaps is enabled
    // if (id == QLatin1String("PerformFiltering"))
    // {
    // }
}

void CGenMips::SetGUIItems() {
    // Init
    updateExpandState();

    QMap<QtProperty*, QString>::ConstIterator itProp = propertyToId.constBegin();
    while (itProp != propertyToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    propertyToId.clear();
    idToProperty.clear();

    // Data
    m_propertyMipLevels = m_variantPropertyManager->addProperty(QtVariantPropertyManager::enumTypeId(), LOWEST_MIP_LEVELS);
    m_MipLevelSizes << "";  // This will be populated by setMipLevelDisplay()
    m_propertyMipLevels->setAttribute("enumNames", m_MipLevelSizes);
    m_propertyMipLevels->setValue(0);
    addProperty(m_propertyMipLevels, QLatin1String(LOWEST_MIP_LEVELS));

    m_propertyGamma = m_variantPropertyManager->addProperty(QVariant::Double, tr("Gamma (Pixel ^ Value)"));
    m_propertyGamma->setValue(1.0);
    m_propertyGamma->setAttribute("minimum", 0.001);
    m_propertyGamma->setAttribute("maximum", 4.00);
    m_propertyGamma->setAttribute("singleStep", 0.001);
    m_propertyGamma->setAttribute("decimals", 3);
    addProperty(m_propertyGamma, QLatin1String("Gamma"));


    m_propertyFilterType = m_variantPropertyManager->addProperty(QtVariantPropertyManager::enumTypeId(), "Filter Type");

    // Note: The enum for this should bt a struct type with proper settings to attribtre for D3DX and other options
    QStringList types;
    types << "CMP  Box"
          << "D3DX Point"
          << "D3DX Linear"
          << "D3DX Triangle"
          << "D3DX Box";

    m_propertyFilterType->setAttribute("enumNames", types);
    m_propertyFilterType->setValue(0);  // "CMP_Box"
    addProperty(m_propertyFilterType, QLatin1String("FilterType"));

    QLabel* LSetting = new QLabel(tr("Setting"));
    LSetting->setStyleSheet("QLabel { background-color : rgb(200, 200, 200); color : Black; font: bold;}");

    m_GroupProperty = m_variantPropertyManager->addProperty(QtVariantPropertyManager::groupTypeId(), tr("D3DX Options"));
    m_GroupProperty->setEnabled(false);

    m_propertyDither = m_variantPropertyManager->addProperty(QVariant::Bool, tr("Dither"));
    m_GroupProperty->addSubProperty(m_propertyDither);
    addD3DXProperty(m_propertyDither, QLatin1String("Dither"));

    m_propertyMirrorPixels = m_variantPropertyManager->addProperty(QVariant::Bool, tr("MirrorPixels"));
    m_GroupProperty->addSubProperty(m_propertyMirrorPixels);
    addD3DXProperty(m_propertyDither, QLatin1String("MirrorPixels"));

    // m_propertyPerformFiltering = m_variantPropertyManager->addProperty(QVariant::Bool, tr("PerformFiltering"));
    // m_propertyPerformFiltering->setEnabled(false);
    // m_GroupProperty->addSubProperty(m_propertyPerformFiltering);

    addProperty(m_GroupProperty, QLatin1String("Options"));


    QScrollArea* GenMipsOptions = new QScrollArea();
    GenMipsOptions->setWidgetResizable(true);
    GenMipsOptions->setMinimumSize(300, 60);
    m_propertyEditor->setMinimumSize(250, 55);
    GenMipsOptions->setWidget(m_propertyEditor);

    // Buttons
    QHBoxLayout* layoutButtons = new QHBoxLayout;
    QPushButton* PBGenerate    = new QPushButton("Generate");
    layoutButtons->addWidget(PBGenerate);
    QPushButton* buttons3 = new QPushButton("Cancel");
    layoutButtons->addWidget(buttons3);

    // Button Events
    connect(buttons3, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(PBGenerate, SIGNAL(clicked()), this, SLOT(onGenerate()));

    QGridLayout* layout = new QGridLayout(this);

    layout->addWidget(GenMipsOptions, 0, 0);
    layout->addLayout(layoutButtons, 1, 0);

    layout->setAlignment(Qt::AlignTop);

    this->setLayout(layout);
}