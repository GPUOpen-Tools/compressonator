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
//=====================================================================

#ifndef _FILEINFO_H
#define _FILEINFO_H

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "objectcontroller.h"
#include "cpprojectdata.h"
#include "accustomdockwidget.h"

class CImagePropertyView : public QDockWidget
// public acCustomDockWidget
{
    Q_OBJECT

public:
    CImagePropertyView(const QString title, QWidget* parent);
    ~CImagePropertyView();

    void refreshView();
    void SetDefaults();
    void setMinMaxStep(QtVariantPropertyManager* manager, QtProperty* m_prop, double min, double max, double step, int decimals);
    void evalProperties();

private:
    ObjectController* m_theController = NULL;
    QString           m_currentClassName;  // Current Name of object been Displayed

    bool m_isEditing_Compress_Options;  // Flag to indicate we are editing m_C_Destination_Options
    // and Apply Cancel Buttons maybe enabled
    // This flag is used only for one type of class and should be enum if more then
    // one is been used.

    QVBoxLayout* m_layoutV;
    QHBoxLayout* m_layoutHButtons;
    QPushButton* m_PBCancel;
    QPushButton* m_PBSave;
    QPushButton* m_PBCompress;

    QTextBrowser* m_infotext;

    void closeEvent(QCloseEvent* event);

public:
    C_Destination_Options* m_holddata              = NULL;  // Generic pointer to class objects received for display
    QObject*               m_data                  = NULL;  // pointer to class objects been displayed
    C_Destination_Options* m_C_Destination_Options = NULL;  // Temp hold of known objects that can be editied

    QtTreePropertyBrowser* m_browser = NULL;

    // Common for all
    QWidget* m_newWidget = NULL;
    QWidget* m_parent    = NULL;

    // Options that can change during editing of C_Destination_Options data type
    void        Init_C_Destiniation_Data_Controller();
    QtProperty* m_propQuality = NULL;
#ifdef USE_ENABLEHQ
    QtProperty* m_propEnableHQ = NULL;
#endif
    QtProperty* m_propFormat            = NULL;
    QtProperty* m_propChannelWeightingR = NULL;
    QtProperty* m_propChannelWeightingG = NULL;
    QtProperty* m_propChannelWeightingB = NULL;
    QtProperty* m_propAlphaThreshold    = NULL;
    QtProperty* m_propAdaptiveColor     = NULL;
    QtProperty* m_propBitrate           = NULL;
    QtProperty* m_propDefog             = NULL;
    QtProperty* m_propExposure          = NULL;
    QtProperty* m_propKneeLow           = NULL;
    QtProperty* m_propKneeHigh          = NULL;
    QtProperty* m_propGamma             = NULL;

    // Mesh Class info
#ifdef USE_MESHOPTIMIZER
    QtProperty* m_propMeshOptimizerSettings = NULL;
#else
    QtProperty* m_propMeshSettings = NULL;
#endif
    QtProperty* m_propMeshCompressionSettings = NULL;

    QtProperty* m_propWidth     = NULL;
    QtProperty* m_propHeight    = NULL;
    QtProperty* m_propCompRatio = NULL;
    QtProperty* m_propCompTime  = NULL;

    // Property class that changed based on compression format
    QtProperty* m_propChannelWeight  = NULL;
    QtProperty* m_propDXT1Alpha      = NULL;
    QtProperty* m_propCodecBlockRate = NULL;
    QtProperty* m_propHDRProperties  = NULL;
    QtProperty* m_propRefine         = NULL;

public slots:
    void onMesh_Optimization(QVariant& value);
    void onMesh_Compression(QVariant& value);
    void OnUpdateData(QObject* data);
    void onCancel();
    void onSave();
    void onCompress();
    void onCompressionStart();
    void onCompressionDone();
    void onCompressDataChanged();
    void compressionValueChanged(QVariant& value);
    void qualityValueChanged(QVariant& value);
    void redwValueChanged(QVariant& value);
    void greenwValueChanged(QVariant& value);
    void bluewValueChanged(QVariant& value);
    void thresholdValueChanged(QVariant& value);
    void bitrateValueChanged(QString& actualbitrate, int& xblock, int& yblock);
    void defogValueChanged(double& defog);
    void exposureValueChanged(double& exposure);
    void kneelowValueChanged(double& kneelow);
    void kneehighValueChanged(double& kneehigh);
    void gammaValueChanged(double& gamma);
    void oncurrentItemChanged(QtBrowserItem*);
    void onImageLoadStart();
    void onImageLoadDone();
    void onSourceImage(int childCount);

Q_SIGNALS:
    void saveSetting(QString* FilePathName);
    void compressImage(QString* FilePathName);
};

extern C_Application_Options g_Application_Options;
#endif