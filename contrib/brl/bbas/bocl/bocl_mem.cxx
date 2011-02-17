#include "bocl_mem.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_cstring.h>

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

  //if memory was allocated and a null pointer was passed in, store it
  if (flags & CL_MEM_ALLOC_HOST_PTR && cpu_buf_ == NULL) {
    status = clGetMemObjectInfo (buffer_, CL_MEM_HOST_PTR, sizeof(void*), cpu_buf_, NULL);
    if (!check_val(status, MEM_FAILURE, "clGetMemObjectInfo CL_MEM_HOST_PTR failed: " + this->id_))
      return MEM_FAILURE;
  }
  return MEM_SUCCESS;
}

bool bocl_mem::create_image_buffer(const cl_mem_flags& flags, const cl_image_format* format,
                                   vcl_size_t width, vcl_size_t height)
{
  cl_int status = MEM_FAILURE;
  buffer_ = clCreateImage2D(this->context_, flags, format, width, height,
                            this->num_bytes_, this->cpu_buf_, &status);
  if (!check_val(status, MEM_FAILURE, "clCreateBuffer failed: " + this->id_))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_mem::release_memory()
{
  //release mem
  cl_int status = clReleaseMemObject(buffer_);
  if (!check_val(status,MEM_FAILURE,"clReleaseMemObject failed: " + this->id_))
    return MEM_FAILURE;

  return MEM_SUCCESS;
}

//: helper method to zero out gpu buffer
bool bocl_mem::zero_gpu_buffer(const cl_command_queue& cmdQueue)
{
  unsigned char* zeros = new unsigned char[this->num_bytes_]; // All 1000 values initialized to zero.
  vcl_memset(zeros, 0, this->num_bytes_);
  ceEvent_ = 0;
  cl_int status = MEM_FAILURE;
  status = clEnqueueWriteBuffer(cmdQueue,
                                this->buffer_,
                                CL_TRUE,          //True=BLocking, False=NonBlocking
                                0,
                                this->num_bytes_,
                                zeros,
                                0,                //cl_uint num_events_in_wait_list
                                0,
                                &ceEvent_);
  delete[] zeros;
  if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer (ZERO BUFFER) failed: " + this->id_ + error_to_string(status)))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

//: write to command queue
bool bocl_mem::write_to_buffer(const cl_command_queue& cmdQueue)
{
  if (!is_gl_) {
    ceEvent_ = 0;
    cl_int status = MEM_FAILURE;
    status = clEnqueueWriteBuffer(cmdQueue,
                                  this->buffer_,
                                  CL_TRUE,          //True=BLocking, False=NonBlocking
                                  0,
                                  this->num_bytes_,
                                  this->cpu_buf_,
                                  0,                //cl_uint num_events_in_wait_list
                                  0,
                                  &ceEvent_);
    if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer failed: " + this->id_ + error_to_string(status)))
      return MEM_FAILURE;
    return MEM_SUCCESS;
  }
  return true;
}

//: read from command queue buffer...
bool bocl_mem::read_to_buffer(const cl_command_queue& cmdQueue)
{
  if (!is_gl_) {
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
                                &ceEvent_);
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
                                  &ceEvent_);
    if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer (async) failed: " + this->id_))
      return MEM_FAILURE;
    return MEM_SUCCESS;
  }
  return true;
}

//: finish write to buffer using clWaitForEvent
bool bocl_mem::finish_write_to_buffer(const cl_command_queue& cmdQueue)
{
  if (!is_gl_) {
    cl_int status = MEM_FAILURE;
    status = clWaitForEvents(1, &event_);
    if (!check_val(status,MEM_FAILURE,"clWaitForEvents failed: " + this->id_))
      return MEM_FAILURE;
    return MEM_SUCCESS;
  }
  return true;
}

//: THIS REQUIRES the queue to be finished
float bocl_mem::exec_time()
{
  cl_ulong tend, tstart;
  int status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  if ( !check_val(status,CL_SUCCESS,"clFinish/ProfilingInfo failed (" + id_ + ") " +error_to_string(status)) )
    return false;

  //store execution time
  return 1e-6f*float(tend - tstart);
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

