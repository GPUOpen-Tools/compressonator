/************************************************************************************/ /**
// Copyright (c) 2006-2024 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief The interface for the Tootle library.
****************************************************************************************/
#ifndef _CMP_MESH_H_
#define _CMP_MESH_H_

namespace CMP_MESH
{

/// The default vertex cache size
#define DEFAULT_VCACHE_SIZE 16

/// The maximum allowed number of faces in the mesh
#define MAX_FACES 0x7fffffff

/// The maximum allowed number of vertices in the mesh
#define MAX_VERTICES 0x7fffffff

/// The parameter for TootleFastOptimize to create more clusters (lower number generates more clusters).
/// This parameter decides where to put extra breaks to create more clusters (refer to the SIGGRAPH 2007 paper
/// for the full description of the parameter.
#define DEFAULT_ALPHA 0.75f

/// Enumeration for face winding order
enum eFaceWinding
{
    CCW = 1,  ///< Face is ordered counter-clockwise
    CW  = 2   ///< Face is ordered clockwise
};

/// Enumeration for the algorithm for vertex optimization
enum eVCacheOptimizer
{
    VCACHE_AUTO     = 1,  ///< If vertex cache size is less than 7, use TSTRIPS algorithm otherwise TIPSY.
    VCACHE_DIRECT3D = 2,  ///< Use D3DXOptimizeFaces to optimize faces.
    VCACHE_LSTRIPS  = 3,  ///< Build a list like triangle strips to optimize faces.
    VCACHE_TIPSY    = 4   ///< Use TIPSY (the algorithm from SIGGRAPH 2007) to optimize faces.
};

/// Enumeration for the algorithm for overdraw optimization.
enum eOverdrawOptimizer
{
    OVERDRAW_AUTO     = 1,  ///< Use either Direct3D or raytracing to reorder clusters (depending on the number of clusters).
    OVERDRAW_DIRECT3D = 2,  ///< Use Direct3D rendering to reorder clusters to optimize overdraw (slow O(N^2)).
    OVERDRAW_RAYTRACE = 3,  ///< Use CPU raytracing to reorder clusters to optimize overdraw (slow O(N^2)).
    OVERDRAW_FAST     = 4   ///< Use a fast approximation algorithm (from SIGGRAPH 2007) to reorder clusters.
};

struct MESH_SETTINGS
{
    char*            m_pViewpointName;
    unsigned int     m_Clustering;
    unsigned int     m_CacheSize;
    bool             m_OptimizeVertexMemory;  // true if you want to optimize vertex memory location, false to skip
    bool             m_MeasureOverdraw;       // true if you want to measure overdraw, false to skip
    eFaceWinding     m_FaceeWinding;
    eVCacheOptimizer m_VCacheOptimizer;
};

}  // namespace CMP_MESH

#endif
