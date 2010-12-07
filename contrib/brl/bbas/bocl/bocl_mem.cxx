#include "bocl_mem.h"
//:
// \file
#include <vcl_iostream.h>

bocl_mem::bocl_mem(const cl_context& context, void* buffer, unsigned num_bytes, vcl_string id)
: context_(context),
  cpu_buf_(buffer),
  num_bytes_(num_bytes),
  id_(id)
{
}

bool bocl_mem::create_buffer(const cl_mem_flags& flags)
{
  cl_int status = MEM_FAILURE;
  // Create and initialize memory objects
  buffer_ = clCreateBuffer(this->context_, flags, this->num_bytes_, this->cpu_buf_, &status);
  if (!check_val(status, MEM_FAILURE, "clCreateBuffer failed: " + this->id_))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_mem::release_memory()
{
  cl_int status = clReleaseMemObject(buffer_);
  if (!check_val(status,MEM_FAILURE,"clReleaseMemObject failed: " + this->id_))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

//: write to command queue
bool bocl_mem::write_to_buffer(const cl_command_queue cmdQueue)
{
  cl_int status = MEM_FAILURE;
  status = clEnqueueWriteBuffer(cmdQueue,
                                this->buffer_,
                                CL_TRUE,          //True=BLocking, False=NonBlocking
                                0,
                                this->num_bytes_,
                                this->cpu_buf_,
                                0,                //cl_uint num_events_in_wait_list
                                0,
                                0);
  if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer failed: " + this->id_))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

//: read from command queue buffer...
bool bocl_mem::read_to_buffer(const cl_command_queue cmdQueue)
{
  cl_event event;
  int status = MEM_FAILURE;
  // Enqueue readBuffers
  status= clEnqueueReadBuffer(cmdQueue,
                              this->buffer_,
                              CL_TRUE,
                              0,
                              this->num_bytes_,
                              this->cpu_buf_,
                              0,
                              NULL,
                              &event);
  if (!check_val(status,MEM_FAILURE,"clEnqueueReadBuffer failed: " + this->id_))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

