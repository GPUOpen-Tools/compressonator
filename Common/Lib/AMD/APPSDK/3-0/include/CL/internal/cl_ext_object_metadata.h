#ifndef __CL_EXT_OBJECT_METADATA_H
#define __CL_EXT_OBJECT_METADATA_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __APPLE__
    #include <OpenCL/cl.h>
    #include <AvailabilityMacros.h>
#else
    #include <CL/cl.h>
#endif

// <amd_internal>
    /*************************
    * cl_ext_object_metadata *
    **************************/
    #define cl_ext_object_metadata 1

    typedef size_t cl_key_ext;

    #define CL_INVALID_OBJECT_EXT    0x403A
    #define CL_INVALID_KEY_EXT       0x403B
    #define CL_PLATFORM_MAX_KEYS_EXT 0x403C

    typedef CL_API_ENTRY cl_key_ext (CL_API_CALL * clCreateKeyEXT_fn)(
        cl_platform_id      /* platform */,
        void (CL_CALLBACK * /* destructor */)( void* /* old_value */),
        cl_int *            /* errcode_ret */) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int (CL_API_CALL * clObjectGetValueForKeyEXT_fn)(
        void *               /* object */,
        cl_key_ext           /* key */,
        void **              /* ret_val */) CL_API_SUFFIX__VERSION_1_1;

    typedef CL_API_ENTRY cl_int (CL_API_CALL * clObjectSetValueForKeyEXT_fn)(
        void *               /* object */,
        cl_key_ext           /* key */,
        void *               /* value */) CL_API_SUFFIX__VERSION_1_1;
// </amd_internal>

#ifdef __cplusplus
}
#endif


#endif /* __CL_EXT_OBJECT_METADATA_H */
