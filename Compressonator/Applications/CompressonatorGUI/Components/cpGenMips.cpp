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

#include "cpGenMips.h"

#define LOWEST_MIP_LEVELS "Lowest Mip-Level (Width x Height)"

CGenMips::CGenMips(const QString title, QWidget *parent) : QWidget(parent),
m_title(title),
m_parent(parent)
{
    setWindowTitle(title);
    setWindowFlags(Qt::Dialog);

    Qt::WindowFlags flags = windowFlags();
    Qt::WindowFlags helpFlag =  Qt::WindowContextHelpButtonHint;
    flags = flags & (~helpFlag);
    setWindowFlags(flags | Qt::WindowStaysOnTopHint | Qt::MSWindowsFixedSizeDialogHint);

    resize(300, 80);
    
    m_cdata = new CData();
    m_cdata->set_str("");

    evalProperties();
    variantManager = new QtVariantPropertyManager(this);
    connect(variantManager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),this, SLOT(valueChanged(QtProperty *, const QVariant &)));
    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory(this);
    
    m_propertyEditor = new QtGroupBoxPropertyBrowser(this);
    // m_propertyEditor = new QtTreePropertyBrowser(this); 
    m_propertyEditor->setFactoryForManager(variantManager, variantFactory);
    
    SetDefaults();

    m_ImageSize_W = 0;
    m_ImageSize_H = 0;
    m_MipLevels   = 0;


}


void CGenMips::setMipLevelDisplay(int Width, int Height)
{
    m_ImageSize_W = Width;
    m_ImageSize_H = Height;
    m_MipLevelSizes.clear();

    m_MipLevels = 0;

    QString level;
    level.append(QString::number(Width));
    level.append("x");
    level.append(QString::number(Height));

    m_MipLevelSizes << level;

    m_MipLevels++;
    do
    {
        Width = (Width>1) ? (Width >> 1) : 1;
        Height = (Height>1) ? (Height >> 1) : 1;

        QString level;
        level.append(QString::number(Width));
        level.append("x");
        level.append(QString::number(Height));

        m_MipLevelSizes << level;

        m_MipLevels++;
    } while (Width > 1 && Height > 1);

    QStringList reversedList = m_MipLevelSizes;
    const int levelSize = m_MipLevelSizes.size();
    const int maxSwap = m_MipLevelSizes.size() / 2;
    for (int i = 0; i < maxSwap; ++i) {
        qSwap(reversedList[i], reversedList[levelSize - 1 - i]);
    }
    m_MipLevelSizes = reversedList;

    m_property->setAttribute("enumNames", m_MipLevelSizes);
    m_property->setValue(0);
}


void CGenMips::onGenerate()
{
    int minsize = m_ImageSize_H;
    if (m_ImageSize_W > m_ImageSize_H)
        minsize = m_ImageSize_W;
    int temp= m_MipLevelSizes.count() - m_MipLevels;
    m_MipLevels = temp - 1;
    minsize = minsize >> m_MipLevels;
    emit generateMIPMap(minsize, this->m_mipsitem);
    hide();
}


CGenMips::~CGenMips()
{
    if (m_cdata)
    {
        delete m_cdata;
        m_cdata = NULL;
    }
}


void CGenMips::closeEvent(QCloseEvent * event)
{
    hide();
    event->ignore();
}

void CGenMips::onCancel()
{
    hide();
}


void CGenMips::updateExpandState()
{
    QList<QtBrowserItem *> list = m_propertyEditor->topLevelItems();
    QListIterator<QtBrowserItem *> it(list);
    while (it.hasNext()) {
        QtBrowserItem *item = it.next();
        QtProperty *prop = item->property();
        idToExpanded[propertyToId[prop]] = false; //  propertyEditor->isExpanded(item);
    }
}

void CGenMips::addProperty(QtVariantProperty *property, const QString &id)
{
    propertyToId[property] = id;
    idToProperty[id] = property;
    m_propertyEditor->addProperty(property);
    // QtBrowserItem *item = propertyEditor->addProperty(property);
    // if (idToExpanded.contains(id))
    //    propertyEditor->setExpanded(item, idToExpanded[id]);
}

void CGenMips::evalProperties()
{
    QObject *wid = m_cdata;
    const QMetaObject *metaobject = wid->metaObject();
    int count = metaobject->propertyCount();
    for (int i = 0; i<count; ++i) {
        QMetaProperty metaproperty = metaobject->property(i);
        const char *name = metaproperty.name();
        QVariant value = wid->property(name);
    }
}

void CGenMips::valueChanged(QtProperty *property, const QVariant &value)
{
    if (!propertyToId.contains(property))
        return;
    QString id = propertyToId[property];
    
    if (id == QLatin1String(LOWEST_MIP_LEVELS))
    {
        m_MipLevels = value.toInt();
    }

    if (m_cdata == NULL) return;

    if (id == QLatin1String("m_str")) {
        m_cdata->set_str(value.value<QString>());
    }
    else
    if (id == QLatin1String("Dither")) {
        m_cdata->set_bDither(value.value<bool>());
    }
    else
    if (id == QLatin1String("MirrorPixels")) {
        m_cdata->set_bDither(value.value<bool>());
    }
    if (id == QLatin1String("PerformFiltering")) {
        m_cdata->set_bDither(value.value<bool>());
    }

}


void CGenMips::SetDefaults()
{
    // Init
    updateExpandState();

    QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
    while (itProp != propertyToId.constEnd()) {
        delete itProp.key();
        itProp++;
    }
    propertyToId.clear();
    idToProperty.clear();

    
#ifdef USE_MULIPLE_FILTERS
    // Methods
    // Notes: LMethod->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QGroupBox *GBCompressionMethod = new QGroupBox();
    QGridLayout *LayoutCompressionMethod = new QGridLayout(GBCompressionMethod);

    QLabel *LCompression = new QLabel(tr("Using Box Filter"));
    
    QComboBox *CBCompression = new QComboBox();
    BOOL        IsEnabled = false;

    // Supported Objects
    CBCompression->addItem(tr("Box"));
    CBCompression->addItem(tr("D3D"));
    CBCompression->addItem(tr("D3DX"));

    // Read the class type and set the index and what to enable here
    CBCompression->setCurrentIndex(0);   // Defaulting to Box
    LayoutCompressionMethod->addWidget(CBCompression, 0, 1);
    LayoutCompressionMethod->addWidget(LCompression, 0, 0);

#endif    

    
    // Data 
    //https://doc.qt.io/archives/qq/qq18-propertybrowser.html

    m_property = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), LOWEST_MIP_LEVELS);
    m_MipLevelSizes << "";
    m_property->setAttribute("enumNames", m_MipLevelSizes);
    m_property->setValue(0);
    addProperty(m_property, QLatin1String(LOWEST_MIP_LEVELS));

#ifdef USE_MULIPLE_FILTERS

    property = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(), "Filter Type");
    QStringList  types;
    types << "None" << "Point" << "Linear" << "Triangle" << "Box";
    property->setAttribute("enumNames", types);
    property->setValue(0); // "None"
    property->setEnabled(IsEnabled);
    addProperty(property, QLatin1String("FilterType"));

    QLabel *LSetting = new QLabel(tr("Setting"));
    LSetting->setStyleSheet("QLabel { background-color : rgb(200, 200, 200); color : Black; font: bold;}");

    GroupProperty = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(), tr("Options"));

   // QObject *wid = m_cdata;
   // const QMetaObject *metaobject = wid->metaObject();
   // int count = metaobject->propertyCount();
   // for (int i = 0; i<count; ++i) {
   //     QMetaProperty metaproperty = metaobject->property(i);
   //     const char *name = metaproperty.name();
   //     property = variantManager->addProperty(QVariant::Bool, name);
   //     QVariant value = wid->property(name);
   //     property->setValue(value);
   //     GroupProperty->addSubProperty(property);
   // }

    property = variantManager->addProperty(QVariant::Bool, tr("Dither"));
    property->setValue(m_cdata->get_bDither());
    //property->setEnabled(IsEnabled);
    GroupProperty->addSubProperty(property);

    property = variantManager->addProperty(QVariant::Bool, tr("MirrorPixels"));
    property->setValue(m_cdata->get_bMirrorPixels());
    //property->setEnabled(IsEnabled);
    GroupProperty->addSubProperty(property);

    property = variantManager->addProperty(QVariant::Bool, tr("PerformFiltering"));
    property->setValue(m_cdata->get_bPerformFiltering());
    //property->setEnabled(IsEnabled);
    GroupProperty->addSubProperty(property);

    GroupProperty->setEnabled(IsEnabled);
    addProperty(GroupProperty, QLatin1String("Options"));
#endif

    QScrollArea *GenMipsOptions = new QScrollArea();
    GenMipsOptions->setWidgetResizable(true);
    GenMipsOptions->setMinimumSize(300,60);
    m_propertyEditor->setMinimumSize(250, 55);
    GenMipsOptions->setWidget(m_propertyEditor);


    // Buttons
    //QGroupBox *GBhorizontalButtons = new QGroupBox();
    QHBoxLayout *layoutButtons = new QHBoxLayout;
    QPushButton *PBGenerate = new QPushButton("Generate");
    layoutButtons->addWidget(PBGenerate);
    QPushButton *buttons3 = new QPushButton("Cancel");
    layoutButtons->addWidget(buttons3);
    //GBhorizontalButtons->setLayout(layoutButtons);

    // Button Events
    connect(buttons3, SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(PBGenerate, SIGNAL(clicked()), this, SLOT(onGenerate()));

    QGridLayout *layout = new QGridLayout(this); 

#ifdef USE_MULIPLE_FILTERS
    layout->addWidget(GBCompressionMethod, 0, 0);
    layout->addWidget(LSetting, 1, 0);
#endif

    layout->addWidget(GenMipsOptions, 0, 0);
    layout->addLayout(layoutButtons, 1, 0);

    layout->setAlignment(Qt::AlignTop);

    this->setLayout(layout);

}