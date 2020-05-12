
#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "Compressonator.h"
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

#ifndef _WIN32
typedef CMP_DWORD* LPDWORD;
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
#ifdef _WIN32
    pPluginVersion->guid                    = g_GUID;
#endif
    pPluginVersion->dwAPIVersionMajor        = TC_API_VERSION_MAJOR;
    pPluginVersion->dwAPIVersionMinor        = TC_API_VERSION_MINOR;
    pPluginVersion->dwPluginVersionMajor    = TC_PLUGIN_VERSION_MAJOR;
    pPluginVersion->dwPluginVersionMinor    = TC_PLUGIN_VERSION_MINOR;
    return 0;
}

int Plugin_TGA::TC_PluginFileLoadTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
   //MessageBox(0,"TC_PluginFileLoadTexture srcTexture","Plugin_TGA",MB_OK);  
    return 0;
 
}

int Plugin_TGA::TC_PluginFileSaveTexture(const char* pszFilename, CMP_Texture *srcTexture)
{
   //MessageBox(0,"TC_PluginFileSaveTexture srcTexture","Plugin_TGA",MB_OK);  
    return 0;
}

// #include "LoadTGA.h"

int Plugin_TGA::TC_PluginFileLoadTexture(const char* pszFilename, MipSet* pMipSet)
{
   CMP_CMIPS lCMips;
   if (!TGA_CMips)
   {
       TGA_CMips = &lCMips;
   }

   // ATI code
   FILE* pFile = NULL;
   pFile = fopen(pszFilename, ("rb"));
   if(pFile == NULL)
    {
        if (TGA_CMips)
            TGA_CMips->PrintError(("Error(%d): TGA Plugin ID(%d) opening file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    // Read the header
    TGAHeader header;
   if(fread(&header, sizeof(TGAHeader), 1, pFile) != 1)
   {
      if (TGA_CMips)
            TGA_CMips->PrintError(("Error(%d): TGA Plugin ID(%d) invalid TGA header. Filename = %s "), EL_Error, IDS_ERROR_NOT_TGA, pszFilename);
      fclose(pFile);
      return -1;
   }

    // Skip the ID field
    if(header.cIDFieldLength)
        fseek(pFile, header.cIDFieldLength, SEEK_CUR);

    if(!TGA_CMips->AllocateMipSet(pMipSet, CF_8bit, TDT_ARGB, TT_2D, header.nWidth, header.nHeight, 1))     // depthsupport, what should nDepth be set as here?
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
            TGA_CMips->PrintError(("Error(%d): TGA Plugin ID(%d) unsupported type Filename = %s "), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, pszFilename);
   fclose(pFile);

   return -1;
}


int Plugin_TGA::TC_PluginFileSaveTexture(const char* pszFilename, MipSet* pMipSet)
{
   CMP_CMIPS lCMips;
   if (!TGA_CMips)
   {
       TGA_CMips = &lCMips;
   }
    assert(pszFilename);
    assert(pMipSet);

   FILE* pFile = NULL;
   pFile = fopen(pszFilename, ("wb"));
   if(pFile == NULL)
   {
        if (TGA_CMips)
            TGA_CMips->PrintError(("Error(%d): TGA Plugin ID(%d) saving file = %s "), EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
        return -1;
    }

    TGAHeader header;
    memset(&header, 0, sizeof(header));
    switch(pMipSet->m_dwFourCC)
    {
        case CMP_MAKEFOURCC('G', '8', ' ', ' '):
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
            TGA_CMips->PrintError(("Error(%d): TGA Plugin ID(%d) unsupported type Filename = %s "), EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, pszFilename);
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
    CMP_DWORD dwSize = pMipSet->m_nWidth *  pMipSet->m_nHeight * sizeof(CMP_COLOR);
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwSize));
    fread(pTempData, dwSize, 1, pFile);
    fclose(pFile);

    CMP_BYTE* pTempPtr = pTempData;

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

    CMP_BYTE nBlue ;
    CMP_BYTE nGreen;
    CMP_BYTE nRed  ;
    CMP_BYTE nAlpha;

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth * sizeof(CMP_COLOR)));
        for(int i=0; i < pMipSet->m_nWidth; i++)
        {
            // Note MIPSet is  RGBA
            // TGA is saved as BGRA
            nBlue  = *pTempPtr++;
            nGreen = *pTempPtr++;
            nRed   = *pTempPtr++;
            nAlpha = *pTempPtr++;

            // printf("{%d,%d,%d,%d}\n", nRed, nGreen, nBlue, nAlpha);

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
    CMP_DWORD dwTempSize = lEndPos - lCurrPos;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwTempSize));
    fread(pTempData, dwTempSize, 1, pFile);
    fclose(pFile);

    CMP_DWORD dwPitch = pMipSet->m_nWidth * sizeof(CMP_COLOR);
    CMP_BYTE* pTempPtr = pTempData;

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
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));

        int nColumn =0;
        while(nColumn < pMipSet->m_nWidth)
        {
            unsigned char nLength = *pTempPtr++;
            if(nLength & 0x80)
            {
                int nRepeat = nLength - 0x7f;
                CMP_BYTE nBlue = *pTempPtr++;
                CMP_BYTE nGreen = *pTempPtr++;
                CMP_BYTE nRed = *pTempPtr++;
                CMP_BYTE nAlpha = *pTempPtr++;

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
                    CMP_BYTE nBlue = *pTempPtr++;
                    CMP_BYTE nGreen = *pTempPtr++;
                    CMP_BYTE nRed = *pTempPtr++;
                    CMP_BYTE nAlpha = *pTempPtr++;
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
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_ARGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat    = CF_8bit;
    pMipSet->m_TextureDataType  = TDT_ARGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_nMipLevels       = 1;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;

    // Allocate a temporary buffer and read the bitmap data into it
    CMP_DWORD dwSize = pMipSet->m_nWidth *  pMipSet->m_nHeight * 3;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwSize));
    fread(pTempData, dwSize, 1, pFile);
    fclose(pFile);

    CMP_BYTE* pTempPtr = pTempData;

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

    CMP_BYTE nBlue;
    CMP_BYTE nGreen;
    CMP_BYTE nRed;

    for(int j = nStart; j != nEnd; j+= nIncrement)
    {
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth * sizeof(CMP_COLOR)));
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
    if(!TGA_CMips->AllocateMipLevelData(TGA_CMips->GetMipLevel(pMipSet, 0), Header.nWidth, Header.nHeight, CF_8bit, TDT_ARGB))
        return PE_Unknown;

    pMipSet->m_ChannelFormat    = CF_8bit;
    pMipSet->m_TextureDataType  = TDT_ARGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_nMipLevels       = 1;
    pMipSet->m_format = CMP_FORMAT_ARGB_8888;

    // Allocate a temporary buffer and read the bitmap data into it
    long lCurrPos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long lEndPos = ftell(pFile);
    fseek(pFile, lCurrPos, SEEK_SET);
    CMP_DWORD dwTempSize = lEndPos - lCurrPos;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwTempSize));
    fread(pTempData, dwTempSize, 1, pFile);
    fclose(pFile);

    CMP_BYTE* pTempPtr = pTempData;
    CMP_DWORD dwPitchOut = pMipSet->m_nWidth * sizeof(CMP_COLOR);

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
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitchOut));

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
                CMP_BYTE nBlue  = *pTempPtr++;
                CMP_BYTE nGreen = *pTempPtr++;
                CMP_BYTE nRed   = *pTempPtr++;
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
                    CMP_BYTE nBlue  = *pTempPtr++;
                    CMP_BYTE nGreen = *pTempPtr++;
                    CMP_BYTE nRed   = *pTempPtr++;
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
    pMipSet->m_dwFourCC             = CMP_MAKEFOURCC('G', '8', ' ', ' ');
    pMipSet->m_dwFourCC2            = 0;
    pMipSet->m_nMipLevels           = 1;

    // Allocate a temporary buffer and read the bitmap data into it
    CMP_DWORD dwSize = pMipSet->m_nWidth *  pMipSet->m_nHeight * sizeof(CMP_BYTE);
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwSize));
    fread(pTempData, dwSize, 1, pFile);
    fclose(pFile);

    CMP_BYTE* pTempPtr = pTempData;

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
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth));
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
    pMipSet->m_TextureDataType  = TDT_ARGB;
    pMipSet->m_dwFourCC         = 0;
    pMipSet->m_dwFourCC2        = 0;
    pMipSet->m_nMipLevels       = 1;
    pMipSet->m_format           = CMP_FORMAT_ARGB_8888;

    // Allocate a temporary buffer and read the bitmap data into it
    long lCurrPos = ftell(pFile);
    fseek(pFile, 0, SEEK_END);
    long lEndPos = ftell(pFile);
    fseek(pFile, lCurrPos, SEEK_SET);
    CMP_DWORD dwTempSize = lEndPos - lCurrPos;
    unsigned char* pTempData = static_cast<unsigned char*>(malloc(dwTempSize));
    fread(pTempData, dwTempSize, 1, pFile);
    fclose(pFile);

    CMP_BYTE* pTempPtr = pTempData;

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
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth * sizeof(CMP_COLOR)));

        int nColumn =0;
        while(nColumn < pMipSet->m_nWidth)
        {
            unsigned char nLength = *pTempPtr++;
            if(nLength & 0x80)
            {
                int nRepeat = nLength - 0x7f;
                CMP_BYTE gray = *pTempPtr++;
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
                CMP_BYTE gray = *pTempPtr++;
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

CMP_BYTE CalcRunLength(CMP_BYTE* pThis, CMP_BYTE* pEnd, int nSize, int nOffset)
{
    CMP_BYTE* pNext = pThis + nOffset;
    CMP_BYTE cRunLength = 1;

    while(pNext < pEnd && memcmp(pThis, pNext, nSize) == 0 && cRunLength < 0x80)
    {
        cRunLength++;
        pNext += nOffset;
    }

    return cRunLength;
}

CMP_BYTE CalcRawLength(CMP_BYTE* pThis, CMP_BYTE* pEnd, int nSize, int nOffset)
{
    CMP_BYTE* pNext = pThis + nOffset;
    CMP_BYTE cRawLength = 1;

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

void SaveLineRLE(FILE* pFile, CMP_BYTE* pThis, CMP_BYTE* pEnd, int nSize, int nOffset)
{
    while (pThis < pEnd)
    {
        CMP_BYTE* pNext = pThis + nOffset;
        // Are the next two pixel the same ?
        if(pNext < pEnd && memcmp(pThis, pNext, nSize) == 0)
        {
            CMP_BYTE cRunLength = CalcRunLength(pThis, pEnd, nSize, nOffset);
            CMP_BYTE cRepetitionCount = (cRunLength - 1) | 0x80;
            fwrite(&cRepetitionCount, sizeof(cRepetitionCount), 1, pFile);
            fwrite(pThis, nSize, 1, pFile);
            pThis += (cRunLength * nOffset);
        }
        else
        {
            CMP_BYTE cRawLength = CalcRawLength(pThis, pEnd, nSize, nOffset);
            CMP_BYTE cRepetitionCount = (cRawLength - 1);
            fwrite(&cRepetitionCount, sizeof(cRepetitionCount), 1, pFile);
            if(nSize == nOffset)
            {
                fwrite(pThis, nSize*cRawLength, 1, pFile);
                pThis += (cRawLength * nOffset);
            }
            else
            {
                for(CMP_BYTE i = 0; i < cRawLength; i++)
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
    CMP_DWORD dwPitch = pMipSet->m_nWidth * nOffset;
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
    {
        CMP_BYTE* pThis = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
        CMP_BYTE* pEnd = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + ((j+1) * dwPitch));

        SaveLineRLE(pFile, pThis, pEnd, nSize, nOffset);
    }

    fclose(pFile);

    return PE_OK;
}

//--------------------------------------------------------
// Source mipset should now always be RGBA
// Code using the pMipSet->m_swizzle should be reviewed
//--------------------------------------------------------

TC_PluginError SaveTGA_ARGB8888(FILE* pFile, const MipSet* pMipSet)
{
    CMP_DWORD dwPitch = pMipSet->m_nWidth * 4;

    //if (pMipSet->m_swizzle)
    {
        CMP_BYTE RGBA[4];
        for (int j = pMipSet->m_nHeight - 1; j >= 0; j--)
        {
            CMP_BYTE* pData = (CMP_BYTE*)(TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
            for (int i = 0; i < pMipSet->m_nWidth; i++)
            {
                RGBA[2] = (CMP_BYTE)*pData++;
                RGBA[1] = (CMP_BYTE)*pData++;
                RGBA[0] = (CMP_BYTE)*pData++;
                RGBA[3] = (CMP_BYTE)*pData++;
                fwrite(RGBA, 4, 1, pFile);
            }
        }
    }
    //else
    //{
    //    for (int j = pMipSet->m_nHeight - 1; j >= 0; j--)
    //    {
    //        CMP_BYTE* pData = (CMP_BYTE*)(TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
    //        fwrite(pData, dwPitch, 1, pFile);
    //    }
    //}

    fclose(pFile);


    return PE_OK;
}

TC_PluginError SaveTGA_ARGB8888_RLE(FILE* pFile, const MipSet* pMipSet)
{
    return SaveRLE(pFile, pMipSet, sizeof(CMP_COLOR), sizeof(CMP_COLOR));
}

TC_PluginError SaveTGA_RGB888(FILE* pFile, const MipSet* pMipSet)
{
    CMP_DWORD dwPitch = pMipSet->m_nWidth * sizeof(CMP_COLOR);
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
        //    for(int j=0; j < pMipSet->m_nHeight-1; j++)
    {
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * dwPitch));
        for(int i=0; i < pMipSet->m_nWidth; i++)
        {
            fwrite(pData, 3, 1, pFile);
            pData += sizeof(CMP_COLOR);
        }
    }

    fclose(pFile);

    return PE_OK;
}

TC_PluginError SaveTGA_RGB888_RLE(FILE* pFile, const MipSet* pMipSet)
{
    return SaveRLE(pFile, pMipSet, 3, sizeof(CMP_COLOR));
}

TC_PluginError SaveTGA_G8(FILE* pFile, const MipSet* pMipSet)
{
    for(int j=pMipSet->m_nHeight-1; j>=0; j--)
    {
        CMP_BYTE* pData = (CMP_BYTE*) (TGA_CMips->GetMipLevel(pMipSet, 0)->m_pbData + (j * pMipSet->m_nWidth));
        fwrite(pData, pMipSet->m_nWidth, 1, pFile);
    }

    fclose(pFile);

    return PE_OK;
}

TC_PluginError SaveTGA_G8_RLE(FILE* pFile, const MipSet* pMipSet)
{
    return SaveRLE(pFile, pMipSet, sizeof(CMP_BYTE), sizeof(CMP_BYTE));
}


// ------------ Registry!
#define DEFAULT_JPG_QUALITY 75
const char szKeyTGAPlugin[] = ("Software\\ATI Research, Inc.\\TextureAPI\\Plugins\\TGA");

void LoadRegistryKeyDefaults(TGA_FileSaveParams* pParams)
{
    memset(pParams, 0, sizeof(TGA_FileSaveParams));
    pParams->dwSize = sizeof(TGA_FileSaveParams);

    pParams->bRLECompressed = true;
}

void LoadRegistryKeys(TGA_FileSaveParams* pParams)
{
#ifdef _WIN32
    LoadRegistryKeyDefaults(pParams);

    HKEY hKey;
    CMP_DWORD dwSize;
    RegOpenKeyEx(HKEY_CURRENT_USER, szKeyTGAPlugin, 0, KEY_ALL_ACCESS, &hKey);

    CMP_DWORD dwTemp;
    if(RegQueryValueEx(hKey, ("RLECompressed"), NULL, NULL, (CMP_BYTE*)&dwTemp, (LPDWORD)&dwSize) != ERROR_SUCCESS)
        pParams->bRLECompressed = dwTemp ? true : false;

    RegCloseKey(hKey);
#endif
}

void SaveRegistryKeys(const TGA_FileSaveParams* pParams)
{
#ifdef _WIN32
    HKEY hKey;
    RegCreateKeyEx(HKEY_CURRENT_USER, szKeyTGAPlugin, 0, (""), REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);

    CMP_DWORD dwTemp;
    dwTemp = pParams->bRLECompressed ? 1 : 0;
    RegSetValueEx(hKey, ("RLECompressed"), NULL, REG_DWORD, (CMP_BYTE*) &dwTemp, sizeof(CMP_DWORD));

    RegCloseKey(hKey);
#endif
}
