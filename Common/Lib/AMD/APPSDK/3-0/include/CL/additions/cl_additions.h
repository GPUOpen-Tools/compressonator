#ifndef __OPENCL_CL_ADDITIONS_H_
#define __OPENCL_CL_ADDITIONS_H_

/*
This file contains additions that are not part of the cl.h file
(either removed from an older version or not yet added to an official version)
This file is a trimmed down version of the file found at //devtools/main/CommonProjects/AMDTOSWrappers/Include/CL/.
It is expected that this file will go away when moving to an APPSDK version that officially supports OCL 2.0
This file is maintained by the CodeXL team and does not come from the driver tree.
*/

#ifdef __cplusplus
extern "C" {
#endif


/*******************************************
    * Shared Virtual Memory (SVM) extension
    * The declarations match the order, naming and values of the original 2.0
    * standard, except for the fact that we added the _AMD suffix to each
    * symbol
    *******************************************/
typedef cl_bitfield                      cl_device_svm_capabilities_amd;
typedef cl_bitfield                      cl_svm_mem_flags_amd;
typedef cl_uint                          cl_kernel_exec_info_amd;

/* cl_device_info */
#define CL_DEVICE_SVM_CAPABILITIES_AMD                     0x1053
#define CL_DEVICE_PREFERRED_PLATFORM_ATOMIC_ALIGNMENT_AMD  0x1054

/* cl_device_svm_capabilities_amd */
#define CL_DEVICE_SVM_COARSE_GRAIN_BUFFER_AMD             (1 << 0)
#define CL_DEVICE_SVM_FINE_GRAIN_BUFFER_AMD               (1 << 1)
#define CL_DEVICE_SVM_FINE_GRAIN_SYSTEM_AMD               (1 << 2)
#define CL_DEVICE_SVM_ATOMICS_AMD                         (1 << 3)

/* cl_svm_mem_flags_amd */
#define CL_MEM_SVM_FINE_GRAIN_BUFFER_AMD                  (1 << 10)
#define CL_MEM_SVM_ATOMICS_AMD                            (1 << 11)

/* cl_mem_info */
#define CL_MEM_USES_SVM_POINTER_AMD                       0x1109

/* cl_kernel_exec_info_amd */
#define CL_KERNEL_EXEC_INFO_SVM_PTRS_AMD                  0x11B6
#define CL_KERNEL_EXEC_INFO_SVM_FINE_GRAIN_SYSTEM_AMD     0x11B7

/* cl_command_type */
#define CL_COMMAND_SVM_FREE_AMD                           0x1209
#define CL_COMMAND_SVM_MEMCPY_AMD                         0x120A
#define CL_COMMAND_SVM_MEMFILL_AMD                        0x120B
#define CL_COMMAND_SVM_MAP_AMD                            0x120C
#define CL_COMMAND_SVM_UNMAP_AMD                          0x120D

typedef CL_API_ENTRY void*
(CL_API_CALL * clSVMAllocAMD_fn)(
    cl_context            /* context */,
    cl_svm_mem_flags_amd  /* flags */,
    size_t                /* size */,
    unsigned int          /* alignment */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY void
(CL_API_CALL * clSVMFreeAMD_fn)(
    cl_context  /* context */,
    void*       /* svm_pointer */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clEnqueueSVMFreeAMD_fn)(
    cl_command_queue /* command_queue */,
    cl_uint          /* num_svm_pointers */,
    void**           /* svm_pointers */,
    void (CL_CALLBACK *)( /*pfn_free_func*/
        cl_command_queue /* queue */,
        cl_uint          /* num_svm_pointers */,
        void**           /* svm_pointers */,
        void*            /* user_data */),
    void*             /* user_data */,
    cl_uint           /* num_events_in_wait_list */,
    const cl_event*   /* event_wait_list */,
    cl_event*         /* event */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clEnqueueSVMMemcpyAMD_fn)(
    cl_command_queue /* command_queue */,
    cl_bool          /* blocking_copy */,
    void*            /* dst_ptr */,
    const void*      /* src_ptr */,
    size_t           /* size */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event*  /* event_wait_list */,
    cl_event*        /* event */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clEnqueueSVMMemFillAMD_fn)(
    cl_command_queue /* command_queue */,
    void*            /* svm_ptr */,
    const void*      /* pattern */,
    size_t           /* pattern_size */,
    size_t           /* size */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event*  /* event_wait_list */,
    cl_event*        /* event */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clEnqueueSVMMapAMD_fn)(
    cl_command_queue /* command_queue */,
    cl_bool          /* blocking_map */,
    cl_map_flags     /* map_flags */,
    void*            /* svm_ptr */,
    size_t           /* size */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event*  /* event_wait_list */,
    cl_event*        /* event */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clEnqueueSVMUnmapAMD_fn)(
    cl_command_queue /* command_queue */,
    void*            /* svm_ptr */,
    cl_uint          /* num_events_in_wait_list */,
    const cl_event*  /* event_wait_list */,
    cl_event*        /* event */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clSetKernelArgSVMPointerAMD_fn)(
    cl_kernel     /* kernel */,
    cl_uint       /* arg_index */,
    const void *  /* arg_value */
) CL_EXT_SUFFIX__VERSION_1_2;

typedef CL_API_ENTRY cl_int
(CL_API_CALL * clSetKernelExecInfoAMD_fn)(
        cl_kernel                /* kernel */,
        cl_kernel_exec_info_amd  /* param_name */,
        size_t                   /* param_value_size */,
        const void *             /* param_value */
) CL_EXT_SUFFIX__VERSION_1_2;

// The following are the amd_internal bits that are missing from the public version of cl_ext.h

// <amd_internal>
/***************************
* cl_amd_command_intercept *
***************************/
#define CL_CONTEXT_COMMAND_INTERCEPT_CALLBACK_AMD   0x403D
#define CL_QUEUE_COMMAND_INTERCEPT_ENABLE_AMD       (1ull << 63)

typedef cl_int (CL_CALLBACK * intercept_callback_fn)(cl_event, cl_int *);

/**************************
* cl_amd_command_queue_info *
**************************/
#define CL_QUEUE_THREAD_HANDLE_AMD                  0x403E

/***************************************
* cl-gl depth buffer interop extension *
****************************************/

#define CL_UNORM_INT24                              0x10DF
#define CL_GL_NUM_SAMPLES                           0x2012
// </amd_internal>

// <amd_internal>
/*************************
* cl_amd_object_metadata *
**************************/
#define cl_amd_object_metadata 1

typedef size_t cl_key_amd;

#define CL_INVALID_OBJECT_AMD    0x403A
#define CL_INVALID_KEY_AMD       0x403B
#define CL_PLATFORM_MAX_KEYS_AMD 0x403C

typedef CL_API_ENTRY cl_key_amd (CL_API_CALL * clCreateKeyAMD_fn)(
    cl_platform_id      /* platform */,
    void (CL_CALLBACK * /* destructor */)( void* /* old_value */),
    cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL * clObjectGetValueForKeyAMD_fn)(
    void *               /* object */,
    cl_key_amd           /* key */,
    void **              /* ret_val */) CL_API_SUFFIX__VERSION_1_1;

typedef CL_API_ENTRY cl_int (CL_API_CALL * clObjectSetValueForKeyAMD_fn)(
    void *               /* object */,
    cl_key_amd           /* key */,
    void *               /* value */) CL_API_SUFFIX__VERSION_1_1;
// </amd_internal>

#ifdef __cplusplus
}
#endif

#endif  /* __OPENCL_CL_ADDITIONS_H_ */
