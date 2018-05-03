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

#include <QtWidgets/qapplication.h>
#include "cpImageLoader.h"
#include "ImfStandardAttributes.h"
#include "ImathBox.h"
#include "ImfArray.h"
#include "ImfRgba.h"
#include <ImfArray.h>
#include "cExr.h"
#include <QtCore/qdebug.h>

bool g_useCPUDecode = true;
MIPIMAGE_FORMAT g_gpudecodeFormat = Format_OpenGL;

extern PluginManager g_pluginManager;
extern MipSet* DecompressMIPSet(MipSet *MipSetIn, CMP_GPUDecode decodeWith, Config* configSetting, CMP_Feedback_Proc pFeedbackProc);
extern QRgb RgbaToQrgba(struct Imf::Rgba imagePixel);
extern int    g_OpenGLMajorVersion;


CMP_FLOAT F16toF32(CMP_HALF f)
{
    half A;
    A.setBits(f);
    return((CMP_FLOAT)A);
}

CMP_HALF F32toF16(CMP_FLOAT f)
{
    return(half(f).bits());
}


CImageLoader::CImageLoader()
{
    m_CMips = new CMIPS();
    m_pluginManager = &g_pluginManager;
    kneeLow = AMD_CODEC_KNEELOW_DEFAULT;
    kneeHigh = AMD_CODEC_KNEEHIGH_DEFAULT;
    exposure = AMD_CODEC_EXPOSURE_DEFAULT;
    defog = (float)AMD_CODEC_DEFOG_DEFAULT;
    gamma = (float)AMD_CODEC_GAMMA_DEFAULT;
}

CImageLoader::CImageLoader(void *plugin)
{
    m_CMips = new CMIPS();
    m_pluginManager = (PluginManager *) plugin;
    kneeLow = 0;
    kneeHigh = 5;
    exposure = 0;
    defog = 0;
}


CImageLoader::~CImageLoader()
{
    if (m_CMips)
    {
        delete m_CMips;
        m_CMips = NULL;
    }
}

CMipImages::CMipImages()
{
    // Init pointers
    mipset = NULL;
    Image_list.clear();
    MIPS2QtFailed = false;
    m_MipImageFormat = MIPIMAGE_FORMAT::Format_QImage;
    m_Error = MIPIMAGE_FORMAT_ERRORS::Format_NoErrors;
    m_DecompressedFormat = MIPIMAGE_FORMAT_DECOMPRESSED::Format_NONE;
    decompressedMipSet = NULL;
    errMsg = "";
}


// Deletes all allocated CMipImage data
bool CImageLoader::clearMipImages(CMipImages *MipImages)
{
    if (!MipImages) return false;

    try
    {
        //if (MipImages->m_MipImageFormat == MIPIMAGE_FORMAT::Format_QImage)
        //{
            for (int i = 0; i < MipImages->Image_list.count(); i++)
            {
                QImage *image = MipImages->Image_list[i];
                if (image)
                {
                    delete image;
                    image = NULL;
                }
            }

            MipImages->Image_list.clear();
        //}

        if (MipImages->mipset)
            m_CMips->FreeMipSet(MipImages->mipset);

        if (MipImages->decompressedMipSet)
            m_CMips->FreeMipSet(MipImages->decompressedMipSet);

        delete MipImages;
        MipImages = NULL;


    }
    catch (...)
    {
        return false;
    }
    return true;
}


void CImageLoader::QImageFormatInfo(QImage *image)
{
    // QImage info
    int numofbitsperpixel = image->depth();
    int bytecount = image->byteCount();
    int bytesPerLine = image->bytesPerLine();
    int height = image->height();
    int numBytes = image->byteCount();
    int numColors = image->colorCount();
    QPixelFormat pixelFormat = image->pixelFormat();
    bool hasAlphaChannel = image->hasAlphaChannel();
    int width = image->width();
    QImage::Format format = image->format();

    // Stop compile messages for unused variables
    Q_UNUSED(numofbitsperpixel);
    Q_UNUSED(bytecount);
    Q_UNUSED(bytesPerLine);
    Q_UNUSED(height);
    Q_UNUSED(numBytes);
    Q_UNUSED(numColors);
    Q_UNUSED(pixelFormat);
    Q_UNUSED(hasAlphaChannel);
    Q_UNUSED(width);
    Q_UNUSED(format);
}


//load ARGB32 Qimage format to Mips
MipSet *CImageLoader::QImage2MIPS(QImage *qimage, CMP_Feedback_Proc pFeedbackProc)
{
    if (qimage == NULL)
    {
        return NULL;
    }

    // QImage info for debugging
    // QImageFormatInfo(qimage);

    // Check supported format
    int format = qimage->format();
    if (!(  (qimage->format() == QImage::Format_ARGB32) || 
            (qimage->format() == QImage::Format_ARGB32_Premultiplied) ||
            (qimage->format() == QImage::Format_RGB32) ||
            (qimage->format() == QImage::Format_Mono)  ||
            (qimage->format() == QImage::Format_Indexed8)))
    {
        return NULL;
    }

    MipSet *pMipSet;
    pMipSet = new MipSet();
    if (pMipSet == NULL)
        return (NULL);
    memset(pMipSet, 0, sizeof(MipSet));



    // Set the channel formats and mip levels
    pMipSet->m_ChannelFormat    = CF_8bit;
    pMipSet->m_TextureDataType  = TDT_ARGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_TextureType      = TT_2D;


    // Allocate default MipSet header
    m_CMips->AllocateMipSet(pMipSet,
                            pMipSet->m_ChannelFormat,
                            pMipSet->m_TextureDataType,
                            pMipSet->m_TextureType,
                            qimage->width(), 
                            qimage->height(), 
                            1);
    
    // Determin buffer size and set Mip Set Levels we want to use for now
    MipLevel *mipLevel = m_CMips->GetMipLevel(pMipSet, 0);
    pMipSet->m_nMipLevels = 1;
    m_CMips->AllocateMipLevelData(mipLevel, pMipSet->m_nWidth, pMipSet->m_nHeight, pMipSet->m_ChannelFormat, pMipSet->m_TextureDataType);

    // We have allocated a data buffer to fill get its referance
    CMP_BYTE* pData = (CMP_BYTE*)(mipLevel->m_pbData);

    QRgb qRGB;
    int i = 0;
    for (int y = 0; y < qimage->height(); y++){
        for (int x = 0; x < qimage->width();x++){
            qRGB = qimage->pixel(x, y);
            pData[i] = qRed(qRGB);
            i++;
            pData[i] = qGreen(qRGB);
            i++;
            pData[i] = qBlue(qRGB);
            i++;
            pData[i] = qAlpha(qRGB);
            i++;
        }
        if (pFeedbackProc)
        {
            float fProgress = 100.f * (y * qimage->width()) / (qimage->width() * qimage->height());
            if (pFeedbackProc(fProgress, NULL, NULL))
                return NULL;
        }
    }

    if (pMipSet->m_format == CMP_FORMAT_Unknown)
    {
        pMipSet->m_format = QFormat2MipFormat(qimage->format());
    }


    return pMipSet;
}

// Finds a matching Qt Image format for the Mip Set
// Qt V5.4 has
// Format_Mono,
// Format_MonoLSB,
// Format_Indexed8,
// Format_RGB32,
// Format_ARGB32,
// Format_ARGB32_Premultiplied,
// Format_RGB16,
// Format_ARGB8565_Premultiplied,
// Format_RGB666,
// Format_ARGB6666_Premultiplied,
// Format_RGB555,
// Format_ARGB8555_Premultiplied,
// Format_RGB888,
// Format_RGB444,
// Format_ARGB4444_Premultiplied,
// Format_RGBX8888,
// Format_RGBA8888,
// Format_RGBA8888_Premultiplied
// Format_BGR30,
// Format_A2BGR30_Premultiplied,
// Format_RGB30,
// Format_A2RGB30_Premultiplied,
//
QImage::Format CImageLoader::MipFormat2QFormat(MipSet *mipset)
{
    QImage::Format format = QImage::Format_Invalid;

    switch (mipset->m_ChannelFormat)
    {
        case CF_8bit        : {format = QImage::Format_ARGB32; break; }
        case CF_Float16     : {format = QImage::Format_ARGB32; break; }
        case CF_Float32     : {format = QImage::Format_ARGB32; break; }
        case CF_Float9995E  : {format = QImage::Format_ARGB32; break; }
        case CF_Compressed  : {break;}
        case CF_16bit       : {break;}
        case CF_2101010     : {break;}
        case CF_32bit       : {format = QImage::Format_ARGB32;  break;}
        default             : {break;}
    }

    return format;
}



CMP_FORMAT CImageLoader::QFormat2MipFormat(QImage::Format qformat)
{
    CMP_FORMAT format = CMP_FORMAT_Unknown;

    switch (qformat)
    {

    case QImage::Format_RGB32:
    
    // Swizzed
    case QImage::Format_RGBX8888:
    case QImage::Format_RGBA8888:
    case QImage::Format_RGBA8888_Premultiplied:

    // Not Swizzed
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied:
    case QImage::Format_Mono:
    case QImage::Format_MonoLSB:
    case QImage::Format_Indexed8:
        format = CMP_FORMAT_ARGB_8888;
         break;

    case QImage::Format_RGB888:
        format = CMP_FORMAT_RGB_888;
        break;

    case QImage::Format_Invalid:
    case QImage::Format_RGB16:
    case QImage::Format_ARGB8565_Premultiplied:
    case QImage::Format_RGB666:
    case QImage::Format_ARGB6666_Premultiplied:
    case QImage::Format_RGB555:
    case QImage::Format_ARGB8555_Premultiplied:
    case QImage::Format_RGB444:
    case QImage::Format_ARGB4444_Premultiplied:
    case QImage::Format_BGR30:
    case QImage::Format_A2BGR30_Premultiplied:
    case QImage::Format_RGB30:
    case QImage::Format_A2RGB30_Premultiplied:
    case QImage::Format_Alpha8:
    case QImage::Format_Grayscale8:
    default:
        format = CMP_FORMAT_Unknown;
        break;
    }

    return format;
}

MipSet *CImageLoader::LoaderDecompressMipSet(CMipImages *MipImages, Config *decompConfig)
{
    MipSet *tmpMipSet             = NULL;
    MipImages->decompressedMipSet = NULL;
    
    if ((MipImages->mipset->m_compressed) || (CompressedFormat(MipImages->mipset->m_format)))
    {
        //=======================================================
        // We use CPU based decode if OpenGL is not at or above 
        // V4.0
        //=======================================================
        if (g_useCPUDecode)
        {
            MipImages->m_DecompressedFormat = MIPIMAGE_FORMAT_DECOMPRESSED::Format_CPU;

            // This call decompresses all MIP levels so it should only be called once
            decompConfig->useCPU= true;
            tmpMipSet = DecompressMIPSet(MipImages->mipset, GPUDecode_INVALID, decompConfig, &ProgressCallback);

            if (tmpMipSet)
            {
                //---------------------------
                // swizzle Decompressed Data!
                //---------------------------
                if (KeepSwizzle(MipImages->mipset->m_format))
                {
                    SwizzleMipMap(tmpMipSet);
                }

                tmpMipSet->m_isDeCompressed = MipImages->mipset->m_format != CMP_FORMAT_Unknown? MipImages->mipset->m_format:CMP_FORMAT_MAX;
                MipImages->m_MipImageFormat = MIPIMAGE_FORMAT::Format_QImage;
                MipImages->decompressedMipSet = tmpMipSet;
            }
        }

        if (tmpMipSet == NULL)
        {
            if (!g_useCPUDecode)
            {
                MipImages->m_DecompressedFormat = MIPIMAGE_FORMAT_DECOMPRESSED::Format_GPU;
                MipImages->m_MipImageFormat     = g_gpudecodeFormat;
                
                CMP_GPUDecode decodeWith = CMP_GPUDecode::GPUDecode_INVALID;
                switch (g_gpudecodeFormat)
                {
                    case MIPIMAGE_FORMAT::Format_OpenGL:
                        decodeWith = CMP_GPUDecode::GPUDecode_OPENGL;
                        break;
                    case MIPIMAGE_FORMAT::Format_DirectX:
                        decodeWith = CMP_GPUDecode::GPUDecode_DIRECTX;
                        break;
                    case MIPIMAGE_FORMAT::Format_Vulkan:
                        decodeWith = CMP_GPUDecode::GPUDecode_VULKAN;
                        break;
                    default:
                        break;
                }

                // This call decompresses all MIP levels so it should only be called once
                decompConfig->useCPU = false;
                tmpMipSet = DecompressMIPSet(MipImages->mipset, decodeWith, decompConfig,&ProgressCallback);

                if (tmpMipSet)
                {
                    tmpMipSet->m_isDeCompressed     = MipImages->mipset->m_format != CMP_FORMAT_Unknown ? MipImages->mipset->m_format : CMP_FORMAT_MAX;
                    MipImages->decompressedMipSet   = tmpMipSet;
                }
            }
            else
                MipImages->m_Error              = MIPIMAGE_FORMAT_ERRORS::Format_CompressedImage;    
            //Reserved: GPUDecode           
        }
        else
            MipImages->m_DecompressedFormat = MIPIMAGE_FORMAT_DECOMPRESSED::Format_CPU;
    }
    else
    {
        tmpMipSet = MipImages->mipset;
        if (!(tmpMipSet->m_compressed))
            tmpMipSet->m_isDeCompressed = tmpMipSet->m_format;
    }

    return tmpMipSet;
}

inline float knee(double x, double f)
{
    return float(Imath::Math<double>::log(x * f + 1.f) / f);
}

float findKneeF(float x, float y)
{
    float f0 = 0;
    float f1 = 1.f;

    while (knee(x, f1) > y) {
        f0 = f1;
        f1 = f1 * 2.f;
    }

    for (int i = 0; i < 30; ++i) {
        const float f2 = (f0 + f1) / 2.f;
        const float y2 = knee(x, f2);

        if (y2 < y) {
            f1 = f2;
        }
        else {
            f0 = f2;
        }
    }

    return (f0 + f1) / 2.f;
}
typedef struct _R9G9B9E5
{
    union
    {
        struct
        {
            uint32_t rm : 9; // r-mantissa
            uint32_t gm : 9; // g-mantissa
            uint32_t bm : 9; // b-mantissa
            uint32_t e : 5; // shared exponent
        };
        uint32_t v;
    };

    operator uint32_t () const { return v; }

    _R9G9B9E5& operator= (const _R9G9B9E5& floatrgb9e5) { v = floatrgb9e5.v; return *this; }
    _R9G9B9E5& operator= (uint32_t Packed) { v = Packed; return *this; }
}R9G9B9E5;
//load data byte in mipset into Qimage ARGB32 format
QImage *CImageLoader::MIPS2QImage(MipSet *tmpMipSet, int level, CMP_Feedback_Proc pFeedbackProc)
{
    if (tmpMipSet == NULL)
    {
        QImage *image = new QImage(":/CompressonatorGUI/Images/CompressedImageError.png");
        return image;
    }

    MipLevel* mipLevel = m_CMips->GetMipLevel(tmpMipSet, level);
    if (!mipLevel)
    {
        return nullptr;
    }

    QImage *image;
    
    if (
        (tmpMipSet->m_TextureDataType == TDT_ARGB) ||
        (tmpMipSet->m_TextureDataType == TDT_XRGB)
        )
   {

        if ((tmpMipSet->m_ChannelFormat == CF_Float32) || (tmpMipSet->m_ChannelFormat == CF_Float16))
        {
            if (tmpMipSet->m_ChannelFormat == CF_Float32)
            {
                float* pData = mipLevel->m_pfData;
                if (pData == NULL)  return nullptr;
            }
            else
            if (tmpMipSet->m_ChannelFormat == CF_Float16)
            {
                CMP_HALF* pData = mipLevel->m_phfData;
                if (pData == NULL)  return nullptr;
            }
            else {
                CMP_WORD* pData = mipLevel->m_pwData;
                if (pData == NULL)  return nullptr;
            }

            image = new QImage(mipLevel->m_nWidth, mipLevel->m_nHeight, MipFormat2QFormat(tmpMipSet));
            if (image == NULL)
            {
                image = new QImage(":/CompressonatorGUI/Images/OutOfMemoryError.png");
                return nullptr;
            }

            loadExrProperties(tmpMipSet,level,image);
        }
        else if (tmpMipSet->m_ChannelFormat == CF_Float9995E)
        {
          
            float* pData = mipLevel->m_pfData;
            if (pData == NULL)  return nullptr;
           
            image = new QImage(mipLevel->m_nWidth, mipLevel->m_nHeight, MipFormat2QFormat(tmpMipSet));
            if (image == NULL)
            {
                image = new QImage(":/CompressonatorGUI/Images/OutOfMemoryError.png");
                return nullptr;
            }
            loadExrProperties(tmpMipSet, level, image);
        }
        else
        {
        
            // We have allocated a data buffer to fill get its referance
            CMP_BYTE* pData = mipLevel->m_pbData;
            if (pData == NULL)  return nullptr;
            
            // We dont support the conversion 
            if (MipFormat2QFormat(tmpMipSet) == QImage::Format_Invalid)
            {
                return nullptr;
            }

            // Allocates a uninitialized buffer of specified size and format
            image = new QImage(mipLevel->m_nWidth, mipLevel->m_nHeight, MipFormat2QFormat(tmpMipSet));
            if (image == NULL)
            {
                image = new QImage(":/CompressonatorGUI/Images/OutOfMemoryError.png");
                return nullptr;
            }

            QImageFormatInfo(image);

            // Initialize the buffer
            CMP_BYTE R, G, B, A;
            int i = 0;
            for (int y = 0; y < mipLevel->m_nHeight; y++){
                for (int x = 0; x < mipLevel->m_nWidth; x++)
                {
                    R = pData[i];
                    i++;
                    G = pData[i];
                    i++;
                    B = pData[i];
                    i++;
                    if (R==G && R==B)
                        A = 255;
                    else
                        A = pData[i];
                    i++;
                    image->setPixel(x, y, qRgba(R, G, B, A));
                }
                if (pFeedbackProc)
                {
                    float fProgress = 100.f * (y * mipLevel->m_nWidth) / (mipLevel->m_nWidth * mipLevel->m_nHeight);
                    if (pFeedbackProc(fProgress, NULL, NULL))
                        return NULL;
                }
            }
        }
    }

    return image;
}


MipSet * CImageLoader::LoadPluginMIPS(QString filename)
{
    QFileInfo fi(filename.toUpper());
    QString name = fi.fileName();
    QStringList list1 = name.split(".");
    QString PlugInType = list1[list1.size() - 1];
    QByteArray ba = PlugInType.toLatin1();
    const char *Ext = ba.data();

    PluginInterface_Image *plugin_Image;
    plugin_Image = reinterpret_cast<PluginInterface_Image *>(m_pluginManager->GetPlugin("IMAGE", (char *)Ext));
    //============================
    // AMD supported file formats 
    //============================
    if (plugin_Image)
    {
       MipSet *pMipSet;
       pMipSet = new MipSet();
       if (pMipSet == NULL)
              return (NULL);

       memset(pMipSet, 0, sizeof(MipSet));

       // for our loading of PNG, BMP etc...
       // We should check file extensions
       // and maybe improve the loader so it always
       // return RGBA formated data
       //pMipSet->m_swizzle = true;

       plugin_Image->TC_PluginSetSharedIO(m_CMips);

       QByteArray array = filename.toLocal8Bit();
       char* pFileNamePath = array.data();

       if (plugin_Image->TC_PluginFileLoadTexture(pFileNamePath, pMipSet) != 0)
        {
            // Process Error
            if (plugin_Image)
                delete plugin_Image;
            if (pMipSet)
                delete pMipSet;
            return (NULL);
        }

        if (plugin_Image)
            delete plugin_Image;
        
        plugin_Image = NULL;

        // bug fix 
        if (pMipSet->m_ChannelFormat == CF_Compressed)
            pMipSet->m_compressed = true;

        return (pMipSet);
    }
    else
        return (NULL);
}



void CImageLoader::float2Pixel(float kl, float f ,float r, float g, float b, float a, int x, int y, QImage *image)
{
    CMP_BYTE r_b, g_b, b_b, a_b;

    float invGamma, scale;
    if (gamma < 1.0f) {
        gamma = 2.2f;
    }

    invGamma = 1.0 / gamma;                    //for gamma correction
    float luminance3f = powf(2, -3.5);         // always assume max intensity is 1 and 3.5f darker for scale later
    scale = 255.0 * powf(luminance3f, invGamma);

    //  1) Compensate for fogging by subtracting defog
    //     from the raw pixel values.
    // We assume a defog of 0
    if (defog > 0.0f)
    {
        r = r - defog;
        g = g - defog;
        b = b - defog;
        a = a - defog;
    }

    //  2) Multiply the defogged pixel values by
    //     2^(exposure + 2.47393).
    const float exposeScale = Imath::Math<float>::pow(2, exposure + 2.47393f);
    r = r * exposeScale;
    g = g * exposeScale;
    b = b * exposeScale;
    a = a * exposeScale;

    //  3) Values that are now 1.0 are called "middle gray".
    //     If defog and exposure are both set to 0.0, then
    //     middle gray corresponds to a raw pixel value of 0.18.
    //     In step 6, middle gray values will be mapped to an
    //     intensity 3.5 f-stops below the display's maximum
    //     intensity.

    //  4) Apply a knee function.  The knee function has two
    //     parameters, kneeLow and kneeHigh.  Pixel values
    //     below 2^kneeLow are not changed by the knee
    //     function.  Pixel values above kneeLow are lowered
    //     according to a logarithmic curve, such that the
    //     value 2^kneeHigh is mapped to 2^3.5.  (In step 6,
    //     this value will be mapped to the the display's
    //     maximum intensity.)
    if (r > kl) {
        r = kl + knee(r - kl, f);
    }
    if (g > kl) {
        g = kl + knee(g - kl, f);
    }
    if (b > kl) {
        b = kl + knee(b - kl, f);
    }
    if (a > kl) {
        a = kl + knee(a - kl, f);
    }

    //  5) Gamma-correct the pixel values, according to the
    //     screen's gamma.  (We assume that the gamma curve
    //     is a simple power function.)
    r = Imath::Math<float>::pow(r, invGamma);
    g = Imath::Math<float>::pow(g, invGamma);
    b = Imath::Math<float>::pow(b, invGamma);
    a = Imath::Math<float>::pow(a, gamma);

    //  6) Scale the values such that middle gray pixels are
    //     mapped to a frame buffer value that is 3.5 f-stops
    //     below the display's maximum intensity. (84.65 if
    //     the screen's gamma is 2.2)
    r *= scale;
    g *= scale;
    b *= scale;
    a *= scale;

    r_b = Imath::clamp<float>(r, 0.f, 255.f);
    g_b = Imath::clamp<float>(g, 0.f, 255.f);
    b_b = Imath::clamp<float>(b, 0.f, 255.f);
    a_b = Imath::clamp<float>(a, 0.f, 255.f);

    image->setPixel(x, y, qRgba(r_b, g_b, b_b, a_b));
}

//
// load Exr Image Properties
//

void CImageLoader::loadExrProperties(MipSet* mipset, int level, QImage *image)
{
    MipLevel* mipLevel = m_CMips->GetMipLevel(mipset, level);
    if (mipLevel->m_pbData == NULL) return;


    float kl = Imath::Math<float>::pow(2.f, kneeLow);
    float f = findKneeF(Imath::Math<float>::pow(2.f, kneeHigh) - kl, Imath::Math<float>::pow(2.f, 3.5f) - kl);

    if (mipset->m_ChannelFormat == CF_Float32)
    {
        float *data = mipLevel->m_pfData;
        float r = 0, g = 0, b = 0, a = 0;
        //copy pixels into image
        for (int y = 0; y < mipLevel->m_nHeight; y++) {
            for (int x = 0; x < mipLevel->m_nWidth; x++) {
                r = *data;
                data++;
                g = *data;
                data++;
                b = *data;
                data++;
                a = *data;
                data++;
                float2Pixel(kl, f, r, g, b, a, x, y, image);
            }

            if ((y % 10) == 0)
                QApplication::processEvents();
        }
    }
    else if (mipset->m_ChannelFormat == CF_Float16)
    {
        CMP_HALF *data = mipLevel->m_phfData;
        CMP_HALF r, g, b, a;
        //copy pixels into image
        for (int y = 0; y < mipLevel->m_nHeight; y++) {
            for (int x = 0; x < mipLevel->m_nWidth; x++) {
                r = *data;
                data++;
                g = *data;
                data++;
                b = *data;
                data++;
                a = *data;
                data++;
                float2Pixel(kl, f,F16toF32(r), F16toF32(g), F16toF32(b), F16toF32(a), x, y, image);
            }

            if ((y % 10) == 0)
                QApplication::processEvents();
        }
    }
    else if (mipset->m_ChannelFormat == CF_Float9995E)
    {
        CMP_DWORD dwSize = mipLevel->m_dwLinearSize;
        CMP_DWORD* pSrc = mipLevel->m_pdwData;
        float r = 0, g = 0, b = 0, a = 0;
        union { float f; int32_t i; } fi;
        float Scale = 0.0f;
        for (int y = 0; y < mipLevel->m_nHeight; y++) {
            for (int x = 0; x < mipLevel->m_nWidth; x++) {
                CMP_DWORD dwSrc = *pSrc++;
                R9G9B9E5 pTemp;

                pTemp.rm = (dwSrc & 0x000001ff);
                pTemp.gm = (dwSrc & 0x0003fe00) >> 9;
                pTemp.bm = (dwSrc & 0x07fc0000) >> 18;
                pTemp.e  = (dwSrc & 0xf8000000) >> 27;
      
                fi.i = 0x33800000 + (pTemp.e << 23);
                Scale = fi.f;
                r = Scale * float(pTemp.rm);
                g = Scale * float(pTemp.gm);
                b = Scale * float(pTemp.bm);
                a = 1.0f;
                float2Pixel(kl, f, r, g, b, a, x, y, image);
            }
            if ((y % 10) == 0)
                QApplication::processEvents();
        }
    }
    else return;

}

// 
// Scans to match MIP levels with Generated Images
//
void CImageLoader::UpdateMIPMapImages(CMipImages *MipImages)
{
    if (!MipImages->mipset) return;
    QImage *image;

    //if (MipImages->m_MipImageFormat == MIPIMAGE_FORMAT::Format_QImage)
    //{
        if (MipImages->Image_list.count() <= MipImages->mipset->m_nMipLevels)
        {
            for (int i = 1; i <= MipImages->mipset->m_nMipLevels; i++)
            {
                if (MipImages->decompressedMipSet)
                    image = MIPS2QImage(MipImages->decompressedMipSet, i);
                else
                    image = MIPS2QImage(MipImages->mipset, i);

                if (image)
                {
                    MipImages->Image_list.append(image);
                }
            }
        }
    //}
}



CMipImages * CImageLoader::LoadPluginImage(QString filename, CMP_Feedback_Proc pFeedbackProc)
{
    CMipImages *MipImages;
    QImage     *image = NULL;
    bool        usedQT = false;

    MipImages = new CMipImages();
    if (MipImages == NULL) return (NULL);
    MipImages->mipset = NULL;

    QFile file(filename);
    if (!file.exists())
    {
        MipImages->m_Error = MIPIMAGE_FORMAT_ERRORS::Format_InvalidFile;
        image = new QImage(":/CompressonatorGUI/Images/ImageFileDoesNotExist.png");
        usedQT = true;
    }

    // -------------------------------------------------------------------------
    // Try Our Plugins First to handle special cases of ASTC, DDS, KTX, EXR etc...
    // -------------------------------------------------------------------------

    QFileInfo fi(filename);
    QString ext = fi.suffix().toUpper();
    bool useAMD_Plugin = true;

    // -------------------------------------------------------
    // Exception on load as DDS for BCn < 6 is not working
    // Enable this to force loading via Qt
    // useAMD_Plugin = (ext.compare("DDS") != 0);
    // -------------------------------------------------------

    if (useAMD_Plugin)
    {
        MipImages->mipset = LoadPluginMIPS(filename);
        if (MipImages->mipset)
        {
            // Check Image Format is valid else try setting one 
            // based on a FourCC value
            if (MipImages->mipset->m_format == CMP_FORMAT_Unknown)
            {
                MipImages->mipset->m_format = GetFormat(MipImages->mipset);
            }

            MipSet *tmpMipSet;
            Config decompSetting;
            decompSetting.errMessage = "";
            tmpMipSet = LoaderDecompressMipSet(MipImages, &decompSetting);

            if (tmpMipSet == NULL)
            {
                MipImages->errMsg = QString::fromStdString(decompSetting.errMessage);
                image = new QImage(":/CompressonatorGUI/Images/DeCompressImageError.png");
                usedQT = true;
            }
            else
                image = MIPS2QImage(tmpMipSet, 0, pFeedbackProc);
        }
    }


    //------------------------------------------------------------------------------
    // Now try to load an image that can be viewed as QImage if not already loaded
    //------------------------------------------------------------------------------

    if (image == NULL)
    {
        image = new QImage(filename);
        usedQT = true;
    }


    //-----------------------------------------
    // Do we have a Image if so keep it
    //-----------------------------------------

    if (image) 
    {
        // validate the format is not compressed!
        QImage::Format  format = image->format();
        if (format != QImage::Format_Invalid)
        {
            MipImages->Image_list.append(image);

            if (MipImages->mipset)
            {
                if (!MipImages->mipset->m_compressed)
                    MipImages->m_MipImageFormat = MIPIMAGE_FORMAT::Format_QImage;

                if (usedQT)
                {
                    MipImages->mipset->m_format = QFormat2MipFormat(format);
                }

            }
            else
                MipImages->m_MipImageFormat = MIPIMAGE_FORMAT::Format_QImage;

        }
        else
        {
            // We dont want to use invalid QImage formats
            delete image;
            image = NULL;
        }
    }

    //---------------------------------------------------------------------------------
    // Failed to create a MipSet and we have a QImage, convert the QImage to a MIP set
    //---------------------------------------------------------------------------------

    if ((MipImages->Image_list.count() > 0) && (MipImages->mipset == NULL))
    {
        MipImages->mipset = QImage2MIPS(MipImages->Image_list[0], pFeedbackProc);
    }

    //-----------------------------------------
    // Update the images for all MIP levels
    //-----------------------------------------

    if (MipImages->mipset)
    {
        if (MipImages->mipset->m_nMipLevels > 1)
            UpdateMIPMapImages(MipImages);
    }


    //-----------------------------------------------------
    // Error : Both Qt and AMD failed to load an Image
    //-----------------------------------------------------

    if ((MipImages->mipset == NULL) && (MipImages->Image_list.count() == 0))
    {
        MipImages->m_Error = MIPIMAGE_FORMAT_ERRORS::Format_NotSupported;
        // we have a bug to fix!!
        QImage *image = new QImage(":/CompressonatorGUI/Images/notsupportedImage.png");
        if (image)
        {
            MipImages->Image_list.append(image);
            MipImages->m_MipImageFormat = MIPIMAGE_FORMAT::Format_QImage;
        }
    }

    return MipImages;
}

