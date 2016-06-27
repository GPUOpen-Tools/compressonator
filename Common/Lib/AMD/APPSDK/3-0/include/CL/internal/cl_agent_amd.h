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

#ifndef __OPENCL_CL_AGENT_AMD_H
#define __OPENCL_CL_AGENT_AMD_H

#include <CL/cl.h>
#include "cl_icd_amd.h"

#define cl_amd_agent 1

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef const struct _cl_agent cl_agent;

#define CL_AGENT_VERSION_1_0 100

/* Context Callbacks */

typedef void (CL_CALLBACK * acContextCreate_fn)(
    cl_agent * /* agent */,
    cl_context /* context */);

typedef void (CL_CALLBACK * acContextFree_fn)(
    cl_agent * /* agent */,
    cl_context /* context */);

/* Command Queue Callbacks */

typedef void (CL_CALLBACK * acCommandQueueCreate_fn)(
    cl_agent *       /* agent */,
    cl_command_queue /* queue */);

typedef void (CL_CALLBACK * acCommandQueueFree_fn)(
    cl_agent *       /* agent */,
    cl_command_queue /* queue */);

/* Event Callbacks */

typedef void (CL_CALLBACK * acEventCreate_fn)(
    cl_agent *      /* agent */,
    cl_event        /* event */,
    cl_command_type /* type */);

typedef void (CL_CALLBACK * acEventFree_fn)(
    cl_agent * /* agent */,
    cl_event   /* event */);

typedef void (CL_CALLBACK * acEventStatusChanged_fn)(
    cl_agent * /* agent */,
    cl_event   /* event */,
    cl_int     /* execution_status */,
    cl_long    /* epoch_time_stamp */);

/* Memory Object Callbacks */

typedef void (CL_CALLBACK * acMemObjectCreate_fn)(
    cl_agent * /* agent */,
    cl_mem     /* memobj */);

typedef void (CL_CALLBACK * acMemObjectFree_fn)(
    cl_agent * /* agent */,
    cl_mem     /* memobj */);

typedef void (CL_CALLBACK * acMemObjectAcquired_fn)(
    cl_agent *   /* agent */,
    cl_mem       /* memobj */,
    cl_device_id /* device */,
    cl_long      /* elapsed_time */);

/* Sampler Callbacks */

typedef void (CL_CALLBACK * acSamplerCreate_fn)(
    cl_agent * /* agent */,
    cl_sampler /* sampler */);

typedef void (CL_CALLBACK * acSamplerFree_fn)(
    cl_agent * /* agent */,
    cl_sampler /* sampler */);

/* Program Callbacks */

typedef void (CL_CALLBACK * acProgramCreate_fn)(
    cl_agent * /* agent */,
    cl_program /* program */);

typedef void (CL_CALLBACK * acProgramFree_fn)(
    cl_agent * /* agent */,
    cl_program /* program */);

typedef void (CL_CALLBACK * acProgramBuild_fn)(
    cl_agent * /* agent */,
    cl_program /* program */);

/* Kernel Callbacks */

typedef void (CL_CALLBACK * acKernelCreate_fn)(
    cl_agent * /* agent */,
    cl_kernel  /* kernel */);

typedef void (CL_CALLBACK * acKernelFree_fn)(
    cl_agent * /* agent */,
    cl_kernel  /* kernel */);

typedef void (CL_CALLBACK * acKernelSetArg_fn)(
    cl_agent *   /* agent */,
    cl_kernel    /* kernel */,
    cl_int       /* arg_index */,
    size_t       /* size */,
    const void * /* value_ptr */);

typedef struct _cl_agent_callbacks
{
    /* Context Callbacks */
    acContextCreate_fn     ContextCreate;
    acContextFree_fn       ContextFree;

   /* Command Queue Callbacks */
   acCommandQueueCreate_fn CommandQueueCreate;
   acCommandQueueFree_fn   CommandQueueFree;

   /* Event Callbacks */
   acEventCreate_fn        EventCreate;
   acEventFree_fn          EventFree;
   acEventStatusChanged_fn EventStatusChanged;

   /* Memory Object Callbacks */
   acMemObjectCreate_fn    MemObjectCreate;
   acMemObjectFree_fn      MemObjectFree;
   acMemObjectAcquired_fn  MemObjectAcquired;

   /* Sampler Callbacks */
   acSamplerCreate_fn      SamplerCreate;
   acSamplerFree_fn        SamplerFree;

   /* Program Callbacks */
   acProgramCreate_fn      ProgramCreate;
   acProgramFree_fn        ProgramFree;
   acProgramBuild_fn       ProgramBuild;

   /* Kernel Callbacks */
   acKernelCreate_fn       KernelCreate;
   acKernelFree_fn         KernelFree;
   acKernelSetArg_fn       KernelSetArg;

} cl_agent_callbacks;

typedef cl_uint cl_agent_capability_action;

#define CL_AGENT_ADD_CAPABILITIES        0x0
#define CL_AGENT_RELINQUISH_CAPABILITIES 0x1

typedef struct _cl_agent_capabilities
{
    cl_bitfield canGenerateContextEvents      : 1;
    cl_bitfield canGenerateCommandQueueEvents : 1;
    cl_bitfield canGenerateEventEvents        : 1;
    cl_bitfield canGenerateMemObjectEvents    : 1;
    cl_bitfield canGenerateSamplerEvents      : 1;
    cl_bitfield canGenerateProgramEvents      : 1;
    cl_bitfield canGenerateKernelEvents       : 1;

} cl_agent_capabilities;

struct _cl_agent
{
    cl_int (CL_API_CALL * GetVersionNumber)(
        cl_agent * /* agent */,
        cl_int *   /* version_ret */);

    cl_int (CL_API_CALL * GetPlatform)(
        cl_agent *       /* agent */,
        cl_platform_id * /* platform_id_ret */);

    cl_int (CL_API_CALL * GetTime) (
        cl_agent * /* agent */,
        cl_long *  /* time_nanos */);

    cl_int (CL_API_CALL * SetCallbacks)(
        cl_agent *                 /* agent */,
        const cl_agent_callbacks * /* callbacks */,
        size_t                     /* size */);


    cl_int (CL_API_CALL * GetPotentialCapabilities)(
        cl_agent *              /* agent */,
        cl_agent_capabilities * /* capabilities */);

    cl_int (CL_API_CALL * GetCapabilities)(
        cl_agent *              /* agent */,
        cl_agent_capabilities * /* capabilities */);

    cl_int (CL_API_CALL * SetCapabilities)(
        cl_agent *                    /* agent */,
        const cl_agent_capabilities * /* capabilities */,
        cl_agent_capability_action    /* action */);


    cl_int (CL_API_CALL * GetICDDispatchTable)(
        cl_agent *              /* agent */,
        cl_icd_dispatch_table * /* table */,
        size_t                  /* size */);

    cl_int (CL_API_CALL * SetICDDispatchTable)(
        cl_agent *                    /* agent */,
        const cl_icd_dispatch_table * /* table */,
        size_t                        /* size */);

    /* add Kernel/Program helper functions, etc... */
};

extern cl_int CL_CALLBACK
clAgent_OnLoad(cl_agent * /* agent */);

extern void CL_CALLBACK
clAgent_OnUnload(cl_agent * /* agent */);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OPENCL_CL_AGENT_AMD_H */
