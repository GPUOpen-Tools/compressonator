#ifdef _WIN32
#include "stdafx.h"
#include <stdio.h>

#include <stdio.h>
#include <stdlib.h>
#include "TC_PluginAPI.h"
#include "TC_PluginInternal.h"
#include "MIPS.h"

#include "ddraw.h"
#include "d3d9types.h"
#include "DDS_Helpers.h"
//#include "DDS_DX10.h"
#include "MIPS.h"
#include "ATI_Compress.h"
#include "DDS.h"

#ifdef USE_DIRECTXTEX
#include "wincodec.h"
#pragma comment(lib, "DirectXTex.lib")
using namespace DirectX;
#endif

CMIPS CMips;

DECLARE_PLUGIN(Plugin_DDS)
SET_PLUGIN_TYPE("IMAGE")
SET_PLUGIN_NAME("DDS")


Plugin_DDS::Plugin_DDS()
{ 
	HRESULT hr;
	// Initialize COM (needed for WIC)
	if( FAILED( hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED) ) )
	{	
		wprintf( L"Failed to initialize COM (%08X)\n", hr);
	} 
}

Plugin_DDS::~Plugin_DDS()
{ 
}

int Plugin_DDS::TC_PluginGetVersion(TC_PluginVersion* pPluginVersion)
{ 
	pPluginVersion->guid					= g_GUID;
	pPluginVersion->dwAPIVersionMajor		= TC_API_VERSION_MAJOR;
	pPluginVersion->dwAPIVersionMinor		= TC_API_VERSION_MINOR;
	pPluginVersion->dwPluginVersionMajor	= TC_PLUGIN_VERSION_MAJOR;
	pPluginVersion->dwPluginVersionMinor	= TC_PLUGIN_VERSION_MINOR;
	return 0;
}

bool ImagetoATITexture(const DirectX::Image* img, ATI_TC_Texture* texture)
{
   memset(texture, 0, sizeof(ATI_TC_Texture));
   texture->dwSize		= sizeof(ATI_TC_Texture);
   texture->dwWidth		= img->width;
   texture->dwHeight	= img->height;
   texture->dwPitch		= img->rowPitch;

   DWORD dwWidth;
   DWORD dwHeight;
   DWORD buffsize;  

   switch(img->format)
   {
	   case  DXGI_FORMAT_BC6H_UF16:
				texture->format		= ATI_TC_FORMAT_BC6H;
				dwWidth  = ((img->width + 3) / 4) * 4;
				dwHeight = ((img->height + 3) / 4) * 4;
				buffsize = dwWidth * dwHeight;  
				if (buffsize < 16) buffsize = 16; 
				texture->dwDataSize = buffsize; // ATI_TC_CalculateBufferSize(texture);
				texture->pData = (ATI_TC_BYTE*) malloc(texture->dwDataSize); 
				break;
	   case DXGI_FORMAT_BC7_UNORM:
		   		texture->format		= ATI_TC_FORMAT_BC7;
				dwWidth  = ((img->width + 3) / 4) * 4;
				dwHeight = ((img->height + 3) / 4) * 4;
				buffsize = dwWidth * dwHeight;  
				if (buffsize < 16) buffsize = 16; 
				texture->dwDataSize = buffsize; // ATI_TC_CalculateBufferSize(texture);
				texture->pData = (ATI_TC_BYTE*) malloc(texture->dwDataSize); 
				break;
	   default:
			return(false); // unsupported format.
   }	
  
   memcpy(texture->pData,img->pixels,texture->dwDataSize);

   return true;

}

bool ImagetoMIPS(const DirectX::Image* img, MipSet* pMipSet)
{
	#define DEPTH_LEVELS  1
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD buffsize;  

	switch(img->format)
	{
	   case  DXGI_FORMAT_BC6H_UF16:
				pMipSet->m_format	= ATI_TC_FORMAT_BC6H;
				dwWidth				= ((img->width + 3) / 4) * 4;
				dwHeight			= ((img->height + 3) / 4) * 4;
				buffsize = dwWidth * dwHeight;  
				if (buffsize < 16) buffsize = 16; 
				pMipSet->m_compressed = true;
				break;
	   case DXGI_FORMAT_BC7_UNORM:
		   		pMipSet->m_format	=  ATI_TC_FORMAT_BC7;
				dwWidth				= ((img->width + 3) / 4) * 4;
				dwHeight			= ((img->height + 3) / 4) * 4;
				buffsize = dwWidth * dwHeight;  
				if (buffsize < 16) buffsize = 16; 
				pMipSet->m_compressed = true;
				break;
	   default:
			return(false); // unsupported format.
	}	

	pMipSet->m_nHeight = dwHeight;
	pMipSet->m_nWidth  = dwWidth;

	if(!CMips.AllocateMipSet(pMipSet, CF_8bit, TDT_XRGB, TT_2D, dwWidth, dwHeight, DEPTH_LEVELS))
	{
		return false;
	}
   
	pMipSet->m_dwFourCC	= 0;
	pMipSet->m_dwFourCC2	= 0;
	pMipSet->m_nMipLevels= DEPTH_LEVELS;
  
	if(!CMips.AllocateMipLevelData(CMips.GetMipLevel(pMipSet,0), dwWidth, dwHeight, CF_8bit, TDT_XRGB))
	{
		return false;
	}

    BYTE* pData = CMips.GetMipLevel(pMipSet,0)->m_pbData;
    memcpy(pData,img->pixels,buffsize);

    return true;

}

int ATITexturetoImage(ATI_TC_Texture* texture, Image* img, TexMetadata *mdata)
{

	img->height		= texture->dwHeight; /*<width of pixel data>*/
	img->width		= texture->dwWidth;  /*<height of pixel data>*/

	switch(texture->format)
    {
	   case  ATI_TC_FORMAT_BC6H:
				img->format = DXGI_FORMAT_BC6H_UF16;	/*<DXGI format>*/
				img->rowPitch	= texture->dwWidth*4;	/*<number of bytes in a scanline of the source data>*/ // ARGB
				img->slicePitch = texture->dwDataSize;  /*<number of bytes in the entire 2D image>*/
				break;

	   case  ATI_TC_FORMAT_BC7:
				img->format = DXGI_FORMAT_BC7_UNORM;
				img->rowPitch	= texture->dwWidth;
				img->slicePitch = texture->dwDataSize;
				mdata->miscFlags2	= 3; // ???
				break;

	   default:
			return(1); // unsupported format.
    }

	img->pixels		= ( uint8_t* ) malloc(texture->dwDataSize); /*<pointer to pixel data>*/

	if (img->pixels)
	{
		// Init source texture
		memcpy(img->pixels,texture->pData,texture->dwDataSize);

		mdata->width		= img->width;
		mdata->height		= img->height;
		mdata->depth		= 1;
		mdata->arraySize	= 1;
		mdata->mipLevels	= 1;
		mdata->format		= img->format;
		mdata->dimension	= TEX_DIMENSION_TEXTURE2D;
	}
	else return (2);	// not enough memory

    return S_OK;

}

int Plugin_DDS::TC_PluginFileLoadTexture(const TCHAR* pszFilename, ATI_TC_Texture *srcTexture)
{

	// Open DDS file for decompression
	TexMetadata		mdata;
	ScratchImage	image;

	// This is incomplete need to add support for 16bit float
    size_t origsize = strlen(pszFilename) + 1;
    const size_t newsize = MAX_PATH;
    size_t convertedChars = 0;
    wchar_t szFile[newsize];
    mbstowcs_s(&convertedChars, szFile, origsize, pszFilename, _TRUNCATE);

	//LPCWSTR szFile = L".\\Test_images\\Test.dds";
	HRESULT hr = LoadFromDDSFile( szFile, DDS_FLAGS_NONE, &mdata, image );
    if ( FAILED(hr) )
    {
        //WCHAR buff[2048];
        //swprintf_s( buff, L"Failed to load texture file\n\nFilename = %s\nHRESULT %08X", szFile, hr );
        //MessageBoxW( NULL, buff, L"DDSView", MB_OK | MB_ICONEXCLAMATION );
        return -1;
    }

	// Init compressed texture storage
	const DirectX::Image* img = image.GetImage(0,0,0);
    ImagetoATITexture(img, srcTexture);

	return 0;
}

int Plugin_DDS::TC_PluginFileSaveTexture(const TCHAR* pszFilename, ATI_TC_Texture *srcTexture)
{
	Image img;
	size_t nimg = 1;

	// Convert to a wchar_t*
    size_t origsize = strlen(pszFilename) + 1;
    const size_t newsize = MAX_PATH;
    size_t convertedChars = 0;
    wchar_t szDest[newsize];
    mbstowcs_s(&convertedChars, szDest, origsize, pszFilename, _TRUNCATE);


	TexMetadata info;
    memset( &info, 0, sizeof(info) );
	
	HRESULT hr = S_OK;
	if (ATITexturetoImage(srcTexture,&img, &info) == 0)
	{
		hr = SaveToDDSFile( &img, 1, info,DDS_FLAGS_NONE, szDest );
	}
	else 
		hr = -1;

	return hr==S_OK?0:-1; // np: need to fix this : make all pligins return long type!
}

HFILETYPE g_FileType = NULL;

#define MAX_ABOUT_SIZE 255
TCHAR g_szAboutInfo[MAX_ABOUT_SIZE+1];

#define MAX_FORMAT_LENGTH 160
#define MAX_ERROR_LENGTH 240

static const DWORD DDS_HEADER = MAKEFOURCC('D', 'D', 'S', ' ');

TC_PluginError SaveDDS(const char * pszFilename, const MipSet* pMipSet);

TC_PluginError LoadDDS_ABGR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_ABGR16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_GR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_FourCC(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB565(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_RGB8888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet, bool bAlpha);
TC_PluginError LoadDDS_ARGB2101010(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_ABGR16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_G16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_AG8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);
TC_PluginError LoadDDS_A8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet);

TC_PluginError SaveDDS_ABGR32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R32F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ABGR16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R16F(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ARGB8888(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ARGB2101010(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_ABGR16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_R16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RG16(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_RGB888(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_FourCC(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_G8(FILE* pFile, const MipSet* pMipSet);
TC_PluginError SaveDDS_A8(FILE* pFile, const MipSet* pMipSet);

const TCHAR* g_pszFilename;

enum OPTIONS    // Note: dwOptions below assumes 32 or less options.
{
    OPT_WIDTH = 1,
    OPT_HEIGHT,
    OPT_MIPLEVELS,
    OPT_FORMAT,
    OPT_FILTER,
    OPT_SRGBI,
    OPT_SRGBO,
    OPT_SRGB,
    OPT_PREFIX,
    OPT_SUFFIX,
    OPT_OUTPUTDIR,
    OPT_FILETYPE,
    OPT_HFLIP,
    OPT_VFLIP,
    OPT_DDS_DWORD_ALIGN,
    OPT_USE_DX10,
    OPT_NOLOGO,
    OPT_SEPALPHA,
    OPT_TYPELESS_UNORM,
    OPT_TYPELESS_FLOAT,
    OPT_PREMUL_ALPHA,
    OPT_EXPAND_LUMINANCE,
    OPT_TA_WRAP,
    OPT_TA_MIRROR,
    OPT_FORCE_SINGLEPROC,
    OPT_NOGPU,
    OPT_FEATURE_LEVEL,
    OPT_FIT_POWEROF2,
    OPT_ALPHA_WEIGHT,
    OPT_MAX
};

int Plugin_DDS::TC_PluginFileLoadTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
	// Process command line
    DWORD dwOptions = 0;
    
	// Convert to a wchar_t*
    size_t origsize = strlen(pszFilename) + 1;
    const size_t newsize = MAX_PATH;
    size_t convertedChars = 0;
    wchar_t szDest[newsize];
    mbstowcs_s(&convertedChars, szDest, origsize, pszFilename, _TRUNCATE);

    TexMetadata info;
    std::unique_ptr<ScratchImage> image( new (std::nothrow) ScratchImage );
	
    if ( !image )
    {
         wprintf( L" ERROR: Memory allocation failed\n" );
         return -1;
    }

	// New Load from DDS
	DWORD ddsFlags = DDS_FLAGS_NONE;
	HRESULT hr = LoadFromDDSFile( szDest, ddsFlags, &info, *image );
    if ( FAILED(hr) )
    {
      wprintf( L" FAILED (%x)\n", hr);
      return -1;
    }

	if ( IsTypeless( info.format ) )
    {
         if ( dwOptions & (1 << OPT_TYPELESS_UNORM) )
         {
             info.format = MakeTypelessUNORM( info.format );
         }
         else if ( dwOptions & (1 << OPT_TYPELESS_FLOAT) )
         {
             info.format = MakeTypelessFLOAT( info.format );
         }

         if ( IsTypeless( info.format ) )
         {
             wprintf( L" FAILED due to Typeless format %d\n", info.format );
             return PE_Unknown;
         }

         image->OverrideFormat( info.format );
    }

	const DirectX::Image* img = image->GetImage(0,0,0);
	size_t nimg = image->GetImageCount();

	if (!ImagetoMIPS(img,pMipSet)) return PE_Unknown;

	return PE_OK;

//	g_pszFilename = pszFilename;
//    FILE* pFile = fopen(pszFilename,"rb");
//  
//    if(pFile == NULL)
//	{
//		Error(PLUGIN_NAME, EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
//		return PE_Unknown;
//	}
//
//	DWORD dwFileHeader;
//	fread(&dwFileHeader ,sizeof(DWORD), 1, pFile);
//	if(dwFileHeader != DDS_HEADER)
//	{
//		fclose(pFile);
//		Error(PLUGIN_NAME, EL_Error, IDS_ERROR_NOT_DDS, pszFilename);
//		return PE_Unknown;
//	}
//
//	DDSD2 ddsd;
//	if(fread(&ddsd, sizeof(DDSD2), 1, pFile) != 1)
//    {
//      fclose(pFile);
//      Error(PLUGIN_NAME, EL_Error, IDS_ERROR_NOT_DDS, pszFilename);
//      return PE_Unknown;
//    }
//
//	if(!(ddsd.dwFlags & DDSD_MIPMAPCOUNT))
//		ddsd.dwMipMapCount = 1;
//	else if(ddsd.dwMipMapCount == 0)
//	{
//		fclose(pFile);
//		Error(PLUGIN_NAME, EL_Error, IDS_ERROR_NOT_DDS, pszFilename);
//		return PE_Unknown;
//	}
//
//	//if(ddsd.ddpfPixelFormat.dwFourCC == MAKEFOURCC('D', 'X', '1', '0'))
//	//	return LoadDDS_DX10(pFile, &ddsd, pMipSet);
//	//else 
//	if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_A32B32G32R32F)
//		return LoadDDS_ABGR32F(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_A16B16G16R16F)
//		return LoadDDS_ABGR16F(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_G32R32F)
//		return LoadDDS_GR32F(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_R32F)
//		return LoadDDS_R32F(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_R16F)
//		return LoadDDS_R16F(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_G16R16F)
//		return LoadDDS_G16R16F(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_A16B16G16R16)
//		return LoadDDS_ABGR16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_Q16W16V16U16)
//		return LoadDDS_ABGR16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_G16R16)
//		return LoadDDS_G16R16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC == D3DFMT_L16)
//		return LoadDDS_R16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwFourCC)
//		return LoadDDS_FourCC(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==8 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE))
//		return LoadDDS_G8(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==16 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE) && (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS))
//		return LoadDDS_AG8(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwLuminanceBitCount==16 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_LUMINANCE))
//		return LoadDDS_G16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwAlphaBitDepth==8 && (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHA))
//		return LoadDDS_A8(pFile, &ddsd, pMipSet);
//	else if((ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) && !(ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) && (ddsd.ddpfPixelFormat.dwRGBBitCount==16))
//		return LoadDDS_RGB565(pFile, &ddsd, pMipSet);
//	else if((ddsd.ddpfPixelFormat.dwFlags & DDPF_RGB) && !(ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) && (ddsd.ddpfPixelFormat.dwRGBBitCount==24))
//		return LoadDDS_RGB888(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwRGBBitCount==32 && (ddsd.ddpfPixelFormat.dwRBitMask==0x3ff || ddsd.ddpfPixelFormat.dwRBitMask==0x3ff00000))
//		return LoadDDS_ARGB2101010(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwRGBBitCount==32 && ddsd.ddpfPixelFormat.dwRBitMask==0xffff && ddsd.ddpfPixelFormat.dwGBitMask==0xffff0000)
//		return LoadDDS_G16R16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwRGBBitCount==16 && ddsd.ddpfPixelFormat.dwRBitMask==0xffff)
//		return LoadDDS_R16(pFile, &ddsd, pMipSet);
//	else if(ddsd.ddpfPixelFormat.dwRGBBitCount==32)
//		return LoadDDS_RGB8888(pFile, &ddsd, pMipSet, (ddsd.ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS) ? true : false);
//
//	fclose(pFile);
//	Error(PLUGIN_NAME, EL_Error, IDS_ERROR_UNSUPPORTED_TYPE, pszFilename);
//	return PE_Unknown;
}

int Plugin_DDS::TC_PluginFileSaveTexture(const TCHAR* pszFilename, MipSet* pMipSet)
{
   assert(pszFilename);
   assert(pMipSet);

   // New codecs - with faked FourCC's
   if( (pMipSet->m_format == ATI_TC_FORMAT_BC7) || (pMipSet->m_format == ATI_TC_FORMAT_BC6H) )
		return SaveDDS(pszFilename, pMipSet);

   //=================== OLD CODEC support ==============================

   FILE* pFile = NULL;
   if(_tfopen_s(&pFile, pszFilename, _T("wb")) != 0 || pFile == NULL)
	{
		Error(PLUGIN_NAME, EL_Error, IDS_ERROR_FILE_OPEN, pszFilename);
		return PE_Unknown;
	}

	fwrite(&DDS_HEADER ,sizeof(DWORD), 1, pFile);


	if(pMipSet->m_dwFourCC == FOURCC_G8)
		return SaveDDS_G8(pFile, pMipSet);
	else if(pMipSet->m_dwFourCC == FOURCC_A8)
		return SaveDDS_A8(pFile, pMipSet);
	//else if(IsD3D10Format(pMipSet))
	//	return SaveDDS_DX10(pFile, pMipSet);
	else if(pMipSet->m_dwFourCC)
		return SaveDDS_FourCC(pFile, pMipSet);
	else if(pMipSet->m_ChannelFormat == CF_Float16)
	{
		if(pMipSet->m_TextureDataType == TDT_R)
			return SaveDDS_R16F(pFile, pMipSet);
		else if(pMipSet->m_TextureDataType == TDT_RG)
			return SaveDDS_RG16F(pFile, pMipSet);
		else
			return SaveDDS_ABGR16F(pFile, pMipSet);
	}
	else if(pMipSet->m_ChannelFormat == CF_Float32)
	{
		if(pMipSet->m_TextureDataType == TDT_R)
			return SaveDDS_R32F(pFile, pMipSet);
		else if(pMipSet->m_TextureDataType == TDT_RG)
			return SaveDDS_RG32F(pFile, pMipSet);
		else
			return SaveDDS_ABGR32F(pFile, pMipSet);
	}
	else if(pMipSet->m_ChannelFormat == CF_2101010)
		return SaveDDS_ARGB2101010(pFile, pMipSet);
	else if(pMipSet->m_ChannelFormat == CF_16bit)
	{
		if(pMipSet->m_TextureDataType == TDT_R)
			return SaveDDS_R16(pFile, pMipSet);
		else if(pMipSet->m_TextureDataType == TDT_RG)
			return SaveDDS_RG16(pFile, pMipSet);
		else
			return SaveDDS_ABGR16(pFile, pMipSet);
	}
	else if(pMipSet->m_TextureDataType == TDT_ARGB)
		return SaveDDS_ARGB8888(pFile, pMipSet);
	else
		return SaveDDS_RGB888(pFile, pMipSet);
}

//==========================================================================

TC_PluginError LoadDDS_FourCC(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Compressed, TDT_XRGB, PreLoopFourCC, LoopFourCC, PostLoopFourCC);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_RGB565(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_8bit, TDT_XRGB, PreLoopRGB565, LoopRGB565, PostLoopRGB565);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_RGB888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_8bit, TDT_XRGB, 
		PreLoopRGB888, LoopRGB888, PostLoopRGB888);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_RGB8888(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet, bool bAlpha)
{
	ARGB8888Struct* pARGB8888Struct = (ARGB8888Struct*)calloc(sizeof(ARGB8888Struct), 1);
	void* extra = pARGB8888Struct;

/*
	if(pDDSD->dwFlags & DDSD_PIXELFORMAT)
	{
		if(pDDSD->ddpfPixelFormat.dwRBitMask != 0xFF0000)
		{
			pARGB8888Struct->nFlags |= EF_UseBitMasks;
		}
		else if(pDDSD->ddpfPixelFormat.dwGBitMask != 0x00FF00)
		{
			pARGB8888Struct->nFlags |= EF_UseBitMasks;
		}
		else if(pDDSD->ddpfPixelFormat.dwBBitMask != 0x0000FF)
		{
			pARGB8888Struct->nFlags |= EF_UseBitMasks;
		}
	}
*/
	pARGB8888Struct->nFlags |= EF_UseBitMasks;

	if(pARGB8888Struct->nFlags & EF_UseBitMasks)
	{	//using bitmasks
		pARGB8888Struct->pMemory = malloc(4 * pDDSD->dwWidth * pDDSD->dwHeight);
		pARGB8888Struct->nRMask = pDDSD->ddpfPixelFormat.dwRBitMask;
		pARGB8888Struct->nGMask = pDDSD->ddpfPixelFormat.dwGBitMask;
		pARGB8888Struct->nBMask = pDDSD->ddpfPixelFormat.dwBBitMask;

		int shift = 0;
		int tempMask = pARGB8888Struct->nRMask;
		while(!(tempMask & 0xFF) && tempMask)
		{
			shift += 8;
			tempMask >>= 8;
		}
		pARGB8888Struct->nRShift = shift;

		shift = 0;
		tempMask = pARGB8888Struct->nGMask;
		while(!(tempMask & 0xFF) && tempMask)
		{
			shift += 8;
			tempMask >>= 8;
		}
		pARGB8888Struct->nGShift = shift;

		shift = 0;
		tempMask = pARGB8888Struct->nBMask;
		while(!(tempMask & 0xFF) && tempMask)
		{
			shift += 8;
			tempMask >>= 8;
		}
		pARGB8888Struct->nBShift = shift;
	}
	pMipSet->m_TextureDataType = bAlpha ? TDT_ARGB : TDT_XRGB;

	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_8bit, pMipSet->m_TextureDataType, 
		PreLoopRGB8888, LoopRGB8888, PostLoopRGB8888);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_ARGB2101010(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	pMipSet->m_TextureDataType = TDT_ARGB;
	ChannelFormat channelFormat = CF_2101010;
	void* pChannelFormat = &channelFormat;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, pChannelFormat, channelFormat, pMipSet->m_TextureDataType, 
		PreLoopDefault, (pDDSD->ddpfPixelFormat.dwRBitMask==0x3ff00000) ? LoopR10G10B10A2 : LoopDefault, PostLoopDefault);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_ABGR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Float32, TDT_ARGB, 
		PreLoopABGR32F, LoopABGR32F, PostLoopABGR32F);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_GR32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Float32, TDT_RG, 
		PreLoopABGR32F, LoopABGR32F, PostLoopABGR32F);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_R32F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Float32, TDT_R, 
		PreLoopABGR32F, LoopABGR32F, PostLoopABGR32F);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Float16, TDT_R, 
		PreLoopABGR16F, LoopABGR16F, PostLoopABGR16F);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_G16R16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Float16, TDT_RG, 
		PreLoopABGR16F, LoopABGR16F, PostLoopABGR16F);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_ABGR16F(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Float16, TDT_ARGB, 
		PreLoopABGR16F, LoopABGR16F, PostLoopABGR16F);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_G8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Compressed, TDT_XRGB, 
		PreLoopG8, LoopG8, PostLoopG8);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_AG8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Compressed, TDT_ARGB, 
		PreLoopAG8, LoopAG8, PostLoopAG8);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_G16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Compressed, TDT_XRGB, 
		PreLoopG16, LoopG16, PostLoopG16);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_A8(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_Compressed, TDT_ARGB, 
		PreLoopA8, LoopA8, PostLoopA8);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_ABGR16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_16bit, TDT_ARGB, 
		PreLoopABGR16, LoopABGR16, PostLoopABGR16);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_G16R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_16bit, TDT_RG, 
		PreLoopG16R16, LoopABGR16, PostLoopG16R16);
	fclose(pFile);
	return err;
}

TC_PluginError LoadDDS_R16(FILE* pFile, DDSD2* pDDSD, MipSet* pMipSet)
{
	void* extra;
	TC_PluginError err = GenericLoadFunction(pFile, pDDSD, pMipSet, extra, CF_16bit, TDT_R, 
		PreLoopG16R16, LoopABGR16, PostLoopG16R16);
	fclose(pFile);
	return err;
}

TC_PluginError SaveDDS(const char * pszFilename, const MipSet* pMipSet)
{
	Image img;
	size_t nimg = 1;

	// Convert to a wchar_t*
    size_t origsize = strlen(pszFilename) + 1;
    const size_t newsize = MAX_PATH;
    size_t convertedChars = 0;
    wchar_t szDest[newsize];
    mbstowcs_s(&convertedChars, szDest, origsize, pszFilename, _TRUNCATE);

	TexMetadata info;
    memset( &info, 0, sizeof(info) );

	ATI_TC_Texture srcTexture;
	memset(&srcTexture,0,sizeof(ATI_TC_Texture));
	srcTexture.dwSize = sizeof(ATI_TC_Texture);

	srcTexture.dwHeight	= pMipSet->m_nHeight;
	srcTexture.dwWidth  = pMipSet->m_nWidth;
	srcTexture.dwPitch = 0;
	srcTexture.format  = pMipSet->m_format;

	MipLevel* pInMipLevel	= CMips.GetMipLevel(pMipSet, 0, 0);
	srcTexture.dwDataSize	= pInMipLevel->m_dwLinearSize;
	srcTexture.pData		= pInMipLevel->m_pbData;

	HRESULT hr = S_OK;

	if (ATITexturetoImage(&srcTexture,&img, &info)  == S_OK)
	{
		hr = SaveToDDSFile( &img, 1, info,DDS_FLAGS_NONE, szDest );
	}

	if (img.pixels)
		free(img.pixels);

	// Expand this code for MIPS Levels
	// 
	//int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	//for(int nSlice = 0; nSlice < nSlices; nSlice++)
	//{
	//	for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
	//	{
	//		ATI_TC_Texture* srcTexture;
	//		srcTexture->dwDataSize = sizeof(ATI_TC_Texture);
	//		srcTexture->pData = CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData;
	//		ATITexturetoImage(srcTexture,&img, &info);
	//		HRESULT hr = SaveToDDSFile( &img, 1, info,DDS_FLAGS_NONE, szDest );
	//	}
	//}

	return PE_OK;
}

TC_PluginError SaveDDS_RGB888(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
	ddsd2.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
	ddsd2.ddpfPixelFormat.dwBBitMask = 0x000000ff;
	ddsd2.lPitch = pMipSet->m_nWidth * 3;
	ddsd2.ddpfPixelFormat.dwRGBBitCount = 24;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_RGB;
	ddsd2.ddpfPixelFormat.dwRGBAlphaBitMask = 0x00000000;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
	{
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
		{
			BYTE* pData = CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData;
			BYTE* pEnd = pData + CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_dwLinearSize;
			while(pData < pEnd)
			{
				fwrite(pData, 3, 1, pFile);
				pData += 4;
			}
		}
	}

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_ARGB8888(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.ddpfPixelFormat.dwRBitMask = 0x00ff0000;
	ddsd2.ddpfPixelFormat.dwGBitMask = 0x0000ff00;
	ddsd2.ddpfPixelFormat.dwBBitMask = 0x000000ff;
	ddsd2.lPitch = pMipSet->m_nWidth * 4;
	ddsd2.ddpfPixelFormat.dwRGBBitCount = 32;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;
	ddsd2.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff000000;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_ARGB2101010(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.ddpfPixelFormat.dwRBitMask = 0x000003ff;
	ddsd2.ddpfPixelFormat.dwGBitMask = 0x000ffc00;
	ddsd2.ddpfPixelFormat.dwBBitMask = 0x3ff00000;
	ddsd2.ddpfPixelFormat.dwRGBAlphaBitMask = 0xc0000000;
	ddsd2.lPitch = pMipSet->m_nWidth * 4;
	ddsd2.ddpfPixelFormat.dwRGBBitCount = 32;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_ALPHAPIXELS|DDPF_RGB;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_ABGR16(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 8;
	ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_R16(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 2;
	ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_L16;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_RG16(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 4;
	ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_ABGR16F(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 8;
	ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_A16B16G16R16F;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_R16F(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 2;
	ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_R16F;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_RG16F(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 4;
	ddsd2.ddpfPixelFormat.dwFlags = DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_G16R16F;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_ABGR32F(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 16;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_A32B32G32R32F;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_R32F(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 4;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_R32F;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_RG32F(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	// Initialise surface descriptor
	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 8;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_FOURCC|DDPF_ALPHAPIXELS;
	ddsd2.ddpfPixelFormat.dwFourCC = D3DFMT_G32R32F;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_FourCC(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, true);

	ddsd2.ddpfPixelFormat.dwFlags=DDPF_FOURCC;
	if(pMipSet->m_TextureDataType == TDT_ARGB)
		ddsd2.ddpfPixelFormat.dwFlags |= DDPF_ALPHAPIXELS;
	if(pMipSet->m_Flags & MS_AlphaPremult)
		ddsd2.ddpfPixelFormat.dwFlags |= DDPF_ALPHAPREMULT;
	ddsd2.ddpfPixelFormat.dwFourCC = pMipSet->m_dwFourCC;
	ddsd2.ddpfPixelFormat.dwPrivateFormatBitCount = pMipSet->m_dwFourCC2;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_G8(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 8;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_LUMINANCE;
	ddsd2.ddpfPixelFormat.dwLuminanceBitCount = 8;
	ddsd2.ddpfPixelFormat.dwLuminanceBitMask = 0xff;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}

TC_PluginError SaveDDS_A8(FILE* pFile, const MipSet* pMipSet)
{
	assert(pFile);
	assert(pMipSet);

	DDSD2 ddsd2;
	SetupDDSD(ddsd2, pMipSet, false);

	ddsd2.lPitch = pMipSet->m_nWidth * 8;
	ddsd2.ddpfPixelFormat.dwFlags=DDPF_ALPHA;
	ddsd2.ddpfPixelFormat.dwAlphaBitDepth = 8;
	ddsd2.ddpfPixelFormat.dwRGBAlphaBitMask = 0xff;

	// Write the data	
	fwrite(&ddsd2, sizeof(DDSD2), 1, pFile);

	int nSlices = (pMipSet->m_TextureType == TT_2D) ? 1 : MaxFacesOrSlices(pMipSet, 0);
	for(int nSlice = 0; nSlice < nSlices; nSlice++)
		for(int nMipLevel = 0 ; nMipLevel < pMipSet->m_nMipLevels ; nMipLevel++)
			fwrite(CMips.GetMipLevel(pMipSet, nMipLevel, nSlice)->m_pbData, CMips.GetMipLevel(pMipSet, nMipLevel)->m_dwLinearSize, 1, pFile);

	fclose(pFile);

	return PE_OK;
}
#endif