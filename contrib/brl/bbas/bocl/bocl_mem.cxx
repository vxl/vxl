#include "bocl_mem.h"
//:
// \file
#include <vcl_iostream.h>

bocl_mem::bocl_mem(const cl_context& context, void* buffer, unsigned num_bytes, vcl_string id)
: cpu_buf_(buffer),
  num_bytes_(num_bytes),
  context_(context),
  id_(id),
  is_gl_(false)
{}

bocl_mem::~bocl_mem()
{
  this->release_memory();
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
bool bocl_mem::write_to_buffer(const cl_command_queue& cmdQueue)
{
  if (!is_gl_) {
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
  return true;
}

//: read from command queue buffer...
bool bocl_mem::read_to_buffer(const cl_command_queue& cmdQueue)
{
  if (!is_gl_) {
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
  return true;
}

//: write to command queue
bool bocl_mem::write_to_buffer_async(const cl_command_queue& cmdQueue)
{
  if (!is_gl_) {
    cl_int status = MEM_FAILURE;
    status = clEnqueueWriteBuffer(cmdQueue,
                                  this->buffer_,
                                  CL_FALSE,          //True=BLocking, False=NonBlocking
                                  0,
                                  this->num_bytes_,
                                  this->cpu_buf_,
                                  0,                //cl_uint num_events_in_wait_list
                                  0,
                                  &event_);
    if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer (async) failed: " + this->id_))
      return MEM_FAILURE;
    return MEM_SUCCESS;
  }
  return true;
}

//: finish write to buffer using clWaitForEvent
bool bocl_mem::finish_write_to_buffer(const cl_command_queue& cmdQueue)
{
    if(!is_gl_) {
      cl_int status = MEM_FAILURE; 
      status = clWaitForEvents(1, &event_); 
      if (!check_val(status,MEM_FAILURE,"clWaitForEvents failed: " + this->id_))
        return MEM_FAILURE;
      return MEM_SUCCESS;
    }
  
}

//---I/O------------------------------------------------------------------------
// Binary write boxm_update_bit_scene_manager scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, bocl_mem const& /*scene*/) {}
void vsl_b_write(vsl_b_ostream& /*os*/, const bocl_mem* & /*p*/) {}
void vsl_b_write(vsl_b_ostream& /*os*/, bocl_mem_sptr& /*sptr*/) {}
void vsl_b_write(vsl_b_ostream& /*os*/, bocl_mem_sptr const& /*sptr*/) {}

// Binary load boxm_update_bit_scene_manager scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, bocl_mem & /*scene*/) {}
void vsl_b_read(vsl_b_istream& /*is*/, bocl_mem* /*p*/) {}
void vsl_b_read(vsl_b_istream& /*is*/, bocl_mem_sptr& /*sptr*/) {}
void vsl_b_read(vsl_b_istream& /*is*/, bocl_mem_sptr const& /*sptr*/) {}

