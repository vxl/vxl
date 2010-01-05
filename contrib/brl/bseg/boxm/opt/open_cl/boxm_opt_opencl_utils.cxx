
#include "boxm_opt_opencl_utils.h"

vcl_size_t RoundUp(int global_size,int group_size) 
{
    int r = global_size % group_size;
    if(r == 0) 
    {
        return global_size;
    } else 
    {
        return global_size + group_size - r;
    }
}
vcl_string error_to_string(cl_int  status )
{
    vcl_string output="";
 
    if( status ==0)output="CL_SUCCESS";
    else if( status  == CL_DEVICE_NOT_FOUND)  output="CL_DEVICE_NOT_FOUND"; 
    else if( status  == CL_DEVICE_NOT_AVAILABLE)  output="CL_DEVICE_NOT_AVAILABLE"; 
    else if( status  == CL_COMPILER_NOT_AVAILABLE)  output="CL_COMPILER_NOT_AVAILABLE"; 
    else if( status  == CL_MEM_OBJECT_ALLOCATION_FAILURE)  output="CL_MEM_OBJECT_ALLOCATION_FAILURE"; 
    else if( status  == CL_OUT_OF_RESOURCES)  output="CL_OUT_OF_RESOURCES"; 
    else if( status  == CL_OUT_OF_HOST_MEMORY)  output="CL_OUT_OF_HOST_MEMORY"; 
    else if( status  == CL_PROFILING_INFO_NOT_AVAILABLE)  output="CL_PROFILING_INFO_NOT_AVAILABLE"; 
    else if( status  == CL_MEM_COPY_OVERLAP)  output="CL_MEM_COPY_OVERLAP"; 
    else if( status  == CL_IMAGE_FORMAT_MISMATCH)  output="CL_IMAGE_FORMAT_MISMATCH"; 
    else if( status  == CL_IMAGE_FORMAT_NOT_SUPPORTED) output="CL_IMAGE_FORMAT_NOT_SUPPORTED"; 
    else if( status  == CL_BUILD_PROGRAM_FAILURE) output="CL_BUILD_PROGRAM_FAILURE"; 
    else if( status  == CL_MAP_FAILURE) output="CL_MAP_FAILURE"; 
    else if( status  == CL_INVALID_VALUE) output="CL_INVALID_VALUE"; 
    else if( status  == CL_INVALID_DEVICE_TYPE) output="CL_INVALID_DEVICE_TYPE"; 
    else if( status  == CL_INVALID_PLATFORM) output="CL_INVALID_PLATFORM"; 
    else if( status  == CL_INVALID_DEVICE) output="CL_INVALID_DEVICE"; 
    else if( status  == CL_INVALID_CONTEXT) output="CL_INVALID_CONTEXT"; 
    else if( status  == CL_INVALID_QUEUE_PROPERTIES) output="CL_INVALID_QUEUE_PROPERTIES"; 
    else if( status  == CL_INVALID_COMMAND_QUEUE) output="CL_INVALID_COMMAND_QUEUE"; 
    else if( status  == CL_INVALID_HOST_PTR) output="CL_INVALID_HOST_PTR"; 
    else if( status  == CL_INVALID_MEM_OBJECT) output="CL_INVALID_MEM_OBJECT"; 
    else if( status  == CL_INVALID_IMAGE_FORMAT_DESCRIPTOR) output="CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"; 
    else if( status  == CL_INVALID_IMAGE_SIZE) output="CL_INVALID_IMAGE_SIZE"; 
    else if( status  == CL_INVALID_SAMPLER) output="CL_INVALID_SAMPLER"; 
    else if( status  == CL_INVALID_BINARY) output="CL_INVALID_BINARY"; 
    else if( status  == CL_INVALID_BUILD_OPTIONS) output="CL_INVALID_BUILD_OPTIONS"; 
    else if( status  == CL_INVALID_PROGRAM) output="CL_INVALID_PROGRAM"              ; 
    else if( status  == CL_INVALID_PROGRAM_EXECUTABLE) output="CL_INVALID_PROGRAM_EXECUTABLE"   ; 
    else if( status  == CL_INVALID_KERNEL_NAME) output="CL_INVALID_KERNEL_NAME"          ; 
    else if( status  == CL_INVALID_KERNEL_DEFINITION) output="CL_INVALID_KERNEL_DEFINITION"    ; 
    else if( status  == CL_INVALID_KERNEL) output="CL_INVALID_KERNEL"               ; 
    else if( status  == CL_INVALID_ARG_INDEX) output="CL_INVALID_ARG_INDEX"            ; 
    else if( status  == CL_INVALID_ARG_VALUE) output="CL_INVALID_ARG_VALUE"            ; 
    else if( status  == CL_INVALID_ARG_SIZE) output="CL_INVALID_ARG_SIZE"             ; 
    else if( status  == CL_INVALID_KERNEL_ARGS) output="CL_INVALID_KERNEL_ARGS"          ; 
    else if( status  == CL_INVALID_WORK_DIMENSION) output="CL_INVALID_WORK_DIMENSION"       ; 
    else if( status  == CL_INVALID_WORK_GROUP_SIZE) output="CL_INVALID_WORK_GROUP_SIZE"      ; 
    else if( status  == CL_INVALID_WORK_ITEM_SIZE) output="CL_INVALID_WORK_ITEM_SIZE"       ; 
    else if( status  == CL_INVALID_GLOBAL_OFFSET) output="CL_INVALID_GLOBAL_OFFSET"        ; 
    else if( status  == CL_INVALID_EVENT_WAIT_LIST) output="CL_INVALID_EVENT_WAIT_LIST"      ; 
    else if( status  == CL_INVALID_EVENT) output="CL_INVALID_EVENT"                ; 
    else if( status  == CL_INVALID_OPERATION) output="CL_INVALID_OPERATION"            ; 
    else if( status  == CL_INVALID_GL_OBJECT) output="CL_INVALID_GL_OBJECT"            ; 
    else if( status  == CL_INVALID_BUFFER_SIZE) output="CL_INVALID_BUFFER_SIZE"          ; 
    else if( status  == CL_INVALID_MIP_LEVEL) output="CL_INVALID_MIP_LEVEL"            ; 
    else output="Unknown Error";

    return output;
}
