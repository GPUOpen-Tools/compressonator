/* ============================================================

Copyright (c) 2010 Advanced Micro Devices, Inc.  All rights reserved.

Redistribution and use of this material is permitted under the following
conditions:

Redistributions must retain the above copyright notice and all terms of this
license.

In no event shall anyone redistributing or accessing or using this material
commence or participate in any arbitration or legal action relating to this
material against Advanced Micro Devices, Inc. or any copyright holders or
contributors. The foregoing shall survive any expiration or termination of
this license or any agreement or access or use related to this material.

ANY BREACH OF ANY TERM OF THIS LICENSE SHALL RESULT IN THE IMMEDIATE REVOCATION
OF ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE THIS MATERIAL.

THIS MATERIAL IS PROVIDED BY ADVANCED MICRO DEVICES, INC. AND ANY COPYRIGHT
HOLDERS AND CONTRIBUTORS "AS IS" IN ITS CURRENT CONDITION AND WITHOUT ANY
REPRESENTATIONS, GUARANTEE, OR WARRANTY OF ANY KIND OR IN ANY WAY RELATED TO
SUPPORT, INDEMNITY, ERROR FREE OR UNINTERRUPTED OPERATION, OR THAT IT IS FREE
FROM DEFECTS OR VIRUSES.  ALL OBLIGATIONS ARE HEREBY DISCLAIMED - WHETHER
EXPRESS, IMPLIED, OR STATUTORY - INCLUDING, BUT NOT LIMITED TO, ANY IMPLIED
WARRANTIES OF TITLE, MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
ACCURACY, COMPLETENESS, OPERABILITY, QUALITY OF SERVICE, OR NON-INFRINGEMENT.
IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, PUNITIVE,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, REVENUE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED OR BASED ON ANY THEORY OF LIABILITY
ARISING IN ANY WAY RELATED TO THIS MATERIAL, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE. THE ENTIRE AND AGGREGATE LIABILITY OF ADVANCED MICRO DEVICES,
INC. AND ANY COPYRIGHT HOLDERS AND CONTRIBUTORS SHALL NOT EXCEED TEN DOLLARS
(US $10.00). ANYONE REDISTRIBUTING OR ACCESSING OR USING THIS MATERIAL ACCEPTS
THIS ALLOCATION OF RISK AND AGREES TO RELEASE ADVANCED MICRO DEVICES, INC. AND
ANY COPYRIGHT HOLDERS AND CONTRIBUTORS FROM ANY AND ALL LIABILITIES,
OBLIGATIONS, CLAIMS, OR DEMANDS IN EXCESS OF TEN DOLLARS (US $10.00). THE
FOREGOING ARE ESSENTIAL TERMS OF THIS LICENSE AND, IF ANY OF THESE TERMS ARE
CONSTRUED AS UNENFORCEABLE, FAIL IN ESSENTIAL PURPOSE, OR BECOME VOID OR
DETRIMENTAL TO ADVANCED MICRO DEVICES, INC. OR ANY COPYRIGHT HOLDERS OR
CONTRIBUTORS FOR ANY REASON, THEN ALL RIGHTS TO REDISTRIBUTE, ACCESS OR USE
THIS MATERIAL SHALL TERMINATE IMMEDIATELY. MOREOVER, THE FOREGOING SHALL
SURVIVE ANY EXPIRATION OR TERMINATION OF THIS LICENSE OR ANY AGREEMENT OR
ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE IS HEREBY PROVIDED, AND BY REDISTRIBUTING OR ACCESSING OR USING THIS
MATERIAL SUCH NOTICE IS ACKNOWLEDGED, THAT THIS MATERIAL MAY BE SUBJECT TO
RESTRICTIONS UNDER THE LAWS AND REGULATIONS OF THE UNITED STATES OR OTHER
COUNTRIES, WHICH INCLUDE BUT ARE NOT LIMITED TO, U.S. EXPORT CONTROL LAWS SUCH
AS THE EXPORT ADMINISTRATION REGULATIONS AND NATIONAL SECURITY CONTROLS AS
DEFINED THEREUNDER, AS WELL AS STATE DEPARTMENT CONTROLS UNDER THE U.S.
MUNITIONS LIST. THIS MATERIAL MAY NOT BE USED, RELEASED, TRANSFERRED, IMPORTED,
EXPORTED AND/OR RE-EXPORTED IN ANY MANNER PROHIBITED UNDER ANY APPLICABLE LAWS,
INCLUDING U.S. EXPORT CONTROL LAWS REGARDING SPECIFICALLY DESIGNATED PERSONS,
COUNTRIES AND NATIONALS OF COUNTRIES SUBJECT TO NATIONAL SECURITY CONTROLS.
MOREOVER, THE FOREGOING SHALL SURVIVE ANY EXPIRATION OR TERMINATION OF ANY
LICENSE OR AGREEMENT OR ACCESS OR USE RELATED TO THIS MATERIAL.

NOTICE REGARDING THE U.S. GOVERNMENT AND DOD AGENCIES: This material is
provided with "RESTRICTED RIGHTS" and/or "LIMITED RIGHTS" as applicable to
computer software and technical data, respectively. Use, duplication,
distribution or disclosure by the U.S. Government and/or DOD agencies is
subject to the full extent of restrictions in all applicable regulations,
including those found at FAR52.227 and DFARS252.227 et seq. and any successor
regulations thereof. Use of this material by the U.S. Government and/or DOD
agencies is acknowledgment of the proprietary rights of any copyright holders
and contributors, including those of Advanced Micro Devices, Inc., as well as
the provisions of FAR52.227-14 through 23 regarding privately developed and/or
commercial computer software.

This license forms the entire agreement regarding the subject matter hereof and
supersedes all proposals and prior discussions and writings between the parties
with respect thereto. This license does not affect any ownership, rights, title,
or interest in, or relating to, this material. No terms of this license can be
modified or waived, and no breach of this license can be excused, unless done
so in a writing signed by all affected parties. Each term of this license is
separately enforceable. If any term of this license is determined to be or
becomes unenforceable or illegal, such term shall be reformed to the minimum
extent necessary in order for this license to remain in effect in accordance

with its terms as modified by such reformation. This license shall be governed
by and construed in accordance with the laws of the State of Texas without
regard to rules on conflicts of law of any state or jurisdiction or the United
Nations Convention on the International Sale of Goods. All disputes arising out
of this license shall be subject to the jurisdiction of the federal and state
courts in Austin, Texas, and all defenses are hereby waived concerning personal
jurisdiction and venue of these courts.

============================================================ */

#ifndef __CL_OPEN_VIDEO_AMD_H
#define __CL_OPEN_VIDEO_AMD_H

/******************************************
* Private AMD extension cl_amd_open_video *
******************************************/
#define cl_amd_open_video   1

#if cl_amd_open_video

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/* cl_device_info*/
#define CL_DEVICE_MAX_VIDEO_SESSIONS_AMD            0x10010
#define CL_DEVICE_NUM_VIDEO_ATTRIBS_AMD             0x10011
#define CL_DEVICE_VIDEO_ATTRIBS_AMD                 0x10012

#define CL_DEVICE_VIDEO_ATTRIBS_ENC_AMD             0x10014
#define CL_DEVICE_MAX_VIDEO_ENC_SESSIONS_AMD        0x10015
#define CL_DEVICE_NUM_VIDEO_ENC_ATTRIBS_AMD         0x10016

/* cl_video_session_flags_amd */
typedef cl_bitfield     cl_video_session_flags_amd;

/* cl_video_config_type_amd enumeration */
//
	// Open Encode Porting TBD check if the 0x4101 enum. that will result is OK
	//

/* cl_video_config_type_amd enumeration */
typedef enum _cl_video_config_type_amd {
    CL_VIDEO_DECODE_CONFIGURATION_AMD         = 0x4100,
    CL_VIDEO_ENCODE_CONFIGURATION_AMD         = 0x4101
} cl_video_config_type_amd;

/* cl_video_session_info_amd enumeration */
typedef enum _cl_video_session_info_amd {
    CL_VIDEO_SESSION_CONFIG_SIZE_AMD          = 0x10013,
    CL_VIDEO_SESSION_CONFIG_TYPE_AMD,
    CL_VIDEO_SESSION_CONFIG_BUDDEF_AMD
} cl_video_session_info_amd;

typedef enum _cl_video_session_enc_info_amd {
    CL_CONFIG_TYPE_NONE                     = 0,
    CL_CONFIG_TYPE_PICTURE_CONTROL 		    = 1,
    CL_CONFIG_TYPE_RATE_CONTROL		        = 2,
    CL_CONFIG_TYPE_MOTION_ESTIMATION	    = 3,
    CL_CONFIG_TYPE_RDO 			            = 4
} cl_video_session_enc_info_amd;

/* cl_amd_open_video typedefs */
typedef struct _cl_video_session_amd* cl_video_session_amd;

//typedef cl_bitfield     cl_video_session_flags_amd;

/* cl_video_decode_profile_amd enumeration */
typedef enum _cl_video_decode_profile_amd
{
    CL_VIDEO_DECODE_PROFILE_H264_BASELINE_AMD   = 1,
    CL_VIDEO_DECODE_PROFILE_H264_MAIN_AMD,
    CL_VIDEO_DECODE_PROFILE_H264_HIGH_AMD,
    CL_VIDEO_DECODE_PROFILE_VC1_SIMPLE_AMD,
    CL_VIDEO_DECODE_PROFILE_VC1_MAIN_AMD,
    CL_VIDEO_DECODE_PROFILE_VC1_ADVANCED_AMD,
	CL_VIDEO_DECODE_PROFILE_MPEG2_VLD_AMD,
} cl_video_decode_profile_amd;

//
// Open Encode ported for encode
//
/* cl_video_encode_profile_amd enumeration */
typedef enum _cl_video_encode_profile_amd
{
    CL_VIDEO_ENCODE_PROFILE_H264_BASELINE_AMD   = 1,
    CL_VIDEO_ENCODE_PROFILE_H264_MAIN_AMD,
    CL_VIDEO_ENCODE_PROFILE_H264_HIGH_AMD,
   
} cl_video_encode_profile_amd;


/* cl_video_format_amd enumeration */
typedef enum _cl_video_format_amd
{
    CL_VIDEO_NV12_INTERLEAVED_AMD = 1,
    CL_VIDEO_YV12_INTERLEAVED_AMD
} cl_video_format_amd;

/* cl_video_attrib_amd */
typedef struct _cl_video_attrib_amd {
    cl_video_decode_profile_amd     profile;
    cl_video_format_amd             format;
} cl_video_attrib_amd;

/* cl_video_decode_desc_amd */
typedef struct _cl_video_decode_desc_amd {
    cl_video_attrib_amd             attrib;
    size_t                          image_width;
    size_t                          image_height;
} cl_video_decode_desc_amd;


//
// Open Encode ported
//

/* cl_video_attrib_amd */
typedef struct _cl_video_attrib_encode_amd {
    cl_video_encode_profile_amd     codec_profile;
    cl_uint                         profile;
    cl_uint                         level;
    cl_video_format_amd             format;
} cl_video_attrib_encode_amd;

/* cl_video_encode_desc_amd */
typedef struct _cl_video_encode_desc_amd {
    cl_video_attrib_encode_amd      attrib;
    size_t                          image_width;
    size_t                          image_height;
    cl_uint                         priority;
    cl_uint                         encodeMode;
    cl_uint                         frameRateNumerator;
    cl_uint                         frameRateDenominator;
    void *                          calVideoProperties;
} cl_video_encode_desc_amd;

/* cl_video_program_type_amd enumeration*/
typedef enum _cl_video_program_type_amd
{
    CL_VIDEO_DECODE = 1,
    CL_VIDEO_ENCODE = 2,
} cl_video_program_type_amd;

/* cl_video_program_data_amd */
typedef struct _cl_video_program_data_amd {
    cl_uint                     size;
    cl_video_program_type_amd   type;
    cl_uint                     flags;
} cl_video_program_data_amd;

/* cl_video_decode_data_amd */
typedef struct _cl_video_decode_data_amd {
    cl_video_program_data_amd   video_type;
    cl_mem                      output_surface;
    void*                       picture_parameter_1;
    void*                       picture_parameter_2;
    cl_uint                     picture_parameter_2_size;
    void*                       bitstream_data;
    cl_uint                     bitstream_data_size;
    void*                       slice_data_control;
    cl_uint                     slice_data_control_size;
} cl_video_decode_data_amd;

/* cl_video_encode_data_amd */
typedef struct _cl_video_encode_data_amd {
    cl_video_program_data_amd   video_type;
    cl_uint                     pictureParam1Size;
    cl_uint                     pictureParam2Size;
    void*                       pictureParam1;
    void*                       pictureParam2;
    cl_uint                     uiTaskID;
} cl_video_encode_data_amd;

/* cl_command_type */
#define CL_COMMAND_VIDEO_DECODE_AMD             0x4101
#define CL_COMMAND_VIDEO_ENCODE_AMD             0x4102
#define CL_COMMAND_VIDEO_QUERY_TASK_AMD         0x4103

/* cl_amd_open_video error codes */
#define CL_INVALID_VIDEO_SESSION_FLAGS_AMD      -1100
#define CL_INVALID_VIDEO_SESSION_AMD            -1101
#define CL_INVALID_VIDEO_CONFIG_TYPE_AMD        -1102
#define CL_INVALID_VIDEO_CONFIG_BUFFER_AMD      -1103
#define CL_INVALID_VIDEO_DATA_AMD               -1104

/* cl_video_session_flags_amd - bitfield */
#define CL_VIDEO_DECODE_ACCELERATION_AMD        (1<<0)
// Open Encode ported
#define CL_VIDEO_ENCODE_ACCELERATION_AMD        (1<<0)

// /* cl_video_session_info_amd enumeration */
//typedef enum _cl_video_session_info_amd {
//    CL_VIDEO_SESSION_CONFIG_SIZE_AMD          = 0x10012,
//    CL_VIDEO_SESSION_CONFIG_TYPE_AMD,
//    CL_VIDEO_SESSION_CONFIG_BUDDEF_AMD
//} cl_video_session_info_amd;

typedef CL_API_ENTRY cl_video_session_amd (CL_API_CALL* clCreateVideoSessionAMD_fn)(
    cl_context                  /* context */,
    cl_device_id                /* device */,
    cl_video_session_flags_amd  /* flags */,
    cl_video_config_type_amd    /* config_buffer_type */,
    cl_uint                     /* config_buffer_size */,
    void*                       /* config_buffer */,
    cl_int*                     /* errcode_ret */);

// Open Encode Changes add Enc CreateSession
typedef CL_API_ENTRY cl_video_session_amd (CL_API_CALL* clCreateVideoEncSessionAMD_fn)(
    cl_context                  /* context */,
    cl_device_id                /* device */,
    cl_video_session_flags_amd  /* flags */,
    cl_video_config_type_amd    /* config_buffer_type */,
    cl_uint                     /* config_buffer_size */,
    void*                       /* config_buffer */,
    cl_int*                     /* errcode_ret */);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clDestroyVideoEncSessionAMD_fn)(
    cl_video_session_amd /*video_session*/);

// Open Encode Changes

typedef CL_API_ENTRY cl_int (CL_API_CALL* clRetainVideoSessionAMD_fn)(
    cl_video_session_amd        /* video_session */);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clReleaseVideoSessionAMD_fn)(
    cl_video_session_amd        /* video_session*/);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clGetVideoSessionInfoAMD_fn)(
    cl_video_session_amd        /* video_session */,
    cl_video_session_info_amd   /* param_name */,
    size_t                      /* param_value_size */,
    void*                       /* param_value */,
    size_t*                     /* param_value_size_ret */);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clGetVideoSessionEncInfoAMD_fn)(
    cl_video_session_amd        /* video_session */,
    cl_video_session_enc_info_amd   /* param_name */,
    size_t                      /* param_value_size */,
    void*                       /* param_value */,
    size_t*                     /* param_value_size_ret */);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clSendEncodeConfigInfoAMD_fn) (
    cl_video_session_amd        /* video_session */,
    size_t                      /* numBuffers */,
    void*                       /* pConfigBuffers */);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clEnqueueRunVideoProgramAMD_fn)(
    cl_video_session_amd        /* video_session */,
    void*                       /* video_data_struct */,
    cl_uint                     /* num_events_in_wait_list */,
    const cl_event*             /* event_wait_list */,
    cl_event*                   /* event*/);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clEncodeGetDeviceCapAMD_fn)(
    cl_device_id                /* device_id */,
    cl_uint                     /* encode_mode */,
    cl_uint                     /* encode_cap_total_size */,
    cl_uint*                    /* num_encode_cap */,
    void*                       /* pEncodeCAP */);

typedef CL_API_ENTRY cl_int (CL_API_CALL* clEncodeQueryTaskDescriptionAMD_fn)(
    cl_video_session_amd        /* video_session */,
    cl_uint	                    /* num_of_task_description_request */,
    cl_uint*                    /* num_of_task_description_return */,
    void *                      /* task_description_list */
    );

typedef CL_API_ENTRY cl_int (CL_API_CALL* clEncodeReleaseOutputResourceAMD_fn)(
    cl_video_session_amd        /* video_session */,
    cl_uint	                    /* task_id */
    );

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /*cl_amd_open_video*/

#endif /*!__CL_OPEN_VIDEO_AMD_H*/
