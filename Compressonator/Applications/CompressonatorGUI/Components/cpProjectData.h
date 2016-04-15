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
#include "PluginManager.h"
#include "Compressonator.h"
#include "cpImageLoader.h"
#include "ATIFormats.h"


#define    TREETYPE_ADD_IMAGE_NODE               0
#define    TREETYPE_IMAGEFILE_DATA_NODE          1      
#define    TREETYPE_IMAGEFILE_DATA               2      // items column (1) uses new allocated varient data for C_FileProperties
#define    TREETYPE_COMPRESSION_DATA             3      // items column (1) uses new allocated varient data for C_CompressOptions

// =======================================================
// COMPRESSION DATA
// =======================================================
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


class DXT1_Alpha :public Compression_Speed
{
Q_OBJECT
    Q_PROPERTY(bool     No_Alpha  MEMBER No_Alpha)
    Q_PROPERTY(bool     Use_Alpha MEMBER Use_Alpha)
    Q_PROPERTY(int      Threshold MEMBER Threshold)

public:

    DXT1_Alpha()
    {
        init();
    }

    void init()
    {
        No_Alpha  = false;
        Use_Alpha = false;
        Threshold = 0;
    }

    bool No_Alpha;
    bool Use_Alpha;
    int  Threshold;
};


class Channel_Weighting : public DXT1_Alpha
{
Q_OBJECT
    Q_PROPERTY(bool     Adaptive   MEMBER Adaptive)
    Q_PROPERTY(int      X_RED      MEMBER X_RED)
    Q_PROPERTY(int      Y_GREEN    MEMBER Y_GREEN)
    Q_PROPERTY(int      Z_BLUE     MEMBER Z_BLUE)

public:
    Channel_Weighting()
    {
        Adaptive = false;
        X_RED = 0.0;
        Y_GREEN = 0.0;
        Z_BLUE = 0.0;
    }

    bool   Adaptive;
    double X_RED;
    double Y_GREEN;
    double Z_BLUE;
};


#define DESTINATION_IMAGE_CLASS_NAME      "Destination Image"
#define DESTINATION_IMAGE_NAME            "Name"
#define DESTINATION_IMAGE_FILESIZE        "File Size"
#define DESTINATION_IMAGE_NOTPROCESSED    "Not Processed"
#define DESTINATION_IMAGE_UNKNOWN         "Unknown"

class C_Destination_Image: 
    // public Channel_Weighting 
    public QObject
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


class C_Destination_Options : public C_Destination_Image
{
    Q_OBJECT
        Q_ENUMS(eCompression)
        Q_PROPERTY(eCompression Format        READ getCompression WRITE setCompression NOTIFY compressionChanged)
        Q_PROPERTY(double       Quality       READ getQuality     WRITE setQuality NOTIFY qualityChanged)

public:
    enum eCompression {
        GT,
        BC1,
        BC2,
        BC3,
        BC4,
        BC5,
        BC6H,
        BC7,
        ASTC,
        ATC_RGB,
        ATC_RGBA_Explicit,
        ATC_RGBA_Interpolated,
        ATI1N,
        ATI2N,
        ATI2N_XY,
        ATI2N_DXT5,
        DXT3,
        DXT5,
        DXT5_xGBR,
        DXT5_RxBG,
        DXT5_RBxG,
        DXT5_xRBG,
        DXT5_RGxB,
        DXT5_xGxR,
        ETC_RGB,
        ARGB_8888,
        //RGB_888,
        //RG_8,
        //R_8,
        //ARGB_2101010,
        //ARGB_16,
        //RG_16,
        //R_16,
        //ARGB_16F,
        //RG_16F,
        //R_16F,
        ARGB_32F,
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
        m_settoUseOnlyBC6 = false;
        m_SourceIscompressedFormat = false;     // Flag indicating source is compressed format
        m_SourceImageSize = 0;
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
        if (m_settoUseOnlyBC6)
        {
            m_Compression = C_Destination_Options::BC6H;
            emit compressionChanged((QVariant &)Compression);
        }
        else
        {
            if (m_Compression != Compression)
            {
                m_data_has_been_changed = true;
                emit dataChanged();
            }
            m_Compression = Compression;
            emit compressionChanged((QVariant &)Compression);
        }
    }

    eCompression getCompression() const
    {
        if (m_settoUseOnlyBC6)
            return C_Destination_Options::BC6H;
        else
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
    bool         m_data_has_been_changed;
    CMipImages  *m_MipImages;
    bool         m_isselected;
    bool         m_settoUseOnlyBC6;
    bool         m_SourceIscompressedFormat;
    long         m_SourceImageSize;

signals:
    void compressionChanged(QVariant &);
    void qualityChanged(QVariant &);
    void dataChanged();

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
        ds.m_compname               = obj.m_compname;
        ds.m_destFileNamePath       = obj.m_destFileNamePath;
        ds.m_editing                = obj.m_editing;
        ds.m_sourceFileNamePath     = obj.m_sourceFileNamePath;
        ds.m_iscompressedFormat     = obj.m_iscompressedFormat;
        ds.m_data_has_been_changed  = obj.m_data_has_been_changed;
        ds.m_settoUseOnlyBC6        = obj.m_settoUseOnlyBC6;
        ds.m_SourceImageSize        = obj.m_SourceImageSize;

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
#define APP_Decompress_image_views_using                "View compressed images using"
#define APP_Reload_image_views_on_selection             "Reload image views on selection"
#define APP_Load_recent_project_on_startup              "Load recent project on startup"
#define APP_Close_all_image_views_prior_to_process      "Close all image views prior to process"

class C_Application_Options :public QObject
{
    Q_OBJECT

#ifdef  ENABLED_USER_GPUVIEW
        Q_PROPERTY(ImageViewDecode  View_compressed_images_using            READ getImageViewDecode     WRITE setImageViewDecode NOTIFY ImageViewDecodeChanged)
#endif
        Q_PROPERTY(bool             Reload_image_views_on_selection         READ getUseNewImageViews        WRITE setUseNewImageViews)
        Q_PROPERTY(bool             Close_all_image_views_prior_to_process  READ getCloseAllImageViews      WRITE setCloseAllImageViews)
        Q_PROPERTY(bool             Load_recent_project_on_startup          READ getLoadRecentFile          WRITE setLoadRecentFile)

public:
    
    enum ImageViewDecode{
        CPU,
        GPU,
    };

    Q_ENUM(ImageViewDecode)


    // Flags how image views are used, True deletes the old view and creates a new one
    // every time user clicks on an image item on the Project View, else it will
    // load the image once and cashe the image views, Default is True

    bool m_useNewImageViews;
    
    
    C_Application_Options()
    {
        m_ImageViewDecode    = ImageViewDecode::CPU;
        m_loadRecentFile     = false;
        m_useNewImageViews   = true;
        m_refreshCurrentView = false;
        m_closeAllDocuments  = true;
    }

    void setImageViewDecode(ImageViewDecode decodewith)
    {
        m_ImageViewDecode = decodewith;
        emit ImageViewDecodeChanged((QVariant &)decodewith);
    }

    ImageViewDecode getImageViewDecode() const
    {
        return m_ImageViewDecode;
    }

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

    ImageViewDecode m_ImageViewDecode;
    bool            m_closeAllDocuments;
    bool            m_loadRecentFile;
    bool            m_refreshCurrentView;

signals:
    void ImageViewDecodeChanged(QVariant &);

};


#define STR_QUALITY_SETTING_HINT        "Quality Setting Range 0 (Poor)to 1 (High)Default is 0.05"
#define STR_QUALITY_SETTING_MINIMUM     "minimum"
#define STR_QUALITY_SETTING_MAXIMUM     "maximum"
#define STR_QUALITY_SETTING_SINGLESTEP  "singleStep"

#define UNKNOWN_IMAGE                    " Unknown"
#define IMAGE_TYPE_PLUGIN                "IMAGE"

#endif