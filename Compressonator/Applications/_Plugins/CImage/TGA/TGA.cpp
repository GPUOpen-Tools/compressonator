//#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "MIPS.h"
#include "TGA.h"

CMIPS *TGA_CMips;
TGA_FileSaveParams g_FileSaveParams;

#ifdef BUILD_AS_PLUGIN_DLL
DECLARE_PLUGIN(Plugin_TGA)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("TGA")
#else
void *make_Plugin_TGA() { return new Plugin_TGA; } 
#endif

/*
..\.\..\..\..\..\Common\Lib\Ext\OpenEXR\v1.4.0\lib_MT\$(Configuration)\$(Platform);
zlibstatic_d.lib
*/

#pragma comment(lib,"advapi32.lib")        // for RegCloseKey and other Reg calls ...

Plugin_TGA::Plugin_TGA()
{ 
    //MessageBox(0,"Plugin_TGA","Plugin_TGA",MB_OK);  
}

Plugin_TGA::~Plugin_TGA()
{ 
    //MessageBox(0,"Plugin_TGA","~Plugin_TGA",MB_OK);  
}

int Plugin_TGA::TC_PluginSetSharedIO(void* Shared)
{
    if (Shared)
    {
        TGA_CMips = static_cast<CMIPS *>(Shared);
        return 0;
    }
    return 1;
}


int Plugin_TGA::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
    //MessageBox(0,"TC_PluginGetVersion","Plugin_TGA",MB_OK);  
    pPluginVersion->guid                    = g_GUID;
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_TGA::TC_PluginFileLoadTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture)
{
   //MessageBox(0,"TC_PluginFileLoadTexture srcTexture","Plugin_TGA",MB_OK);  
    return 0;
 
}

int Plugin_TGA::TC_PluginFileSaveTexture(const TCHAR* pszFilename, CMP_Texture *srcTexture)
{
   //MessageBox(0,"TC_PluginFileSaveTexture srcTexture","Plugin_TGA",MB_OK);  
    return 0;
}

// #include "LoadTGA.h"

int Plugin_TGA::TC_PluginFileLoadTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{

   // ATI code
   FILE* pFile = NULL;
   if(_tfopen_s(&pFile, pszFilename, _T("rb")) != 0 || pFile == NULL)
    {
        if (TGA_CMips)
            TGA_CMips->PrintError(_T("Error(%d): TGA Plugin ID(%d) opening file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    // Read the header
    TGAHeader header;
   if(fread(&header, sizeof(TGAHeader), 1, pFile) != 1)
   {
      if (TGA_CMips)
            TGA_CMips->PrintError(_T("Error(%d): TGA Plugin ID(%d) invalid TGA header. Filename = %s "), EL_Error, IDS_ERROR_NOT_TGA, pszFilename);
      fclose(pFile);
      return -1;
   }

    // Skip the ID field
    if(header.cIDFieldLength)
        fseek(pFile, header.cIDFieldLength, SEEK_CUR);

    if(!TGA_CMips->AllocateMipSet(pMipSet, CF_8bit, TDT_ARGB, TT_2D, header.nWidth, header.nHeight, 1))
    {
        fclose(pFile);
        return PE_Unknown;
    }

    if(header.cColorMapType == 0)
    {
        if(header.cImageType == ImageType_ARGB8888 && header.cColorDepth == 32)
            return LoadTGA_ARGB8888(pFile, pMipSet, header);
        else if(header.cImageType == ImageType_ARGB8888_RLE && header.cColorDepth == 32)
            return LoadTGA_ARGB8888_RLE(pFile, pMipSet, header);
        else if(header.cImageType == ImageType_ARGB8888 && header.cColorDepth == 24)
            return LoadTGA_RGB888(pFile, pMipSet, header);
        else if(header.cImageType == ImageType_ARGB8888_RLE && header.cColorDepth == 24)
            return LoadTGA_RGB888_RLE(pFile, pMipSet, header);
        else if(header.cImageType == ImageType_G8 && header.cColorDepth == 8)
            return LoadTGA_G8(pFile, pMipSet, header);
        else if(header.cImageType == ImageType_G8_RLE && header.cColorDepth == 8)
            return LoadTGA_G8_RLE(pFile, pMipSet, header);
    }

   if (TGA_CMips)
            TGA_CMips->PrintError(_T("Error(%d): TGA Plugin ID(%d) unsupported type Filename = %s "), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, pszFilename);
   fclose(pFile);

   return -1;
}


int Plugin_TGA::TC_PluginFileSaveTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
    assert(pszFilename);
    assert(pMipSet);

   FILE* pFile = NULL;
   if(_tfopen_s(&pFile, pszFilename, _T("wb")) != 0 || pFile == NULL)
   {
        if (TGA_CMips)
            TGA_CMips->PrintError(_T("Error(%d): TGA Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    TGAHeader header;
    memset(&header, 0, sizeof(header));
    switch(pMipSet->m_dwFourCC)
    {
        case MAKEFOURCC('G', '8', ' ', ' '):
            header.cImageType = ImageType_G8_RLE; 
            header.cColorDepth = 8;
            break;
        case 0: // Not a FourCC texture
        default:
            if(pMipSet->m_TextureDataType == TDT_ARGB)
            {
                header.cColorDepth = 32;
                header.cFormatFlags = 0x8;
            }
            else
                header.cColorDepth = 24;
            header.cImageType = ImageType_ARGB8888;
            break;
    }

    // napatel need to add this somehow!
    //if(pTGAFileSaveParams->bRLECompressed)
    //    header.cImageType |= 0x8;

    header.nWidth = static_cast<short>(pMipSet->m_nWidth);
    header.nHeight = static_cast<short>(pMipSet->m_nHeight);

    fwrite(&header, sizeof(header), 1, pFile);

    if(header.cImageType == ImageType_G8)
        return SaveTGA_G8(pFile, pMipSet);
    else if(header.cImageType == ImageType_G8_RLE)
        return SaveTGA_G8_RLE(pFile, pMipSet);
    else if(header.cImageType == ImageType_ARGB8888 && pMipSet->m_TextureDataType == TDT_ARGB)
        return SaveTGA_ARGB8888(pFile, pMipSet);
    else if(header.cImageType == ImageType_ARGB8888_RLE && pMipSet->m_TextureDataType == TDT_ARGB)
        return SaveTGA_ARGB8888_RLE(pFile, pMipSet);
    else if(header.cImageType == ImageType_ARGB8888 && pMipSet->m_TextureDataType == TDT_XRGB)
        return SaveTGA_RGB888(pFile, pMipSet);
    else if(header.cImageType == ImageType_ARGB8888_RLE && pMipSet->m_TextureDataType == TDT_XRGB)
        return SaveTGA_RGB888_RLE(pFile, pMipSet);

   if (TGA_CMips)
            TGA_CMips->PrintError(_T("Error(%d): TGA Plugin ID(%d) unsupported type Filename = %s "), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, pszFilename);
   fclose(pFile);

   return -1;
}


//---------------- TGA Code -----------------------------------


TC_PluginError LoadTGA_ARGB8888(FILE* pFile, MipSet* pMipSet, TGAHeader& Header)
{
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_ARGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat = CF_8bit;
    pMipSet->m_TextureDataType = TDT_ARGB;
    pMipSet->m_dwFourCC = 0;
    pMipSet->m_dwFourCC2 = 0;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;
    pMipSet->m_nMipLevels = 1;

    // Allocate a temporary buffer and read the bitmap data into it
    DWORD dwSize = pMipSet->m_nWidth *  pMipSet->m_nHeight * sizeof(COLOR);
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwSize));
    fread(pTempData, dwSize, 1, pFile);
    fclose(pFile);

    BYTE* pTempPtr = pTempData;

    int nStart, nEnd, nIncrement;
    // Bottom up ?
    if(Header.cFormatFlags & 0x20)
    {
        nStart = 0;
        nEnd = pMipSet->m_nHeight;
        nIncrement = 1;
    }
    else
    {
        nStart = pMipSet->m_nHeight-1;
        nEnd = -1;
        nIncrement = -1;
    }

    char nBlue ;
    char nGreen;
    char nRed  ;
    char nAlpha;

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth * sizeof(COLOR)));
        for(int i=0; i < pMipSet->m_nWidth; i++)
        {
            // Note MIPSet is  RGBA
            // TGA is saved as BGRA
            nBlue  = *pTempPtr++;
            nGreen = *pTempPtr++;
            nRed   = *pTempPtr++;
            nAlpha = *pTempPtr++;

            *pData++ = nRed;
            *pData++ = nGreen;
            *pData++ = nBlue;
            *pData++ = nAlpha;


        }
    }

    free(pTempData);

    return PE_OK;
}

TC_PluginError LoadTGA_ARGB8888_RLE(FILE* pFile, MipSet* pMipSet, TGAHeader& Header)
{
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_ARGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat = CF_8bit;
    pMipSet->m_TextureDataType = TDT_ARGB;
    pMipSet->m_dwFourCC = 0;
    pMipSet->m_dwFourCC2 = 0;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;
    pMipSet->m_nMipLevels = 1;

    // Allocate a temporary buffer and read the bitmap data into it
    long lCurrPos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long lEndPos = ftell(pFile);
    fseek(pFile, lCurrPos, SEEK_SET);
    DWORD dwTempSize = lEndPos - lCurrPos;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwTempSize));
    fread(pTempData, dwTempSize, 1, pFile);
    fclose(pFile);

    DWORD dwPitch = pMipSet->m_nWidth * sizeof(COLOR);
    BYTE* pTempPtr = pTempData;

    int nStart, nEnd, nIncrement;
    // Bottom up ?
    if(Header.cFormatFlags & 0x20)
    {
        nStart = 0;
        nEnd = pMipSet->m_nHeight;
        nIncrement = 1;
    }
    else
    {
        nStart = pMipSet->m_nHeight-1;
        nEnd = -1;
        nIncrement = -1;
    }

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));

        int nColumn =0;
        while(nColumn < pMipSet->m_nWidth)
        {
            unsigned char nLength = *pTempPtr++;
            if(nLength & 0x80)
            {
                int nRepeat = nLength - 0x7f;
                char nBlue = *pTempPtr++;
                char nGreen = *pTempPtr++;
                char nRed = *pTempPtr++;
                char nAlpha = *pTempPtr++;

                while(nRepeat && nColumn < pMipSet->m_nWidth)
                {
                    *pData++ = nRed;
                    *pData++ = nGreen;
                    *pData++ = nBlue;
                    *pData++ = nAlpha;
                    nColumn++;
                    nRepeat--;
                }

            }
            else
            {
                int nRepeat = nLength+1;
                while(nRepeat && nColumn < pMipSet->m_nWidth)
                {
                    char nBlue = *pTempPtr++;
                    char nGreen = *pTempPtr++;
                    char nRed = *pTempPtr++;
                    char nAlpha = *pTempPtr++;
                    *pData++ = nRed;
                    *pData++ = nGreen;
                    *pData++ = nBlue;
                    *pData++ = nAlpha;
                    nColumn++;
                    nRepeat--;
                }
            }
        }
    }

    free(pTempData);

    return PE_OK;
}

TC_PluginError LoadTGA_RGB888(FILE* pFile, MipSet* pMipSet, TGAHeader& Header)
{
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_XRGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat    = CF_8bit;
    pMipSet->m_TextureDataType  = TDT_XRGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_nMipLevels       = 1;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;

    // Allocate a temporary buffer and read the bitmap data into it
    DWORD dwSize = pMipSet->m_nWidth *  pMipSet->m_nHeight * 3;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwSize));
    fread(pTempData, dwSize, 1, pFile);
    fclose(pFile);

    BYTE* pTempPtr = pTempData;

    int nStart, nEnd, nIncrement;
    // Bottom up ?
    if(Header.cFormatFlags & 0x20)
    {
        nStart = 0;
        nEnd = pMipSet->m_nHeight;
        nIncrement = 1;
    }
    else
    {
        nStart = pMipSet->m_nHeight-1;
        nEnd = -1;
        nIncrement = -1;
    }

    char nBlue;
    char nGreen;
    char nRed;

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth * sizeof(COLOR)));
        for(int i=0; i < pMipSet->m_nWidth; i++)
        {
            nBlue    = *pTempPtr++;
            nGreen   = *pTempPtr++;
            nRed     = *pTempPtr++;

            *pData++ = nRed;
            *pData++ = nGreen;
            *pData++ = nBlue;
            *pData++ = 0xff;
        }
    }

    free(pTempData);

    return PE_OK;
}

TC_PluginError LoadTGA_RGB888_RLE(FILE* pFile, MipSet* pMipSet, TGAHeader& Header)
{
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_XRGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat    = CF_8bit;
    pMipSet->m_TextureDataType  = TDT_XRGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_nMipLevels       = 1;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;

    // Allocate a temporary buffer and read the bitmap data into it
    long lCurrPos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long lEndPos = ftell(pFile);
    fseek(pFile, lCurrPos, SEEK_SET);
    DWORD dwTempSize = lEndPos - lCurrPos;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwTempSize));
    fread(pTempData, dwTempSize, 1, pFile);
    fclose(pFile);

    BYTE* pTempPtr = pTempData;
    DWORD dwPitchOut = pMipSet->m_nWidth * sizeof(COLOR);

    int nStart, nEnd, nIncrement;
    // Bottom up ?
    if(Header.cFormatFlags & 0x20)
    {
        nStart = 0;
        nEnd = pMipSet->m_nHeight;
        nIncrement = 1;
    }
    else
    {
        nStart = pMipSet->m_nHeight-1;
        nEnd = -1;
        nIncrement = -1;
    }

    unsigned char nRepeat = 0;
    unsigned char nLength = 0;
    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitchOut));

        int nColumn =0;
        while(nColumn < pMipSet->m_nWidth)
        {
            if(nRepeat == 0)
            {
                nLength = *pTempPtr++;
                nRepeat = (nLength & 0x7f) + 1;
            }

            if(nLength & 0x80)
            {
                char nBlue  = *pTempPtr++;
                char nGreen = *pTempPtr++;
                char nRed   = *pTempPtr++;
                while(nRepeat && nColumn < pMipSet->m_nWidth)
                {
                    *pData++ = nRed;
                    *pData++ = nGreen;
                    *pData++ = nBlue;
                    *pData++ = 0xff;
                    nColumn++;
                    nRepeat--;
                }
            }
            else
            {
                while(nRepeat && nColumn < pMipSet->m_nWidth)
                {
                    char nBlue  = *pTempPtr++;
                    char nGreen = *pTempPtr++;
                    char nRed   = *pTempPtr++;
                    *pData++    = nRed;
                    *pData++    = nGreen;
                    *pData++    = nBlue;
                    *pData++    = 0xff;
                    nColumn++;
                    nRepeat--;
                }
            }
        }
    }

    free(pTempData);

    return PE_OK;
}

TC_PluginError LoadTGA_G8(FILE* pFile, MipSet* pMipSet, TGAHeader& Header)
{
    if(!TGA_CMips->AllocateCompressedMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, Header.nWidth * Header.nHeight))
        return PE_Unknown;

    pMipSet->m_ChannelFormat        = CF_Compressed;
    pMipSet->m_TextureDataType      = TDT_XRGB;
    pMipSet->m_dwFourCC             = MAKEFOURCC('G', '8', ' ', ' ');
    pMipSet->m_dwFourCC2            = 0;
    pMipSet->m_nMipLevels           = 1;

    // Allocate a temporary buffer and read the bitmap data into it
    DWORD dwSize = pMipSet->m_nWidth *  pMipSet->m_nHeight * sizeof(BYTE);
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwSize));
    fread(pTempData, dwSize, 1, pFile);
    fclose(pFile);

    BYTE* pTempPtr = pTempData;

    int nStart, nEnd, nIncrement;
    // Bottom up ?
    if(Header.cFormatFlags & 0x20)
    {
        nStart = 0;
        nEnd = pMipSet->m_nHeight;
        nIncrement = 1;
    }
    else
    {
        nStart = pMipSet->m_nHeight-1;
        nEnd = -1;
        nIncrement = -1;
    }

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth));
        memcpy(pData, pTempPtr, pMipSet->m_nWidth);
        pTempPtr += pMipSet->m_nWidth;
    }

    free(pTempData);

    return PE_OK;
}

TC_PluginError LoadTGA_G8_RLE(FILE* pFile, MipSet* pMipSet, TGAHeader& Header)
{
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_ARGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat    = CF_8bit;
    pMipSet->m_TextureDataType  = TDT_XRGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_nMipLevels       = 1;
    pMipSet->m_format           = CMP_FORMAT_ARGB_8888;

    // Allocate a temporary buffer and read the bitmap data into it
    long lCurrPos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long lEndPos = ftell(pFile);
    fseek(pFile, lCurrPos, SEEK_SET);
    DWORD dwTempSize = lEndPos - lCurrPos;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwTempSize));
    fread(pTempData, dwTempSize, 1, pFile);
    fclose(pFile);

    BYTE* pTempPtr = pTempData;

    int nStart, nEnd, nIncrement;
    // Bottom up ?
    if(Header.cFormatFlags & 0x20)
    {
        nStart = 0;
        nEnd = pMipSet->m_nHeight;
        nIncrement = 1;
    }
    else
    {
        nStart = pMipSet->m_nHeight-1;
        nEnd = -1;
        nIncrement = -1;
    }

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth * sizeof(COLOR)));

        int nColumn =0;
        while(nColumn < pMipSet->m_nWidth)
        {
            unsigned char nLength = *pTempPtr++;
            if(nLength & 0x80)
            {
                int nRepeat = nLength - 0x7f;
                BYTE gray = *pTempPtr++;
                while(nRepeat && nColumn < pMipSet->m_nWidth)
                {
                    *pData++ = gray;
                    *pData++ = gray;
                    *pData++ = gray;
                    *pData++ = 0xff;
                    nColumn++;
                    nRepeat--;
                }
            }
            else
            {
                int nRepeat = nLength+1;
                BYTE gray = *pTempPtr++;
                while(nRepeat && nColumn < pMipSet->m_nWidth)
                {
                    *pData++ = gray;
                    *pData++ = gray;
                    *pData++ = gray;
                    *pData++ = 0xff;
                    nColumn++;
                    nRepeat--;
                }
            }
        }
    }

    free(pTempData);

    return PE_OK;
}

BYTE CalcRunLength(BYTE* pThis, BYTE* pEnd, int nSize, int nOffset)
{
    BYTE* pNext = pThis + nOffset;
    BYTE cRunLength = 1;

    while(pNext < pEnd && memcmp(pThis, pNext, nSize) == 0 && cRunLength < 0x80)
    {
        cRunLength++;
        pNext += nOffset;
    }

    return cRunLength;
}

BYTE CalcRawLength(BYTE* pThis, BYTE* pEnd, int nSize, int nOffset)
{
    BYTE* pNext = pThis + nOffset;
    BYTE cRawLength = 1;

    while(pNext < pEnd && memcmp(pThis, pNext, nSize) != 0 && cRawLength < 0x80)
    {
        cRawLength++;
        pThis += nOffset;
        pNext += nOffset;
    }

    return cRawLength;
}


// *** NOTE ****
// The Save data code needs to be rewored as 
// TGA is saved as BGRA and our MipSet is using RGBA
//
// Current implementation is saving in wronge format as RGBA 
//
// Need to Fix this

void SaveLineRLE(FILE* pFile, BYTE* pThis, BYTE* pEnd, int nSize, int nOffset)
{
    while (pThis < pEnd)
    {
        BYTE* pNext = pThis + nOffset;
        // Are the next two pixel the same ?
        if(pNext < pEnd && memcmp(pThis, pNext, nSize) == 0)
        {
            BYTE cRunLength = CalcRunLength(pThis, pEnd, nSize, nOffset);
            BYTE cRepetitionCount = (cRunLength - 1) | 0x80;
            fwrite(&cRepetitionCount, sizeof(cRepetitionCount), 1, pFile);
            fwrite(pThis, nSize, 1, pFile);
            pThis += (cRunLength * nOffset);
        }
        else
        {
            BYTE cRawLength = CalcRawLength(pThis, pEnd, nSize, nOffset);
            BYTE cRepetitionCount = (cRawLength - 1);
            fwrite(&cRepetitionCount, sizeof(cRepetitionCount), 1, pFile);
            if(nSize == nOffset)
            {
                fwrite(pThis, nSize*cRawLength, 1, pFile);
                pThis += (cRawLength * nOffset);
            }
            else
            {
                for(BYTE i = 0; i < cRawLength; i++)
                {
                    fwrite(pThis, nSize, 1, pFile);
                    pThis += nOffset;
                }
            }
        }
    }
}

TC_PluginError SaveRLE(FILE* pFile, const MipSet* pMipSet, int nSize, int nOffset)
{
    DWORD dwPitch = pMipSet->m_nWidth * nOffset;
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
    {
        BYTE* pThis = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
        BYTE* pEnd = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + ((j+1) * dwPitch));

        SaveLineRLE(pFile, pThis, pEnd, nSize, nOffset);
    }

    fclose(pFile);

    return PE_OK;
}

TC_PluginError SaveTGA_ARGB8888(FILE* pFile, const MipSet* pMipSet)
{
    DWORD dwPitch = pMipSet->m_nWidth * sizeof(COLOR);
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
        fwrite(pData, dwPitch, 1, pFile);
    }

    fclose(pFile);

    return PE_OK;
}

TC_PluginError SaveTGA_ARGB8888_RLE(FILE* pFile, const MipSet* pMipSet)
{
    return SaveRLE(pFile, pMipSet, sizeof(COLOR), sizeof(COLOR));
}

TC_PluginError SaveTGA_RGB888(FILE* pFile, const MipSet* pMipSet)
{
    DWORD dwPitch = pMipSet->m_nWidth * sizeof(COLOR);
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
        //    for(int j=0; j < pMipSet->m_nHeight-1; j++)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
        for(int i=0; i < pMipSet->m_nWidth; i++)
        {
            fwrite(pData, 3, 1, pFile);
            pData += sizeof(COLOR);
        }
    }

    fclose(pFile);

    return PE_OK;
}

TC_PluginError SaveTGA_RGB888_RLE(FILE* pFile, const MipSet* pMipSet)
{
    return SaveRLE(pFile, pMipSet, 3, sizeof(COLOR));
}

TC_PluginError SaveTGA_G8(FILE* pFile, const MipSet* pMipSet)
{
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
    {
        BYTE* pData = (BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth));
        fwrite(pData, pMipSet->m_nWidth, 1, pFile);
    }

    fclose(pFile);

    return PE_OK;
}

TC_PluginError SaveTGA_G8_RLE(FILE* pFile, const MipSet* pMipSet)
{
    return SaveRLE(pFile, pMipSet, sizeof(BYTE), sizeof(BYTE));
}


// ------------ Registry!
#define DEFAULT_JPG_QUALITY 75
const TCHAR szKeyTGAPlugin[] = _T("Software\\ATI Research, Inc.\\TextureAPI\\Plugins\\TGA");

void LoadRegistryKeyDefaults(TGA_FileSaveParams* pParams)
{
    memset(pParams, 0, sizeof(TGA_FileSaveParams));
    pParams->dwSize = sizeof(TGA_FileSaveParams);

    pParams->bRLECompressed = true;
}

void LoadRegistryKeys(TGA_FileSaveParams* pParams)
{
    LoadRegistryKeyDefaults(pParams);

    HKEY hKey;
    DWORD dwSize;
    RegOpenKeyEx(HKEY_CURRENT_USER, szKeyTGAPlugin, 0, KEY_ALL_ACCESS, &hKey);

    DWORD dwTemp;
    if(RegQueryValueEx(hKey, _T("RLECompressed"), NULL, NULL, (BYTE*) &dwTemp, &dwSize) != ERROR_SUCCESS)
        pParams->bRLECompressed = dwTemp ? true : false;

    RegCloseKey(hKey);
}

void SaveRegistryKeys(const TGA_FileSaveParams* pParams)
{
    HKEY hKey;
    RegCreateKeyEx(HKEY_CURRENT_USER, szKeyTGAPlugin, 0, _T(""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

    DWORD dwTemp;
    dwTemp = pParams->bRLECompressed ? 1 : 0;
    RegSetValueEx(hKey, _T("RLECompressed"), NULL, REG_DWORD, (BYTE*) &dwTemp, sizeof(DWORD));

    RegCloseKey(hKey);
}
