#ifndef _COMMON_KERNEL_DEFINITIONS_H
#define _COMMON_KERNEL_DEFINITIONS_H

namespace CMP_Encoder
{
// defined when running code on cpu 
#ifndef __OPENCL_VERSION__
    #define __kernel 
    #define __global
    #define __constant          const
    #define __local             const
#endif


#define USE_PROCESS_SEPERATE_ALPHA   // Enable this to use higher quality code using CompressDualIndexBlock

#define COMPRESSED_BLOCK_SIZE           16              // Size of a compressed block in bytes
#define MAX_DIMENSION_BIG               4               // Max number of channels  (RGBA)
#define MAX_SUBSETS                     3               // Maximum number of possible subsets
#define MAX_SUBSET_SIZE                 16              // Largest possible size for an individual subset

//#define USE_BLOCK_LINEAR
//#define USE_DOUBLE          // Default is to use float, enable to use double

#define USE_OPT_QUANTIZER     // High Quality
#define USE_OPT_QUANTIZER2    // Poor Quality
#define USE_optQUANT          // Good Quality

#ifndef __OPENCL_VERSION__
#define USE_QSORT             // Default use non qsort, enable to use qsort (qsort function only works for cpu)
#endif

typedef int                 CGU_INT;        //16bits
typedef unsigned int        CGU_UINT;       //16bits
typedef unsigned int        CGU_DWORD;      //32bits
typedef unsigned char       CGU_BYTE;       //8bits
typedef int                 CGU_BOOL;       // ?? change to unsigned char! 
typedef unsigned short      CGU_SHORT;

#ifdef USE_DOUBLE 
typedef double              CGU_FLOAT;
#define CMP_DBL_MAX         1.7976931348623158e+308 // max value
#define CMP_DBL_MAX_EXP     1024
#else
typedef float               CGU_FLOAT;
#define CMP_DBL_MAX         FLT_MAX // max value
#define CMP_DBL_MAX_EXP     128
#endif

typedef struct
{
    int  m_src_width;
    int  m_src_height;
    int  m_width_in_blocks;
    int  m_height_in_blocks;
} Source_Info;

typedef struct
{
    unsigned int     size;          // Size of data 
    unsigned int     Compute_type;  // Compute Type : default is OpenCL
    unsigned int     data_type;     // Codec Format
    unsigned int     use_code;      // Determines what compiler definition path is used defulat is USE_OPTIMIZED_CODE_0
    void *data;                     // data to pass down from CPU to kernel
    void *dataSVM;                  // data allocated as Shared by CPU and GPU (used only when code is running in 64bit and devices support SVM)
} KernalOptions;

typedef struct
{
    CGU_BYTE  blockType;                                       // 0 = Single 1 = Dual
    CGU_BYTE  best_blockMode;                                  // Bock is Invalid if value is 99: No suitable mode was found
    CGU_BYTE  best_indices[MAX_SUBSETS][MAX_SUBSET_SIZE];

    // This can be union
    int       best_Endpoints[MAX_SUBSETS][2][MAX_DIMENSION_BIG];
    CGU_DWORD packedEndpoints[MAX_SUBSETS][2];

    // Specific to Dual
    CGU_BYTE  best_indexSelection;                             // either 0 or 1
    CGU_BYTE  best_rotation;                                   // either 0 or 2

                                                               // Specific to Single
    CGU_BYTE   bestPartition;
} QUANTIZEBLOCK;

#define QUANTIZEBLOCK_ID "A896F2E4-0C7F-41F1-B074-22545FDE4808"
typedef struct
{
    int         DataType = 0;  // 0 = 64 Bits, 1 = 128 Bits 2 = 512 bits
    int         w = 0;                          // Origin Image size
    int         h = 0;                          // 
    int         x = 4;                          //Block Sizes: Width
    int         y = 4;                          //Block Sizes: Height
    int         curr_blockx = 0;                //Current block been processed 
    int         curr_blocky = 0;                //Current block been processed 
    int         compressed_size = 0;            //Track the current compressed block size
    int         source_size = 64;               //Usually fixed at 64 bytes max

    QUANTIZEBLOCK buffer;

} Quantizer_Buffer;



} // namespace CMP_Encoder
#endif

//### Automated section set by ComputeLib SDK at run time, do not edit
//
//###

