#ifdef _WIN32
#ifndef _PLUGIN_IMAGE_H
#define _PLUGIN_IMAGE_H

#define USE_DIRECTXTEX

#ifdef USE_DIRECTXTEX
#include <initguid.h>
#include "DirectXTex.h"
#endif

#include "PluginInterface.h"
#include "ddraw.h"
#include "d3d9types.h"

// {F3D02C4D-BE5F-4074-9098-BB13D59EF875}
static const GUID g_GUID ={ 0xf3d02c4d, 0xbe5f, 0x4074, { 0x90, 0x98, 0xbb, 0x13, 0xd5, 0x9e, 0xf8, 0x75 } };

#define TC_PLUGIN_VERSION_MAJOR	1
#define TC_PLUGIN_VERSION_MINOR	0

class Plugin_DDS : public PluginInterface_Image
{
	public: 
		Plugin_DDS();
		virtual ~Plugin_DDS();

		int TC_PluginGetVersion(TC_PluginVersion* pPluginVersion);
		int TC_PluginFileLoadTexture(const TCHAR* pszFilename, MipSet* pMipSet);
		int TC_PluginFileSaveTexture(const TCHAR* pszFilename, MipSet* pMipSet);
		int TC_PluginFileLoadTexture(const TCHAR* pszFilename, ATI_TC_Texture *srcTexture);
		int TC_PluginFileSaveTexture(const TCHAR* pszFilename, ATI_TC_Texture *srcTexture);

};

#define PLUGIN_NAME _T("DDS Plugin")
#define IDS_ERROR_FILE_OPEN             1
#define IDS_ERROR_REGISTER_FILETYPE     2
#define IDS_ERROR_NOT_DDS               3
#define IDS_ERROR_UNSUPPORTED_TYPE      4


extern const TCHAR* g_pszFilename;

// DDSD2
// Required for 64bit compatability
typedef struct _DDSD2
{
	DWORD               dwSize;                 // size of the DDSURFACEDESC structure
	DWORD               dwFlags;                // determines what fields are valid
	DWORD               dwHeight;               // height of surface to be created
	DWORD               dwWidth;                // width of input surface
	union
	{
		LONG            lPitch;                 // distance to start of next line (return value only)
		DWORD           dwLinearSize;           // Formless late-allocated optimized surface size
	} DUMMYUNIONNAMEN(1);
	union
	{
		DWORD           dwBackBufferCount;      // number of back buffers requested
		DWORD           dwDepth;                // the depth if this is a volume texture 
	} DUMMYUNIONNAMEN(5);
	union
	{
		DWORD           dwMipMapCount;          // number of mip-map levels requestde
		// dwZBufferBitDepth removed, use ddpfPixelFormat one instead
		DWORD           dwRefreshRate;          // refresh rate (used when display mode is described)
		DWORD           dwSrcVBHandle;          // The source used in VB::Optimize
	} DUMMYUNIONNAMEN(2);
	DWORD               dwAlphaBitDepth;        // depth of alpha buffer requested
	DWORD               dwReserved;             // reserved
	void* POINTER_32	lpSurface;              // pointer to the associated surface memory
	union
	{
		DDCOLORKEY      ddckCKDestOverlay;      // color key for destination overlay use
		DWORD           dwEmptyFaceColor;       // Physical color for empty cubemap faces
	} DUMMYUNIONNAMEN(3);
	DDCOLORKEY          ddckCKDestBlt;          // color key for destination blt use
	DDCOLORKEY          ddckCKSrcOverlay;       // color key for source overlay use
	DDCOLORKEY          ddckCKSrcBlt;           // color key for source blt use
	union
	{
		DDPIXELFORMAT   ddpfPixelFormat;        // pixel format description of the surface
		DWORD           dwFVF;                  // vertex format description of vertex buffers
	} DUMMYUNIONNAMEN(4);
	DDSCAPS2            ddsCaps;                // direct draw surface capabilities
	DWORD               dwTextureStage;         // stage in multitexture cascade
} DDSD2;


#endif
#endif