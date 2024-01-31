/************************************************************************************/ /**
// Copyright (c) 2006-2024 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
****************************************************************************************/
#ifndef _JRT_COMMON_H_
#define _JRT_COMMON_H_

/// epsilon value used in intersection computations.
#define JRTEPSILON 0.00001f

#include "tootlepch.h"

#include <iostream>

#ifdef __linux__
typedef unsigned long long UINT64;
#else
typedef unsigned __int64 UINT64;
#endif
typedef unsigned int   UINT;
typedef unsigned short USHORT;
typedef unsigned char  UBYTE;

#include "jml.h"
using namespace JML;

#define JRT_ASSERT(x) assert(x)
#define JRT_SAFE_DELETE(x) \
    {                      \
        if (x)             \
            delete x;      \
        x = NULL;          \
    }
#define JRT_SAFE_DELETE_ARRAY(x) \
    {                            \
        if (x)                   \
            delete[] x;          \
        x = NULL;                \
    }

#endif
