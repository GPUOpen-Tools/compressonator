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

#ifndef _SETCOMPRESSOPTIONS_H
#define _SETCOMPRESSOPTIONS_H

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include "objectcontroller.h"
#include <QtWidgets>
#include "pluginmanager.h"
#include "compressonator.h"
#include "common.h"
#include "cpprojectdata.h"

class CSetCompressOptions : public QDialog
{
    Q_OBJECT

public:
    CSetCompressOptions(const QString title, QWidget* parent);
    ~CSetCompressOptions();

#ifdef USE_TREEVIEW_PROP
    QTreeWidgetItem* AddRootItem(int col, QString itemtext, bool checkable);
    QTreeWidgetItem* AddChildItem(QTreeWidgetItem* parent, int col, QString itemtext, bool checkable);
#endif

    QTreeWidgetItem*      m_item;             // Ref to a project's item when the dialog was called: Must be set if Saving the data int a project tree
    C_Destination_Options m_DestinationData;  // Our local data settings: copy of original or default
    C_Destination_Options m_dataOriginal;     // Original Data prior to Edit

    bool updateDisplayContent();                 // Update data to all widgets and vaildate compressable image format support
    bool updateFileFormat(QFileInfo& fileinfo);  // Update the Image Type Combo box by matching the input image format : return false if imput is not supported
    void resetData();                            // Reset all data back to defaults
    void SaveCompressedInfo();                   // Save compress setting
    void setMinMaxStep(QtVariantPropertyManager* manager, QtProperty* m_prop, double min, double max, double step, int decimals);
    QString GetFormatString();  // Reads the m_data format and returns format enum as a string

    bool m_isEditing;            // True when dialog is shown and in edit mode
    bool m_automaticProcessing;  // Set when the user automatically processes each file without manually setting each destination file
    bool m_isInit;
    int  m_extnum;

    bool m_showDestinationEXTSetting;  // Dropdown list for DDS, KTX, ...
    bool m_showTheControllerSetting;   // Property page for BCn, ETC1 ...
    bool m_showTheInfoTextSetting;

    QString       m_destFilePath;
    QTextBrowser* m_infotext;
    QLineEdit*    m_DestinationFolder;
    QPushButton*  m_PBDestFileFolder;
    QLineEdit*    m_LEName;
    QLineEdit*    m_LESourceFile;
    QComboBox*    m_CBSourceFile;
    QGroupBox*    GBDestinationFile;

private:
    // Common for all
    QHBoxLayout* m_HlayoutName;
    QHBoxLayout* m_HlayoutDestination;
    QHBoxLayout* m_HlayoutButtons;
    QVBoxLayout* m_VlayoutWindow;
    QVBoxLayout* m_VlayoutDestination;
    QVBoxLayout* m_VlayoutSource;
    QHBoxLayout* m_HlayoutSourceName;

    QWidget*      m_newWidget;
    const QString m_title;
    QWidget*      m_parent;
    QPushButton*  m_PBSaveSettings;
    QPushButton*  m_PBCompress;
    QPushButton*  m_PBCancel;

    QLineEdit*  m_DestinationFile;
    QComboBox*  m_CBCompression;
    QComboBox*  m_fileFormats;
    QStringList m_AllFileTypes;

    ObjectController* m_theController;
    ObjectController* m_theControllerOptions;

    QTreeWidget* m_trewwview;
    bool         changeSelf;

    // Options that can change during editing
#ifdef USE_MESHOPTIMIZER
    QtProperty* m_propMeshOptimizerSettings;
#else
    QtProperty* m_propMeshSettings;
#endif
    QtProperty* m_propMeshCompressionSettings;
    QtProperty* m_propFormat;
    QtProperty* m_propQuality;
#ifdef USE_ENABLEHQ
    QtProperty* m_propEnableHQ;
#endif
    QtProperty* m_propChannelWeightingR;
    QtProperty* m_propChannelWeightingG;
    QtProperty* m_propChannelWeightingB;
    QtProperty* m_propAlphaThreshold;
    QtProperty* m_propAdaptiveColor;
    QtProperty* m_propBitrate;

    // Options for input HDR image properties
    QtProperty* m_propDefog;
    QtProperty* m_propExposure;
    QtProperty* m_propKneeLow;
    QtProperty* m_propKneeHigh;
    QtProperty* m_propGamma;

    // Property class that changed based on compression format
    QtProperty* m_propDestImage;
    QtProperty* m_propChannelWeight;
    QtProperty* m_propDXT1Alpha;
    QtProperty* m_propCodecBlockRate;
    QtProperty* m_propHDRProperties;
    QtProperty* m_propRefine;

    // some helper functions
    QString GenerateDefaultDestName();
    void    ReduceNumChildrenIndex();

    // overriding the function for handling the X button being pressed, so that m_extnum can be properly updated
    void closeEvent(QCloseEvent* e) override;

signals:
    void SaveCompressSettings(QTreeWidgetItem* m_item, C_Destination_Options& m_data);

public Q_SLOTS:

    void compressionValueChanged(QVariant& value);
    void redwValueChanged(QVariant& value);
    void greenwValueChanged(QVariant& value);
    void bluewValueChanged(QVariant& value);
    void thresholdValueChanged(QVariant& value);
    void PBSaveCompressSetting();
    void onDestFileFolder();
    void onPBCancel();
    void onDestinationFileEditingFinished();
    void onNameTextChanged(QString text);
    void onNameEditingFinished();

    void qualityValueChanged(QVariant& value);
    void bitrateValueChanged(QString& actualbitrate, int& xblock, int& yblock);
    void defogValueChanged(double& defog);
    void exposureValueChanged(double& exposure);
    void kneelowValueChanged(double& kneelow);
    void kneehighValueChanged(double& kneehigh);
    void gammaValueChanged(double& gamma);
    void oncurrentItemChanged(QtBrowserItem*);
    void onSourceNameSelectionChanged(int index);
};

extern PluginManager         g_pluginManager;
extern C_Application_Options g_Application_Options;

#endif