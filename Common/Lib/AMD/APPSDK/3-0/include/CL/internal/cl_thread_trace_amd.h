/* ============================================================

Copyright (c) 2012 Advanced Micro Devices, Inc.  All rights reserved.

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

#ifndef __CL_THREAD_TRACE_AMD_H
#define __CL_THREAD_TRACE_AMD_H

#include "CL/cl_platform.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct _cl_threadtrace_amd * cl_threadtrace_amd;
typedef cl_uint cl_thread_trace_param;
typedef cl_uint cl_threadtrace_info;

/* cl_command_type */
#define CL_COMMAND_THREAD_TRACE_MEM 0x4500
#define CL_COMMAND_THREAD_TRACE     0x4501

/* cl_threadtrace_command_name_amd enumeration */
typedef enum  _cl_threadtrace_command_name_amd {
    CL_THREAD_TRACE_BEGIN_COMMAND,
    CL_THREAD_TRACE_END_COMMAND,
    CL_THREAD_TRACE_PAUSE_COMMAND,
    CL_THREAD_TRACE_RESUME_COMMAND
} cl_threadtrace_command_name_amd;

// Thread trace parameters
enum ThreadTraceParameter
{
    CL_THREAD_TRACE_PARAM_TOKEN_MASK,
    CL_THREAD_TRACE_PARAM_REG_MASK,
    CL_THREAD_TRACE_PARAM_COMPUTE_UNIT_TARGET,
    CL_THREAD_TRACE_PARAM_SHADER_ARRAY_TARGET,
    CL_THREAD_TRACE_PARAM_SIMD_MASK,
    CL_THREAD_TRACE_PARAM_VM_ID_MASK,
    CL_THREAD_TRACE_PARAM_RANDOM_SEED,
    CL_THREAD_TRACE_PARAM_CAPTURE_MODE,
    CL_THREAD_TRACE_PARAM_INSTRUCTION_MASK,
    CL_THREAD_TRACE_PARAM_USER_DATA,
    CL_THREAD_TRACE_PARAM_IS_WRAPPED
};

// CL_THREAD_TRACE_PARAM_TOKEN_MASK data selects for SI
enum CL_THREAD_TRACE_TOKEN_MASK
{
    //Time passed
    CL_THREAD_TRACE_TOKEN_MASK_TIME_SI            = 0x00000001,
    //Resync the timestamp
    CL_THREAD_TRACE_TOKEN_MASK_TIMESTAMP_SI       = 0x00000002,
    //A register write has occurred
    CL_THREAD_TRACE_TOKEN_MASK_REG_SI             = 0x00000004,
    //A wavefront has started
    CL_THREAD_TRACE_TOKEN_MASK_WAVE_START_SI      = 0x00000008,
    //Output space has been allocated for color/Z [Should be used for cl-gl]
    CL_THREAD_TRACE_TOKEN_MASK_WAVE_PS_ALLOC_SI   = 0x00000010,
    //Output space has been allocated for vertex position [Should be used for cl-gl]
    CL_THREAD_TRACE_TOKEN_MASK_WAVE_VS_ALLOC_SI   = 0x00000020,
    //Wavefront completion
    CL_THREAD_TRACE_TOKEN_MASK_WAVE_END_SI        = 0x00000040,
    //An event has reached the top of a shader stage. In-order with WAVE_START
    CL_THREAD_TRACE_TOKEN_MASK_EVENT_SI           = 0x00000080,
    //An event has reached the top of a compute shader stage. In-order with WAVE_START
    CL_THREAD_TRACE_TOKEN_MASK_EVENT_CS_SI        = 0x00000100,
    //An event has reached the top of a shader stage for the second GFX pipe. In-order with WAVE_START.
    //[Should be used for cl-gl]
    CL_THREAD_TRACE_TOKEN_MASK_EVENT_GFX_SI       = 0x00000200,
    //The kernel has executed an instruction
    CL_THREAD_TRACE_TOKEN_MASK_INST_SI            = 0x00000400,
    //The kernel has explicitly written the PC value
    CL_THREAD_TRACE_TOKEN_MASK_INST_PC_SI         = 0x00000800,
    //The kernel has written user data into the thread trace buffer
    CL_THREAD_TRACE_TOKEN_MASK_INST_USERDATA_SI   = 0x00001000,
    //Provides information about instruction scheduling
    CL_THREAD_TRACE_TOKEN_MASK_ISSUE_SI           = 0x00002000,
    //The performance counter delta has been updated
    CL_THREAD_TRACE_TOKEN_MASK_PERF_SI            = 0x00004000,
    //A miscellaneous event has been sent
    CL_THREAD_TRACE_TOKEN_MASK_MISC_SI            = 0x00008000,
    //All possible tokens
    CL_THREAD_TRACE_TOKEN_MASK_ALL_SI             = 0x0000ffff,
};

//CL_THREAD_TRACE_PARAM_REG_MASK data selects
enum CL_THREAD_TRACE_REG_MASK
{
   //Event initiator
   CL_THREAD_TRACE_REG_MASK_EVENT_SI      = 0x00000001,
   //Draw initiator [Should be used for cl-gl]
   CL_THREAD_TRACE_REG_MASK_DRAW_SI       = 0x00000002,
   //Dispatch initiator
   CL_THREAD_TRACE_REG_MASK_DISPATCH_SI   = 0x00000004,
   //User data from host
   CL_THREAD_TRACE_REG_MASK_USERDATA_SI   = 0x00000008,
   //GFXDEC register (8-state) [Should be used for cl-gl]
   CL_THREAD_TRACE_REG_MASK_GFXDEC_SI     = 0x00000020,
   //SHDEC register (many state)
   CL_THREAD_TRACE_REG_MASK_SHDEC_SI      = 0x00000040,
   //Other registers
   CL_THREAD_TRACE_REG_MASK_OTHER_SI      = 0x00000080,
   //All possible registers types
   CL_THREAD_TRACE_REG_MASK_ALL_SI        = 0x000000ff,
};

//CL_THREAD_TRACE_PARAM_VM_ID_MASK data selects
enum CL_THREAD_TRACE_VM_ID_MASK
{
   //Capture only data from the VM_ID used to write {SQTT}_BASE
   CL_THREAD_TRACE_VM_ID_MASK_SINGLE              = 0,
   //Capture all data from all VM_IDs
   CL_THREAD_TRACE_VM_ID_MASK_ALL                 = 1,
   //Capture all data but only get target (a.k.a. detail) data from VM_ID used to write {SQTT}_BASE
   CL_THREAD_TRACE_VM_ID_MASK_SINGLE_DETAIL       = 2
};

//CL_THREAD_TRACE_PARAM_CAPTURE_MODE data
enum CL_THREAD_TRACE_CAPTURE_MODE
{
    //Capture all data in the thread trace buffer
    CL_THREAD_TRACE_CAPTURE_ALL                    = 0,
    //Capture only data between THREAD_TRACE_START and THREAD_TRACE_STOP events
    CL_THREAD_TRACE_CAPTURE_SELECT                 = 1,
    //Capture data between THREAD_TRACE_START and THREAD_TRACE_/STOP events,
    //and global/reference data at all times
    CL_THREAD_TRACE_CAPTURE_SELECT_DETAIL          = 2
};

//CL_THREAD_TRACE_PARAM_INSTRUCTION_MASK data selects
enum CL_THREAD_TRACE_INSTRUCTION_MASK
{
    //Generate {SQTT}_TOKEN_INST tokens for all instructions
    CL_THREAD_TRACE_INST_MASK_ALL,
    //Generate {SQTT}_TOKEN_INST tokens for stalled instructions only
    CL_THREAD_TRACE_INST_MASK_STALLED,
    //Generate {SQTT}_TOKEN_INST messages for stalled and other (no op/wait/set prio/etc) instructions
    CL_THREAD_TRACE_INST_MASK_STALLED_AND_IMMEDIATE,
    //Generate {SQTT}_TOKEN_INST messages for immediate instructions only only [ Should be used only for CI]
    CL_THREAD_TRACE_INST_MASK_IMMEDIATE_CI,
};

enum ThreadTraceInfo
{
    CL_THREAD_TRACE_SE,
    CL_THREAD_TRACE_BUFFERS_FILLED,
    CL_THREAD_TRACE_BUFFERS_SIZE
};


/*! \brief Creates a new cl_threadtrace_amd object
 *
 *  \param device must be a valid OpenCL device.
 *
 *  \param errcode_ret  A non zero value if OpenCL failed to create threadTrace
 *  -CL_INVALID_DEVICE if devices contains an invalid device.
 *  -CL_DEVICE_NOT_AVAILABLE if a device is currently not available even
 *                            though the device was returned by clGetDeviceIDs.
 *  -CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the
 *                       OpenCL  implementation on the device.
 *  -CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the
                          OpenCL implementation on the host.
 *
 *  \return the created threadTrace object
 */
extern CL_API_ENTRY cl_threadtrace_amd CL_API_CALL
clCreateThreadTraceAMD(
    cl_device_id                /* device */,
    cl_int*                     /* errcode_ret */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Destroys a cl_threadtrace_amd object.
 *
 *  \param threadTrace the cl_threadtrace_amd object for release
 *
 *  \return A non zero value if OpenCL failed to release threadTrace
 *  -CL_INVALID_VALUE if the thread_trace is not a valid  OpenCL thread trace object (cl_threadtrace_amd) .
 *  -CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the
 *                       OpenCL  implementation on the device.
 *  -CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the
                      OpenCL  implementation on the host.
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clReleaseThreadTraceAMD(
    cl_threadtrace_amd  /* threadTrace */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Increments the cl_threadtrace_amd object reference count.
 *
 *  \param threadTrace the cl_threadtrace_amd object for retain
 *
 *  \return A non zero value if OpenCL failed to retain threadTrace
 *  -CL_INVALID_VALUE if the thread_trace is not a valid thread trace object (cl_threadtrace_amd) .
 *  -CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the
                         OpenCL implementation on the device.
 *  -CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the
                           OpenCL implementation on the host.
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clRetainThreadTraceAMD(
    cl_threadtrace_amd  /* threadTrace */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Sets the cl_threadtrace_amd object configuration parameter.
 *
 *  \param thread_trace the cl_threadtrace_amd object to set configuration parameter 
 *
 *  \param config_param the cl_thread_trace_param 
 *
 *  \param param_value corresponding to configParam
 *
 *  \return A non zero value if OpenCL failed to set threadTrace buffer parameter
 *  - CL_INVALID_VALUE if the thread_trace  is invalid thread trace object.
 *  - CL_INVALID_VALUE if the invalid config_param or param_value enum values , are used.
 *  - CL_INVALID_EVENT_WAIT_LIST if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0,
 *  -                            or if event objects in event_wait_list are not valid events.
 *  - CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the OpenCL implementation on the device.
 *  - CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the
                           OpenCL implementation on the host.
 */

extern CL_API_ENTRY cl_int CL_API_CALL
clSetThreadTraceParamAMD(cl_threadtrace_amd    /*thread_trace*/ ,
                         cl_thread_trace_param /*config_param*/ ,
                         cl_uint                /*param_value*/ 
)CL_API_SUFFIX__VERSION_1_0;

/* \brief Enqueues the binding command to bind cl_threadtrace_amd to cl_mem object for trace recording..
 *
 *  \param command_queue must be a valid OpenCL command queue.
 *
 *  \param thread_trace specifies the cl_threadtrace_amd object.
 *
 *  \param mem_objects the cl_mem objects for trace recording
 *
 *  \param mem_objects_num the number of cl_mem objects in the mem_objects
 *
 *  \param buffer_size the size of each cl_mem object from mem_objects
 *
 *  \param event_wait_list specify [is a pointer to] events that need to
 *  complete before this particular command can be executed.
 *  If \a event_wait_list is NULL, then this particular command does not wait
 *  on any event to complete. If \a event_wait_list is NULL,
 *  \a num_events_in_wait_list must be 0. If \a event_wait_list is not NULL,
 *  the list of events pointed to by \a event_wait_list must be valid and
 *  \a num_events_in_wait_list must be greater than 0. The events specified in
 *  \a event_wait_list act as synchronization points.
 *
 *  \param num_events_in_wait_list specify the number of events in
 *  \a event_wait_list. It must be 0 if \a event_wait_list is NULL. It  must be
 *  greater than 0 if \a event_wait_list is not NULL.
 *
 *  \param event returns an event object that identifies this particular
 *  command and can be used to query or queue a wait for this particular
 *  command to complete. \a event can be NULL in which case it will not be
 *  possible for the application to query the status of this command or queue a
 *  wait for this command to complete.
 *  \return A non zero value if OpenCL failed to set threadTrace buffer parameter
 *  - CL_INVALID_COMMAND_QUEUE if command_queue is not a valid command-queue.
 *  - CL_INVALID_CONTEXT if the context associated with command_queue and  events in event_wait_list are not the same.
 *  - CL_INVALID_VALUE if the thread_trace  is invalid thread trace object.
 *  - CL_INVALID_VALUE if the buffer_size is negative or zero.
 *  - CL_INVALID_VALUE if the  sub_buffers_num I less than 1.
 *  - CL_INVALID_OPERATION if the mem_objects_num is not equal to the number of Shader Engines of the [GPU] device.
 *  - CL_INVALID_MEM_OBJECT if one on memory objects in the mem_objects array is not a valid memory object or memory_objects is NULL.
 *  - CL_MEM_OBJECT_ALLOCATION_FAILURE if there is a failure to allocate memory for the data store associated from the memory objects of the mem_objects array.
 *  - CL_INVALID_EVENT_WAIT_LIST if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.
 *  - CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the OpenCL implementation on the device.
 *  - CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the
 *     OpenCL implementation on the host.
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueBindThreadTraceBufferAMD(cl_command_queue    command_queue ,
                                          cl_threadtrace_amd  /*thread_trace*/ ,
                                          cl_mem*             /*mem_objects*/ ,
                                          cl_uint             /*mem_objects_num*/ ,
                                          cl_uint             /*buffer_size*/ ,
                                          cl_uint             /*num_events_in_wait_list*/ ,
                                          const cl_event*     /*event_wait_list*/ ,
                                          cl_event*           /*event*/
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Get specific information about the OpenCL Thread Trace.
 *
 *  \param thread_trace_info_param is an enum that identifies the Thread Trace information being
 *  queried.
 *
 *  \param param_value is a pointer to memory location where appropriate values
 *  for a given \a threadTrace_info_param will be returned. If \a param_value is NULL,
 *  it is ignored.
 *
 *  \param param_value_size specifies the size in bytes of memory pointed to by
 *  \a param_value. This size in bytes must be >= size of return type.
 *
 *  \param param_value_size_ret returns the actual size in bytes of data being
 *  queried by param_value. If \a param_value_size_ret is NULL, it is ignored.
 *
 *  \return One of the following values:
 *      CL_INVALID_OPERATION if cl_threadtrace_amd object is not valid
 *    - CL_INVALID_VALUE if \a param_name is not one of the supported
 *      values or if size in bytes specified by \a param_value_size is < size of
 *      return type and \a param_value is not a NULL value.
 *      CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the
 *      OpenCL implementation on the host.
 *      CL_SUCCESS if the function is executed successfully.
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clGetThreadTraceInfoAMD(
    cl_threadtrace_amd    /* thread_trace */,
    cl_threadtrace_info   /*thread_trace_info_param*/,
    size_t                /*param_value_size*/,
    void*                 /*param_value*/,
    size_t*               /*param_value_size_ret*/
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Enqueues the thread trace command for the specified thread trace object.
 *
 *  \param command_queue must be a valid OpenCL command queue.
 *
 *  \param threadTraces specifies an array of cl_threadtrace_amd objects.
 *
 *  \return A non zero value if OpenCL failed to release threadTrace
 *  - CL_INVALID_COMMAND_QUEUE if command_queue is not a valid command-queue.
 *  - CL_INVALID_CONTEXT if the context associated with command_queue and  events in event_wait_list are not the same.
 *  - CL_INVALID_VALUE if the thread_trace is invalid thread trace object .
 *  - CL_INVALID_VALUE if the invalid command name enum value , not  described in the cl_threadtrace_command_name_amd, is used.
 *  - CL_INVALID_OPERATION if the command enqueue failed. It can happen in the following cases:
 *          o BEGIN_COMMAND is queued for thread trace object for which memory object/s was/were not bound..
 *          o END_COMMAND is queued for thread trace object, for which BEGIN_COMMAND was not queued.
 *          o PAUSE_COMMAND is queued for thread trace object, for which BEGIN_COMMAND was not queued.
 *          o RESUME_COMMAND is queued for thread trace object, for which  PAUSE_COMMAND was not queued.
 *  - CL_INVALID_EVENT_WAIT_LIST if event_wait_list is NULL and num_events_in_wait_list > 0, or event_wait_list is not NULL and num_events_in_wait_list is 0, or if event objects in event_wait_list are not valid events.
 *  - CL_OUT_OF_RESOURCES if there is a failure to allocate resources required by the OpenCL implementation on the device.
 *  - CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the OpenCL implementation on the host.
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueThreadTraceCommandAMD(
    cl_command_queue     /*command_queue*/ ,
    cl_threadtrace_amd   /*thread_trace*/ ,
    cl_threadtrace_command_name_amd /*command_name*/ ,
    cl_uint              /*num_events_in_wait_list*/ ,
    const cl_event*      /*event_wait_list*/,
    cl_event*            /*event*/
)CL_API_SUFFIX__VERSION_1_0;


#ifdef __cplusplus
} /*extern "C"*/
#endif /*__cplusplus*/

#endif  /*__CL_THREAD_TRACE_AMD_H*/
