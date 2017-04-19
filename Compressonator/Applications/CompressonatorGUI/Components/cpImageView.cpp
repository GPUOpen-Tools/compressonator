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

#include "cpImageView.h"
#include "acImageView.h"


void cpImageView::oncpImageViewVirtualMousePosition(QPointF *scenePos, QPointF *localPos, int onID)
{
    oncpImageViewMousePosition(scenePos, localPos, onID);
}

void cpImageView::showToobarButton(bool show)
{
    if (custTitleBar)
        custTitleBar->setButtonToolBarShow(show);
}

void cpImageView::oncpImageViewMousePosition(QPointF *scenePos, QPointF *localPos, int onID)
{
    Q_UNUSED(scenePos);
    Q_UNUSED(onID);

    // is mouse inside image view
    if (localPos) 
    {
        // Rounds up pixel co-ordinates
        // if in debugMode: blocks at 0,0 will start at 1,1
        int x = qRound(localPos->rx() + 0.5);
        int y = qRound(localPos->ry() + 0.5);
        if (x < 0) x = 0;
        if (y < 0) y = 0;

        QRgb localRgb;

        // if using original image for cursor event, comment out the following line
        if(m_useOriginalImageCursor)
            localRgb = m_acImageView->m_MipImages->Image_list.first()->pixel(localPos->rx(), localPos->ry());

        // else, using 3 lines below for imageview cursor line. 
        else
        {
            QPixmap pixmap = m_acImageView->m_imageItem->pixmap();
            QImage img = pixmap.toImage();
            localRgb = img.pixel(localPos->rx(), localPos->ry());
        }

        QColor color(localRgb);
        QString Txt2;

#ifdef _DEBUG
        if (m_acImageView->m_debugMode)
        {
            m_labelColorTxt->setText("");
            XBlockNum = x;
            YBlockNum = y;
            Txt2 = QString::number(x) + "," +
                QString::number(y) + " Block";
        }
        else
#endif
        {
            QString Txt;
            Txt = " RGBA " +
                QString::number(color.red()) + "," +
                QString::number(color.green()) + ',' +
                QString::number(color.blue()) + "," +
                QString::number(color.alpha());

            m_labelColorTxt->setText(Txt);

            Txt2 = QString::number(x) + "," +
                QString::number(y) + " px";
        }

        m_labelPos->setText(Txt2);

        QPalette palette;
        palette.setColor(QPalette::Background, color);
        palette.setColor(QPalette::WindowText, color);
        m_labelColorRGBA->setPalette(palette);
    }
    else
    {
       m_labelPos->setText("");
       m_labelColorTxt->setText("");
       QPalette palette;
       palette.setColor(QPalette::Background, Qt::black);
       palette.setColor(QPalette::WindowText, Qt::black);
       m_labelColorRGBA->setPalette(palette);
    }
}

void cpImageView::keyPressEvent(QKeyEvent *event)
{
    if (m_acImageView == NULL) return;
    if (m_OriginalMipImages->mipset == NULL) return;

#ifdef _DEBUG
    if (event->key() == Qt::Key_Space)
    {
        if (m_acImageView->m_debugMode)
        {
            MipLevel   *mipLevel = m_CMips->GetMipLevel(m_OriginalMipImages->mipset, 0);
            
            if (!mipLevel) return;

            // Check the block size matches what we expect for  BC6H or BC7
            if (m_acImageView->m_graphicsScene->cursorBlockX != 4) return;
            if (m_acImageView->m_graphicsScene->cursorBlockY != 4) return;

            // Flag to indicate we have data to process
            bool hasData = false;

            // Calc data stride for each row index = width * num channels
            int  row_stride = m_OriginalMipImages->mipset->m_nWidth * 4;

            // (BC6 Block Width * BC6 Block Height * Num Channels) * (Image Width / BC6 Block Width)
            // (4*4*4) * (Width / 4) = 4*4*Width
            int  block_offset = 4 * 4 * m_OriginalMipImages->mipset->m_nWidth;

            // Get the block position we need to fill from
            // 16 = BC6 Block Width * Num Channels
            int start_index = (((XBlockNum - 1) * 16)) + ((YBlockNum - 1) * block_offset);
            int index;

            // napatel check format before getting data!
            // we have two sets Float16 (2 Bytes alligned) and Float32 (4 Bytes alligned)

            if (m_acImageView->m_debugFormat == "BC6H" || m_acImageView->m_debugFormat == "BC6H_SF")
            {
                // Encoder input to fill with data
                float  in[16][4];
                float  dec_out[16][4];
                BYTE   cmp_in[16];

                if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float16)
                {
                    // Get origin data pointer
                    WORD *data = mipLevel->m_pwData;
                    int  d = 0;
                    for (int row = 0; row < 4; row++)
                    {
                        index = (row * row_stride) + start_index;
                        for (int col = 0; col < 4; col++)
                        {
                            in[d][0] = data[index];
                            in[d][1] = data[index + 1];
                            in[d][2] = data[index + 2];
                            in[d][3] = data[index + 3];
                            d++;
                            index += 4;
                        }
                    }
                    hasData = true;
                }
                else
                if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float32)
                {
                    // Get origin data pointer
                    float *data = mipLevel->m_pfData;
                    int  d = 0;
                    for (int row = 0; row < 4; row++)
                    {
                        index = (row * row_stride) + start_index;
                        for (int col = 0; col < 4; col++)
                        {
                            in[d][0] = data[index];
                            in[d][1] = data[index + 1];
                            in[d][2] = data[index + 2];
                            in[d][3] = data[index + 3];
                            d++;
                            index += 4;
                        }
                    }
                    hasData = true;
                }
                else
                if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Compressed)
                {
                    // Get origin data pointer
                    BYTE *data = mipLevel->m_pbData;

                    // set the data offset to the compressed block row,col which is in 16 byte incriments
                    // Note : 16 bytes per block * (Width in pixels / width of block which is 4 ) = 4 * width
                    int  cmp_block_offset = 4 * m_OriginalMipImages->mipset->m_nWidth;
                    int  offset = (((XBlockNum - 1) * 16)) + ((YBlockNum - 1) * cmp_block_offset);
                    data = data + offset;
                    for (int d = 0; d < 16; d++)
                    {
                            cmp_in[d] = data[d];
                    }
                    hasData = true;
                }

                if (hasData)
                {
                    // Use SDK interface to Encode a Block for debugging with
                    BC6HBlockEncoder *blockEncode;
                    BYTE              output[16];
                    if (CMP_InitializeBCLibrary() == BC_ERROR_NONE)
                    {
                        CMP_BC6H_BLOCK_PARAMETERS  user_settings;
                        user_settings.fQuality = 1.0;
                        user_settings.bUsePatternRec = false;
                        if(m_acImageView->m_debugFormat == "BC6H")
                            user_settings.bIsSigned = false;
                        else
                            user_settings.bIsSigned = true;
                        user_settings.fExposure = 1.00;
                        if (CMP_CreateBC6HEncoder(user_settings, &blockEncode) == BC_ERROR_NONE)
                        {
                            if (m_OriginalMipImages->mipset->m_ChannelFormat != CF_Compressed)
                            {
                                CMP_EncodeBC6HBlock(blockEncode, in, output);
                                // Feed compressed output to be decoded into dec_out
                                CMP_DecodeBC6HBlock(output, dec_out);
                            }
                            else
                            {
                                CMP_DecodeBC6HBlock(cmp_in, dec_out);
                            }
                            CMP_DestroyBC6HEncoder(blockEncode);
                        }
                        CMP_ShutdownBCLibrary();
                    }
                }
            }
            else
            if (m_acImageView->m_debugFormat == "BC7")
            {
                // Encoder input to fill with data
                double  in[16][4];
                double  dec_out[16][4];


                // Get origin data pointer
                BYTE *data = mipLevel->m_pbData;
                int  d = 0;
                for (int row = 0; row < 4; row++)
                {
                    index = (row * row_stride) + start_index;
                    for (int col = 0; col < 4; col++)
                    {
                        in[d][0] = data[index];
                        in[d][1] = data[index + 1];
                        in[d][2] = data[index + 2];
                        in[d][3] = data[index + 3];
                        d++;
                        index += 4;
                    }
                }
                hasData = true;

                if (hasData)
                {
                    // Use SDK interface to Encode a Block for debugging with
                    BC7BlockEncoder *blockEncode;
                    BYTE              output[16];
                    if (CMP_InitializeBCLibrary() == BC_ERROR_NONE)
                    {
                        CMP_BC6H_BLOCK_PARAMETERS  user_settings;
                        user_settings.fQuality = 1.0;
                        user_settings.bUsePatternRec = false;
                        user_settings.bIsSigned = false;
                        user_settings.fExposure = 1.00;
                        if (CMP_CreateBC7Encoder(0.05,false,false, 0xCF, 1.0, &blockEncode) == BC_ERROR_NONE)
                        {
                            CMP_EncodeBC7Block(blockEncode, in, output);
                            // Feed compressed output to be decoded into dec_out
                            CMP_DecodeBC7Block(output, dec_out);
                            CMP_DestroyBC7Encoder(blockEncode);
                        }
                        CMP_ShutdownBCLibrary();
                    }
                }
            }
        }
        else
            return; 
    }
#endif

    return;
}

void cpImageView::EnableMipLevelDisplay(int level)
{
    if (level <= 1) return;

    for (int num = 0; num < level; num++)
    {
        m_CBimageview_MipLevel->addItem("MipLevel : " + QString::number(num));
    }
    m_CBimageview_MipLevel->setEnabled(true);
    m_MipLevels = level;
}

cpImageView::~cpImageView()
{
    if (m_localMipImages)
    {
        if (m_MipImages)
            m_imageLoader->clearMipImages(m_MipImages);
        delete m_imageLoader;
        m_imageLoader = NULL;
    }

    if (m_imageLoader)
    {
        delete m_imageLoader;
        m_imageLoader = NULL;
    }

    if (m_CMips)
    {
        delete m_CMips;
        m_CMips = NULL;
    }

    if (Plastique_style)
    {
        delete Plastique_style;
        Plastique_style = NULL;
    }
}


void cpImageView::InitData()
{
    m_imageSize = { 0,0 };
    ID = 0;
    m_localMipImages                        = false;
    m_originalImage                         = false;
    m_MipLevels                             = 0;
    m_FitOnShow                             = true;
    m_imageLoader                           = NULL;
    m_acImageView                           = NULL;
    m_MipImages                             = NULL;
    m_newWidget                             = NULL;
    m_layout                                = NULL;
    m_parent                                = NULL;
    m_button2                               = NULL;
    m_button                                = NULL;
    m_toolBar                               = NULL;
    m_statusBar                             = NULL;
    m_buttonNavigate                        = NULL;
    m_labelColorTxt                         = NULL;
    m_labelColorRGBA                        = NULL;
    m_labelPos                              = NULL;
    m_pMyWidget                             = NULL;
    m_pixmap                                = NULL;
    imageview_ResetImageView                = NULL;
    imageview_ToggleChannelR                = NULL;
    imageview_ToggleChannelG                = NULL;
    imageview_ToggleChannelB                = NULL;
    imageview_ToggleChannelA                = NULL;
    imageview_ToggleGrayScale               = NULL;
    imageview_InvertImage                   = NULL;
    imageview_ImageBrightnessUp             = NULL;
    imageview_ImageBrightnessDown           = NULL;
    imageview_MirrorHorizontal              = NULL;
    imageview_MirrorVirtical                = NULL;
    imageview_RotateRight                   = NULL;
    imageview_RotateLeft                    = NULL;
    imageview_ZoomIn                        = NULL;
    imageview_ZoomOut                       = NULL;
    imageview_ViewImageOriginalSize         = NULL;
    imageview_FitInWindow                   = NULL;
    m_CBimageview_GridBackground            = NULL;
    m_CBimageview_ToolList                  = NULL;
    m_CBimageview_MipLevel                  = NULL;
    m_CBimage_DecompressUsing               = NULL;
    m_ExrProperties                         = NULL;
    m_OriginalMipImages                     = NULL;
    m_bOnacScaleChange                      = false;
    m_useOriginalImageCursor                = false;
    XBlockNum                               = 1;
    YBlockNum                               = 1;
}


cpImageView::cpImageView(const QString filePathName, const QString Title, QWidget *parent, CMipImages *MipImages, Setting *setting, CMipImages *OriginalMipImages) : acCustomDockWidget(filePathName, parent)
{
    InitData();
    m_parent    = parent;
    m_fileName  = filePathName;
    m_localMipImages = false;           // Flags if we used our own MipImage and not from parameter
    m_CBimageview_MipLevel = NULL;
    m_CMips = new CMIPS();
    Plastique_style = QStyleFactory::create("Plastique");
    
    if (MipImages)
    {
        if (setting->reloadImage && !setting->generateDiff && !setting->generateMips)
        {
            m_imageLoader = new CImageLoader();
            if (m_imageLoader)
            {
                m_MipImages = m_imageLoader->LoadPluginImage(filePathName);
            }
        }
        else
            m_MipImages = MipImages;
    }
    else
    {
        m_imageLoader = new CImageLoader();
        if (m_imageLoader)
        {
            m_MipImages = m_imageLoader->LoadPluginImage(filePathName);
            m_localMipImages = true;
        }
        else
            m_MipImages = NULL;
    }

    // if our current image is compressed, Are we suppiled with a pointer to its uncomprssed source image miplevels
    // else we just use the current miplevels, this pointer can be either compressed or uncompressed data
    // check its MIPIMAGE_FORMAT property to determine which one it is 

    if (OriginalMipImages)
    {
        m_OriginalMipImages = OriginalMipImages;
    }
    else
    {
        m_OriginalMipImages = m_MipImages;
    }

    QFile f(filePathName);
    QFileInfo fileInfo(f.fileName());
    m_tabName = fileInfo.fileName();

    setWindowTitle(m_tabName);

    this->m_CustomTitle = Title;

    if (Title.contains("Original"))
                m_originalImage = true;
    else
                m_originalImage = false;

    custTitleBar->setTitle(m_CustomTitle);
    custTitleBar->setToolTip(filePathName);

    //===============
    // Center Widget 
    //===============
    m_newWidget = new QWidget(parent);
    if (!m_newWidget)
    {
        // ToDo::Need to process error!
        return;
    }

    //===================
    // Get MipLevels
    //===================
    if (m_MipImages)
    {
        if (m_MipImages->mipset)
        {
            m_MipLevels = m_MipImages->mipset->m_nMipLevels;
            // check levels with number of images to view
            //if (m_MipImages->m_MipImageFormat == MIPIMAGE_FORMAT::Format_QImage)
            //{
                int count = m_MipImages->Image_list.count();
                if (count <= 1)
                    m_MipLevels = 0;
            //}
        }

    }
    else
        m_MipLevels = 0;

    //================================
    // Image/Texture Viewer Component
    //================================
    m_acImageView = new acImageView(filePathName, this, NULL, m_MipImages);

    m_viewContextMenu = new QMenu(m_acImageView);

    // Image View Context Menu Item
    m_acImageView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_acImageView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(onViewCustomContextMenu(const QPoint &)));
    actSaveView = new QAction("Save View as...", this);
    connect(actSaveView, SIGNAL(triggered()), this, SLOT(onSaveViewAs()));
    m_viewContextMenu->addAction(actSaveView);

    if (Title.contains("File#"))
    {
        custTitleBar->setTitle(Title+": "+ filePathName);
    }
    else
    {
        // Need to check if MipImages is valid here!!
        if (m_MipImages)
        {
            QString gpuView = "";
            bool useGPUView = false;
            switch (m_MipImages->m_DecompressedFormat)
            {
            case MIPIMAGE_FORMAT_DECOMPRESSED::Format_CPU:
                custTitleBar->setTitle("Compressed Image: CPU View");
                break;
            case MIPIMAGE_FORMAT_DECOMPRESSED::Format_GPU:
                useGPUView = true;
                gpuView = "Compressed Image: GPU View ";
                break;
            default:
            case MIPIMAGE_FORMAT_DECOMPRESSED::Format_NONE:
                break;
            }

            if (useGPUView)
            {
                switch (m_MipImages->m_MipImageFormat)
                {
                case MIPIMAGE_FORMAT::Format_OpenGL:
                    gpuView += "using OpenGL";
                    custTitleBar->setTitle(gpuView);
                    break;
                case MIPIMAGE_FORMAT::Format_DirectX:
                    gpuView += "using DirectX";
                    custTitleBar->setTitle(gpuView);
                    break;
                case MIPIMAGE_FORMAT::Format_Vulkan:
                    gpuView += "using Vulkan";
                    custTitleBar->setTitle(gpuView);
                    break;
                default:
                    custTitleBar->setTitle(gpuView);
                    break;
                }
            }
        }
    }

    if (m_acImageView->m_graphicsScene)
    {
        ID = m_acImageView->m_graphicsScene->ID;
        m_imageSize = m_acImageView->m_MipImages->Image_list.first()->size();
        m_acImageView->enableNavigation(true);

        connect(m_acImageView, SIGNAL(acImageViewMousePosition(QPointF *, QPointF *, int)), this, SLOT(oncpImageViewMousePosition(QPointF *, QPointF *, int)));
        connect(m_acImageView, SIGNAL(acImageViewVirtualMousePosition(QPointF *, QPointF *, int)), this, SLOT(oncpImageViewVirtualMousePosition(QPointF *, QPointF *, int)));
        connect(custTitleBar, SIGNAL(ToolBarCliked()), this, SLOT(OnToolBarClicked()));
    }

    //  //===============
    //  // Virtual mouse with block
    //  //===============
    //  connect(this->m_acImageView, SIGNAL(acImageViewMousePosition(QPointF *, QPointF *, int)), &m_customMouse, SLOT(onVirtualMouseMoveEvent(QPointF *, QPointF *, int)));
    //  
    //  connect(&m_customMouse, SIGNAL(VirtialMousePosition(QPointF *, QPointF *, int)), this->m_acImageView, SLOT(onVirtualMouseMoveEvent(QPointF *, QPointF *, int)));

    //===============
    // Tool Bar
    //===============
    m_toolBar = new QToolBar("Tools");
    m_toolBar->setStyleSheet("QToolBar{spacing: 0px;} QToolButton {width:15px;} ");
    m_toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Grid Background

    QLabel *GridLabel = new QLabel(this);
    GridLabel->setText("Grid:");
    //GridLabel->setMinimumWidth(15);
    //GridLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_toolBar->addWidget(GridLabel);
    m_CBimageview_GridBackground = new QComboBox(this);
	m_CBimageview_GridBackground->addItem(tr("ChkBox"));
    m_CBimageview_GridBackground->addItem(tr("Black"));
    m_CBimageview_GridBackground->addItem(tr("Lines"));
    m_CBimageview_GridBackground->addItem(tr("Points"));
    m_CBimageview_GridBackground->setStyle(Plastique_style);
    //m_CBimageview_GridBackground->setMinimumWidth(15);
    //m_CBimageview_GridBackground->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);

    // http://blog.qt.io/blog/2012/10/30/cleaning-up-styles-in-qt5-and-adding-fusion/
    //m_CBimageview_GridBackground->setStyleSheet("QComboBox { border: 1px solid gray; border - radius: 3px; padding: 1px 18px 1px 3px; min - width: 6em; }");
    //m_CBimageview_GridBackground->setStyleSheet("QComboBox::drop-down {subcontrol-origin: padding; subcontrol-position: top right; width: 15px; border-left-width: 1px; border-left-color: darkgray;     border-left-style: solid; border-top-right-radius: 3px; border-bottom-right-radius: 3px; }");

    connect(m_CBimageview_GridBackground, SIGNAL(currentIndexChanged(int)), m_acImageView, SLOT(onGridBackground(int)));
    
    int i = m_CBimageview_GridBackground->findText("ChkBox");
    m_CBimageview_GridBackground->setCurrentIndex(i);
    m_toolBar->addWidget(m_CBimageview_GridBackground);

    m_toolBar->addSeparator();

    // Zoom level
    m_ZoomLevel = new QSpinBox(this);
    if (m_ZoomLevel)
    {
        QLabel *ZoomLabel = new QLabel(this);
        ZoomLabel->setText("Zoom:");
        m_toolBar->addWidget(ZoomLabel);
        connect(m_ZoomLevel, SIGNAL(valueChanged(int)), this, SLOT(onZoomLevelChanged(int)));
        connect(this, SIGNAL(OnSetScale(int)), m_acImageView, SLOT(onSetScale(int)));
        connect(m_acImageView, SIGNAL(acScaleChanged(int)), this, SLOT(onacScaleChange(int)));

        m_ZoomLevel->setRange(AC_IMAGE_MIN_ZOOM, AC_IMAGE_MAX_ZOOM);
        m_ZoomLevel->setSingleStep(10);
        m_ZoomLevel->setValue(100);
        m_toolBar->addWidget(m_ZoomLevel);
    }


    imageview_ViewImageOriginalSize = new QAction(QIcon(":/CompressonatorGUI/Images/cx100.png"), tr("Original Size"), this);
    if (imageview_ViewImageOriginalSize)
    {
        m_toolBar->addAction(imageview_ViewImageOriginalSize);
        connect(imageview_ViewImageOriginalSize, SIGNAL(triggered()), m_acImageView, SLOT(onViewImageOriginalSize()));
    }

    imageview_FitInWindow = new QAction(QIcon(":/CompressonatorGUI/Images/cxFit.png"), tr("&Fit in Window"), this);
    if (imageview_FitInWindow)
    {
        m_toolBar->addAction(imageview_FitInWindow);
        connect(imageview_FitInWindow, SIGNAL(triggered()), m_acImageView, SLOT(onFitInWindow()));
    }

    imageview_ResetImageView = new QAction(QIcon(":/CompressonatorGUI/Images/OriginalImage.png"), tr("Reset Image View"), this);
    if (imageview_ResetImageView)
    {
        m_toolBar->addAction(imageview_ResetImageView);
        connect(imageview_ResetImageView, SIGNAL(triggered()), m_acImageView, SLOT(onResetImageView()));
        connect(imageview_ResetImageView, SIGNAL(triggered()), this, SLOT(oncpResetImageView()));
    }

    m_toolBar->addSeparator();

    imageview_ToggleChannelR = new QAction(QIcon(":/CompressonatorGUI/Images/cxRed.png"), tr("Show or Hide  Red channel"), this);
    if (imageview_ToggleChannelR)
    {
        imageview_ToggleChannelR->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelR);
        connect(imageview_ToggleChannelR, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelR()));
    }

    imageview_ToggleChannelG = new QAction(QIcon(":/CompressonatorGUI/Images/cxGreen.png"), tr("Show or Hide Green channel"), this);
    if (imageview_ToggleChannelG)
    {
        imageview_ToggleChannelG->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelG);
        connect(imageview_ToggleChannelG, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelG()));
    }

    imageview_ToggleChannelB = new QAction(QIcon(":/CompressonatorGUI/Images/cxBlue.png"), tr("Show or Hide Blue channel"), this);
    if (imageview_ToggleChannelB)
    {
        imageview_ToggleChannelB->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelB);
        connect(imageview_ToggleChannelB, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelB()));
    }

    imageview_ToggleChannelA = new QAction(QIcon(":/CompressonatorGUI/Images/cxAlpha.png"), tr("Show or Hide Alpha channel"), this);
    if (imageview_ToggleChannelA)
    {
        imageview_ToggleChannelA->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelA);
        connect(imageview_ToggleChannelA, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelA()));
    }

    if (setting->onBrightness)
    {
        imageview_ImageBrightnessUp = new QAction(QIcon(":/CompressonatorGUI/Images/brightnessup.png"), tr("Increase Image Brightness"), this);
        if (imageview_ImageBrightnessUp)
        {
            imageview_ImageBrightnessUp->setCheckable(false);
            m_toolBar->addAction(imageview_ImageBrightnessUp);
            connect(imageview_ImageBrightnessUp, SIGNAL(triggered()), m_acImageView, SLOT(onToggleImageBrightnessUp()));
        }

        imageview_ImageBrightnessDown = new QAction(QIcon(":/CompressonatorGUI/Images/brightnessdown.png"), tr("Decrease Image Brightness"), this);
        if (imageview_ImageBrightnessDown)
        {
            imageview_ImageBrightnessDown->setCheckable(false);
            m_toolBar->addAction(imageview_ImageBrightnessDown);
            connect(imageview_ImageBrightnessDown, SIGNAL(triggered()), m_acImageView, SLOT(onToggleImageBrightnessDown()));
        }
    }

    m_toolBar->addSeparator();

    imageview_ToggleGrayScale = new QAction(QIcon(":/CompressonatorGUI/Images/cxgrayscale.png"), tr("Gray Scale"), this);
    if (imageview_ToggleGrayScale)
    {
        imageview_ToggleGrayScale->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleGrayScale);
        connect(imageview_ToggleGrayScale, SIGNAL(triggered()), m_acImageView, SLOT(onToggleGrayScale()));
    }

    imageview_InvertImage = new QAction(QIcon(":/CompressonatorGUI/Images/cxInvert.png"), tr("Invert Image"), this);
    if (imageview_InvertImage)
    {
        m_toolBar->addAction(imageview_InvertImage);
        connect(imageview_InvertImage, SIGNAL(triggered()), m_acImageView, SLOT(onInvertImage()));
    }

    imageview_MirrorHorizontal = new QAction(QIcon(":/CompressonatorGUI/Images/MirrorHorizonal.png"), tr("Mirror Image Horizontally"), this);
    if (imageview_MirrorHorizontal)
    {
        m_toolBar->addAction(imageview_MirrorHorizontal);
        connect(imageview_MirrorHorizontal, SIGNAL(triggered()), m_acImageView, SLOT(onMirrorHorizontal()));
    }

    imageview_MirrorVirtical = new QAction(QIcon(":/CompressonatorGUI/Images/MirrorVertical.png"), tr("Mirror Image Vertically"), this);
    if (imageview_MirrorVirtical)
    {
        m_toolBar->addAction(imageview_MirrorVirtical);
        connect(imageview_MirrorVirtical, SIGNAL(triggered()), m_acImageView, SLOT(onMirrorVirtical()));
    }

    imageview_RotateRight = new QAction(QIcon(":/CompressonatorGUI/Images/cxRotationR.png"), tr("Rotate Image 90 Degrees"), this);
    if (imageview_RotateRight)
    {
        m_toolBar->addAction(imageview_RotateRight);
        connect(imageview_RotateRight, SIGNAL(triggered()), m_acImageView, SLOT(onRotateRight()));
    }

    imageview_RotateLeft = new QAction(QIcon(":/CompressonatorGUI/Images/cxRotationL.png"), tr("Rotate Image -90 Degrees"), this);
    if (imageview_RotateLeft)
    {
        m_toolBar->addAction(imageview_RotateLeft);
        connect(imageview_RotateLeft, SIGNAL(triggered()), m_acImageView, SLOT(onRotateLeft()));
    }

    if (m_MipLevels > 0)
    {
        m_toolBar->addSeparator();
        QLabel *MipLevelLabel = new QLabel(this);
        MipLevelLabel->setText("MipLevel:");
        m_toolBar->addWidget(MipLevelLabel);

        m_CBimageview_MipLevel = new QComboBox(this);

        for (int num = 0; num < m_MipLevels; num++)
        {
            if (m_MipImages)
            {
                if (m_MipImages->Image_list.count() > num)
                {
                    QString mipLevelList = QString::number(num);
                    QImage *image = m_MipImages->Image_list[num];
                    mipLevelList.append(QString(" ("));
                    mipLevelList.append(QString::number(image->width()));
                    mipLevelList.append(QString("x"));
                    mipLevelList.append(QString::number(image->height()));
                    mipLevelList.append(QString(")"));
                    m_CBimageview_MipLevel->addItem(mipLevelList);
                }
            }
        }

        //m_CBimageview_MipLevel->setStyleSheet("QComboBox { border: 1px solid gray; border - radius: 3px; padding: 1px 18px 1px 3px; min - width: 6em; }");
        connect(m_CBimageview_MipLevel, SIGNAL(currentIndexChanged(int)), m_acImageView, SLOT(onImageLevelChanged(int)));
        connect(m_CBimageview_MipLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(onResetHDR(int)));
        m_toolBar->addWidget(m_CBimageview_MipLevel);

    }

    m_toolBar->addSeparator();

#ifdef _DEBUG
    // Debug Checkbox
    m_CBimageview_Debug = new QComboBox(this);
    m_CBimageview_Debug->addItem(tr("Debug..."));
    m_CBimageview_Debug->addItem(tr("BC6H"));
    m_CBimageview_Debug->addItem(tr("BC6H_SF"));
    m_CBimageview_Debug->addItem(tr("BC7"));
    m_toolBar->addWidget(m_CBimageview_Debug);

    connect(m_CBimageview_Debug, SIGNAL(activated(int)), m_acImageView, SLOT(onToggleDebugChanged(int)));

    m_toolBar->addSeparator();
#endif

    if (m_MipImages && (m_MipImages->mipset != NULL))
    {
        if (m_MipImages->mipset->m_format == CMP_FORMAT_ARGB_32F || (m_MipImages->mipset->m_format == CMP_FORMAT_ARGB_16F) 
            || (m_MipImages->mipset->m_format == CMP_FORMAT_RGBE_32F) || (m_MipImages->mipset->m_format == CMP_FORMAT_BC6H)
            || (m_MipImages->mipset->m_format == CMP_FORMAT_BC6H_SF))
        {
            m_ExrProperties = new acEXRTool();
            // Tool list
            QLabel *GridLabel = new QLabel(this);
            GridLabel->setText("");

            m_toolBar->addWidget(GridLabel);
            m_CBimageview_ToolList = new QComboBox(this);
            m_CBimageview_ToolList->addItem(tr("View..."));
            m_CBimageview_ToolList->addItem(tr("HDR Properties"));
            m_CBimageview_ToolList->setStyle(Plastique_style);
            m_toolBar->addWidget(m_CBimageview_ToolList);
            connect(m_CBimageview_ToolList, SIGNAL(activated(int)), this, SLOT(onToolListChanged(int)));
            if (m_ExrProperties)
            {
                connect(m_ExrProperties->exrExposureBox, SIGNAL(valueChanged(double)), m_acImageView, SLOT(onExrExposureChanged(double)));
                connect(m_ExrProperties->exrDefogBox, SIGNAL(valueChanged(double)), m_acImageView, SLOT(onExrDefogChanged(double)));
                connect(m_ExrProperties->exrKneeLowBox, SIGNAL(valueChanged(double)), m_acImageView, SLOT(onExrKneeLowChanged(double)));
                connect(m_ExrProperties->exrKneeHighBox, SIGNAL(valueChanged(double)), m_acImageView, SLOT(onExrKneeHighChanged(double)));
            }
        }
    }

#ifdef USE_INTERNAL_DECOMPRESS
    
    // This code may not be usable as decompression can be done 
    // outside of this class, The decompressed images are also 
    // used to calculate the statistics of image diff

    m_CBimage_DecompressUsing = new QComboBox(this);
    m_CBimage_DecompressUsing->addItem("View: CPU");
    m_CBimage_DecompressUsing->addItem("View: OpenGL");
    m_CBimage_DecompressUsing->setStyleSheet("QComboBox { border: 1px solid gray; border - radius: 3px; padding: 1px 18px 1px 3px; min - width: 6em; }");
    connect(m_CBimage_DecompressUsing, SIGNAL(currentIndexChanged(int)), this, SLOT(onDecompressUsing(int)));
    m_toolBar->addWidget(m_CBimageview_MipLevel);
#endif

    m_toolBar->setMaximumHeight(25);

    m_toolBar->hide();

    QHBoxLayout *hlayout2 = new QHBoxLayout;
    hlayout2->setSpacing(0);
    hlayout2->setMargin(0);
    hlayout2->setContentsMargins(0, 0, 0, 0);
    hlayout2->addWidget(m_toolBar, 0);

    QString Navigation = QStringLiteral(":/CompressonatorGUI/Images/navigate.png");

    m_statusBar = new QStatusBar(this);
    m_statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey; border-bottom: 1px outset grey;}");

    m_labelColorRGBA = new QLabel(this);
    m_labelColorRGBA->setAlignment(Qt::AlignLeft);
    m_labelColorRGBA->setAutoFillBackground(true);

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window,     Qt::black);
    sample_palette.setColor(QPalette::WindowText, Qt::black);
    m_labelColorRGBA->setPalette(sample_palette);
    m_labelColorRGBA->setText("RGBA");

    m_labelColorTxt  = new QLabel(this);
    m_labelColorTxt->setText("0,0,0,0");
    m_labelColorTxt->setAlignment(Qt::AlignLeft);
    
    m_labelPos = new QLabel(this);
    m_labelPos->setText("");
    m_labelPos->setAlignment(Qt::AlignLeft);

    m_statusBar->addPermanentWidget(m_labelColorRGBA);
    m_statusBar->addPermanentWidget(m_labelColorTxt,40);
    m_statusBar->addPermanentWidget(m_labelPos, 40);

       
    m_layout = new QGridLayout(m_newWidget);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(hlayout2, 0, 0);
    m_layout->addWidget(m_acImageView, 1, 0);
    m_layout->addWidget(m_statusBar, 2, 0);

    m_newWidget->setLayout(m_layout);

    setWidget(m_newWidget);
}


void cpImageView::oncpResetImageView()
{
    imageview_ToggleChannelR->setChecked(false);
    imageview_ToggleChannelG->setChecked(false);
    imageview_ToggleChannelB->setChecked(false);
    imageview_ToggleChannelA->setChecked(false);
    imageview_ToggleGrayScale->setCheckable(false);

    onResetHDR(0);
}

void cpImageView::onDecompressUsing(int useDecomp)
{
    Q_UNUSED(useDecomp);
}

void cpImageView::onResetHDR(int value)
{
    Q_UNUSED(value);
    if (m_ExrProperties)
    {
        if (m_ExrProperties->isVisible())
            m_ExrProperties->hide();
        m_ExrProperties->exrExposureBox->setValue(DEFAULT_EXPOSURE);
        m_ExrProperties->exrDefogBox->setValue(DEFAULT_DEFOG);
        m_ExrProperties->exrKneeLowBox->setValue(DEFAULT_KNEELOW);
        m_ExrProperties->exrKneeHighBox->setValue(DEFAULT_KNEEHIGH);
    }
}

void cpImageView::onToolListChanged(int index)
{
    switch (index) 
    {
    case 1:  //EXR
        if (m_ExrProperties)
        {
            if (m_ExrProperties->isVisible())
                m_ExrProperties->raise();
            else
                m_ExrProperties->show();
        }
        break;
    default:
        if (m_ExrProperties)
            m_ExrProperties->hide();
        break;
    }
}

void cpImageView::onViewCustomContextMenu(const QPoint &point)
{
    m_viewContextMenu->exec(m_acImageView->mapToGlobal(point));
}

void cpImageView::onSaveViewAs()
{
    if (m_acImageView)
    {
        QString filePathName = QFileDialog::getSaveFileName(this, tr("Save Image View as"), "ImageView", tr("Image View files (*.bmp)"));
        if (filePathName.length() == 0) return;
        QPixmap pixmap = m_acImageView->m_imageItem->pixmap();
        QImage img = pixmap.toImage();
        img.save(filePathName);
    }
}

void cpImageView::showToobar(bool show)
{
    if (show)
        m_toolBar->show();
    else
        m_toolBar->hide();
}

void cpImageView::OnToolBarClicked()
{
    if (m_toolBar->isVisible()) 
        m_toolBar->hide();
    else 
        m_toolBar->show();
}


void cpImageView::paintEvent(QPaintEvent * event)
{
    Q_UNUSED(event);

    if (m_FitOnShow)
    {
        imageview_FitInWindow->trigger();
        m_FitOnShow = false;
    }

    if (m_CBimageview_MipLevel)
    {
        if ((m_MipImages) && (m_CBimageview_MipLevel->isEnabled() == false))
        {
            // need to find root cause of 0xFEEEFEEE
            if ((m_MipImages->mipset) && (m_MipImages->mipset != (void*)0xFEEEFEEE))
            {
                if (m_MipLevels != m_MipImages->mipset->m_nMipLevels)
                {
                    EnableMipLevelDisplay(m_MipImages->mipset->m_nMipLevels);
                }
            }
        }
    }

    if (m_originalImage == false)
    {
        // Emit any data we want to show in main apps poperty page
        // emit UpdateData(NULL);
    }
}

void cpImageView::showEvent(QShowEvent *)
{

}


// This slot is received when user changes zoom using tool bar zoom

void cpImageView::onZoomLevelChanged(int value)
{
    if (!m_bOnacScaleChange)
        emit OnSetScale(value);
}

// This slot is received when user changes zoom using mouse wheel event in acImageView

void cpImageView::onacScaleChange(int value)
{
    m_bOnacScaleChange = true;
    m_ZoomLevel->setValue(value);
    m_bOnacScaleChange = false;
}



