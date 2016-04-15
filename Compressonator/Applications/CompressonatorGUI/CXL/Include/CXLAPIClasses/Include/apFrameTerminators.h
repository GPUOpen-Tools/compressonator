//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file apFrameTerminators.h 
/// 
//==================================================================================

//------------------------------ apFrameTerminators.h ------------------------------

#ifndef __APFRAMETERMINATORS
#define __APFRAMETERMINATORS

// Enumeration containing functions that may be OpenGL render frame terminators.
// (Commands that end a rendered frame).
enum apFrameTerminators
{
    AP_SWAP_BUFFERS_TERMINATOR          = 0x00000001,   // Win32 / egl /Linux SwapBuffers functions.
    AP_GL_FLUSH_TERMINATOR              = 0x00000002,   // OpenGL glFlush function.
    AP_GL_FINISH_TERMINATOR             = 0x00000004,   // OpenGL glFinish function.
    AP_SWAP_LAYER_BUFFERS_TERMINATOR    = 0x00000008,   // win32 wglSwapLayerBuffers function.
    AP_MAKE_CURRENT_TERMINATOR          = 0x00000010,   // Win32 / egl / Linux MakeCurrent (wglMakeCurrent / glXMakeCurrent & glXMakeContextCurrent / eglMakeCurrent) function.
    AP_GL_CLEAR_TERMINATOR              = 0x00000020,   // OpenGL glClear function.
    AP_GL_FRAME_TERMINATOR_GREMEDY      = 0x00000040,   // OpenGL GREMEDY Extension glFrameTerminatorGREMEDY function.

    AP_ALL_GL_FRAME_TERMINATORS         = 0x0000FFFF,   // All OpenGL frame terminators are selected.

    AP_CL_GREMEDY_COMPUTATION_FRAME_TERMINATORS = 0x00010000,   // Pairs of clBeginComputationFrameGREMEDY and clEndComputationFrameGREMEDY functions.
    AP_CL_FLUSH_TERMINATOR                      = 0x00020000,   // OpenCL clFlush function.
    AP_CL_FINISH_TERMINATOR                     = 0x00040000,   // OpenCL clFinish function.
    AP_CL_WAIT_FOR_EVENTS_TERMINATOR            = 0x00080000,   // OpenCL clWaitForEvents function.

    AP_ALL_CL_FRAME_TERMINATORS                 = 0xFFFF0000,   // All OpenCL frame terminators are selected.
};

#define AP_DEFAULT_GL_FRAME_TERMINATOR AP_SWAP_BUFFERS_TERMINATOR
#define AP_DEFAULT_CL_FRAME_TERMINATOR AP_CL_FLUSH_TERMINATOR
#define AP_DEFAULT_FRAME_TERMINATORS (AP_DEFAULT_GL_FRAME_TERMINATOR | AP_DEFAULT_CL_FRAME_TERMINATOR)

#endif  // __APFRAMETERMINATORS
