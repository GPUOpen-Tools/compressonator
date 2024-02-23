//=====================================================================
// Copyright 2020-2024 (c), Advanced Micro Devices, Inc. All rights reserved.
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

#ifndef _IMAGEVIEW_H
#define _IMAGEVIEW_H

#include <QtWidgets>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QIcon>
#include <QMap>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsSceneEvent>
#include <qtimer.h>

#include "cpimageview.h"
#include "acimageview.h"
#include "acexrtool.h"
#include "cpimageloader.h"
#include "accustomdockwidget.h"

#define TXT_IMAGE_PROCESSED "Processed"
#define TXT_IMAGE_ORIGINAL "Original"
#define TXT_IMAGE_DIFF "Diff"

enum eImageViewState
{
    isProcessed = 0,  // we are viewing the processed image (child of an original image at root of project explorer)
    isOriginal,       // Original image as seen at a root node of project explorer
    isDiff            // Viewing a image differance from (origin - processed)
};

typedef struct
{
    bool            onBrightness  = false;
    bool            reloadImage   = false;
    bool            generateDiff  = false;
    bool            generateMips  = false;
    float           fDiffContrast = eImageViewState::isOriginal;
    eImageViewState input_image;
} Setting;

class PushButtonAction : public QWidgetAction
{
public:
    explicit PushButtonAction(const QIcon& icon, const QString& text, QObject* parent = 0)
        : QWidgetAction(parent)
    {
        setIcon(icon);
        setObjectName(text);
    }

protected:
    virtual QWidget* createWidget(QWidget* parent)
    {
        return new QPushButton(icon(), objectName(), parent);
    }

    virtual void deleteWidget(QWidget* widget)
    {
        delete widget;
        widget = NULL;
    }
};

class cpImageView : public acCustomDockWidget
{
    Q_OBJECT
public:
    cpImageView(const QString filePathName,
                const QString Title,
                QWidget*      parent,
                CMipImages*   MipImages,
                Setting*      setting,
                CMipImages*   OriginalMipImages = NULL);
    ~cpImageView();

    void InitData();

    void showToobar(bool show);
    void showToobarButton(bool show);

    void EnableMipLevelDisplay(int level);
    void EnableDepthLevelDisplay(int level);

    int     activeview = 0;
    bool    m_CompressedMipImages;
    QPointF m_localPos;

    Setting m_setting;  // Local copy of settings

    CImageLoader* m_imageLoader;
    acImageView*  m_acImageView;
    QMenu*        m_viewContextMenu;
    CMipImages*   m_processedMipImages;
    CMipImages*   m_OriginalMipImages;  // Read only pointer to original images
    acEXRTool*    m_ExrProperties;      // HDR image properties window

    QAction* actSaveView;
    QAction* actSaveBlockView;

    QSize m_imageSize;
    int   ID;
    int   XBlockNum;
    int   YBlockNum;
    int   m_source_BlockXPos;
    int   m_source_BlockYPos;

    bool m_localMipImages;
    bool m_bOnacScaleChange;
    bool m_useOriginalImageCursor;
    bool m_DiffOnOff;

protected:
    void keyPressEvent(QKeyEvent* event);

public slots:
    void OnToolBarClicked();  // Hook into the CustomeWidgets TitleBars On Tool Button Clicked events
    void oncpImageViewMousePosition(QPointF* scenePos, QPointF* localPos, int onID);
    void oncpImageViewVirtualMousePosition(QPointF* scenePos, QPointF* localPos, int onID);
    void onDecompressUsing(int useDecomp);
    void oncpResetImageView();
    void onZoomLevelChanged(int value);
    void onacScaleChange(int value);
    void onResetHDRandDiff(int value);
    void onacPSNRUpdated(double value);
    void onToolListChanged(int index);
    void onViewCustomContextMenu(const QPoint& point);
    void onSaveViewAs();
    void onSaveBlockView();
    void onBrightnessLevelChanged(int value);
    void onToggleViewChanged(int view);

private:
    void showEvent(QShowEvent*);
    void paintEvent(QPaintEvent* event);
    void setActionForImageViewStateChange();
    void closeEvent(QCloseEvent* event);
    void getSupportedImageFormats();
    void GetSourceBlock(int BlockX, int BlockY, std::string filename);

    eImageViewState m_ImageViewState;
    // Common for all
    QWidget*     m_newWidget;
    QGridLayout* m_layout;
    QWidget*     m_parent;
    QToolButton* m_button2;
    QToolButton* m_button;
    QToolBar*    m_toolBar;
    QStatusBar*  m_statusBar;
    QPushButton* m_buttonNavigate;

    QLabel*   m_labelColorTxt;
    QLabel*   m_labelTxtView;
    QLabel*   m_labelColorRGBA;
    QLabel*   m_labelPos;
    QLabel*   m_labelBlockPos;
    QWidget*  m_pMyWidget;
    QSpinBox* m_ZoomLevel;
    QSpinBox* m_BrightnessLevel;
    QLabel*   m_PSNRLabel;

    QStyle* Plastique_style;  // Combobox Style

    QPixmap* m_pixmap;
    QString  m_QtImageFilter;

    int  m_MipLevels;
    int  m_MaxDepthLevel;  // 1..CMP_MIPSET_MAX_DEPTHS
    int  m_DepthLevel;     // depthsupport
    bool m_FitOnShow;      // Flaged used to to indicate a fit image into view when widget is shown

    // Image View Actions
    QAction* imageview_ResetImageView;
    QAction* imageview_ToggleChannelR;
    QAction* imageview_ToggleChannelG;
    QAction* imageview_ToggleChannelB;
    QAction* imageview_ToggleChannelA;
    QAction* imageview_ToggleGrayScale;
    QAction* imageview_InvertImage;
    QAction* imageview_ImageBrightnessUp;
    QAction* imageview_ImageBrightnessDown;
    QAction* imageview_MirrorHorizontal;
    QAction* imageview_MirrorVirtical;
    QAction* imageview_RotateRight;
    QAction* imageview_RotateLeft;
    QAction* imageview_ZoomIn;
    QAction* imageview_ZoomOut;
    QAction* imageview_ViewImageOriginalSize;
    QAction* imageview_FitInWindow;
    QAction* imageview_ImageDiff;
#ifdef _DEBUG
    QComboBox* m_CBimageview_Debug;
#endif
    QComboBox* m_CBimageview_Toggle;
    QComboBox* m_CBimageview_GridBackground;
    QComboBox* m_CBimageview_ToolList;
    QComboBox* m_CBimageview_MipLevel;
    QComboBox* m_CBimageview_DepthLevel;
    QComboBox* m_CBimage_DecompressUsing;

    acVirtualMouseHub m_customMouse;

    CMIPS* m_CMips;

Q_SIGNALS:
    void UpdateData(QObject* data);
    void OnSetScale(int value);

public slots:
    void onImageDiff();
};

#endif  // _IMAGEVIEW_H