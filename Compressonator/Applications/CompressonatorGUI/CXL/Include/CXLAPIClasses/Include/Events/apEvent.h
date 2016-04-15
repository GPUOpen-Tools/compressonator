//==================================================================================
// Copyright (c) 2016 , Advanced Micro Devices, Inc.  All rights reserved.
//
/// \author AMD Developer Tools Team
/// \file apEvent.h 
/// 
//==================================================================================

//------------------------------ apEvent.h ------------------------------

#ifndef __APEVENT
#define __APEVENT

// Infra:
#include <CXLOSWrappers/Include/osOSDefinitions.h>
#include <CXLOSWrappers/Include/osTransferableObject.h>

// Local:
#include <CXLAPIClasses/Include/apAPIClassesDLLBuild.h>


// ----------------------------------------------------------------------------------
// Class Name:           apEvent : public osTransferableObject
// General Description:
//   Base class for all event types.
//   Events can be triggered and observed.
//
// Author:  AMD Developer Tools Team
// Creation Date:        30/3/2004
// ----------------------------------------------------------------------------------
class AP_API apEvent : public osTransferableObject
{
public:
    enum EventType
    {
        AP_EVENT_UNKNOWN,
        // GRProcessDebugger events:
        AP_API_CONNECTION_ESTABLISHED,
        AP_API_CONNECTION_ENDED,
        AP_COMPUTE_CONTEXT_CREATED_EVENT,
        AP_COMPUTE_CONTEXT_DELETED_EVENT,
        AP_BEFORE_DEBUGGED_PROCESS_RUN_RESUMED,
        AP_BREAKPOINT_HIT,
        AP_DEBUGGED_PROCESS_OUTPUT_STRING,
        AP_DETECTED_ERROR_EVENT,
        AP_EXCEPTION,
        AP_GDB_ERROR,
        AP_GDB_LISTENER_THREAD_WAS_SUSPENDED_EVENT,
        AP_GDB_OUTPUT_STRING,
        AP_INFRASTRUCTURE_ENDS_BEING_BUSY_EVENT,
        AP_INFRASTRUCTURE_FAILURE_EVENT,
        AP_INFRASTRUCTURE_STARTS_BEING_BUSY_EVENT,
        AP_MEMORY_LEAK,
        AP_MODULE_LOADED,
        AP_MODULE_UNLOADED,
        AP_OUTPUT_DEBUG_STRING,
        AP_USER_WARNING,
        AP_DEBUGGED_PROCESS_CREATED,
        AP_DEBUGGED_PROCESS_RUN_RESUMED,
        AP_DEBUGGED_PROCESS_RUN_STARTED,
        AP_DEBUGGED_PROCESS_RUN_STARTED_EXTERNALLY,
        AP_DEBUGGED_PROCESS_RUN_SUSPENDED,
        AP_DEBUGGED_PROCESS_TERMINATED,
        AP_DEBUGGED_PROCESS_IS_DURING_TERMINATION,
        AP_DEBUGGED_PROCESS_CREATION_FAILURE,
        AP_RENDER_CONTEXT_CREATED_EVENT,
        AP_RENDER_CONTEXT_DELETED_EVENT,
        AP_CONTEXT_UPDATED_EVENT,
        AP_GL_DEBUG_OUTPUT_MESSAGE,
        AP_SEARCHING_FOR_MEMORY_LEAKS,
        AP_THREAD_CREATED,
        AP_THREAD_TERMINATED,
        AP_OPENCL_ERROR,
        AP_OPENCL_QUEUE_CREATED_EVENT,
        AP_OPENCL_QUEUE_DELETED_EVENT,
        AP_OPENCL_PROGRAM_CREATED_EVENT,
        AP_OPENCL_PROGRAM_DELETED_EVENT,
        AP_SPY_PROGRESS_EVENT,
        AP_TECHNOLOGY_MONITOR_FAILURE_EVENT,
        GD_MONITORED_OBJECT_SELECTED_EVENT,
        GD_MONITORED_OBJECT_ACTIVATED_EVENT,
        AP_BEFORE_KERNEL_DEBUGGING_EVENT,
        AP_AFTER_KERNEL_DEBUGGING_EVENT,
        AP_KERNEL_CURRENT_WORK_ITEM_CHANGED_EVENT,
        AP_KERNEL_DEBUGGING_FAILED_EVENT,
        AP_KERNEL_DEBUGGING_INTERRUPTED_EVENT,
        AP_FLUSH_TEXTURE_IMAGES_EVENT,
        AP_BREAKPOINTS_UPDATED_EVENT,
        AP_KERNEL_SOURCE_BREAKPOINTS_UPDATED_EVENT,
        AP_OPENCL_PROGRAM_BUILD_EVENT,
        AP_OPENCL_PROGRAM_BUILD_FAILED_WITH_DEBUG_FLAGS_EVENT,
        AP_EXECUTION_MODE_CHANGED_EVENT,
        AP_CALL_STACK_FRAME_SELECTED_EVENT,
        AP_DEFERRED_COMMAND_EVENT,
        AP_ADD_WATCH_EVENT,
        AP_HEX_CHANGED_EVENT,
        AP_MDI_CREATED_EVENT,
        AP_MDI_ACTIVATED_EVENT,
        AP_MEMORY_ALLOCATION_FAILURE_EVENT,

        // gDebugger Application events:
        APP_GLOBAL_VARIABLE_CHANGED,
        APP_UPDATE_UI_EVENT,

        // CPU Profiler events:
        AP_PROFILE_PROCESS_TERMINATED,
        AP_PROFILE_PROGRESS_EVENT,

        AP_NUMBER_OF_EVENT_TYPES
    };


    apEvent(osThreadId triggeringThreadId = OS_NO_THREAD_ID) { _triggeringThreadId = triggeringThreadId; };
    virtual ~apEvent();

    // Overrides osTransferableObject:
    virtual osTransferableObjectType type() const = 0;
    virtual bool writeSelfIntoChannel(osChannel& ipcChannel) const = 0;
    virtual bool readSelfFromChannel(osChannel& ipcChannel) = 0;
    virtual bool isEventObject() const;

    void setTriggeringThreadId(osThreadId triggeringThreadId) { _triggeringThreadId = triggeringThreadId; };
    osThreadId triggeringThreadId() const { return _triggeringThreadId; };

    // Must be implemented by sub-classes:
    virtual EventType eventType() const = 0;
    virtual apEvent* clone() const = 0;

private:
    // The id of the debugged process thread that triggered the event:
    osThreadId _triggeringThreadId;
};


#endif  // __APEVENT
