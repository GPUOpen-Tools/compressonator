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
//=====================================================================

#ifndef _SETCOMPRESSOPTIONS_H
#define _SETCOMPRESSOPTIONS_H

#include "qtpropertymanager.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include "objectcontroller.h"
#include <QtWidgets>
#include "PluginManager.h"
#include "Compressonator.h"
#include "cpProjectData.h"



class CSetCompressOptions : public QDialog
{
    Q_OBJECT

public:
    CSetCompressOptions(const QString title, QWidget *parent);
    ~CSetCompressOptions();

#ifdef USE_TREEVIEW_PROP
    QTreeWidgetItem* AddRootItem(int col, QString itemtext, bool checkable);
    QTreeWidgetItem* AddChildItem(QTreeWidgetItem *parent, int col, QString itemtext, bool checkable);
#endif

    QTreeWidgetItem             *m_item;                    // Ref to a projects item when the dialog was called: Must be set if Saving the data int a project tree
	C_Destination_Options           m_data;                    // Our loacal data settings: copy of orignal or default
	C_Destination_Options           m_data2;                    // Our loacal data settings: copy of orignal or default
	C_Destination_Options           m_dataOriginal;            // Original Data prior to Edit

    bool updateDisplayContent();                            // Update data to all widgets and vaildate compressable image format support
    bool updateFileFormat(QFileInfo &fileinfo);             // Update the Image Type Combo box by matching the input image format : return false if imput is not supported
    void resetData();                                       // Reset all data back to defaults

	bool isEditing;											// True when dislog is shown and in edit mode
	QTextBrowser	*m_infotext;

private:
    // Common for all
    QHBoxLayout                 *m_HlayoutName;
    QHBoxLayout                 *m_HlayoutDestination;
    QHBoxLayout                 *m_HlayoutButtons;
    QVBoxLayout                 *m_VlayoutWindow;
    QVBoxLayout                 *m_VlayoutDestination;

    QWidget                     *m_newWidget;
    const QString                m_title;
    QWidget                     *m_parent;
    QPushButton                 *m_PBSaveSettings;
    QPushButton                 *m_PBCompress;
    QPushButton                 *m_PBCancel;
    QPushButton                 *m_PBDestFileFolder;
    QLineEdit                   *m_DestinationFile;
    QLineEdit                   *m_LEName;
    QLineEdit                   *m_DestinationFolder;
    QComboBox                   *m_CBCompression;
    QComboBox                   *m_fileFormats;
    QStringList                 m_AllFileTypes;

    ObjectController            *m_theController;
    ObjectController            *m_theControllerOptions;

    QTreeWidget                 *m_trewwview;
    bool                        changeSelf;

    // Options that can change during editing
    QtProperty                  *m_propQuality;
	QtProperty					*m_propFormat;
    QtProperty                  *m_propDestImage;

signals:
	void SaveCompressSettings(QTreeWidgetItem *m_item, C_Destination_Options &m_data);

public Q_SLOTS:

    void    compressionValueChanged(QVariant &value);
    void    PBSaveCompressSetting();
    void    onDestFileFolder();
    void    onPBCancel();
    void    onDestinationFileEditingFinished();
    void    onNameTextChanged(QString text);
    void    onNameEditingFinished();

	void	qualityValueChanged(QVariant &value);
	void	oncurrentItemChanged(QtBrowserItem *);


};


extern PluginManager g_pluginManager;

#endif