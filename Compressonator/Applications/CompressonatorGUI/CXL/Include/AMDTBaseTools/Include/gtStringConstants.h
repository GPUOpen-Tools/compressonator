//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file gtStringConstants.h 
/// 
//=====================================================================

//------------------------------ gtStringConstants.h ------------------------------

#ifndef __GTSTRINGCONSTANTS_H
#define __GTSTRINGCONSTANTS_H

#define GT_STR_ASSERTION_FAILURE_MSG_PREFIX L"Assertion failure ("
#define GT_STR_ASSERTION_FAILURE_MSG_SUFFIX L")"
#define GT_STR_MEMORY_ALLOCATION_FAILURE_MESSAGE L"Failed to allocate memory - terminating the application"

#define GT_STR_MEMORY_ALLOCATION_FAILURE_USER_TITLE L"Memory Error"
#define GT_STR_MEMORY_ALLOCATION_FAILURE_USER_MESSAGE L"Failed to allocate memory. CodeXL will terminate."

// The amount of bits in a single byte:
#define GT_BITS_PER_BYTE 8

#define GT_THOUSANDS_SEPARATOR ','
#define GT_THOUSANDS_SEPARATOR_DISTANCE 3

// Format strings used for printing pointers
// Note that the width parameter includes the "0x" given by the # parameter, so
// to get (a minimum of) 8 hexadecimal digits, we need to use 8+2 = 10 as the
// width, and 18 width for 16 digits.
// The # parameter also adds 0X when used with %X. To get 0xFEEDFACE instead
// of 0XFEEDFACE, we don't use it in the uppercase varieties.
// These should be used instead of "%p" when the pointer might be of a size
// different than the local (native) size.
#define GT_64_BIT_POINTER_FORMAT_LOWERCASE L"0x%016llx"
#define GT_64_BIT_POINTER_FORMAT_UPPERCASE L"0x%016llX"
#define GT_32_BIT_POINTER_FORMAT_LOWERCASE L"0x%08x"
#define GT_32_BIT_POINTER_FORMAT_UPPERCASE L"0x%08X"
#define GT_UNSIGNED_INT_HEXADECIMAL_2_CHAR_FORMAT L"0x%02x"
#define GT_UNSIGNED_INT_HEXADECIMAL_4_CHAR_FORMAT L"0x%04x"
#define GT_UNSIGNED_INT_HEXADECIMAL_8_CHAR_FORMAT L"0x%08x"
#define GT_UNSIGNED_INT_HEXADECIMAL_16_CHAR_FORMAT L"0x%016llx"

#endif //__GTSTRINGCONSTANTS_H

