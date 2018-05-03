// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <windowsx.h>
#include <wrl.h>

// C RunTime Header Files
#include <malloc.h>
#include <tchar.h>
#include <atlbase.h>

#include <cassert>

#include "vulkan/vulkan.h"

#include "Error.h"

#include <DirectXMath.h>
using namespace DirectX;

#include <map>
#include <string>
#include <vector>


#include "Defines.h"


// TODO: reference additional headers your program requires here
