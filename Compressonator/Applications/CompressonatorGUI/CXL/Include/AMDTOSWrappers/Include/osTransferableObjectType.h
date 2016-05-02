//=====================================================================
// Copyright 2016 (c), Advanced Micro Devices, Inc. All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file osTransferableObjectType.h
///
//=====================================================================

//------------------------------ osTransferableObjectType.h ------------------------------

#ifndef __OSTRANSFERABLEOBJECTTYPE
#define __OSTRANSFERABLEOBJECTTYPE

// Pre-deceleration:
class osChannel;

// Local:
#include <AMDTOSWrappers/Include/osOSWrappersDLLBuild.h>


// Contains the transferable objects types.
// (Each item in this enum represents a sub-class of osTransferableObject)
enum osTransferableObjectType
{
    // Pointer parameters:
    OS_TOBJ_ID_POINTER_PARAMETER,               // Pointer.
    OS_TOBJ_ID_POINTER_TO_POINTER_PARAMETER,    // Pointer to a pointer.

    // Collection parameters:
    OS_TOBJ_ID_VECTOR_PARAMETER,        // Vector parameter.
    OS_TOBJ_ID_MATRIX_PARAMETER,        // Matrix parameter.

    OS_TOBJ_ID_NOT_AVAILABLE_PARAMETER, // A "Not available" parameter.
    OS_TOBJ_ID_NOT_SUPPORTED_PARAMETER, // A "Not supported" parameter.
    OS_TOBJ_ID_REMOVED_PARAMETER,       // A "Removed from OpenGL" (deprecation model) parameter.

    // Pseudo parameters:
    OS_TOBJ_ID_ASSOCIATED_TEXTURE_NAMES_PSEUDO_PARAMETER,   // Texture names that are associated
    // with a function call.

    OS_TOBJ_ID_ASSOCIATED_PROGRAM_NAME_PSEUDO_PARAMETER,    // Program name that is associated
    // with a function call.

    OS_TOBJ_ID_ASSOCIATED_SHADER_NAME_PSEUDO_PARAMETER,     // Shader name is are associated
    // with a function call.

    // C / C++ parameter types:
    OS_TOBJ_ID_INT_PARAMETER,           // int parameter.
    OS_TOBJ_ID_UNSIGNED_INT_PARAMETER,  // unsigned int parameter.
    OS_TOBJ_ID_SIZE_T_PARAMETER,        // size_t parameter.
    OS_TOBJ_ID_BYTES_SIZE_PARAMETER,    // size in bytes parameter.
    OS_TOBJ_ID_FLOAT_PARAMETER,         // float parameter.
    OS_TOBJ_ID_STRING_PARAMETER,        // string parameter.

    // Pointer to C / C++ parameter types:
    OS_TOBJ_ID_P_CHAR_PARAMETER,        // pointer to char parameter
    OS_TOBJ_ID_PP_CHAR_PARAMETER,       // pointer to pointer to char
    OS_TOBJ_ID_P_INT_PARAMETER,         // pointer to int parameter.
    OS_TOBJ_ID_P_UINT_PARAMETER,        // pointer to unsigned int parameter.
    OS_TOBJ_ID_P_SIZE_T_PARAMETER,      // pointer to size_t parameter.
    OS_TOBJ_ID_P_BYTES_SIZE_T_PARAMETER,// pointer to size int bytes parameter.
    OS_TOBJ_ID_PP_VOID_PARAMETER,       // pointer to void pointer parameter.


    // OpenGL parameter types:
    OS_TOBJ_ID_GL_ENUM_PARAMETER,                   // GLenum parameter - OpenGL enumeration.
    OS_TOBJ_ID_GL_PRIMITIVE_TYPE_PARAMETER,         // GLenum parameter that represents OpenGL
    // primitive types (GL_LINES, GL_TRIANGLES, etc)
    OS_TOBJ_ID_GL_PIXEL_INTERNAL_FORMAT_PARAMETER,  // GLenum parameter that represents OpenGL
    // Pixel Internal Format (GL_ALPHA, GL_ALPHA4, GL_ALPHA8, etc.)
    OS_TOBJ_ID_GL_BOOL_PARAMETER,                   // GLboolean parameter - OpenGL bool.
    OS_TOBJ_ID_GL_BITFIELD_PARAMETER,               // GLbitfield parameter - OpenGL bit field.
    OS_TOBJ_ID_GL_CLEAR_BITFIELD_PARAMETER,         // GLClearBitfield parameter - OpenGL bit field for glClear.
    OS_TOBJ_ID_GL_BYTE_PARAMETER,                   // GLbyte parameter - OpenGL byte (1-byte signed).
    OS_TOBJ_ID_GL_SHORT_PARAMETER,                  // GLshort parameter - OpenGL short (2-byte signed).
    OS_TOBJ_ID_GL_INT_PARAMETER,                    // GLint parameter - OpenGL int (4-byte signed).
    OS_TOBJ_ID_GL_INT_64_PARAMETER,                 // GLint64 parameter - OpenGL int 64 (8-byte signed).
    OS_TOBJ_ID_GL_UINT_64_PARAMETER,                // GLuint64 parameter - OpenGL int 64 (8-byte unsigned).
    OS_TOBJ_ID_GL_UINT_64_ADDRESS_PARAMETER,        // GLuint64 as address parameter - OpenGL int 64 (8-byte unsigned), when used as address.
    OS_TOBJ_ID_GL_UBYTE_PARAMETER,                  // GLubyte - OpenGL unsigned byte (1-byte unsigned).
    OS_TOBJ_ID_GL_USHORT_PARAMETER,                 // GLushort - OpenGL unsigned short (2-byte unsigned).
    OS_TOBJ_ID_GL_UINT_PARAMETER,                   // GLuint - OpenGL unsigned int (4-byte unsigned).
    OS_TOBJ_ID_GL_SIZEI_PARAMETER,                  // GLsizei - OpenGL size (4-byte signed).
    OS_TOBJ_ID_GL_FLOAT_PARAMETER,                  // GLfloat - OpenGL single precision float.
    OS_TOBJ_ID_GL_CLAMPF_PARAMETER,                 // GLclampf - OpenGL single precision float in [0,1].
    OS_TOBJ_ID_GL_DOUBLE_PARAMETER,                 // GLdouble - OpenGL double precision float.
    OS_TOBJ_ID_GL_CLAMPD_PARAMETER,                 // GLclampd - OpenGL double precision float in [0,1].
    OS_TOBJ_ID_GL_INTPTR_PARAMETER,                 // GLintptr parameter - OpenGL type for handling large vertex buffer objects.
    OS_TOBJ_ID_GL_SIZEIPTR_PARAMETER,               // GLsizeiptr parameter - OpenGL type for handling large vertex buffer objects.
    OS_TOBJ_ID_GL_STRING_PARAMETER,                 // OpenGL string parameter.
    OS_TOBJ_ID_GL_MULTI_STRING_PARAMETER,           // OpenGL multi string parameter: a vector of strings.
    OS_TOBJ_ID_GL_SYNC_PARAMETER,                   // OpenGL GLsync object

    // Pointers to OpenGL parameter types:
    // (These pointers do not have their own class. Instead, they are represented
    //  using apPointerParameter and apPointerToPointerParameter)
    OS_TOBJ_ID_GL_P_BOOL_PARAMETER,     // Pointer to GLboolean.
    OS_TOBJ_ID_GL_P_VOID_PARAMETER,     // Pointer to GLvoid.
    OS_TOBJ_ID_GL_P_BYTE_PARAMETER,     // Pointer to GLbyte.
    OS_TOBJ_ID_GL_P_SHORT_PARAMETER,    // Pointer to GLshort.
    OS_TOBJ_ID_GL_P_INT_PARAMETER,      // Pointer to GLint.
    OS_TOBJ_ID_GL_P_INT_64_PARAMETER,   // Pointer to GLint64.
    OS_TOBJ_ID_GL_P_UINT_64_PARAMETER,  // Pointer to GLUint64.
    OS_TOBJ_ID_GL_P_UBYTE_PARAMETER,    // Pointer to GLubyte.
    OS_TOBJ_ID_GL_P_USHORT_PARAMETER,   // Pointer to GLushort.
    OS_TOBJ_ID_GL_P_UINT_PARAMETER,     // Pointer to GLuint.
    OS_TOBJ_ID_GL_P_FLOAT_PARAMETER,    // Pointer to GLfloat.
    OS_TOBJ_ID_GL_P_CLAMPF_PARAMETER,   // Pointer to GLclampf.
    OS_TOBJ_ID_GL_P_DOUBLE_PARAMETER,   // Pointer to GLdouble.
    OS_TOBJ_ID_GL_PP_VOID_PARAMETER,    // Pointer to a GLvoid pointer.
    OS_TOBJ_ID_GL_P_SIZEI_PARAMETER,    // Pointer to GLsizei.
    OS_TOBJ_ID_GL_P_SIZEIPTR_PARAMETER, // Pointer to GLsizeiptr.
    OS_TOBJ_ID_GL_P_INTPTR_PARAMETER,   // Pointer to GLintptr.
    OS_TOBJ_ID_GL_P_ENUM_PARAMETER,     // Pointer to GLEnum.
    OS_TOBJ_ID_GL_P_CHAR,               // Pointer to char.
    OS_TOBJ_ID_GL_PP_CHAR,              // Pointer to char pointer.

    // CGL parameter types:
    OS_TOBJ_ID_CGL_PIXEL_FORMAT_ATTRIBUTE_PARAMETER,    // CGL pixel format attribute.
    OS_TOBJ_ID_CGL_CONTEXT_ENABLE_PARAMETER,            // CGL context enable.
    OS_TOBJ_ID_CGL_CONTEXT_PARAMETER_PARAMETER,         // CGL context parameter.
    OS_TOBJ_ID_CGL_GLOBAL_OPTION_PARAMETER,             // CGL global option.
    OS_TOBJ_ID_CGL_RENDERER_PROPERTY_PARAMETER,         // CGL renderer property.
    OS_TOBJ_ID_CGL_BUFFER_MODE_MASK_PARAMETER,          // CGL buffer mode mask.
    OS_TOBJ_ID_CGL_COLOR_BUFFER_FORMAT_MASK_PARAMETER,  // CGL color buffer fomrat mask.
    OS_TOBJ_ID_CGL_RENDERER_ID_MASK_PARAMETER,          // CGL renderer id mask.
    OS_TOBJ_ID_CGL_SAMPLING_MODE_MASK_PARAMETER,        // CGL sampling mode mask.
    OS_TOBJ_ID_CGL_STENCIL_AND_DEPTH_MASK_PARAMETER,    // CGL stencil and depth mask.

    // OpenGL ES parameter types:
    OS_TOBJ_ID_GL_CLAMPX_PARAMETER,     // GLclampx - Float represented as int.
    OS_TOBJ_ID_GL_FIXED_PARAMETER,      // GLfixed - Float represented as int.

    // Pointers to OpenGL ES parameter types:
    OS_TOBJ_ID_GL_P_CLAMPX_PARAMETER,       // Pointer to GLclampx.
    OS_TOBJ_ID_GL_P_FIXED_PARAMETER,        // Pointer to GLfixed.

    // EGL parameter types:
    OS_TOBJ_ID_EGL_BOOLEAN_PARAMETER,       // EGLBoolean.
    OS_TOBJ_ID_EGL_INT_PARAMETER,           // EGLint.
    OS_TOBJ_ID_EGL_DISPLAY_PARAMETER,       // EGLDisplay.
    OS_TOBJ_ID_EGL_CONFIG_PARAMETER,        // EGLConfig.
    OS_TOBJ_ID_EGL_SURFACE_PARAMETER,       // EGLSufrace.
    OS_TOBJ_ID_EGL_CONTEXT_PARAMETER,       // EGLContext.

    // Pointer to EGL parameters:
    OS_TOBJ_ID_P_EGL_INT_PARAMETER,         // Pointer to EGLint.
    OS_TOBJ_ID_P_EGL_CONFIG_PARAMETER,      // Pointer to EGLConfig.

    // EAGL parameter types:
    OS_TOBJ_ID_EAGL_RENDERING_API_PARAMETER,    // EAGLRenderingAPI

    // WGL parameter types:
    OS_TOBJ_ID_GL_HPBUFFERARB,          // HPBUFFERARB.

    // OpenCL parameter types:
    OS_TOBJ_ID_CL_UCHAR_PARAMETER,                          // cl_uchar - OpenCL unsigned char.
    OS_TOBJ_ID_CL_CHAR_PARAMETER,                           // cl_char - OpenCL char.
    OS_TOBJ_ID_CL_UINT_PARAMETER,                           // cl_uint - OpenCL unsigned int.
    OS_TOBJ_ID_CL_INT_PARAMETER,                            // cl_int - OpenCL int.
    OS_TOBJ_ID_CL_ULONG_PARAMETER,                          // cl_ulong - OpenCL unsigned long.
    OS_TOBJ_ID_CL_LONG_PARAMETER,                           // cl_long - OpenCL long.
    OS_TOBJ_ID_CL_BOOL_PARAMETER,                           // cl_bool - OpenCL boolean
    OS_TOBJ_ID_CL_MEM_FLAGS_PARAMETER,                      // cl_mem_flags - OpenCL memory flags bitfield.
    OS_TOBJ_ID_CL_SVM_MEM_FLAGS_PARAMETER,                  // cl_svm_mem_flags - OpenCL SVM memory flag bitfield.
    OS_TOBJ_ID_CL_GL_OBJECT_TYPE_PARAMETER,                 // cl_gl_object_type - OpenCL OpenGL object type.
    OS_TOBJ_ID_CL_GL_TEXTURE_INFO_PARAMETER,                // cl_gl_texture_info - OpenCL OpenGL texture info.
    OS_TOBJ_ID_CL_MULTI_STRING_PARAMETER,                   // OpenCL multi string parameter: a vector of strings.
    OS_TOBJ_ID_CL_HANDLE_PARAMETER,                         // OpenCL handle (cl_mem / cl_command_queue / cl_context etc.):
    OS_TOBJ_ID_CL_DEVICE_TYPE_PARAMETER,                    // OpenCL device type
    OS_TOBJ_ID_CL_DEVICE_EXECUTION_CAPABILITIES_PARAMETER,  // OpenCL device execution capabilities (cl_device_exec_capabilities)
    OS_TOBJ_ID_CL_DEVICE_FLOATING_POINT_CONFIG_PARAMETER,   // OpenCL device floating point configuration (cl_device_fp_config)
    OS_TOBJ_ID_CL_COMMAND_QUEUE_PROPERTIES_PARAMETER,       // OpenCL cl_command_queue_properties
    OS_TOBJ_ID_CL_MAP_FLAGS_PARAMETER,                      // OpenCL cl_map_flags
    OS_TOBJ_ID_CL_MEM_MIGRATION_FLAGS_PARAMETER,            // OpenCL cl_mem_migration_flags
    OS_TOBJ_ID_CL_DEVICE_AFFINITY_DOMAIN_PARAMETER,         // OpenCL cl_device_affinity_domain
    OS_TOBJ_ID_CL_DEVICE_SVM_CAPABILITIES_PARAMETER,        // OpenCL cl_device_svm_capabilities
    OS_TOBJ_ID_CL_KERNEL_ARG_TYPE_QUALIFIER_PARAMETER,      // OpenCL cl_kernel_arg_type_qualifier
    OS_TOBJ_ID_CL_ENUM_PARAMETER,                           // OpenCL cl_uint types, which are used as enumerations
    OS_TOBJ_ID_CL_BUFFER_REGION_PARAMETER,                  // cl_buffer_region - OpenCL sub buffer creation flags.
    OS_TOBJ_ID_CL_IMAGE_DESCRIPTION_PARAMETER,              // cl_image_desc - OpenCL image description.
    OS_TOBJ_ID_CL_CONTEXT_PROPERTIES_LIST_PARAMETER,        // cl_context_properties* - a zero-terminated list of context properties
    OS_TOBJ_ID_CL_COMMAND_QUEUE_PROPERTIES_LIST_PARAMETER,  // cl_command_queue_properties* - a zero-terminated list of queue properties
    OS_TOBJ_ID_CL_PIPE_PROPERTIES_LIST_PARAMETER,           // cl_pipe_properties* - a zero-terminated list of pipe properties
    OS_TOBJ_ID_CL_SAMPLER_PROPERTIES_LIST_PARAMETER,        // cl_sampler_properties* - a zero-terminated list of sampler properties

    // OpenCL pointer to types:
    OS_TOBJ_ID_CL_P_UINT_PARAMETER,                     // Pointer to cl_uint
    OS_TOBJ_ID_CL_P_INT_PARAMETER,                      // Pointer to cl_int

    // Win32 parameter types:
    OS_TOBJ_ID_WIN32_BOOL_PARAMETER,        // Win32 BOOL.
    OS_TOBJ_ID_WIN32_UINT_PARAMETER,        // Win32 UINT.
    OS_TOBJ_ID_WIN32_INT_PARAMETER,         // Win32 INT.
    OS_TOBJ_ID_WIN32_FLOAT_PARAMETER,       // Win32 FLOAT.
    OS_TOBJ_ID_WIN32_DWORD_PARAMETER,       // Win32 DWORD.
    OS_TOBJ_ID_WIN32_HANDLE_PARAMETER,      // Win32 HANDLE (used only in WGL_ARB_buffer_region so far - Uri 8/9/08)

    // Pointers to Win32 parameter types:
    OS_TOBJ_ID_WIN32_HDC_PARAMETER,                 // Win32 HDC.
    OS_TOBJ_ID_WIN32_HGLRC_PARAMETER,               // Win32 HGLRC.
    OS_TOBJ_ID_LP_PIXELFORMATDESCRIPTOR_PARAMETER,  // Win32 LPPIXELFORMATDESCRIPTOR.
    OS_TOBJ_ID_P_PIXELFORMATDESCRIPTOR_PARAMETER,   // Pointer to Win32 PIXELFORMATDESCRIPTOR
    OS_TOBJ_ID_LP_LAYERPLANEDESCRIPTOR_PARAMETER,   // Win32 LPLAYERPLANEDESCRIPTOR.
    OS_TOBJ_ID_LP_GLYPHMETRICSFLOAT_PARAMETER,      // Win32 LPGLYPHMETRICSFLOAT.
    OS_TOBJ_ID_LP_CSTR_PARAMETER,                   // Win32 LPCSTR.
    OS_TOBJ_ID_P_WGLSWAP_PARAMETER,                 // Pointer to Win32 WGLSWAP.
    OS_TOBJ_ID_P_COLORREF_PARAMETER,                // Pointer to Win32 COLORREF.
    OS_TOBJ_ID_P_WIN32_FLOAT_PARAMETER,             // Pointer to Win32 FLOAT.
    OS_TOBJ_ID_P_WIN32_UINT_PARAMETER,              // Pointer to Win32 UINT.
    OS_TOBJ_ID_P_WIN32_INT_PARAMETER,               // Pointer to Win32 INT.

    // Linux parameter types:
    OS_TOBJ_ID_X11_BOOL_PARAMETER,                  // X11 Bool parameter.
    OS_TOBJ_ID_XORG_CARD32_PARAMETER,               // Xorg CARD32 parameter.
    OS_TOBJ_ID_XID_PARAMETER,                       // XID used in GLX functions.
    OS_TOBJ_ID_LONG_BITFIELD_PARAMETER,             // unsigned long parameter - used in glX as masks.
    OS_TOBJ_ID_GLX_ENUM_PARAMETER,                  // For glX enum values that reuse OpenGL numbers.

    // GROSWrappers classes:
    OS_TOBJ_ID_FILE_PATH,                   // osFilePath class type.
    OS_TOBJ_ID_CALLS_STACK,                 // osCallStack class type.
    OS_TOBJ_ID_CALLS_STACK_FRAME,           // osCallStackFrame class type.
    OS_TOBJ_ID_RAW_MEMORY_BUFFER,           // osRawMemoryBuffer class type.
    OS_TOBJ_ID_DIRECTORY,                   // osDirectory class type.

    // ApiClasses classes:
    OS_TOBJ_ID_FUNCTION_CALL,                   // apFunctionCall.
    OS_TOBJ_ID_MONITORED_FUNC_BREAKPOINT,       // apMonitoredFunctionBreakPoint.
    OS_TOBJ_ID_KERNEL_SOURCE_CODE_BREAKPOINT,   // apKernelSourceCodeBreakpoint.
    OS_TOBJ_ID_HOST_SOURCE_CODE_BREAKPOINT,     // apHostSourceBreakpoint
    OS_TOBJ_ID_SOURCE_CODE_BREAKPOINT,          // apSourceCodeBreakpoint.
    OS_TOBJ_ID_KERNEL_FUNC_NAME_BREAKPOINT,     // apKernelFunctionNameBreakpoint.
    OS_TOBJ_ID_GENERIC_BREAKPOINT,              // apGenericBreakpoint.
    OS_TOBJ_ID_APIFUNCTIONS_INIT_DATA,          // apGRApiFunctionsInitializationData.
    OS_TOBJ_ID_2D_POINT,                        // ap2DPoint.
    OS_TOBJ_ID_2D_RECTANGLE,                    // ap2DRectangle.
    OS_TOBJ_ID_FPS_DISPLAY_ITEM,                // apFPSDisplayItem.
    OS_TOBJ_ID_GL_TEXTURE,                      // apGLTexture.
    OS_TOBJ_ID_GL_TEXTURE_THUMBNAIL_DATA,       // apGLTextureThumbnailData.
    OS_TOBJ_ID_GL_TEXTURE_DATA,                 // apGLTextureData.
    OS_TOBJ_ID_GL_TEXTURE_MEMORY_DATA,          // apGLTextureMemoryData.
    OS_TOBJ_ID_GL_SAMPLER,                      // apGLSampler.
    OS_TOBJ_ID_GL_FBO,                          // apGLFBO.
    OS_TOBJ_ID_GL_VBO,                          // apGLVBO.
    OS_TOBJ_ID_GL_SYNC,                         // apGLSync.
    OS_TOBJ_ID_GL_BIND_OBJECT,                  // apFBOBindObject.
    OS_TOBJ_ID_GL_RENDER_BUFFER,                // apGLRenderBuffer.
    OS_TOBJ_ID_GL_STATIC_BUFFER,                // apStaticBuffer.
    OS_TOBJ_ID_GL_PBUFFER,                      // apPBuffer
    OS_TOBJ_ID_GL_PROGRAM,                      // apGLProgram.
    OS_TOBJ_ID_GL_PIPELINE,                     // apGLPipeline.
    OS_TOBJ_ID_GL_VERTEX_SHADER,                // apGLVertexShader.
    OS_TOBJ_ID_GL_TESSELLATION_CONTROL_SHADER,  // apGLTessellationControlShader.
    OS_TOBJ_ID_GL_TESSELLATION_EVALUATION_SHADER, // apGLTessellationEvaluationShader.
    OS_TOBJ_ID_GL_GEOMETRY_SHADER,              // apGLGeometryShader.
    OS_TOBJ_ID_GL_FRAGMENT_SHADER,              // apGLFragmentShader.
    OS_TOBJ_ID_GL_COMPUTE_SHADER,               // apGLComputeShader.
    OS_TOBJ_ID_GL_UNSUPPORTED_SHADER,           // apGLUnsupportedShader.
    OS_TOBJ_ID_GL_TRANSFORM_FEEDBACK_OBJECT,    // apGLTransformFeedbackObject.
    OS_TOBJ_ID_GL_ITEMS_COLLECTION,             // apGLItemsCollection
    OS_TOBJ_ID_GL_RENDER_CONTEXT_INFO,          // apGLRenderContextInfo
    OS_TOBJ_ID_GL_RENDER_CONTEXT_GRAPHICS_INFO, // apGLRenderContextGraphicsInfo
    OS_TOBJ_ID_COUNTER_INFO,                    // apCounterInfo
    OS_TOBJ_ID_COUNTER_ACTIVATION_INFO,         // apCounterActivationInfo
    OS_TOBJ_ID_ENUM_USAGE_STATISTICS,           // apEnumeratorUsageStatistics
    OS_TOBJ_ID_FUNC_CALL_STATISTICS,            // apFunctionCallStatistics
    OS_TOBJ_ID_DETECTED_ERROR_PARAMS,           // apDetectedErrorParameters,
    OS_TOBJ_ID_OPENCL_ERROR_PARAMS,             // apOpenCLErrorParameters,
    OS_TOBJ_ID_DISPLAY_LIST,                    // apGLDisplayList,
    OS_TOBJ_ID_FUNCTION_DEPRECATION,            // apFunctionDeprecation
    OS_TOBJ_ID_STATE_VARIABLE_VALUE,            // apStateVariableValue
    OS_TOBJ_ID_STATE_VARIABLE_SNAPSHOT,         // apStateVariablesSnapShot
    OS_TOBJ_ID_PROCESS_CREATION_DATA,           // apDebugProjectSettings
    OS_TOBJ_ID_CL_PROGRAM,                      // apCLProgram
    OS_TOBJ_ID_CL_KERNEL,                       // apCLKernel
    OS_TOBJ_ID_CL_DEVICE,                       // apCLDevice
    OS_TOBJ_ID_CL_COMMAND_QUEUE,                // apCLCommandQueue
    OS_TOBJ_ID_CL_BUFFER,                       // apCLBuffer
    OS_TOBJ_ID_CL_SUB_BUFFER,                   // apCLSubBuffer
    OS_TOBJ_ID_CL_CONTEXT,                      // apCLContext
    OS_TOBJ_ID_CL_CONTEXT_PROPERTIES,           // apCLContextProperties
    OS_TOBJ_ID_CL_IMAGE,                        // apCLTexture
    OS_TOBJ_ID_CONTEXT_ID,                      // apContextId
    OS_TOBJ_ID_COUNTER_SCOPE_ID,                // apQueueID
    OS_TOBJ_ID_CL_OBJECT_ID,                    // apCLObjectId
    OS_TOBJ_ID_CL_EVENT,                        // apCLEvent
    OS_TOBJ_ID_CL_SAMPLER,                      // apCLSampler
    OS_TOBJ_ID_CL_PIPE,                         // apCLPipe

    // APIClasses Event classes:
    OS_TOBJ_ID_DEBUGGED_PROCESS_CREATED_EVENT,          // apDebuggedProcessCreatedEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_RUN_STARTED_EVENT,      // apDebuggedProcessRunStartedEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_TERMINATED_EVENT,       // apDebuggedProcessTerminatedEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_CREATION_FAILURE_EVENT, // apDebuggedProcessCreationFailureEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_RUN_SUSPENDED_EVENT,    // apDebuggedProcessRunSuspendedEvent
    OS_TOBJ_ID_BEFORE_DEBUGGED_PROCESS_RUN_RESUMED_EVENT, // apBeforeDebuggedProcessRunResumedEvent
    OS_TOBJ_ID_DURING_DEBUGGED_PROCESS_TERMINATION_EVENT, // apDebuggedProcessIsDuringTerminationEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_RUN_RESUMED_EVENT,      // apDebuggedProcessRunResumedEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_RUN_STARTED_EXTERNALLY_EVENT, // apDebuggedProcessRunStartedExternallyEvent
    OS_TOBJ_ID_THREAD_CREATED_EVENT,                    // apThreadCreatedEvent
    OS_TOBJ_ID_THREAD_TERMINATED_EVENT,                 // apThreadTerminatedEvent
    OS_TOBJ_ID_MODULE_LOADED_EVENT,                     // apModuleLoadedEvent
    OS_TOBJ_ID_MODULE_UNLOADED_EVENT,                   // apModuleUnloadedEvent
    OS_TOBJ_ID_EXCEPTION_EVENT,                         // apExceptionEvent
    OS_TOBJ_ID_BREAKPOINT_HIT_EVENT,                    // apBreakpointHitEvent
    OS_TOBJ_ID_OUTPUT_DEBUG_STRING_EVENT,               // apOutputDebugStringEvent
    OS_TOBJ_ID_USER_WARNING_EVENT,                      // apUserWarningEvent
    OS_TOBJ_ID_GDB_OUTPUT_STRING_EVENT,                 // apGDBOutputStringEvent
    OS_TOBJ_ID_GDB_ERROR_EVENT,                         // apGDBErrorEvent
    OS_TOBJ_ID_GDB_LISTENER_THREAD_WAS_SUSPENDED_EVENT, // apGDBListenerThreadWasSuspendedEvent
    OS_TOBJ_ID_DEBUGGED_PROCESS_OUTPUT_STRING_EVENT,    // apDebuggedProcessOutputStringEvent
    OS_TOBJ_ID_API_CONNECTION_ESTABLISHED_EVENT,        // apApiConnectionEstablishedEvent
    OS_TOBJ_ID_API_CONNECTION_ENDED_EVENT,              // apApiConnectionEndedEvent
    OS_TOBJ_ID_DETECTED_ERROR_EVENT,                    // apDebuggedProcessDetectedErrorEvent
    OS_TOBJ_ID_INFRASTRUCTURE_STARTS_BEING_BUSY_EVENT,  // apInfrastructureStartsBeingBusyEvent
    OS_TOBJ_ID_INFRASTRUCTURE_ENDS_BEING_BUSY_EVENT,    // apInfrastructureEndsBeingBusyEvent
    OS_TOBJ_ID_INFRASTRUCTURE_FAILURE_EVENT,            // apInfrastructureFailureEvent
    OS_TOBJ_ID_MEMORY_LEAK_EVENT,                       // apMemoryLeakEvent
    OS_TOBJ_ID_SEARCHING_FOR_MEMORY_LEAKS_EVENT,        // apSearchingForMemoryLeaksEvent
    OS_TOBJ_ID_COMPUTE_CONTEXT_CREATED_EVENT,           // apComputeContextCreatedEvent
    OS_TOBJ_ID_COMPUTE_CONTEXT_DELETED_EVENT,           // apComputeContextDeletedEvent
    OS_TOBJ_ID_RENDER_CONTEXT_CREATED_EVENT,            // apRenderContextCreatedEvent
    OS_TOBJ_ID_RENDER_CONTEXT_DELETED_EVENT,            // apRenderContextDeletedEvent
    OS_TOBJ_ID_CONTEXT_UPDATED_EVENT,                   // apContextDataSnapshotWasUpdatedEvent
    OS_TOBJ_ID_GL_DEBUG_OUTPUT_MESSAGE_EVENT,           // apGLDebugOutputMessageEvent
    OS_TOBJ_ID_OPENCL_ERROR_EVENT,                      // apOpenCLErrorEvent
    OS_TOBJ_ID_COMMAND_QUEUE_CREATED_EVENT,             // apOpenCLQueueCreatedEvent
    OS_TOBJ_ID_COMMAND_QUEUE_DELETED_EVENT,             // apOpenCLQueueDeletedEvent
    OS_TOBJ_ID_CL_PROGRAM_CREATED_EVENT,                // apOpenCLProgramCreatedEvent
    OS_TOBJ_ID_CL_PROGRAM_DELETED_EVENT,                // apOpenCLProgramDeletedEvent
    OS_TOBJ_ID_SPY_PROGRESS_EVENT,                      // apSpyProgressEvent
    OS_TOBJ_ID_TECHNOLOGY_MONITOR_FAILURE_EVENT,        // apTechnologyMonitorFailureEvent
    OS_TOBJ_ID_MONITORED_OBJECT_TREE_SELECTED_EVENT,    // apMonitoredObjectsTreeSelectedEvent
    OS_TOBJ_ID_MONITORED_OBJECT_TREE_ACTIVATED_EVENT,   // apMonitoredObjectsTreeActivatedEvent
    OS_TOBJ_ID_BEFORE_KERNEL_DEBUGGING_EVENT,           // apBeforeKernelDebuggingEvent
    OS_TOBJ_ID_AFTER_KERNEL_DEBUGGING_EVENT,            // apAfterKernelDebuggingEvent
    OS_TOBJ_ID_KERNEL_CURRENT_WORK_ITEM_CHANGED_EVENT,  // apKernelWorkItemChangedEvent
    OS_TOBJ_ID_KERNEL_DEBUGGING_FAILED_EVENT,           // apKernelDebuggingFailedEvent
    OS_TOBJ_ID_KERNEL_DEBUGGING_INTERRUPTED_EVENT,      // apKernelDebuggingInterruptedEvent
    OS_TOBJ_ID_FLUSH_TEXTURE_IMAGES_EVENT,              // apFlushTextureImageEvent
    OS_TOBJ_ID_BREAKPOINTS_UPDATED_EVENT,               // apBreakpointsUpdatedEvent
    OS_TOBJ_ID_KERNEL_SOURCE_BREAKPOINTS_UPDATED_EVENT, // apKernelSourceCodeBreakpointsUpdatedEvent
    OS_TOBJ_ID_CL_PROGRAM_BUILD_EVENT,                  // apOpenCLProgramBuildEvent
    OS_TOBJ_ID_CL_PROGRAM_BUILD_FAILED_WITH_DEBUG_FLAGS_EVENT, // apOpenCLProgramBuildFailedWithDebugFlagsEvent
    OS_TOBJ_ID_CALL_STACK_FRAME_SELECTED_EVENT,         // apCallStackFrameSelectedEvent
    OS_TOBJ_ID_DEFERRED_COMMAND_EVENT,                  // apDeferredCommandEvent
    OS_TOBJ_ID_MEMORY_ALLOCATION_FAILURE_EVENT,         // apMemoryAllocationFailureEvent
    OS_TOBJ_ID_MDI_VIEW_CREATION_EVENT,                 // apMDIViewCreateEvent
    OS_TOBJ_ID_MDI_VIEW_ACTIVATED_EVENT,                // apMDIViewActivatedEvent
    OS_TOBJ_ID_MDI_SOURCE_CODE_VIEW_CREATION_EVENT,     // apMDISourceCodeViewCreateEvent
    OS_TOBJ_ID_MDI_ADD_WATCH_EVENT,                     // apAddWatchEvent
    OS_TOBJ_ID_MDI_IMAGE_BUFFER_VIEW_CREATION_EVENT,    // apMDIImageBufferViewCreateEvent
    OS_TOBJ_ID_SHOW_HEX_CHANGED_EVENT,                  // apHexChangedEvent
    OS_TOBJ_ID_EXECUTION_MODE_CHANGED_EVENT,            // apExecutionModeChangedEvent

    OS_TOBJ_ID_MDI_CPU_PROFILE_VIEW_CREATION_EVENT,     // apCpuProfileViewCreateEvent
    OS_TOBJ_ID_PROFILE_PROCESS_TERMINATED_EVENT,        // apProcessTerminatedEvent
    OS_TOBJ_ID_PROFILE_PROGRESS_EVENT,                  // apProfileProgressEvent

    OS_TOBJ_ID_MDI_GPU_PROFILE_VIEW_CREATION_EVENT,     // apGpuProfileViewCreateEvent
    OS_TOBJ_ID_MDI_THREAD_PROFILE_VIEW_CREATION_EVENT,  // apThreadProfileViewCreateEvent
    OS_TOBJ_ID_MDI_KAVIEW_CREATION_EVENT,               // apKAViewCreateEvent
    OS_TOBJ_ID_GENETIC_VIEW_CREATION_EVENT,             // apGenericViewCreateEvent

    // APIClasses enqueued commands classes (apCLEnqueuedCommand):
    OS_TOBJ_ID_CL_ACQUIRE_GL_OBJECTS_COMMAND,   // apCLAcquireGLObjectsCommand
    OS_TOBJ_ID_CL_BARRIER_COMMAND,              // apCLBarrierCommand
    OS_TOBJ_ID_CL_COPY_BUFFER_COMMAND,          // apCLCopyBufferCommand
    OS_TOBJ_ID_CL_COPY_BUFFER_RECT_COMMAND,     // apCLCopyBufferRectCommand
    OS_TOBJ_ID_CL_COPY_BUFFER_TO_IMAGE_COMMAND, // apCLCopyBufferToImageCommand
    OS_TOBJ_ID_CL_COPY_IMAGE_COMMAND,           // apCLCopyImageCommand
    OS_TOBJ_ID_CL_COPY_IMAGE_TO_BUFFER_COMMAND, // apCLCopyImageToBufferCommand
    OS_TOBJ_ID_CL_MAP_BUFFER_COMMAND,           // apCLMapBufferCommand
    OS_TOBJ_ID_CL_MAP_IMAGE_COMMAND,            // apCLMapImageCommand
    OS_TOBJ_ID_CL_MARKER_COMMAND,               // apCLMarkerCommand
    OS_TOBJ_ID_CL_NATIVE_KERNEL_COMMAND,        // apCLNativeKernelCommand
    OS_TOBJ_ID_CL_ND_RANGE_KERNEL_COMMAND,      // apCLNDRangeKernelCommand
    OS_TOBJ_ID_CL_READ_BUFFER_COMMAND,          // apCLReadBufferCommand
    OS_TOBJ_ID_CL_READ_BUFFER_RECT_COMMAND,     // apCLReadBufferRectCommand
    OS_TOBJ_ID_CL_READ_IMAGE_COMMAND,           // apCLReadImageCommand
    OS_TOBJ_ID_CL_RELEASE_GL_OBJECTS_COMMAND,   // apCLReleaseGLObjectsCommand
    OS_TOBJ_ID_CL_TASK_COMMAND,                 // apCLTaskCommand
    OS_TOBJ_ID_CL_UNMAP_MEM_OBJECT_COMMAND,     // apCLUnmapMemObjectCommand
    OS_TOBJ_ID_CL_WAIT_FOR_EVENTS_COMMAND,      // apCLWaitForEventsCommand
    OS_TOBJ_ID_CL_WRITE_BUFFER_COMMAND,         // apCLWriteBufferCommand
    OS_TOBJ_ID_CL_WRITE_BUFFER_RECT_COMMAND,    // apCLWriteBufferRectCommand
    OS_TOBJ_ID_CL_WRITE_IMAGE_COMMAND,          // apCLWriteImageCommand
    OS_TOBJ_ID_CL_QUEUE_IDLE_TIME,              // apCLQueueIdle
    OS_TOBJ_ID_CL_FILL_BUFFER_COMMAND,          // apCLFillBufferCommand
    OS_TOBJ_ID_CL_FILL_IMAGE_COMMAND,           // apCLFillImageCommand
    OS_TOBJ_ID_CL_MIGRATE_MEM_OBJECTS_COMMAND,  // apCLMigrateMemObjectsCommand
    OS_TOBJ_ID_CL_MARKER_WITH_WAIT_LIST_COMMAND,// apCLMarkerWithWaitListCommand
    OS_TOBJ_ID_CL_BARRIER_WITH_WAIT_LIST_COMMAND,// apCLBarrierWithWaitListCommand
    OS_TOBJ_ID_CL_SVM_FREE_COMMAND,             // apCLSVMFreeCommand
    OS_TOBJ_ID_CL_SVM_MEMCPY_COMMAND,           // apCLSVMMemcpyCommand
    OS_TOBJ_ID_CL_SVM_MEM_FILL_COMMAND,         // apCLSVMMemFillCommand
    OS_TOBJ_ID_CL_SVM_MAP_COMMAND,              // apCLSVMMapCommand
    OS_TOBJ_ID_CL_SVM_UNMAP_COMMAND,            // apCLSVMUnmapCommand

    // gDebugger Application events:
    OS_TOBJ_ID_GLOBAL_VARIABLE_CHANGED_EVENT,   // gdGDebuggerGlobalVariableChangedEvent
    OS_TOBJ_ID_UPDATE_UI_EVENT,                 // gdUpdateUIEvent

    // GRLicenseManger classes:
    OS_TOBJ_ID_LICENSE_PARAMETERS,          // lmLicenseParameters

    // The amount of transferable objects types:
    OS_AMOUNT_OF_TRANSFERABLE_OBJECT_TYPES
};


// Channel read / write operators:
OS_API osChannel& operator<<(osChannel& ipcChannel, osTransferableObjectType objType);
OS_API osChannel& operator>>(osChannel& ipcChannel, osTransferableObjectType& objType);


#endif  // __OSTRANSFERABLEOBJECTTYPE
