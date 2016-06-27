/* ============================================================

Copyright (c) 2009 Advanced Micro Devices, Inc.  All rights reserved.

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

#ifndef __CL_PROFILE_AMD_H
#define __CL_PROFILE_AMD_H

#include "CL/cl_platform.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

typedef struct _cl_perfcounter_amd * cl_perfcounter_amd;
typedef cl_ulong cl_perfcounter_property;
typedef cl_uint cl_perfcounter_info;

/* cl_perfcounter_info */
enum PerfcounterInfo
{
    CL_PERFCOUNTER_NONE                 = 0x0,
    CL_PERFCOUNTER_REFERENCE_COUNT      = 0x1,
    CL_PERFCOUNTER_DATA                 = 0x2,
    CL_PERFCOUNTER_GPU_BLOCK_INDEX      = 0x3,
    CL_PERFCOUNTER_GPU_COUNTER_INDEX    = 0x4,
    CL_PERFCOUNTER_GPU_EVENT_INDEX      = 0x5,
    CL_PERFCOUNTER_LAST
};

/*! \brief Creates a new HW performance counter
 *   for the specified OpenCL context.
 *
 *  \param device must be a valid OpenCL device.
 *
 *  \param properties the list of properties of the hardware counter
 *
 *  \param errcode_ret  A non zero value if OpenCL failed to create PerfCounter
 *  - CL_SUCCESS if the function is executed successfully.
 *  - CL_INVALID_CONTEXT if the specified context is invalid.
 *  - CL_OUT_OF_RESOURCES if we couldn't create the object
 *
 *  \return the created perfcounter object
 */
extern CL_API_ENTRY cl_perfcounter_amd CL_API_CALL
clCreatePerfCounterAMD(
    cl_device_id                /* device */,
    cl_perfcounter_property*    /* properties */,
    cl_int*                     /* errcode_ret */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Destroy a performance counter object.
 *
 *  \param perf_counter the perfcounter object for release
 *
 *  \return A non zero value if OpenCL failed to release PerfCounter
 *  - CL_SUCCESS if the function is executed successfully.
 *  - CL_INVALID_OPERATION if we failed to release the object
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clReleasePerfCounterAMD(
    cl_perfcounter_amd  /* perf_counter */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Increments the perfcounter object reference count.
 *
 *  \param perf_counter the perfcounter object for retain
 *
 *  \return A non zero value if OpenCL failed to retain PerfCounter
 *  - CL_SUCCESS if the function is executed successfully.
 *  - CL_INVALID_OPERATION if we failed to release the object
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clRetainPerfCounterAMD(
    cl_perfcounter_amd  /* perf_counter */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Enqueues the begin command for the specified counters.
 *
 *  \param command_queue must be a valid OpenCL command queue.
 *
 *  \param num_perf_counters the number of perfcounter objects in the array.
 *
 *  \param perf_counters specifies an array of perfcounter objects.
 *
 *  \return A non zero value if OpenCL failed to release PerfCounter
 *  - CL_SUCCESS if the function is executed successfully.
 *  - CL_INVALID_OPERATION if we failed to enqueue the begin operation
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueBeginPerfCounterAMD(
    cl_command_queue    /* command_queue */,
    cl_uint             /* num_perf_counters */,
    cl_perfcounter_amd* /* perf_counters */,
    cl_uint             /* num_events_in_wait_list */,
    const cl_event*     /* event_wait_list */,
    cl_event*           /* event */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Enqueues the end command for the specified counters.
 *
 *  \param command_queue must be a valid OpenCL command queue.
 *
 *  \param num_perf_counters the number of perfcounter objects in the array.
 *
 *  \param perf_counters specifies an array of perfcounter objects.
 *
 *  \param event the event object associated with the end operation.
 *
 *  \return A non zero value if OpenCL failed to release PerfCounter
 *  - CL_SUCCESS if the function is executed successfully.
 *  - CL_INVALID_OPERATION if we failed to enqueue the end operation
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clEnqueueEndPerfCounterAMD(
    cl_command_queue    /* command_queue */,
    cl_uint             /* num_perf_counters */,
    cl_perfcounter_amd* /* perf_counters */,
    cl_uint             /* num_events_in_wait_list */,
    const cl_event*     /* event_wait_list */,
    cl_event*           /* event */
) CL_API_SUFFIX__VERSION_1_0;

/*! \brief Retrieves the results from the counter objects.
 *
 *  \param perf_counter specifies a perfcounter objects for query.
 *
 *  \param param_name specifies the information to query.
 *
 *  \param param_value is a pointer to memory where the appropriate result
 *  being queried is returned. If \a param_value is NULL, it is ignored.
 *
 *  \param param_value_size is used to specify the size in bytes of memory
 *  pointed to by \a param_value. This size must be >= size of return type.
 *
 *  \param param_value_size_ret returns the actual size in bytes of data copied
 *  to \a param_value. If \a param_value_size_ret is NULL, it is ignored.
 *
 *  \param values must be a valid pointer to an array of 64-bit values
 *  and the array size must be equal to num_perf_counters.
 *
 *  \return
 *  - CL_SUCCESS if the function is executed successfully.
 *  - CL_PROFILING_INFO_NOT_AVAILABLE if event isn't finished.
 *  - CL_INVALID_OPERATION if we failed to get the data
 */
extern CL_API_ENTRY cl_int CL_API_CALL
clGetPerfCounterInfoAMD(
    cl_perfcounter_amd  /* perf_counter */,
    cl_perfcounter_info /* param_name */,
    size_t              /* param_value_size */,
    void*               /* param_value */,
    size_t*             /* param_value_size_ret */
) CL_API_SUFFIX__VERSION_1_0;

#ifdef __cplusplus
} /*extern "C"*/
#endif /*__cplusplus*/

#endif  /*__CL_PROFILE_AMD_H*/
