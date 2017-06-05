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

#ifndef CPPROJECTDATA_H
#define CPPROJECTDATA_H

// #define ENABLED_USER_GPUVIEW

//#include "qtpropertymanager.h"
//#include "qtvariantproperty.h"
//#include "qtgroupboxpropertybrowser.h"
//#include "objectcontroller.h"
#include <QtWidgets>
#include <QtQml\QQmlListProperty>
#include "PluginManager.h"
#include "Compressonator.h"
#include "cpImageLoader.h"
#include "ATIFormats.h"
#include "TextureIO.h"
#include "Common.h"

#define    TREETYPE_ADD_IMAGE_NODE               0
#define    TREETYPE_IMAGEFILE_DATA_NODE          1      
#define    TREETYPE_IMAGEFILE_DATA               2      // items column (1) uses new allocated varient data for C_FileProperties
#define    TREETYPE_COMPRESSION_DATA             3      // items column (1) uses new allocated varient data for C_CompressOptions

// =======================================================
// COMPRESSION DATA
// =======================================================

#ifdef USECOMPSPEED
enum eCompression_Speed {
    Normal,
    Fast,
    Super_Fast
};

class Compression_Speed :public QObject
{
    Q_OBJECT
        Q_PROPERTY(bool     No_Alpha  MEMBER No_Alpha)
        Q_PROPERTY(eCompression_Speed Speed   READ Speed WRITE setPriority NOTIFY priorityChanged)


public:
    Q_ENUMS(eCompression_Speed)

        Compression_Speed()
    {
        No_Alpha = false;
        m_Speed = Normal;
    }

    void setPriority(eCompression_Speed priority)
    {
        m_Speed = priority;
        emit priorityChanged(priority);
    }

    eCompression_Speed Speed() const
    {
        return m_Speed;
    }

    bool No_Alpha;
    eCompression_Speed m_Speed;

signals:
    void priorityChanged(eCompression_Speed);


};
#endif
class C_Input_HDR_Image_Properties : public QObject
{

    Q_OBJECT
        Q_PROPERTY(double    Defog      READ getDefog       WRITE setDefog      NOTIFY defogChanged)
        Q_PROPERTY(double    Exposure   READ getExposure    WRITE setExposure   NOTIFY exposureChanged)
        Q_PROPERTY(double    KneeLow    READ getKneeLow     WRITE setKneeLow    NOTIFY kneeLowChanged)
        Q_PROPERTY(double    KneeHigh   READ getKneeHigh    WRITE setKneeHigh   NOTIFY kneeHighChanged)
        Q_PROPERTY(double    Gamma      READ getGamma       WRITE setGamma      NOTIFY gammaChanged)

public:

    C_Input_HDR_Image_Properties()
    {
        m_Defog    = AMD_CODEC_DEFOG_DEFAULT;
        m_Exposure = AMD_CODEC_EXPOSURE_DEFAULT;
        m_KneeLow  = AMD_CODEC_KNEELOW_DEFAULT;
        m_KneeHigh = AMD_CODEC_KNEEHIGH_DEFAULT;
        m_Gamma    = AMD_CODEC_GAMMA_DEFAULT;
    }

    void setDefog(double defog)
    {
        if (m_Defog == defog)
            return;

        if (defog < 0.000) {
            m_Defog = 0.0000;
        }
        else if (defog > 0.0100) {
            m_Defog = 0.0100;
        }

        if (m_Defog != defog)
        {
            m_Defog = defog;
            m_data_has_been_changed = true;
            emit dataChanged();
        }

        emit defogChanged(m_Defog);
    }

    void setExposure(double exposure)
    {
        if (m_Exposure == exposure)
            return;

        if (exposure < -10.0) {
            m_Exposure = -10.0;
        }
        else if (exposure > 10.0) {
            m_Exposure = 10.0;
        }

        if (m_Exposure != exposure)
        {
            m_Exposure = exposure;
            m_data_has_been_changed = true;
            emit dataChanged();
        }

        emit exposureChanged(m_Exposure);
    }

    void setKneeLow(double kl)
    {
        if (m_KneeLow == kl)
            return;

        if (kl < -3.0) {
            m_KneeLow = -3.00;
        }
        else if (kl > 3.0) {
            m_KneeLow = 3.00;
        }

        if (m_KneeLow != kl)
        {
            m_KneeLow = kl;
            m_data_has_been_changed = true;
            emit dataChanged();
        }

        emit kneeLowChanged(m_KneeLow);
    }


    void setKneeHigh(double kh)
    {
        if (m_KneeHigh == kh)
            return;

        if (kh < 3.50) {
            m_KneeHigh = 3.50;
        }
        else if (kh > 7.50) {
            m_KneeHigh = 7.50;
        }

        if (m_KneeHigh != kh)
        {
            m_KneeHigh = kh;
            m_data_has_been_changed = true;
            emit dataChanged();
        }

        emit kneeHighChanged(m_KneeHigh);
    }

    void setGamma(double gamma)
    {
        if (m_Gamma == gamma)
            return;

        if (gamma < 1.0) {
            m_Gamma = 1.0;
        }
        else if (gamma > 2.6) {
            m_Gamma = 2.6;
        }

        if (m_Gamma != gamma)
        {
            m_Gamma = gamma;
            m_data_has_been_changed = true;
            emit dataChanged();
        }

        emit gammaChanged(m_Gamma);
    }

    double getDefog() const
    {
        return m_Defog;
    }

    double getExposure() const
    {
        return m_Exposure;
    }

    double getKneeLow() const
    {
        return m_KneeLow;
    }

    double getKneeHigh() const
    {
        return m_KneeHigh;
    }

    double getGamma() const
    {
        return m_Gamma;
    }

    double m_Defog;
    double m_Exposure;
    double m_KneeLow;
    double m_KneeHigh;
    double m_Gamma;
    bool m_data_has_been_changed;

signals:
    void defogChanged(double&);
    void exposureChanged(double&);
    void kneeLowChanged(double&);
    void kneeHighChanged(double&);
    void gammaChanged(double&);
    void dataChanged();
};

class C_ASTC_BlockRate : 
    public C_Input_HDR_Image_Properties
    //public QObject
{
   
    Q_OBJECT
        Q_PROPERTY(QString Bitrate  READ getBitrate     WRITE setBitrate NOTIFY bitrateChanged)

public:

    C_ASTC_BlockRate()
    {
        m_Bitrate = "8.00";
        m_correctBitrate = "8.00";
        xblock = 4;
        yblock = 4;
        
    }

    void setBitrate(QString bitrate)
    {
        if (bitrate == "")
            return;

        if (m_Bitrate != bitrate)
        {
            QRegExp digits("\\d*");  //to check all digits
            //check user input: either bitrate or block numbers
            if (strchr(bitrate.toUtf8().constData(), '.') != NULL || digits.exactMatch(bitrate))
            {
                float bitrateF = bitrate.toFloat();
                if (bitrateF)
                {
                    if (bitrateF > 8.00)
                    {
                        m_Bitrate = "8.00";
                        m_correctBitrate = "8.00";
                    }
                    else if (bitrateF < 0)
                    {
                        m_Bitrate = "0.89";
                        m_correctBitrate = "0.89";
                    }
                    else
                    {
                        m_Bitrate = bitrate;
                        m_correctBitrate = bitrate;
                    }
                    astc_find_closest_blockdim_2d(bitrateF, &xblock, &yblock, 1);
                    m_correctBitrate = bitrateF;
                    emit bitrateChanged(m_Bitrate, xblock, yblock);
                    m_data_has_been_changed = true;
                    emit dataChanged();
                }
            }
            else  if ((strchr(bitrate.toUtf8().constData(), 'x') != NULL) || (strchr(bitrate.toUtf8().constData(), 'X') != NULL))//block number user form input
            {
                int dimensions = sscanf(bitrate.toUtf8().constData(), "%dx%dx", &xblock, &yblock);
                if (dimensions < 2) 
                    return;
                astc_find_closest_blockxy_2d(&xblock, &yblock, 0);
                if (xblock < 3 || xblock > 12 || yblock < 3 || yblock > 12)
                {
                    xblock = -1;
                    yblock = -1;
                    emit bitrateChanged(m_Bitrate, xblock, yblock);
                    m_Bitrate = "8.00";
                    m_correctBitrate = "8.00";
                }
                else
                {
                    float bitrateF = float(128.0f / (xblock*yblock));
                    m_Bitrate = bitrate;
                    m_correctBitrate = QString::number(xblock) + "x" + QString::number(yblock);
                    emit bitrateChanged(QString::number(bitrateF, 'f', 2), xblock, yblock);
                    m_data_has_been_changed = true;
                    emit dataChanged();
                }
            }
            else
            {
                xblock = -1;
                yblock = -1;
                emit bitrateChanged(m_Bitrate, xblock, yblock);
                m_Bitrate = "8.00";
                m_correctBitrate = "8.00";
            }
        }
    }

    QString getBitrate() const
    {
        return m_Bitrate;
    }


    QString m_Bitrate;
    QString m_correctBitrate;
    int xblock;
    int yblock;
    bool m_data_has_been_changed;

signals:
    void bitrateChanged(QString &, int&, int&);
    void dataChanged();
};

class DXT1_Alpha :
    //public Compression_Speed
    public C_ASTC_BlockRate
    //public QObject
{
Q_OBJECT
    Q_PROPERTY(bool     No_Alpha   READ isNoAlpha WRITE setNoAlpha NOTIFY noAlphaChannel)
    Q_PROPERTY(bool     Use_Alpha  READ isUseAlpha WRITE setUseAlpha NOTIFY hasAlphaChannel)
    Q_PROPERTY(int      Threshold  READ getThreshold    WRITE setThreshold    NOTIFY thresholdChanged)

public:

    DXT1_Alpha()
    {
        init();
    }

    void init()
    {
        No_Alpha  = true;
        Use_Alpha = false;
        Threshold = 0;
        setNoAlpha(true);
    }

    bool No_Alpha;
    bool Use_Alpha;
    int  Threshold;

    void setThreshold(int threshold)
    {
        if (Use_Alpha && Threshold!= threshold)
        {
            m_data_has_been_changed = true;
            emit dataChanged();
        }
        if (threshold > 255)
            Threshold = 255;
        else
            if (threshold< 0)
                Threshold = 0;
            else
                Threshold = threshold;

        emit thresholdChanged((QVariant &)threshold);
    }

    double getThreshold() const
    {
        return Threshold;
    }

    void setUseAlpha(bool useAlpha)
    {
        if (Use_Alpha != useAlpha)
        {
            Use_Alpha = useAlpha;
            m_data_has_been_changed = true;
            emit dataChanged();
            if (useAlpha && No_Alpha)
            {
                No_Alpha = false;
                emit hasAlphaChannel();
            }
        }
    }

    bool isUseAlpha() const
    {
        return Use_Alpha;
    }

    void setNoAlpha(bool noAlpha)
    {
        if (No_Alpha != noAlpha)
        {
            No_Alpha = noAlpha;
            m_data_has_been_changed = true;
            emit dataChanged();
            if (noAlpha && Use_Alpha)
            {
                Use_Alpha = false;
                emit noAlphaChannel();
            }
        }
    }

    bool isNoAlpha() const
    {
        return No_Alpha;
    }

signals:
    void thresholdChanged(QVariant &);
    void noAlphaChannel();
    void hasAlphaChannel();
};


class Channel_Weighting : public DXT1_Alpha
{
Q_OBJECT
    Q_PROPERTY(bool     Adaptive   MEMBER Adaptive)
    Q_PROPERTY(double    X_RED    READ getX_Red    WRITE setX_Red    NOTIFY redwChanged)
    Q_PROPERTY(double    Y_GREEN    READ getY_Green    WRITE setY_Green    NOTIFY greenwChanged)
    Q_PROPERTY(double    Z_BLUE    READ getZ_Blue    WRITE setZ_Blue    NOTIFY bluewChanged)

public:
    Channel_Weighting()
    {
        Adaptive = false;
        X_RED = 0.3086;
        Y_GREEN = 0.6094;
        Z_BLUE = 0.082;
    }

    bool   Adaptive;
    double X_RED;
    double Y_GREEN;
    double Z_BLUE;

    void setX_Red(double xredw)
    {
        if (X_RED != xredw)
        {
            m_data_has_been_changed = true;
            emit dataChanged();
        }
        if (xredw > 1)
            X_RED = 1;
        else
            if (xredw < 0)
                X_RED = 0;
            else
                X_RED = xredw;

        emit redwChanged((QVariant &)xredw);
    }

    double getX_Red() const
    {
        return  X_RED;
    }

    void setY_Green(double ygreenw)
    {
        if (Y_GREEN != ygreenw)
        {
            m_data_has_been_changed = true;
            emit dataChanged();
        }
        if (ygreenw > 1)
            Y_GREEN = 1;
        else
            if (ygreenw < 0)
                Y_GREEN = 0;
            else
                Y_GREEN = ygreenw;

        emit greenwChanged((QVariant &)ygreenw);
    }

    double getY_Green() const
    {
        return  Y_GREEN;
    }

    void setZ_Blue(double zbluew)
    {
        if (Z_BLUE != zbluew)
        {
            m_data_has_been_changed = true;
            emit dataChanged();
        }
        if (zbluew > 1)
            Z_BLUE = 1;
        else
            if (zbluew < 0)
                Z_BLUE = 0;
            else
                Z_BLUE = zbluew;

        emit bluewChanged((QVariant &)zbluew);
    }

    double getZ_Blue() const
    {
        return  Z_BLUE;
    }

signals:
    void redwChanged(QVariant &);
    void greenwChanged(QVariant &);
    void bluewChanged(QVariant &);
};


#define DESTINATION_IMAGE_CLASS_NAME      "Destination Image"
#define CHANNEL_WEIGHTING_CLASS_NAME      "Channel Weighting"
#define DXT1_ALPHA_CLASS_NAME             "DXT1 Alpha"
#define ASTC_BLOCKRATE_CLASS_NAME         "ASTC BlockRate"
#define HDR_PROP_CLASS_NAME               "Input HDR Image Properties"

#define DESTINATION_IMAGE_NAME            "Name"
#define DESTINATION_IMAGE_FILESIZE        "File Size"
#define DESTINATION_IMAGE_NOTPROCESSED    "Not Processed"
#define DESTINATION_IMAGE_UNKNOWN         "Unknown"

class C_Destination_Image: 
    public Channel_Weighting 
    //public QObject
{
    Q_OBJECT
        Q_PROPERTY(QString  _Name                MEMBER m_FileInfoDestinationName)
        Q_PROPERTY(QString   _File_Size          MEMBER m_FileSizeStr)
        Q_PROPERTY(QString   _Width              MEMBER m_WidthStr)
        Q_PROPERTY(QString   _Height             MEMBER m_HeightStr)
        Q_PROPERTY(QString  _Compression_Ratio   MEMBER m_CompressionRatio);
        Q_PROPERTY(QString  _Compression_Time    MEMBER m_CompressionTimeStr);

public:
    C_Destination_Image()
    {
        m_FileSize = 0;
        m_Width = 0;
        m_Height = 0;
        m_FileSizeStr   = "";
        m_WidthStr      = "";
        m_HeightStr     = "";
        m_FileInfoDestinationName = "";
        m_CompressionRatio = DESTINATION_IMAGE_UNKNOWN;
        m_CompressionTime = 0;
        m_CompressionTimeStr = DESTINATION_IMAGE_NOTPROCESSED;
    }

    QString m_FileInfoDestinationName;
    QString m_CompressionRatio;
    int     m_Width;
    int     m_Height;
    int     m_FileSize;
    double  m_CompressionTime;
    QString m_WidthStr;
    QString m_HeightStr;
    QString m_FileSizeStr;
    QString m_CompressionTimeStr;
};

#define COMPRESS_OPTIONS_QUALITY  "Quality"
#define COMPRESS_OPTIONS_FORMAT   "Format"
#define COMPRESS_OPTIONS_CHANNEL_WEIGHTING_R  "X RED"
#define COMPRESS_OPTIONS_CHANNEL_WEIGHTING_G  "Y GREEN"
#define COMPRESS_OPTIONS_CHANNEL_WEIGHTING_B  "Z BLUE"
#define COMPRESS_OPTIONS_ALPHATHRESHOLD  "Threshold"
#define COMPRESS_OPTIONS_ADAPTIVECOLOR  "Adaptive"
#define COMPRESS_OPTIONS_USEALPHA  "Use Alpha"
#define COMPRESS_OPTIONS_NOALPHA  "No Alpha"
#define COMPRESS_OPTIONS_BITRATE  "Bitrate"

//HDR image input properties
#define COMPRESS_OPTIONS_DEFOG  "Defog"
#define COMPRESS_OPTIONS_EXPOSURE  "Exposure"
#define COMPRESS_OPTIONS_KNEELOW  "KneeLow"
#define COMPRESS_OPTIONS_KNEEHIGH  "KneeHigh"
#define COMPRESS_OPTIONS_GAMMA "Gamma"

class C_Destination_Options : public C_Destination_Image
{
    Q_OBJECT
        Q_ENUMS(eCompression)
        Q_PROPERTY(eCompression Format        READ getCompression WRITE setCompression NOTIFY compressionChanged)
        Q_PROPERTY(double       Quality       READ getQuality     WRITE setQuality NOTIFY qualityChanged)

public:
    enum eCompression {
        BC1,
        BC2,
        BC3,
        BC4,
        BC5,
        BC6H,
        BC6H_SF,
        BC7,
        ASTC,
        ATC_RGB,
        ATC_RGBA_Explicit,
        ATC_RGBA_Interpolated,
        ATI1N,
        ATI2N,
        ATI2N_XY,
        ATI2N_DXT5,
        DXT1,
        DXT3,
        DXT5,
        DXT5_xGBR,
        DXT5_RxBG,
        DXT5_RBxG,
        DXT5_xRBG,
        DXT5_RGxB,
        DXT5_xGxR,
        ETC_RGB,
        ETC2_RGB,
        ARGB_8888,
        ARGB_16F,
        ARGB_32F,
#ifdef USE_GT
        GT,
#endif
        //RGB_888,
        //RG_8,
        //R_8,
        //ARGB_2101010,
        //ARGB_16,
        //RG_16,
        //R_16,
        //RG_16F,
        //R_16F,
        //RG_32F,
        //R_32F,
    };

    C_Destination_Options()
    {
        init();
    }


    void init()
    {
        m_MipImages = NULL;
        m_OriginalMipImages = NULL;

        m_FileSize = 0;
        m_Width = 0;
        m_Height = 0;
        m_Compression = C_Destination_Options::BC7;
        //m_Encoding    = No_Encoding;
        m_Quality = AMD_CODEC_QUALITY_DEFAULT;
        m_isselected = false;               // Flag to force Project View to use the datas child item in compression

        // These items are not set by PropertyManager 
        // they are done via seperate widgets
        m_compname = "";
        m_destFileNamePath = "";
        m_decompressedFileNamePath = "";
        m_editing = false;
        m_iscompressedFormat = false;       // Flag to indicate the target will be saved as a  compressed file
        m_data_has_been_changed = false;    // Set if any data has changed value
        //m_settoUseOnlyBC6 = false;
        m_SourceIscompressedFormat = false;     // Flag indicating source is compressed format
        m_SourceIsFloatFormat = false;
        m_SourceImageSize = 0;
        setNoAlpha(true);
    }


    // Create a temp unique file name to use for a decompressed image
    // and save it for use in analysis later
    void CreateTempFile()
    {
        QTemporaryFile CTemporaryFile;
        CTemporaryFile.setFileTemplate("Compress_TMP_XXXXXX.bmp");
        if (CTemporaryFile.open())
        {
            m_decompressedFileNamePath = CTemporaryFile.fileName();
            CTemporaryFile.close();
        }
    }


    ~C_Destination_Options()
    {
        if (m_decompressedFileNamePath.length() > 0)
        {
            QFile::remove(m_decompressedFileNamePath);
        }
    }

    void setCompression(eCompression Compression)
    {
        //if (m_settoUseOnlyBC6)
        //{
        //    //m_Compression = C_Destination_Options::BC6H;
        //    emit compressionChanged((QVariant &)Compression);
        //}
        //else
        //{
            if (m_Compression != Compression)
            {
                m_data_has_been_changed = true;
                emit dataChanged();
            }
            m_Compression = Compression;
            emit compressionChanged((QVariant &)Compression);
        //}
    }

    eCompression getCompression() const
    {
        //if (m_settoUseOnlyBC6)
        //    return C_Destination_Options::BC6H;
        //else
            return m_Compression;
    }


    void setQuality(float quality)
    {
        if (m_Quality != quality)
        {
            m_data_has_been_changed = true;
            emit dataChanged();
        }
       if (quality > 1)
           m_Quality = 1;
       else
            if (quality < 0)
                m_Quality = 0;
            else
                m_Quality = quality;

       emit qualityChanged((QVariant &)quality);
    }

    double getQuality() const
    {
        return m_Quality;
    }

    bool doSwizzle()
    {
        // determin of the swizzle flag needs to be turned on!
        switch (m_Compression)
        {
        case BC4:
        case ATI1N:        // same as BC4    
        case ATI2N:        // same as BC4    
        case BC5:
        case ATI2N_XY:    // same as BC5    
        case ATI2N_DXT5:    // same as BC5    
        case BC1:
        case BC2:
        case DXT3:        // same as BC2     
        case BC3:
        case DXT5:        // same as BC3     
            return true;
            break;
        default:
            break;
        }

        return false;
    }

    QString      m_compname;
    QString      m_sourceFileNamePath;
    QString      m_destFileNamePath;
    QString      m_decompressedFileNamePath;
    eCompression m_Compression;
    bool         m_editing;
    double       m_Quality;
    bool         m_iscompressedFormat;
    //bool         m_data_has_been_changed;
    CMipImages  *m_MipImages;
    bool         m_isselected;
    //bool         m_settoUseOnlyBC6;
    bool         m_SourceIscompressedFormat;
    bool         m_SourceIsFloatFormat;
    long         m_SourceImageSize;

    // Use this as  Read Only property. It points to the original Image used for this destination setting
    // so dont delete its ref using this class. Parent class will clean it up
    CMipImages  *m_OriginalMipImages;

signals:
    void compressionChanged(QVariant &);
    void qualityChanged(QVariant &);

private:

    friend  C_Destination_Options &operator << (C_Destination_Options &ds, const C_Destination_Options &obj)
    {
        // Assign Varient data used by the PropertyManager

        QVariant var;
        int count = obj.metaObject()->propertyCount();
        int count2 = ds.metaObject()->propertyCount();
        if (count == count2)
        {
            for (int i = 0; i<count; ++i) {
                if (obj.metaObject()->property(i).isStored(&obj)) {
                    ds.metaObject()->property(i).write(&ds, obj.metaObject()->property(i).read(&obj));
                }
            }
        }

        // Assign none property data used by the class
        ds.m_Compression              = obj.m_Compression;
        ds.m_compname                 = obj.m_compname;
        ds.m_destFileNamePath         = obj.m_destFileNamePath;
        ds.m_editing                  = obj.m_editing;
        ds.m_sourceFileNamePath       = obj.m_sourceFileNamePath;
        ds.m_iscompressedFormat       = obj.m_iscompressedFormat;
        ds.m_SourceIscompressedFormat = obj.m_SourceIscompressedFormat;
        ds.m_SourceIsFloatFormat      = obj.m_SourceIsFloatFormat;
        ds.m_data_has_been_changed    = obj.m_data_has_been_changed;
        ds.m_SourceImageSize          = obj.m_SourceImageSize;
        ds.X_RED                      = obj.X_RED;
        ds.Y_GREEN                    = obj.Y_GREEN;
        ds.Z_BLUE                     = obj.Z_BLUE;
        ds.Threshold                  = obj.Threshold;

        return ds;
    }
};


// =======================================================
// ORIGINAL IMAGE FILE 
// =======================================================

// http://doc.qt.io/qt-5/properties.html
// For the Propert View we use _ in string names to indicate a read only field 
// in the editor

class C_Source_Image: public QObject
{
Q_OBJECT
    Q_PROPERTY(QString  _Name       MEMBER m_Name)
    Q_PROPERTY(QString  _Full_Path  MEMBER m_Full_Path)
    Q_PROPERTY(QString  _File_Size  MEMBER m_FileSizeStr)
    Q_PROPERTY(QString  _Image_Size MEMBER m_ImageSizeStr)
    Q_PROPERTY(QString  _Width      MEMBER m_WidthStr)
    Q_PROPERTY(QString  _Height     MEMBER m_HeightStr)
    Q_PROPERTY(int      _Mip_Levels MEMBER m_Mip_Levels)
    Q_PROPERTY(QString  _Format     MEMBER m_FormatStr)


    Q_ENUMS(CMP_FORMAT)

public:
    C_Source_Image()
    {
        m_Name          = "";
        m_Full_Path     = "";
        m_ImageSize     = 0;
        m_FileSize      = 0;
        m_Width         = 0;
        m_Height        = 0;
        m_FileSizeStr   = "";
        m_ImageSizeStr  = "";
        m_WidthStr      = "";
        m_HeightStr     = "";
        m_Mip_Levels    = 0;

        // Used to index new file name
        m_extnum = 0;
        m_MipImages = NULL;

        m_FormatStr = GetFormatDesc(CMP_FORMAT::CMP_FORMAT_Unknown);
        m_Format    = CMP_FORMAT::CMP_FORMAT_Unknown;
    }


    QString m_Name;
    QString m_Full_Path;
    QString m_WidthStr;
    QString m_HeightStr;
    QString m_FileSizeStr;
    QString m_ImageSizeStr;
    QString m_FormatStr;

    CMP_FORMAT m_Format;
    int     m_Width;
    int     m_Height;
    int     m_FileSize;
    int     m_Mip_Levels;
    int     m_extnum;
    long    m_ImageSize;
    CMipImages *m_MipImages;


};


// =======================================================
// APPLICATION DATA
// =======================================================
#ifdef USE_COMPUTE
#define APP_compress_image_using                        "Encode with"
#endif
#define APP_Decompress_image_views_using                "Decode with"
#define APP_Reload_image_views_on_selection             "Reload image views on selection"
#define APP_Load_recent_project_on_startup              "Load recent project on startup"
#define APP_Close_all_image_views_prior_to_process      "Close all image views prior to process"

//class C_GPU_Decompress_Options : public QObject
//{
//
//    Q_OBJECT
//        Q_ENUMS(DecompressAPI)
//        Q_PROPERTY(DecompressAPI GPU_Decompress  READ getGPUDecompress     WRITE setGPUDecompress NOTIFY GPUDecompressChanged)
//
//public:
//    
//    enum DecompressAPI {
//        OpenGL,
//        DirectX,
//        Vulkan
//    };
//
//    C_GPU_Decompress_Options()
//    {
//        m_gpudecomp = OpenGL;
//    }
//
//    void setGPUDecompress(DecompressAPI decodewith)
//    {
//        m_gpudecomp = decodewith;
//        emit GPUDecompressChanged((QVariant &)decodewith);
//    }
//
//    DecompressAPI getGPUDecompress() const
//    {
//        return m_gpudecomp;
//    }
//
//    DecompressAPI m_gpudecomp;
//
//signals:
//    void GPUDecompressChanged(QVariant &);
//};
//
//class C_GPU_Compress_Options : public C_GPU_Decompress_Options
//{
//
//    Q_OBJECT
//        Q_ENUMS(CompressAPI)
//        Q_PROPERTY(CompressAPI GPU_Compress  READ getGPUCompress     WRITE setGPUCompress NOTIFY GPUCompressChanged)
//
//public:
//
//    // Note:
//    // Keep order of list as its ref is saved in CompressSettings.ini
//    // we should change the save to use string name instead of indexes to the enum
//    // GPU_Compress=1
//    // change to 
//    // GPU_Compress=OpenCL 
//    //
//    // Note this must also match Compressonator.h definition CMP_Compute_type
//    enum CompressAPI {
//        OpenCL,
//        DirectX,
//        Vulkan
//    };
//
//    C_GPU_Compress_Options()
//    {
//        m_gpucomp = OpenCL;
//    }
//
//    void setGPUCompress(CompressAPI encodewith)
//    {
//        m_gpucomp = encodewith;
//        emit GPUCompressChanged((QVariant &)encodewith);
//    }
//
//    CompressAPI getGPUCompress() const
//    {
//        return m_gpucomp;
//    }
//
//    CompressAPI m_gpucomp;
//
//signals:
//    void GPUCompressChanged(QVariant &);
//};

class C_Application_Options :public QObject
{
    Q_OBJECT
        Q_ENUMS(ImageEncodeWith)
        Q_ENUMS(ImageDecodeWith)
#ifdef USE_COMPUTE
        Q_PROPERTY(ImageEncodeWith  Encode_with                   READ getImageEncode             WRITE setImageEncode NOTIFY ImageEncodeChanged)
#endif
        Q_PROPERTY(ImageDecodeWith  Decode_with            READ getImageViewDecode         WRITE setImageViewDecode NOTIFY ImageViewDecodeChanged)
        Q_PROPERTY(bool             Reload_image_views_on_selection         READ getUseNewImageViews        WRITE setUseNewImageViews)
        Q_PROPERTY(bool             Close_all_image_views_prior_to_process  READ getCloseAllImageViews      WRITE setCloseAllImageViews)
        Q_PROPERTY(bool             Load_recent_project_on_startup          READ getLoadRecentFile          WRITE setLoadRecentFile)

public:
    // Keep order of list as its ref is saved in CompressSettings.ini
    // we should change the save to use string name instead of indexes to the enum
    enum class ImageEncodeWith {
        CPU,
        GPU_OpenCL,
        GPU_DirectX,
        GPU_Vulkan
    };

    enum class ImageDecodeWith {
        CPU,
        GPU_OpenGL,
        GPU_DirectX,
        GPU_Vulkan
    };
    // Flags how image views are used, True deletes the old view and creates a new one
    // every time user clicks on an image item on the Project View, else it will
    // load the image once and cashe the image views, Default is True

    bool m_useNewImageViews;
    
    
    C_Application_Options()
    {
        m_ImageViewDecode    = ImageDecodeWith::CPU;
#ifdef USE_COMPUTE
        m_ImageEncode        = ImageEncodeWith::CPU;
#endif
        m_loadRecentFile     = false;
        m_useNewImageViews   = true;
        m_refreshCurrentView = false;
        m_closeAllDocuments  = true;
    }

    void setImageViewDecode(ImageDecodeWith decodewith)
    {
        m_ImageViewDecode = decodewith;
        emit ImageViewDecodeChanged((QVariant &)decodewith);
    }

    ImageDecodeWith getImageViewDecode() const
    {
        return m_ImageViewDecode;
    }
#ifdef USE_COMPUTE
    void setImageEncode(ImageEncodeWith encodewith)
    {
        m_ImageEncode = encodewith;
        emit ImageEncodeChanged((QVariant &)encodewith);
    }

    ImageEncodeWith getImageEncode() const
    {
        return m_ImageEncode;
    }
#endif
    void setCloseAllImageViews(bool recent)
    {
        m_closeAllDocuments = recent;
    }

    double getCloseAllImageViews() const
    {
        return m_closeAllDocuments;
    }

    void setLoadRecentFile(bool recent)
    {
        m_loadRecentFile = recent;
    }

    double getLoadRecentFile() const
    {
        return m_loadRecentFile;
    }

    void setUseNewImageViews(bool recent)
    {
        m_useNewImageViews = recent;
    }

    double getUseNewImageViews() const
    {
        return m_useNewImageViews;
    }

    ImageDecodeWith m_ImageViewDecode;
#ifdef USE_COMPUTE
    ImageEncodeWith m_ImageEncode;
#endif
    bool            m_closeAllDocuments;
    bool            m_loadRecentFile;
    bool            m_refreshCurrentView;

signals:
    void ImageViewDecodeChanged(QVariant &);
#ifdef USE_COMPUTE
signals :
    void ImageEncodeChanged(QVariant &);
#endif

};


#define STR_QUALITY_SETTING_HINT        "Quality Setting Range 0 (Poor)to 1 (High)Default is 0.05"
#define STR_FORMAT_SETTING_HINT        "Please refer to format description below for more info on compression format"
#define STR_SETTING_MINIMUM     "minimum"
#define STR_SETTING_MAXIMUM     "maximum"
#define STR_SETTING_SINGLESTEP  "singleStep"


#define STR_CHANNELWEIGHTR_SETTING_HINT "Channel Weight Setting Range 0 (Poor)to 1 (High)Default R Weightiing is 0.3086"
#define STR_CHANNELWEIGHTG_SETTING_HINT "Channel Weight Setting Range 0 (Poor)to 1 (High)Default G Weightiing is 0.6094"
#define STR_CHANNELWEIGHTB_SETTING_HINT "Channel Weight Setting Range 0 (Poor)to 1 (High)Default B Weightiing is 0.0820"

#define STR_ALPHATHRESHOLD_HINT        "Alpha Threshold Range 1 to 255. Default is 128"

#define STR_BITRATE_SETTING_HINT        "The maximum ASTC bitrate allowed is 8.00. The closest bitrate will be determined. Default is 8.00(4x4)"

#define STR_DEFOG_SETTING_HINT        "The defog range supported is 0.000  to 0.100. Default is 0."
#define STR_EXPOSURE_SETTING_HINT        "The exposure range supported is -10 to 10. Default is 0."
#define STR_KNEELOW_SETTING_HINT        "The kneelow range supported is -3.0 to 3.0. Default is 0."
#define STR_KNEEHIGH_SETTING_HINT        "The kneehigh range supported is 3.5 to 7.5. Default is 5."
#define STR_GAMMA_SETTING_HINT        "The gamma range supported is 1 to 2.6. Default is 2.2."

#define UNKNOWN_IMAGE                    " Unknown"
#define IMAGE_TYPE_PLUGIN                "IMAGE"

#endif