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

#include "cpimageview.h"
#include "acimageview.h"

void cpImageView::oncpImageViewVirtualMousePosition(QPointF* scenePos, QPointF* localPos, int onID)
{
    oncpImageViewMousePosition(scenePos, localPos, onID);
}

void cpImageView::showToobarButton(bool show)
{
    if (custTitleBar)
        custTitleBar->setButtonToolBarShow(show);
}

void cpImageView::oncpImageViewMousePosition(QPointF* scenePos, QPointF* localPos, int onID)
{
    Q_UNUSED(scenePos);
    Q_UNUSED(onID);

    if (!m_acImageView)
        return;

    // is mouse inside image view
    if (localPos)
    {
        m_localPos = *localPos;
        // Rounds up pixel co-ordinates
        // if in debugMode: blocks at 0,0 will start at 1,1
        int x = qRound(localPos->rx() + 0.5);
        int y = qRound(localPos->ry() + 0.5);
        if (x < 0)
            x = 0;
        if (y < 0)
            y = 0;

        QColor  color;
        QString Txt = "";

        switch (m_ImageViewState)
        {
        case eImageViewState::isOriginal: {
            //m_labelTxtView->setText("Original");
            if (m_acImageView->m_imageItem_Original)
            {
                color = m_acImageView->m_imageItem_Original->pixmap().toImage().pixel(localPos->rx(), localPos->ry());
            }
            else
            {
                // original image is a root on project explorer
                if (m_acImageView->m_imageItem_Processed)
                    color = m_acImageView->m_imageItem_Processed->pixmap().toImage().pixel(localPos->rx(), localPos->ry());
            }

            bool dispDefault = true;

            // New feature for debugging code that uses alpha channel
            if (m_OriginalMipImages && m_CMips)
            {
                if (m_OriginalMipImages->mipset)
                {
                    MipLevel* mipLevel = m_CMips->GetMipLevel(m_OriginalMipImages->mipset, m_acImageView->m_currentMiplevel, m_DepthLevel);
                    if (mipLevel)
                    {
                        if (m_OriginalMipImages->mipset->m_format == CMP_FORMAT_RGBA_8888_S)
                        {
                            CMP_SBYTE* pData = mipLevel->m_psbData;
                            if (pData)
                            {
                                if ((y <= mipLevel->m_nHeight) && (x <= mipLevel->m_nWidth))
                                {
                                    // For Channel data RGBA 8:8:8:8 in mipset
                                    if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_8bit)
                                    {
                                        CMP_DWORD pixoffset = 0;
                                        if ((x > 0) && (y > 0))
                                            pixoffset = (y - 1) * mipLevel->m_nWidth * 4 + (x - 1) * 4;
                                        if ((mipLevel->m_dwLinearSize > 3) && (pixoffset < mipLevel->m_dwLinearSize - 3))
                                        {
                                            dispDefault = false;
                                            Txt.sprintf("RGBA_S Source (%3d,%3d,%3d,%3d) Rendered (%3d,%3d,%3d,%3d)",
                                                        pData[pixoffset],
                                                        pData[pixoffset + 1],
                                                        pData[pixoffset + 2],
                                                        pData[pixoffset + 3],
                                                        color.red(),
                                                        color.green(),
                                                        color.blue(),
                                                        color.alpha());
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            CMP_BYTE* pData = mipLevel->m_pbData;
                            if (pData)
                            {
                                if ((y <= mipLevel->m_nHeight) && (x <= mipLevel->m_nWidth))
                                {
                                    // For Channel data RGBA 8:8:8:8 in mipset
                                    if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_8bit)
                                    {
                                        CMP_DWORD pixoffset = 0;
                                        if ((x > 0) && (y > 0))
                                            pixoffset = (y - 1) * mipLevel->m_nWidth * 4 + (x - 1) * 4;
                                        if ((mipLevel->m_dwLinearSize > 3) && (pixoffset < mipLevel->m_dwLinearSize - 3))
                                        {
                                            dispDefault = false;
                                            Txt.sprintf("RGBA Source (%3d,%3d,%3d,%3d) ",
                                                        pData[pixoffset],
                                                        pData[pixoffset + 1],
                                                        pData[pixoffset + 2],
                                                        pData[pixoffset + 3]);
                                            // Rendered (%3d,%3d,%3d,%3d) ",
                                            //color.red(), color.green(), color.blue(), color.alpha());
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }

            if (dispDefault)
                Txt.sprintf(" RGBA Rendered (%3d,%3d,%3d,%3d)", color.red(), color.green(), color.blue(), color.alpha());
        }
        break;
        case eImageViewState::isDiff:
            m_labelTxtView->setText(TXT_IMAGE_DIFF);
            if (m_acImageView->m_imageItem_Processed)
            {
                color = m_acImageView->m_imageItem_Processed->pixmap().toImage().pixel(localPos->rx(), localPos->ry());
                // normalize color back prior to any brightness or contrast adjustments for the user view
                int r = color.red();
                int g = color.green();
                int b = color.blue();
                if (g_Application_Options.m_imagediff_contrast != 0)
                {
                    r = (r / g_Application_Options.m_imagediff_contrast);
                    g = (g / g_Application_Options.m_imagediff_contrast);
                    b = (b / g_Application_Options.m_imagediff_contrast);
                }
                color.setRed(r);
                color.setGreen(g);
                color.setBlue(b);
                Txt.sprintf("RGB Diff [%3d,%3d,%3d]", color.red(), color.green(), color.blue());
            }
            break;
        case eImageViewState::isProcessed:
            m_labelTxtView->setText(TXT_IMAGE_PROCESSED);
            if (m_acImageView->m_imageItem_Processed)
            {
                bool dispDefault = true;

                if (m_acImageView->m_MipImages)
                {
                    color = m_acImageView->m_imageItem_Processed->pixmap().toImage().pixel(localPos->rx(), localPos->ry());
                    if (m_acImageView->m_MipImages->decompressedMipSet)
                    {
                        MipLevel* mipLevel =
                            m_CMips->GetMipLevel(m_acImageView->m_MipImages->decompressedMipSet, m_acImageView->m_currentMiplevel, m_DepthLevel);
                        if (mipLevel)
                        {
                            if (m_acImageView->m_MipImages->decompressedMipSet->m_format == CMP_FORMAT_RGBA_8888_S)
                            {
                                CMP_SBYTE* pData = mipLevel->m_psbData;
                                if (pData)
                                {
                                    if ((y <= mipLevel->m_nHeight) && (x <= mipLevel->m_nWidth))
                                    {
                                        // For Channel data RGBA 8:8:8:8 in mipset
                                        if (m_acImageView->m_MipImages->decompressedMipSet->m_ChannelFormat == CF_8bit)
                                        {
                                            CMP_DWORD pixoffset = 0;
                                            if ((x > 0) && (y > 0))
                                                pixoffset = (y - 1) * mipLevel->m_nWidth * 4 + (x - 1) * 4;
                                            if ((mipLevel->m_dwLinearSize > 3) && (pixoffset < mipLevel->m_dwLinearSize - 3))
                                            {
                                                dispDefault = false;
                                                Txt.sprintf("RGBA_S Source (%3d,%3d,%3d,%3d) Rendered (%3d,%3d,%3d,%3d)",
                                                            pData[pixoffset],
                                                            pData[pixoffset + 1],
                                                            pData[pixoffset + 2],
                                                            pData[pixoffset + 3],
                                                            color.red(),
                                                            color.green(),
                                                            color.blue(),
                                                            color.alpha());
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                CMP_BYTE* pData = mipLevel->m_pbData;
                                if (pData)
                                {
                                    if ((y <= mipLevel->m_nHeight) && (x <= mipLevel->m_nWidth))
                                    {
                                        // For Channel data RGBA 8:8:8:8 in mipset
                                        if (m_acImageView->m_MipImages->decompressedMipSet->m_ChannelFormat == CF_8bit)
                                        {
                                            CMP_DWORD pixoffset = 0;
                                            if ((x > 0) && (y > 0))
                                                pixoffset = (y - 1) * mipLevel->m_nWidth * 4 + (x - 1) * 4;
                                            if ((mipLevel->m_dwLinearSize > 3) && (pixoffset < mipLevel->m_dwLinearSize - 3))
                                            {
                                                dispDefault = false;
                                                Txt.sprintf("RGBA Source (%3d,%3d,%3d,%3d)",
                                                            pData[pixoffset],
                                                            pData[pixoffset + 1],
                                                            pData[pixoffset + 2],
                                                            pData[pixoffset + 3]);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (dispDefault)
                    Txt.sprintf(" RGBA Rendered (%3d,%3d,%3d,%3d)", color.red(), color.green(), color.blue(), color.alpha());
            }
            break;
        default:
            m_labelTxtView->setText("?");
            break;
        }

        m_labelColorTxt->setText(Txt);

        QString Txt2;

#ifdef USE_BCN_IMAGE_DEBUG
#ifdef _DEBUG
        if (m_acImageView->m_debugMode)
        {
            m_labelColorTxt->setText("");
            XBlockNum = x;
            YBlockNum = y;
            Txt2      = QString::number(x) + "," + QString::number(y) + " Block";
        }
#endif
#endif
        Txt2 = QString::number(x) + "," + QString::number(y) + " px";
        m_labelPos->setText(Txt2);

        if (x == 0)
            x = 1;
        if (y == 0)
            y = 1;
        m_source_BlockXPos = (x - 1) / 4;
        m_source_BlockYPos = (y - 1) / 4;

        Txt2 = QString::number(m_source_BlockXPos) + "," + QString::number(m_source_BlockYPos) + " (4x4)";

        m_labelBlockPos->setText(Txt2);

        QPalette palette;
        palette.setColor(QPalette::Background, color);
        palette.setColor(QPalette::WindowText, color);
        m_labelColorRGBA->setPalette(palette);
    }
    else
    {
        m_labelPos->setText("");
        m_labelBlockPos->setText("");
        m_labelColorTxt->setText("");
        QPalette palette;
        palette.setColor(QPalette::Background, Qt::black);
        palette.setColor(QPalette::WindowText, Qt::black);
        m_labelColorRGBA->setPalette(palette);
    }
}

#include "cexr.h"
#include "ImfArray.h"
#include "ImfRgba.h"

void cpImageView::onImageDiff()
{
    if (!m_CBimageview_Toggle)
        return;

    if (qApp)
        qApp->setOverrideCursor(Qt::BusyCursor);

    imageview_ImageDiff->setDisabled(true);

    if (m_ImageViewState == eImageViewState::isDiff)
    {
        m_ImageViewState = eImageViewState::isProcessed;
        m_CBimageview_Toggle->setItemText(0, TXT_IMAGE_PROCESSED);
    }
    else
    {
        m_ImageViewState = eImageViewState::isDiff;
        m_CBimageview_Toggle->setItemText(0, TXT_IMAGE_DIFF);
    }

    activeview = 0;
    m_CBimageview_Toggle->setCurrentIndex(0);

    if (m_acImageView)
    {
        m_acImageView->onToggleImageViews(0);
        m_acImageView->onSetPixelDiffView(m_ImageViewState == eImageViewState::isDiff);
    }

    oncpImageViewMousePosition(0, &m_localPos, 0);

    if (qApp)
        qApp->restoreOverrideCursor();

    imageview_ImageDiff->setDisabled(false);
    setActionForImageViewStateChange();
}

void cpImageView::keyPressEvent(QKeyEvent* event)
{
    if (m_acImageView == NULL)
        return;

#ifndef __linux__
    //------------------------------------------------------------------------------------
    // Feature enabled to allow user to copy an image view onto window system clipboard
    // This is only available through keyboard entry using:
    // Ctrl+C to copy the viewed image (Scaled accroding to zoom setting)
    // Alt+C to copy using the original image size
    // in both cases aspect ratio is maintained.
    //------------------------------------------------------------------------------------
    if (event->key() == Qt::Key_C)
    {
        bool CTRL_key = event->modifiers() & Qt::ControlModifier;
        bool ALT_key  = event->modifiers() & Qt::AltModifier;
        if (CTRL_key || ALT_key)
        {
            if (!m_acImageView->m_imageItem_Processed)
                return;

            // Copy Scaled image size
            if (CTRL_key)
            {
                int   w        = m_acImageView->m_imageItem_Processed->pixmap().width();
                int   h        = m_acImageView->m_imageItem_Processed->pixmap().height();
                qreal scale    = m_acImageView->m_imageItem_Processed->scale();  // same value as m_ZoomLevel->value() / 100.0f
                w              = int(w * scale);
                h              = int(h * scale);
                QPixmap pixmap = m_acImageView->m_imageItem_Processed->pixmap().scaled(w, h, Qt::KeepAspectRatio);
                QApplication::clipboard()->setPixmap(pixmap, QClipboard::Clipboard);
            }
            else
            {  // Copy Original Size
                QPixmap pixmap = m_acImageView->m_imageItem_Processed->pixmap();
                QApplication::clipboard()->setPixmap(pixmap, QClipboard::Clipboard);
            }
        }
    }
#endif

    if (m_OriginalMipImages->mipset == NULL)
        return;
    if (!m_CBimageview_Toggle)
        return;

    // Checks if we are viewing a processed image view (child node in project explorer)
    if (m_CBimageview_Toggle)
    {
        if (event->key() == Qt::Key_D)
        {  // Set View to Processed Image (Index 0) and Enable Diff if applicable
            onImageDiff();
        }
        else if (event->key() == Qt::Key_P)
        {  // View Processed Image (Index 0)
            activeview = 0;
            m_CBimageview_Toggle->setCurrentIndex(0);
            // switch to a processed image view
            if (m_acImageView)
                m_acImageView->onToggleImageViews(0);
            oncpImageViewMousePosition(0, &m_localPos, 0);
            if (m_ImageViewState == eImageViewState::isDiff)
            {
                // the current processed view is an image diff to reset the view back to processed with no diff
                onImageDiff();
            }
            m_ImageViewState = eImageViewState::isProcessed;
        }
        else if (event->key() == Qt::Key_O)
        {  // View Original Image (Index 1)
            activeview = 1;
            m_CBimageview_Toggle->setCurrentIndex(1);
            // switch to a original image view
            if (m_acImageView)
                m_acImageView->onToggleImageViews(1);
            oncpImageViewMousePosition(0, &m_localPos, 0);
            m_ImageViewState = eImageViewState::isOriginal;
        }
        else if (event->key() == Qt::Key_Space || event->key() == Qt::Key_S)
        {
            if (activeview == 0)
            {
                activeview       = 1;  // Original
                m_ImageViewState = eImageViewState::isOriginal;
            }
            else
            {
                activeview = 0;  // Processed or "Processed Diff" view
                if (m_DiffOnOff)
                    m_ImageViewState = eImageViewState::isDiff;
                else
                    m_ImageViewState = eImageViewState::isProcessed;
            }
            m_CBimageview_Toggle->setCurrentIndex(activeview);
            if (m_acImageView)
                m_acImageView->onToggleImageViews(activeview);
            oncpImageViewMousePosition(0, &m_localPos, 0);
        }
    }

// This feature can be enabled in Debug Mode and allows users to breakpoint into various
// codecs at the lowest block level of encoding.
#ifdef USE_BCN_IMAGE_DEBUG
#ifdef _DEBUG
    if (event->key() == Qt::Key_F1)
    {
        if (m_acImageView->m_debugMode)
        {
            MipLevel* mipLevel = m_CMips->GetMipLevel(m_OriginalMipImages->mipset, m_DepthLevel);

            if (!mipLevel)
                return;

            // Check the block size matches what we expect for  BC6H or BC7
            if (m_acImageView->m_graphicsScene->cursorBlockX != 4)
                return;
            if (m_acImageView->m_graphicsScene->cursorBlockY != 4)
                return;

            // Flag to indicate we have data to process
            bool hasData = false;

            // Calc data stride for each row index = width * num channels
            int row_stride = m_OriginalMipImages->mipset->m_nWidth * 4;

            // (BC6 Block Width * BC6 Block Height * Num Channels) * (Image Width / BC6 Block Width)
            // (4*4*4) * (Width / 4) = 4*4*Width
            int block_offset = 4 * 4 * m_OriginalMipImages->mipset->m_nWidth;

            // Get the block position we need to fill from
            // 16 = BC6 Block Width * Num Channels
            int start_index = (((XBlockNum - 1) * 16)) + ((YBlockNum - 1) * block_offset);
            int index;

            // napatel check format before getting data!
            // we have two sets Float16 (2 Bytes alligned) and Float32 (4 Bytes alligned)

            if (m_acImageView->m_debugFormat == "BC6H" || m_acImageView->m_debugFormat == "BC6H_SF")
            {
                // Encoder input to fill with data
                CMP_FLOAT in[16][4];
                float     dec_out[16][4];
                BYTE      cmp_in[16];

                if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float16)
                {
                    // Get origin data pointer
                    CMP_HALFSHORT* data = mipLevel->m_phfsData;
                    CMP_HALF*      temp = (CMP_HALF*)data;
                    Array2D<Rgba>  pixels(4, 4);
                    pixels.resizeErase(4, 4);
                    int d = 0;
                    for (int row = 0; row < 4; row++)
                    {
                        index = (row * row_stride) + start_index;
                        for (int col = 0; col < 4; col++)
                        {
                            in[d][0] = (float)temp[index];
                            pixels[row][col].r.setBits(data[index]);
                            in[d][1] = (float)temp[index + 1];
                            pixels[row][col].g.setBits(data[index + 1]);
                            in[d][2] = (float)temp[index + 2];
                            pixels[row][col].b.setBits(data[index + 2]);
                            in[d][3] = (float)temp[index + 3];
                            pixels[row][col].a.setBits(data[index + 3]);
                            d++;
                            index += 4;
                        }
                    }
                    hasData = true;
                    if (event->key() == Qt::Key_S)
                    {
                        string filename = "exr16f_srcblock_x" + to_string(XBlockNum) + "_y" + to_string(YBlockNum) + ".exr";
                        Exr::writeRgba(filename, pixels, 4, 4);
                    }
                }
                else if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float32)
                {
                    // Get origin data pointer
                    float*        data = mipLevel->m_pfData;
                    Array2D<Rgba> pixels(4, 4);
                    pixels.resizeErase(4, 4);
                    int d = 0;
                    for (int row = 0; row < 4; row++)
                    {
                        index = (row * row_stride) + start_index;
                        for (int col = 0; col < 4; col++)
                        {
                            in[d][0]           = data[index];
                            pixels[row][col].r = data[index];
                            in[d][1]           = data[index + 1];
                            pixels[row][col].g = data[index + 1];
                            in[d][2]           = data[index + 2];
                            pixels[row][col].b = data[index + 1];
                            in[d][3]           = data[index + 3];
                            pixels[row][col].a = data[index + 1];
                            d++;
                            index += 4;
                        }
                    }
                    hasData = true;
                    if (event->key() == Qt::Key_S)
                    {
                        string filename = "exr32f_srcblock_" + to_string(XBlockNum) + "_y" + to_string(YBlockNum) + ".exr";
                        Exr::writeRgba(filename, pixels, 4, 4);
                    }
                }
                else if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Compressed)
                {
                    // Get origin data pointer
                    BYTE* data = mipLevel->m_pbData;

                    // set the data offset to the compressed block row,col which is in 16 byte incriments
                    // Note : 16 bytes per block * (Width in pixels / width of block which is 4 ) = 4 * width
                    int cmp_block_offset = 4 * m_OriginalMipImages->mipset->m_nWidth;
                    int offset           = (((XBlockNum - 1) * 16)) + ((YBlockNum - 1) * cmp_block_offset);
                    data                 = data + offset;
                    for (int d = 0; d < 16; d++)
                    {
                        cmp_in[d] = data[d];
                    }
                    hasData = true;
                }

                if (hasData)
                {
                    // Use SDK interface to Encode a Block for debugging with
                    BC6HBlockEncoder* blockEncode;
                    BYTE              output[16];
                    if (CMP_InitializeBCLibrary() == BC_ERROR_NONE)
                    {
                        CMP_BC6H_BLOCK_PARAMETERS user_settings;
                        user_settings.fQuality       = 1.0;
                        user_settings.bUsePatternRec = false;
                        if (m_acImageView->m_debugFormat == "BC6H")
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
            else if (m_acImageView->m_debugFormat == "BC7")
            {
                // Encoder input to fill with data
                double in[16][4];
                double dec_out[16][4];

                // Get origin data pointer
                BYTE* data = mipLevel->m_pbData;
                int   d    = 0;
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
                    BC7BlockEncoder* blockEncode;
                    BYTE             output[16];
                    if (CMP_InitializeBCLibrary() == BC_ERROR_NONE)
                    {
                        CMP_BC6H_BLOCK_PARAMETERS user_settings;
                        user_settings.fQuality       = 1.0;
                        user_settings.bUsePatternRec = false;
                        user_settings.bIsSigned      = false;
                        user_settings.fExposure      = 1.00;
                        if (CMP_CreateBC7Encoder(0.05, false, false, 0xCF, 1.0, &blockEncode) == BC_ERROR_NONE)
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
#endif
    setActionForImageViewStateChange();
    return;
}

void cpImageView::GetSourceBlock(int BlockX, int BlockY, string filename)
{
    if (!m_CMips)
        return;
    if (!m_OriginalMipImages)
        return;

    // Compressed source files not supported.
    if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Compressed)
        return;

    MipLevel* mipLevel = m_CMips->GetMipLevel(m_OriginalMipImages->mipset, m_DepthLevel);
    if (!mipLevel)
        return;

    if (m_acImageView->m_graphicsScene->cursorBlockX != 4)
        return;
    if (m_acImageView->m_graphicsScene->cursorBlockY != 4)
        return;

#define CHANNEL_SIZE 4  // 4 channels RGBA
#define XBLOCK_SIZE 4   // 4 pixels
#define YBLOCK_SIZE 4   // 4 pixels

    // Calc data stride for each row index = width * num channels
    int row_stride = m_OriginalMipImages->mipset->m_nWidth * CHANNEL_SIZE;

    // Start position of the first pixel to save in the 4x4 block
    int start_index = (BlockX * XBLOCK_SIZE * CHANNEL_SIZE) + (BlockY * row_stride * YBLOCK_SIZE);

    // Incrimental start pixel index from (row,col = 0) position of each 4x4 block
    int index;

    if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float16)
    {
        // Encoder input to fill with data
        CMP_FLOAT in[16][4];
        // Get origin data pointer
        CMP_HALFSHORT* data = mipLevel->m_phfsData;
        CMP_HALF*      temp = (CMP_HALF*)data;
        Array2D<Rgba>  pixels(4, 4);
        pixels.resizeErase(4, 4);
        int d = 0;
        for (int row = 0; row < 4; row++)
        {
            index = (row * row_stride) + start_index;
            for (int col = 0; col < 4; col++)
            {
                in[d][0] = (float)temp[index];
                pixels[row][col].r.setBits(data[index]);
                in[d][1] = (float)temp[index + 1];
                pixels[row][col].g.setBits(data[index + 1]);
                in[d][2] = (float)temp[index + 2];
                pixels[row][col].b.setBits(data[index + 2]);
                in[d][3] = (float)temp[index + 3];
                pixels[row][col].a.setBits(data[index + 3]);
                d++;
                index += 4;
            }
        }
        Exr::writeRgba(filename, pixels, 4, 4);
    }
    else if (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float32)
    {
        // Encoder input to fill with data
        CMP_FLOAT in[16][4];
        // Get origin data pointer
        float*        data = mipLevel->m_pfData;
        Array2D<Rgba> pixels(4, 4);
        pixels.resizeErase(4, 4);
        int d = 0;
        for (int row = 0; row < 4; row++)
        {
            index = (row * row_stride) + start_index;
            for (int col = 0; col < 4; col++)
            {
                in[d][0]           = data[index];
                pixels[row][col].r = data[index];
                in[d][1]           = data[index + 1];
                pixels[row][col].g = data[index + 1];
                in[d][2]           = data[index + 2];
                pixels[row][col].b = data[index + 1];
                in[d][3]           = data[index + 3];
                pixels[row][col].a = data[index + 1];
                d++;
                index += 4;
            }
        }
        Exr::writeRgba(filename, pixels, 4, 4);
    }  // else CF_Float3
    else
    {
        QColor  color;
        QPixmap pixmap(4, 4);
        QImage  image = pixmap.toImage();

        CMP_BYTE* data = mipLevel->m_pbData;
        int       d    = 0;
        for (int h = 0; h < 4; h++)
        {
            index = (h * row_stride) + start_index;
            for (int w = 0; w < 4; w++)
            {
                color.setRed(data[index]);
                color.setGreen(data[index + 1]);
                color.setBlue(data[index + 2]);
                color.setAlpha(data[index + 3]);
                image.setPixel(w, h, color.rgba());
                d++;
                index += 4;
            }
        }
        image.save(filename.c_str());
    }
}

void cpImageView::EnableMipLevelDisplay(int level)
{
    if (level <= 1)
        return;

    for (int num = 0; num < level; num++)
    {
        m_CBimageview_MipLevel->addItem("MipLevel : " + QString::number(num));
    }
    m_CBimageview_MipLevel->setEnabled(true);
    m_MipLevels = level;
}

void cpImageView::EnableDepthLevelDisplay(int level)
{
    if (level < 0)
        return;

    for (int num = 0; num < level; num++)
    {
        m_CBimageview_DepthLevel->addItem("Frames : " + QString::number(num));
    }
    m_CBimageview_DepthLevel->setEnabled(true);
    m_DepthLevel = level;
}

cpImageView::~cpImageView()
{
    if (m_ExrProperties)
    {
        delete m_ExrProperties;
    }

    if (m_localMipImages)
    {
        if (m_processedMipImages)
            m_imageLoader->clearMipImages(&m_processedMipImages);
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

void cpImageView::setActionForImageViewStateChange()
{
    switch (m_ImageViewState)
    {
    case eImageViewState::isOriginal:
    case eImageViewState::isDiff:
        if (imageview_ResetImageView)
            imageview_ResetImageView->setEnabled(false);
        if (imageview_ToggleChannelR)
            imageview_ToggleChannelR->setEnabled(false);
        if (imageview_ToggleChannelG)
            imageview_ToggleChannelG->setEnabled(false);
        if (imageview_ToggleChannelB)
            imageview_ToggleChannelB->setEnabled(false);
        if (imageview_ToggleChannelA)
            imageview_ToggleChannelA->setEnabled(false);
        if (imageview_ToggleGrayScale)
            imageview_ToggleGrayScale->setEnabled(false);
        if (imageview_InvertImage)
            imageview_InvertImage->setEnabled(false);
        if (imageview_MirrorHorizontal)
            imageview_MirrorHorizontal->setEnabled(false);
        if (imageview_MirrorVirtical)
            imageview_MirrorVirtical->setEnabled(false);
        if (imageview_RotateRight)
            imageview_RotateRight->setEnabled(false);
        if (imageview_RotateLeft)
            imageview_RotateLeft->setEnabled(false);
        if (m_BrightnessLevel)
            m_BrightnessLevel->setEnabled(false);
        if (m_CBimageview_ToolList)
            m_CBimageview_ToolList->setEnabled(false);
        if (m_ExrProperties)
        {
            if (m_ExrProperties->isVisible())
                m_ExrProperties->hide();
        }

        break;
    case eImageViewState::isProcessed:
        if (imageview_ResetImageView)
            imageview_ResetImageView->setEnabled(true);
        if (imageview_ToggleChannelR)
            imageview_ToggleChannelR->setEnabled(true);
        if (imageview_ToggleChannelG)
            imageview_ToggleChannelG->setEnabled(true);
        if (imageview_ToggleChannelB)
            imageview_ToggleChannelB->setEnabled(true);
        if (imageview_ToggleChannelA)
            imageview_ToggleChannelA->setEnabled(true);
        if (imageview_ToggleGrayScale)
            imageview_ToggleGrayScale->setEnabled(true);
        if (imageview_InvertImage)
            imageview_InvertImage->setEnabled(true);
        if (imageview_MirrorHorizontal)
            imageview_MirrorHorizontal->setEnabled(true);
        if (imageview_MirrorVirtical)
            imageview_MirrorVirtical->setEnabled(true);
        if (imageview_RotateRight)
            imageview_RotateRight->setEnabled(true);
        if (imageview_RotateLeft)
            imageview_RotateLeft->setEnabled(true);
        if (m_BrightnessLevel)
            m_BrightnessLevel->setEnabled(true);
        if (m_CBimageview_ToolList)
            m_CBimageview_ToolList->setEnabled(true);

        break;
    }
}

void cpImageView::InitData()
{
    m_imageSize                     = {0, 0};
    ID                              = 0;
    m_localMipImages                = false;
    m_MipLevels                     = 0;
    m_DepthLevel                    = 0;
    m_MaxDepthLevel                 = 1;
    m_FitOnShow                     = true;
    m_imageLoader                   = NULL;
    m_acImageView                   = NULL;
    m_processedMipImages            = NULL;
    m_newWidget                     = NULL;
    m_layout                        = NULL;
    m_parent                        = NULL;
    m_button2                       = NULL;
    m_button                        = NULL;
    m_toolBar                       = NULL;
    m_statusBar                     = NULL;
    m_buttonNavigate                = NULL;
    m_labelColorTxt                 = NULL;
    m_labelColorRGBA                = NULL;
    m_labelPos                      = NULL;
    m_labelBlockPos                 = NULL;
    m_pMyWidget                     = NULL;
    m_pixmap                        = NULL;
    imageview_ResetImageView        = NULL;
    imageview_ToggleChannelR        = NULL;
    imageview_ToggleChannelG        = NULL;
    imageview_ToggleChannelB        = NULL;
    imageview_ToggleChannelA        = NULL;
    imageview_ToggleGrayScale       = NULL;
    imageview_InvertImage           = NULL;
    imageview_MirrorHorizontal      = NULL;
    imageview_MirrorVirtical        = NULL;
    imageview_RotateRight           = NULL;
    imageview_RotateLeft            = NULL;
    imageview_ZoomIn                = NULL;
    imageview_ZoomOut               = NULL;
    imageview_ViewImageOriginalSize = NULL;
    imageview_FitInWindow           = NULL;
    imageview_ImageDiff             = NULL;
    m_CBimageview_GridBackground    = NULL;
    m_CBimageview_ToolList          = NULL;
    m_CBimageview_MipLevel          = NULL;
    m_CBimageview_DepthLevel        = NULL;
    m_CBimage_DecompressUsing       = NULL;
    m_ExrProperties                 = NULL;
    m_OriginalMipImages             = NULL;
    m_bOnacScaleChange              = false;
    m_useOriginalImageCursor        = false;
    XBlockNum                       = 1;
    YBlockNum                       = 1;
    m_source_BlockXPos              = 1;
    m_source_BlockYPos              = 1;
    m_DiffOnOff                     = false;
    m_ImageViewState                = eImageViewState::isProcessed;
}

cpImageView::cpImageView(const QString filePathName,
                         const QString Title,
                         QWidget*      parent,
                         CMipImages*   MipImages,
                         Setting*      setting,
                         CMipImages*   CompressedMipImages)
    : acCustomDockWidget(filePathName, parent)
{
    if (!setting)
        return;
    InitData();
    m_parent                 = parent;
    m_fileName               = filePathName;
    m_setting                = *setting;
    m_localMipImages         = false;  // Flags if we used our own MipImage and not from parameter
    m_CBimageview_MipLevel   = NULL;
    m_CBimageview_DepthLevel = NULL;
    m_CMips                  = new CMIPS();
    Plastique_style          = QStyleFactory::create("Plastique");

    getSupportedImageFormats();

    if (MipImages)
    {
        if (setting->reloadImage && !setting->generateDiff && !setting->generateMips)
        {
            m_imageLoader = new CImageLoader();
            if (m_imageLoader)
            {
                m_processedMipImages = m_imageLoader->LoadPluginImage(filePathName.toStdString());
            }
        }
        else
            m_processedMipImages = MipImages;
    }
    else
    {
        m_imageLoader = new CImageLoader();
        if (m_imageLoader)
        {
            m_processedMipImages = m_imageLoader->LoadPluginImage(filePathName.toStdString());
            m_localMipImages     = true;
        }
        else
            m_processedMipImages = NULL;
    }

    // if our current image is compressed, Are we suppiled with a pointer to its uncomprssed source image miplevels
    // else we just use the current miplevels, this pointer can be either compressed or uncompressed data
    // check its MIPIMAGE_FORMAT property to determine which one it is

    if (CompressedMipImages)
    {
        if (CompressedMipImages->mipset)
        {
            m_CompressedMipImages = CompressedMipImages->mipset->m_compressed;
        }
        else
        {
            m_CompressedMipImages = true;
        }
        m_OriginalMipImages = CompressedMipImages;
        m_ImageViewState    = eImageViewState::isProcessed;
    }
    else
    {
        m_CompressedMipImages = false;
        m_OriginalMipImages   = m_processedMipImages;
        m_ImageViewState      = eImageViewState::isOriginal;
    }

    QFile     f(filePathName);
    QFileInfo fileInfo(f.fileName());
    m_tabName = fileInfo.fileName();

    setWindowTitle(m_tabName);

    this->m_CustomTitle = Title;
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
    if (m_processedMipImages)
    {
        if (m_processedMipImages->mipset)
        {
            m_MipLevels     = m_processedMipImages->mipset->m_nMipLevels;
            m_MaxDepthLevel = m_processedMipImages->mipset->m_nDepth;
            // check levels with number of images to view
            //if (m_processedMipImages->m_MipImageFormat == MIPIMAGE_FORMAT::Format_QImage)
            //{
            int count = (int)m_processedMipImages->QImage_list[0].size();
            if (count <= 1)
            {
                m_MipLevels = 0;
            }
            //}
        }
    }
    else
    {
        m_MipLevels  = 0;
        m_DepthLevel = 0;
    }

    //================================
    // Image/Texture Viewer Component
    //================================
    m_acImageView = new acImageView(filePathName, this, m_OriginalMipImages, m_processedMipImages);

    m_viewContextMenu = new QMenu(m_acImageView);

    // Image View Context Menu Item
    m_acImageView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_acImageView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(onViewCustomContextMenu(const QPoint&)));

    actSaveView = new QAction("Save View as...", this);
    connect(actSaveView, SIGNAL(triggered()), this, SLOT(onSaveViewAs()));
    m_viewContextMenu->addAction(actSaveView);

    actSaveBlockView = new QAction(" ", this);  // Text for this action item is auto set prior to context menu view
    connect(actSaveBlockView, SIGNAL(triggered()), this, SLOT(onSaveBlockView()));
    m_viewContextMenu->addAction(actSaveBlockView);

    if (Title.contains("File#"))
    {
        custTitleBar->setTitle(Title + ": " + filePathName);
    }
    else
    {
        // Need to check if MipImages is valid here!!
        if (m_processedMipImages)
        {
            QString gpuView    = "";
            bool    useGPUView = false;
            switch (m_processedMipImages->m_DecompressedFormat)
            {
            case MIPIMAGE_FORMAT_DECOMPRESSED::Format_CPU:
                custTitleBar->setTitle("Compressed Image: CPU View");
                break;
            case MIPIMAGE_FORMAT_DECOMPRESSED::Format_GPU:
                useGPUView = true;
                gpuView    = "Compressed Image: GPU View ";
                break;
            default:
            case MIPIMAGE_FORMAT_DECOMPRESSED::Format_NONE:
                break;
            }

            if (useGPUView)
            {
                switch (m_processedMipImages->m_MipImageFormat)
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
        ID          = m_acImageView->m_graphicsScene->ID;
        m_imageSize = m_acImageView->m_MipImages->QImage_list[0].front()->size();
        m_acImageView->enableNavigation(true);

        connect(m_acImageView, SIGNAL(acImageViewMousePosition(QPointF*, QPointF*, int)), this, SLOT(oncpImageViewMousePosition(QPointF*, QPointF*, int)));
        connect(m_acImageView,
                SIGNAL(acImageViewVirtualMousePosition(QPointF*, QPointF*, int)),
                this,
                SLOT(oncpImageViewVirtualMousePosition(QPointF*, QPointF*, int)));
        connect(custTitleBar, SIGNAL(ToolBarCliked()), this, SLOT(OnToolBarClicked()));
    }

    //  //===============
    //  // Virtual mouse with block
    //  //===============
    //  connect(this->m_acImageView, SIGNAL(acImageViewMousePosition(QPointF *, QPointF *, int)), &m_customMouse, SLOT(onVirtualMouseMoveEvent(QPointF *, QPointF *, int)));
    //
    connect(&m_customMouse, SIGNAL(VirtialMousePosition(QPointF*, QPointF*, int)), this->m_acImageView, SLOT(onVirtualMouseMoveEvent(QPointF*, QPointF*, int)));

    //===============
    // Tool Bar
    //===============
    m_toolBar = new QToolBar("Tools");
    m_toolBar->setStyleSheet("QToolBar{spacing: 0px;} QToolButton {width:15px;} ");
    m_toolBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    // Grid Background

    QLabel* GridLabel = new QLabel(this);
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
        QLabel* ZoomLabel = new QLabel(this);
        ZoomLabel->setText("Zoom:");
        m_toolBar->addWidget(ZoomLabel);
        connect(m_ZoomLevel, SIGNAL(valueChanged(int)), this, SLOT(onZoomLevelChanged(int)));
        connect(this, SIGNAL(OnSetScale(int)), m_acImageView, SLOT(onSetScale(int)));
        connect(m_acImageView, SIGNAL(acScaleChanged(int)), this, SLOT(onacScaleChange(int)));
        connect(m_acImageView, SIGNAL(acPSNRUpdated(double)), this, SLOT(onacPSNRUpdated(double)));

        m_ZoomLevel->setRange(AC_IMAGE_MIN_ZOOM, AC_IMAGE_MAX_ZOOM);
        m_ZoomLevel->setSingleStep(10);
        m_ZoomLevel->setValue(100);
        m_toolBar->addWidget(m_ZoomLevel);
    }

    //always enable brightness icons in gui and cursor indicates original RGBA data
    this->m_useOriginalImageCursor = true;
    setting->onBrightness          = true;
    m_BrightnessLevel              = new QSpinBox(this);
    if (m_BrightnessLevel)
    {
        QLabel* ZoomLabel = new QLabel(this);
        ZoomLabel->setText(" Brightness:");
        m_toolBar->addWidget(ZoomLabel);
        connect(m_BrightnessLevel, SIGNAL(valueChanged(int)), this, SLOT(onBrightnessLevelChanged(int)));
        m_BrightnessLevel->setRange(-100, 100);
        m_BrightnessLevel->setSingleStep(1);
        m_BrightnessLevel->setValue(0);
        m_toolBar->addWidget(m_BrightnessLevel);
    }

    imageview_ViewImageOriginalSize = new QAction(QIcon(":/compressonatorgui/images/cx100.png"), tr("Original Size"), this);
    if (imageview_ViewImageOriginalSize)
    {
        m_toolBar->addAction(imageview_ViewImageOriginalSize);
        connect(imageview_ViewImageOriginalSize, SIGNAL(triggered()), m_acImageView, SLOT(onViewImageOriginalSize()));
    }

    imageview_FitInWindow = new QAction(QIcon(":/compressonatorgui/images/cxfit.png"), tr("&Fit in Window"), this);
    if (imageview_FitInWindow)
    {
        m_toolBar->addAction(imageview_FitInWindow);
        connect(imageview_FitInWindow, SIGNAL(triggered()), m_acImageView, SLOT(onFitInWindow()));
    }

    imageview_ResetImageView = new QAction(QIcon(":/compressonatorgui/images/originalimage.png"), tr("Reset Image View"), this);
    if (imageview_ResetImageView)
    {
        m_toolBar->addAction(imageview_ResetImageView);
        connect(imageview_ResetImageView, SIGNAL(triggered()), m_acImageView, SLOT(onResetImageView()));
        connect(imageview_ResetImageView, SIGNAL(triggered()), this, SLOT(oncpResetImageView()));
    }

    m_toolBar->addSeparator();

    imageview_ToggleChannelR = new QAction(QIcon(":/compressonatorgui/images/cxred.png"), tr("Show or Hide  Red channel"), this);
    if (imageview_ToggleChannelR)
    {
        imageview_ToggleChannelR->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelR);
        connect(imageview_ToggleChannelR, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelR()));
    }

    imageview_ToggleChannelG = new QAction(QIcon(":/compressonatorgui/images/cxgreen.png"), tr("Show or Hide Green channel"), this);
    if (imageview_ToggleChannelG)
    {
        imageview_ToggleChannelG->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelG);
        connect(imageview_ToggleChannelG, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelG()));
    }

    imageview_ToggleChannelB = new QAction(QIcon(":/compressonatorgui/images/cxblue.png"), tr("Show or Hide Blue channel"), this);
    if (imageview_ToggleChannelB)
    {
        imageview_ToggleChannelB->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelB);
        connect(imageview_ToggleChannelB, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelB()));
    }

    imageview_ToggleChannelA = new QAction(QIcon(":/compressonatorgui/images/cxalpha.png"), tr("Show or Hide Alpha channel"), this);
    if (imageview_ToggleChannelA)
    {
        imageview_ToggleChannelA->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleChannelA);
        connect(imageview_ToggleChannelA, SIGNAL(triggered()), m_acImageView, SLOT(onToggleChannelA()));
    }

    m_toolBar->addSeparator();

    imageview_ToggleGrayScale = new QAction(QIcon(":/compressonatorgui/images/cxgrayscale.png"), tr("Gray Scale"), this);
    if (imageview_ToggleGrayScale)
    {
        imageview_ToggleGrayScale->setCheckable(true);
        m_toolBar->addAction(imageview_ToggleGrayScale);
        connect(imageview_ToggleGrayScale, SIGNAL(triggered()), m_acImageView, SLOT(onToggleGrayScale()));
    }

    imageview_InvertImage = new QAction(QIcon(":/compressonatorgui/images/cxinvert.png"), tr("Invert Image"), this);
    if (imageview_InvertImage)
    {
        m_toolBar->addAction(imageview_InvertImage);
        connect(imageview_InvertImage, SIGNAL(triggered()), m_acImageView, SLOT(onInvertImage()));
    }

    imageview_MirrorHorizontal = new QAction(QIcon(":/compressonatorgui/images/mirrorhorizonal.png"), tr("Mirror Image Horizontally"), this);
    if (imageview_MirrorHorizontal)
    {
        m_toolBar->addAction(imageview_MirrorHorizontal);
        connect(imageview_MirrorHorizontal, SIGNAL(triggered()), m_acImageView, SLOT(onMirrorHorizontal()));
    }

    imageview_MirrorVirtical = new QAction(QIcon(":/compressonatorgui/images/mirrorvertical.png"), tr("Mirror Image Vertically"), this);
    if (imageview_MirrorVirtical)
    {
        m_toolBar->addAction(imageview_MirrorVirtical);
        connect(imageview_MirrorVirtical, SIGNAL(triggered()), m_acImageView, SLOT(onMirrorVirtical()));
    }

    imageview_RotateRight = new QAction(QIcon(":/compressonatorgui/images/cxrotationr.png"), tr("Rotate Image 90 Degrees"), this);
    if (imageview_RotateRight)
    {
        m_toolBar->addAction(imageview_RotateRight);
        connect(imageview_RotateRight, SIGNAL(triggered()), m_acImageView, SLOT(onRotateRight()));
    }

    imageview_RotateLeft = new QAction(QIcon(":/compressonatorgui/images/cxrotationl.png"), tr("Rotate Image -90 Degrees"), this);
    if (imageview_RotateLeft)
    {
        m_toolBar->addAction(imageview_RotateLeft);
        connect(imageview_RotateLeft, SIGNAL(triggered()), m_acImageView, SLOT(onRotateLeft()));
    }

    if (m_MaxDepthLevel > 1)
    {
        m_toolBar->addSeparator();
        QLabel* MDepthLevelLabel = new QLabel(this);
        MDepthLevelLabel->setText("Frame:");
        m_toolBar->addWidget(MDepthLevelLabel);
        m_CBimageview_DepthLevel = new QComboBox(this);

        for (int num = 0; num < m_MaxDepthLevel; num++)
        {
            QString depthLevelList = QString::number(num + 1);
            m_CBimageview_DepthLevel->addItem(depthLevelList);
        }

        connect(m_CBimageview_DepthLevel, SIGNAL(currentIndexChanged(int)), m_acImageView, SLOT(onImageDepthChanged(int)));
        m_toolBar->addWidget(m_CBimageview_DepthLevel);
    }

    if (m_MipLevels > 0)
    {
        m_toolBar->addSeparator();
        QLabel* MipLevelLabel = new QLabel(this);
        MipLevelLabel->setText("MipLevel:");
        m_toolBar->addWidget(MipLevelLabel);

        m_CBimageview_MipLevel = new QComboBox(this);

        int processedImage_miplevel_max = (int)m_processedMipImages->QImage_list[0].size();

        // check if we have miplevels in Original Image if its available match its level with the processed
        if (m_OriginalMipImages && (setting->input_image == eImageViewState::isProcessed))
        {
            if (m_OriginalMipImages->QImage_list[0].size() < processedImage_miplevel_max)
            {
                // Remove this
                // QMessageBox msgBox;
                // QMessageBox::warning(this,
                //                      "MipLevel",
                //                      "Original image MipMap Levels do not match the Processed image levels.\nLevels will be limited, retry by regenerating "
                //                      "original image mip levels",
                //                      QMessageBox::Ok);
                processedImage_miplevel_max = (int)m_OriginalMipImages->QImage_list[0].size();
            }
        }

        for (int num = 0; num < m_MipLevels; num++)
        {
            if (m_processedMipImages)
            {
                if (processedImage_miplevel_max > num)
                {
                    QString mipLevelList = QString::number(num + 1);
                    QImage* image        = m_processedMipImages->QImage_list[0][num];
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
        connect(m_CBimageview_MipLevel, SIGNAL(currentIndexChanged(int)), m_acImageView, SLOT(onImageMipLevelChanged(int)));
        connect(m_CBimageview_MipLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(onResetHDRandDiff(int)));
        m_toolBar->addWidget(m_CBimageview_MipLevel);
    }

    m_toolBar->addSeparator();

    if (setting->input_image == eImageViewState::isProcessed)
    {
        // This combo box is used as one of two methods to changes image view states, the other is user keypress
        m_CBimageview_Toggle = new QComboBox(this);
        if (m_CBimageview_Toggle)
        {
            m_CBimageview_Toggle->addItem(tr(TXT_IMAGE_PROCESSED));
            m_CBimageview_Toggle->addItem(tr(TXT_IMAGE_ORIGINAL));
            connect(m_CBimageview_Toggle, SIGNAL(currentIndexChanged(int)), this, SLOT(onToggleViewChanged(int)));
            m_CBimageview_Toggle->setToolTip(tr("Switch views to 'Processed' or 'Original' Image [can use 'p','o' or 'space bar' to toggles views"));
            m_toolBar->addWidget(m_CBimageview_Toggle);
        }

        imageview_ImageDiff =
            new QAction(QIcon(":/compressonatorgui/images/imagediff.png"), tr("&View Image Diff [can use 'd' key to toggle on or off]"), this);
        if (imageview_ImageDiff)
        {
            m_toolBar->addAction(imageview_ImageDiff);
            connect(imageview_ImageDiff, SIGNAL(triggered()), this, SLOT(onImageDiff()));
        }
        m_toolBar->addSeparator();
    }
    else
        m_CBimageview_Toggle = NULL;

#ifdef USE_BCN_IMAGE_DEBUG
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
#endif

    if (m_processedMipImages && (m_processedMipImages->mipset != NULL))
    {
        if (m_processedMipImages->mipset->m_format == CMP_FORMAT_ARGB_32F || (m_processedMipImages->mipset->m_format == CMP_FORMAT_ARGB_16F) ||
            (m_processedMipImages->mipset->m_format == CMP_FORMAT_RGBA_16F) || (m_processedMipImages->mipset->m_format == CMP_FORMAT_RGBE_32F) ||
            (m_processedMipImages->mipset->m_format == CMP_FORMAT_BC6H) || (m_processedMipImages->mipset->m_format == CMP_FORMAT_BC6H_SF))
        {
            m_ExrProperties = new acEXRTool();
            // Tool list
            QLabel* GridLabel = new QLabel(this);
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
                connect(m_ExrProperties->exrGammaBox, SIGNAL(valueChanged(double)), m_acImageView, SLOT(onExrGammaChanged(double)));
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

    m_PSNRLabel = new QLabel(this);
    m_PSNRLabel->setText("PSNR: N/A");

    m_toolBar->setMaximumHeight(25);

    m_toolBar->hide();

    QHBoxLayout* hlayout2 = new QHBoxLayout;
    hlayout2->setSpacing(0);
    hlayout2->setMargin(0);
    hlayout2->setContentsMargins(0, 0, 0, 0);
    hlayout2->addWidget(m_toolBar, 0);

    QString Navigation = QStringLiteral(":/compressonatorgui/images/navigate.png");

    m_statusBar = new QStatusBar(this);
    m_statusBar->setStyleSheet("QStatusBar{border-top: 1px outset grey; border-bottom: 1px outset grey;}");

    m_labelColorRGBA = new QLabel(this);
    m_labelColorRGBA->setAlignment(Qt::AlignLeft);
    m_labelColorRGBA->setAutoFillBackground(true);

    QPalette sample_palette;
    sample_palette.setColor(QPalette::Window, Qt::black);
    sample_palette.setColor(QPalette::WindowText, Qt::black);
    m_labelColorRGBA->setPalette(sample_palette);
    m_labelColorRGBA->setText("RGBA");

    m_labelColorTxt = new QLabel(this);
    m_labelColorTxt->setText("0,0,0,0");
    m_labelColorTxt->setAlignment(Qt::AlignLeft);

    m_labelPos = new QLabel(this);
    m_labelPos->setText("");
    m_labelPos->setAlignment(Qt::AlignLeft);

    m_labelBlockPos = new QLabel(this);
    m_labelBlockPos->setText("");
    m_labelBlockPos->setAlignment(Qt::AlignLeft);

    m_labelTxtView = new QLabel(this);
    switch (setting->input_image)
    {
    case eImageViewState::isOriginal:
        m_ImageViewState = eImageViewState::isOriginal;
        m_labelTxtView->setText(TXT_IMAGE_ORIGINAL);
        break;
    case eImageViewState::isDiff:
        m_ImageViewState = eImageViewState::isDiff;
        // Set a default Contrast for image diff views
        if (m_acImageView)
        {
            if (m_acImageView->m_imageItem_Processed)
            {
                m_acImageView->m_imageItem_Processed->m_fContrast = g_Application_Options.m_imagediff_contrast;
                m_acImageView->setBrightnessLevel(0);
            }
        }
        m_labelTxtView->setText(TXT_IMAGE_DIFF);
        setActionForImageViewStateChange();
        break;
    case eImageViewState::isProcessed:
        m_ImageViewState = eImageViewState::isProcessed;
        m_labelTxtView->setText(TXT_IMAGE_PROCESSED);
        break;
    }

    m_labelTxtView->setAlignment(Qt::AlignLeft);

    m_statusBar->addPermanentWidget(m_labelColorRGBA);
    m_statusBar->addPermanentWidget(m_labelTxtView, 10);
    m_statusBar->addPermanentWidget(m_PSNRLabel, 15);
    m_statusBar->addPermanentWidget(m_labelColorTxt, 40);
    m_statusBar->addPermanentWidget(m_labelPos, 40);
    m_statusBar->addPermanentWidget(m_labelBlockPos, 20);

    m_layout = new QGridLayout(m_newWidget);

    m_layout->setSpacing(0);
    m_layout->setMargin(0);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(hlayout2, 0, 0);
    m_layout->addWidget(m_acImageView, 1, 0);
    m_layout->addWidget(m_statusBar, 2, 0);

    m_newWidget->setLayout(m_layout);

    setWidget(m_newWidget);

    //  Process any quality stats
    m_acImageView->processPSNR();
}

// User selected a view from drop down combo box or called from a key event
void cpImageView::onToggleViewChanged(int view)
{
    if (!m_CBimageview_Toggle)
        return;

    QString itemView = m_CBimageview_Toggle->itemText(view);

    if (itemView.compare(TXT_IMAGE_PROCESSED) == 0)
    {
        m_ImageViewState = eImageViewState::isProcessed;
        m_labelTxtView->setText(TXT_IMAGE_PROCESSED);
    }
    else if (itemView.compare(TXT_IMAGE_DIFF) == 0)
    {
        m_ImageViewState = eImageViewState::isDiff;
        m_labelTxtView->setText(TXT_IMAGE_DIFF);
    }
    else if (itemView.compare(TXT_IMAGE_ORIGINAL) == 0)
    {
        m_ImageViewState = eImageViewState::isOriginal;
        m_labelTxtView->setText(TXT_IMAGE_ORIGINAL);
    }
    else
    {
        // send an error message ...
        return;
    }

    setActionForImageViewStateChange();

    // acImage view has two states 0 = Processed and 1 = Original
    if (m_acImageView)
        m_acImageView->onToggleImageViews(view);
}

void cpImageView::oncpResetImageView()
{
    imageview_ToggleChannelR->setChecked(false);
    imageview_ToggleChannelG->setChecked(false);
    imageview_ToggleChannelB->setChecked(false);
    imageview_ToggleChannelA->setChecked(false);
    imageview_ToggleGrayScale->setCheckable(false);

    onResetHDRandDiff(0);
    m_BrightnessLevel->setValue(0);
}

void cpImageView::onDecompressUsing(int useDecomp)
{
    Q_UNUSED(useDecomp);
}

void cpImageView::onResetHDRandDiff(int MipLevel)
{
    // on a MipLevel Change Diff views are reset back to processed views
    // Makesure the text in the image view ComboBox lable is "Processed"
    if (MipLevel > 0)
    {
        if (m_CBimageview_Toggle)
        {
            if (m_ImageViewState == eImageViewState::isDiff)
            {
                onImageDiff();
            }
        }
    }

    if (m_ExrProperties)
    {
        if (m_ExrProperties->isVisible())
            m_ExrProperties->hide();
        m_ExrProperties->exrExposureBox->setValue(DEFAULT_EXPOSURE);
        m_ExrProperties->exrDefogBox->setValue(DEFAULT_DEFOG);
        m_ExrProperties->exrKneeLowBox->setValue(DEFAULT_KNEELOW);
        m_ExrProperties->exrKneeHighBox->setValue(DEFAULT_KNEEHIGH);
        m_ExrProperties->exrGammaBox->setValue(DEFAULT_GAMMA);
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

void cpImageView::onViewCustomContextMenu(const QPoint& point)
{
    if (actSaveBlockView)
    {
        QString strSaveBlock;
        strSaveBlock.sprintf("Save Source Block (%d,%d) as", m_source_BlockXPos, m_source_BlockYPos);
        actSaveBlockView->setText(strSaveBlock);
    }
    m_viewContextMenu->exec(m_acImageView->mapToGlobal(point));
}

void cpImageView::onSaveViewAs()
{
    if (m_acImageView && actSaveBlockView)
    {
        QString ImageFilter;
        ImageFilter       = m_QtImageFilter;
        bool hasFloatData = false;

        // Add EXR if source is HDR
        if (m_processedMipImages)
        {
            if ((m_processedMipImages->mipset->m_ChannelFormat == CF_Float16) || (m_processedMipImages->mipset->m_ChannelFormat == CF_Float32))
            {
                ImageFilter.insert(ImageFilter.length() - 1, "*.exr;");
            }
        }

        if (!hasFloatData)
            ImageFilter.insert(ImageFilter.length() - 1, "*.bmp;");

        QFileInfo fileInfo(m_fileName);
        QDir      dir(fileInfo.absoluteDir());
        QString   SuggetedFileNamePath;
        SuggetedFileNamePath = dir.absolutePath();
        SuggetedFileNamePath.append("/");
        SuggetedFileNamePath.append(fileInfo.baseName());

        // Set suggested file target type
        if (hasFloatData)
        {
            SuggetedFileNamePath.append("_view.dds");
        }
        else
            SuggetedFileNamePath.append("_view.bmp");

        std::string ext;
        QString     filePathName;
        bool        done = false;
        do
        {
            filePathName = QFileDialog::getSaveFileName(this, tr("Save Image View as"), SuggetedFileNamePath, ImageFilter);
            if (filePathName.length() == 0)
                return;
            ext = CMP_GetFilePathExtension(filePathName.toStdString());
            transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            string supported_ExtListings = ImageFilter.toStdString();
            if (supported_ExtListings.find(ext) != std::string::npos)
            {
                done = true;
            }
            else
            {
                if (QMessageBox::question(this, "Save Image View", "File extension is not supported try again?", QMessageBox::Yes | QMessageBox::No) ==
                    QMessageBox::No)
                    return;
            }

        } while (!done);

        // get file extension to choose for Qt file or Compressonator File save using .dds,.ktx or .exr plugins
        if ((ext.compare("exr") == 0) || (ext.compare("dds") == 0) || (ext.compare("ktx") == 0) || (ext.compare("ktx2") == 0))
        {
            int mipLevel = 0;
            if (m_CBimageview_MipLevel)
            {
                mipLevel = m_CBimageview_MipLevel->currentIndex();
            }

            int depthLevel = 0;
            if (m_CBimageview_DepthLevel)
            {
                depthLevel = m_CBimageview_DepthLevel->currentIndex();
            }

            if (m_processedMipImages && hasFloatData)
            {
                if (mipLevel > 0)
                {
                    MipLevel* pInMipLevel = m_CMips->GetMipLevel(m_processedMipImages->mipset, mipLevel, depthLevel);

                    if (pInMipLevel == NULL)
                    {
                        PrintInfo("Error: MipLevel Data failed to retrieved.");
                        return;
                    }

                    // Create a temporary mipset for saving the miplevel data
                    MipSet* pMipLevelMipSet;
                    pMipLevelMipSet = new MipSet();
                    if (pMipLevelMipSet == NULL)
                    {
                        PrintInfo("Error: Failed to allocate mipset for saving.");
                        return;
                    }
                    memset(pMipLevelMipSet, 0, sizeof(MipSet));

                    // Set the channel formats and mip levels
                    pMipLevelMipSet->m_ChannelFormat   = m_processedMipImages->mipset->m_ChannelFormat;
                    pMipLevelMipSet->m_TextureDataType = m_processedMipImages->mipset->m_TextureDataType;
                    pMipLevelMipSet->m_dwFourCC        = m_processedMipImages->mipset->m_dwFourCC;
                    pMipLevelMipSet->m_dwFourCC2       = m_processedMipImages->mipset->m_dwFourCC2;
                    pMipLevelMipSet->m_TextureType     = m_processedMipImages->mipset->m_TextureType;
                    pMipLevelMipSet->m_nWidth          = m_processedMipImages->QImage_list[0][mipLevel]->width();
                    pMipLevelMipSet->m_nHeight         = m_processedMipImages->QImage_list[0][mipLevel]->height();
                    pMipLevelMipSet->m_nDepth          = m_processedMipImages->mipset->m_nDepth;  // depthsupport
                    if (pMipLevelMipSet->m_nDepth == 0)
                        pMipLevelMipSet->m_nDepth = 1;

                    // Allocate default MipSet header
                    m_CMips->AllocateMipSet(pMipLevelMipSet,
                                            pMipLevelMipSet->m_ChannelFormat,
                                            pMipLevelMipSet->m_TextureDataType,
                                            pMipLevelMipSet->m_TextureType,
                                            m_processedMipImages->QImage_list[0][mipLevel]->width(),
                                            m_processedMipImages->QImage_list[0][mipLevel]->height(),
                                            pMipLevelMipSet->m_nDepth);

                    // Determin buffer size and set Mip Set Levels we want to use for now
                    MipLevel* mipLevelInfo        = m_CMips->GetMipLevel(pMipLevelMipSet, mipLevel, depthLevel);
                    pMipLevelMipSet->m_nMipLevels = 1;
                    m_CMips->AllocateMipLevelData(mipLevelInfo,
                                                  pMipLevelMipSet->m_nWidth,
                                                  pMipLevelMipSet->m_nHeight,
                                                  pMipLevelMipSet->m_ChannelFormat,
                                                  pMipLevelMipSet->m_TextureDataType);

                    // We have allocated a data buffer to fill get its referance
                    mipLevelInfo->m_pbData = pInMipLevel->m_pbData;
                    AMDSaveMIPSTextureImage(filePathName.toStdString().c_str(), pMipLevelMipSet, false, g_CmdPrams.CompressOptions);

                    // delete the temporary mipset used for saving
                    delete pMipLevelMipSet;
                }
                else
                    AMDSaveMIPSTextureImage(filePathName.toStdString().c_str(), m_processedMipImages->mipset, false, g_CmdPrams.CompressOptions);
            }
            else
            {
                if (m_OriginalMipImages)
                {
                    AMDSaveMIPSTextureImage(filePathName.toStdString().c_str(), m_OriginalMipImages->mipset, false, g_CmdPrams.CompressOptions);
                }
            }
        }
        else
        {
            QPixmap pixmap = m_acImageView->m_imageItem_Processed->pixmap();
            QImage  img    = pixmap.toImage();
            img.save(filePathName);
        }
    }
}

void cpImageView::getSupportedImageFormats()
{
    m_QtImageFilter = "Images (";

#ifdef USE_SaveViewAs_ALL_FILE_FORMATS
    // Get a list of all Supported file formats from Qt Plugins
    QList<QByteArray> QtFormats = QImageReader::supportedImageFormats();

    // Upppercase List
    QList<QByteArray>::Iterator i;
    for (i = QtFormats.begin(); i != QtFormats.end(); ++i)
    {
        QByteArray fformat = (*i);
        fformat            = fformat.toUpper();
        m_QtImageFilter.append("*.");
        m_QtImageFilter.append(fformat);
        m_QtImageFilter.append(";");
    }

    // Add DDS and KTX
    m_QtImageFilter.append("*.dds;*.ktx;");
#else
    m_QtImageFilter.append("*.dds;");
#endif
    m_QtImageFilter.append(")");
}

void cpImageView::onSaveBlockView()
{
    if (m_acImageView && m_OriginalMipImages)
    {
        QString ImageFilter;

        if ((m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float16) || (m_OriginalMipImages->mipset->m_ChannelFormat == CF_Float32))
            ImageFilter = "Image files (*.exr)";
        else
            ImageFilter = "Image files (*.bmp)";
        ;

        QFileInfo fileInfo(m_fileName);
        QDir      dir(fileInfo.absoluteDir());
        QString   SuggetedFileNamePath;
        SuggetedFileNamePath = dir.absolutePath();
        SuggetedFileNamePath.append("/");
        SuggetedFileNamePath.append(fileInfo.baseName());
        SuggetedFileNamePath.append("_" + QString::number(m_source_BlockXPos) + "_" + QString::number(m_source_BlockYPos));

        std::string ext;
        QString     filePathName;
        bool        done = false;
        do
        {
            filePathName = QFileDialog::getSaveFileName(this, tr("Save Block Image as"), SuggetedFileNamePath, ImageFilter);
            if (filePathName.length() == 0)
                return;
            ext = CMP_GetFilePathExtension(filePathName.toStdString());
            transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            string supported_ExtListings = ImageFilter.toStdString();
            if (supported_ExtListings.find(ext) != std::string::npos)
            {
                done = true;
            }
            else
            {
                if (QMessageBox::question(this, "Save Block Image", "File extension is not supported try again?", QMessageBox::Yes | QMessageBox::No) ==
                    QMessageBox::No)
                    return;
            }
        } while (!done);

        // Create the 4x4 image block from MipSet data and save to file
        GetSourceBlock(m_source_BlockXPos, m_source_BlockYPos, filePathName.toStdString());
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

void cpImageView::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if (m_FitOnShow)
    {
        imageview_FitInWindow->trigger();
        m_FitOnShow = false;
    }

    if (m_CBimageview_MipLevel)
    {
        if ((m_processedMipImages) && (m_CBimageview_MipLevel->isEnabled() == false))
        {
            // need to find root cause of 0xFEEEFEEE
            if ((m_processedMipImages->mipset) && (m_processedMipImages->mipset != (void*)0xFEEEFEEE))
            {
                if (m_MipLevels != m_processedMipImages->mipset->m_nMipLevels)
                {
                    EnableMipLevelDisplay(m_processedMipImages->mipset->m_nMipLevels);
                }
            }
        }
    }

    if (m_CBimageview_DepthLevel)
    {
        if ((m_processedMipImages) && (m_CBimageview_DepthLevel->isEnabled() == false))
        {
            // need to find root cause of 0xFEEEFEEE
            if ((m_processedMipImages->mipset) && (m_processedMipImages->mipset != (void*)0xFEEEFEEE))
            {
                if (m_MaxDepthLevel != m_processedMipImages->mipset->m_nDepth)
                {
                    EnableDepthLevelDisplay(m_processedMipImages->mipset->m_nDepth);
                }
            }
        }
    }
}

void cpImageView::showEvent(QShowEvent*)
{
}

void cpImageView::closeEvent(QCloseEvent*)
{
    if (m_ExrProperties)
    {
        if (m_ExrProperties->isVisible())
            m_ExrProperties->hide();
    }
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

void cpImageView::onacPSNRUpdated(double value)
{
    if (m_PSNRLabel && value > 0)
    {
        char buff[16];
        snprintf(buff, sizeof(buff), "PSNR: %3.2f dB", value);
        m_PSNRLabel->setText(buff);
    }
}

// This slot is received when user changes brightness level using tool bar zoom

void cpImageView::onBrightnessLevelChanged(int value)
{
    if (m_acImageView->m_imageItem_Processed)
    {
        m_acImageView->setBrightnessLevel(value);
    }
}
