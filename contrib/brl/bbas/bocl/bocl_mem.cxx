#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <utility>
#include "bocl_mem.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <bocl/bocl_kernel.h>

bocl_mem::bocl_mem(const cl_context& context, void* buffer, unsigned num_bytes, std::string id)
: cpu_buf_(buffer),
  delete_cpu_(false),
  queue_(nullptr),
  num_bytes_(num_bytes),
  context_(context),
  id_(std::move(id)),
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
  if (!check_val(status, MEM_FAILURE, "clCreateBuffer failed: " + this->id_  + ", with status: " + error_to_string(status)))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_mem::create_buffer(const cl_mem_flags& flags, cl_command_queue& queue)
{
  cl_int status = MEM_FAILURE;

  // Create and initialize memory objects
  buffer_ = clCreateBuffer(this->context_, flags, this->num_bytes_, this->cpu_buf_, &status );
  if (!check_val(status, MEM_FAILURE, "clCreateBuffer failed: " + this->id_ + ", with status: " + error_to_string(status)))
    return MEM_FAILURE;

  //if memory was allocated and a null pointer was passed in, store it
  if ( (flags & CL_MEM_ALLOC_HOST_PTR) && !cpu_buf_) {
#ifdef DEBUG
    std::cout<<"bocl_mem is allocating host pointer"<<std::endl;
#endif
    cpu_buf_ = clEnqueueMapBuffer(queue,
                                  buffer_,
                                  CL_TRUE,
                                  CL_MAP_READ & CL_MAP_WRITE,
                                  0,
                                  this->num_bytes_,
                                  0,nullptr, nullptr, &status);
    clFinish(queue);
    if (!check_val(status, MEM_FAILURE, "clEnqueueMapBuffer CL_MEM_HOST_PTR failed: " + this->id_))
      return MEM_FAILURE;

    //this buffer owns its CPU memory, make sure it gets deleted
    delete_cpu_ = true;
    queue_ = &queue;
  }
  return MEM_SUCCESS;
}

bool bocl_mem::create_image_buffer(const cl_mem_flags& flags, const cl_image_format* format,
                                   std::size_t width, std::size_t height)
{
  cl_int status = MEM_FAILURE;
  //
  // NOTE: clCreateImage2D has been replaced with clCreateImage as of v1.2
  // CMakeLists defines a switch based on OpenCL version present on system
  // Even better would be for FindOpenCL.cmake to automatically figure this out.
  // - D. Crispell 4/2013
  //
#if VXL_OPENCL_IS_PRE_V12 // use this call for versions prior to v1.2
  buffer_ = clCreateImage2D(this->context_, flags, format, width, height,
                            this->num_bytes_, this->cpu_buf_, &status);
#else // use this call for version v1.2 and later
  // create image description structure
  cl_image_desc image_desc;
  image_desc.image_type = CL_MEM_OBJECT_IMAGE2D;
  image_desc.image_width = width;
  image_desc.image_height = height;
  image_desc.image_row_pitch = 0; // force calculation of row pitch

  buffer_ = clCreateImage(this->context_, flags, format, &image_desc,
                          this->cpu_buf_, &status);
#endif
  if (!check_val(status, MEM_FAILURE, "clCreateImage failed: " + this->id_))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_mem::release_memory()
{
  //release mapped memory
  if (delete_cpu_ && cpu_buf_ && queue_) {
    cl_int status = clEnqueueUnmapMemObject (*queue_, buffer_, cpu_buf_, 0, nullptr, nullptr);
    if (!check_val(status,MEM_FAILURE,"clEnqueueUnmapMemObject failed: " + this->id_))
      return MEM_FAILURE;
  }

  //release mem
  cl_int status = clReleaseMemObject(buffer_);
  if (!check_val(status,MEM_FAILURE,"clReleaseMemObject failed: " + this->id_))
    return MEM_FAILURE;

  return MEM_SUCCESS;
}


//: helper method to initialize gpu buffer with a constant value
bool bocl_mem::init_gpu_buffer(void const* init_val, std::size_t value_size, cl_command_queue& cmd_queue)
{
  // sanity check on sizes
  if (this->num_bytes_ % value_size != 0) {
    std::cerr << "ERROR: bocl_mem::init_gpu_buffer(): value_size does not divide evenly into buffer size.\n";
    return MEM_FAILURE;
  }
  auto* init_buff = new unsigned char[this->num_bytes_];
  unsigned int num_values = this->num_bytes_ / value_size;
  std::cout << "value_size = " << value_size << std::endl
           << "num_values = " << num_values << std::endl;

  // fill in buffer with copies of init value
  unsigned char* buff_ptr = init_buff;
  for (unsigned int i=0; i<num_values; ++i) {
    std::memcpy(buff_ptr, init_val, value_size);
    buff_ptr += value_size;
  }
  // copy buffer over to GPU
  ceEvent_ = nullptr;
  cl_int status = MEM_FAILURE;
  status = clEnqueueWriteBuffer(cmd_queue,
                                this->buffer_,
                                CL_TRUE,          //True=BLocking, False=NonBlocking
                                0,
                                this->num_bytes_,
                                init_buff,
                                0,                //cl_uint num_events_in_wait_list
                                nullptr,
                                &ceEvent_);
  delete[] init_buff;
  if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer (INIT BUFFER) failed: " + this->id_ + error_to_string(status)))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

//: write to command queue
bool bocl_mem::write_to_buffer(const cl_command_queue& cmd_queue, bool blocking)
{
  if (!is_gl_)
    return this->write_to_gpu_mem(cmd_queue, this->cpu_buf_, this->num_bytes_, blocking);
  return true;
}

//: read from command queue buffer...
bool bocl_mem::read_to_buffer(const cl_command_queue& cmd_queue, bool blocking)
{
  if (!is_gl_)
    return this->read_from_gpu_mem(cmd_queue, this->cpu_buf_, this->num_bytes_, blocking);
  return true;
}

bool bocl_mem::write_to_gpu_mem(const cl_command_queue& cmd_queue, void* buff, std::size_t size, bool blocking)
{
  assert(size <= this->num_bytes_);
  ceEvent_ = nullptr;
  cl_int status = MEM_FAILURE;
  status = clEnqueueWriteBuffer(cmd_queue,
                                this->buffer_,
                                blocking,          //True=BLocking, False=NonBlocking
                                0,
                                size,
                                buff,
                                0,                //cl_uint num_events_in_wait_list
                                nullptr,
                                &ceEvent_);
  if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer failed: " + this->id_ + error_to_string(status)))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_mem::read_from_gpu_mem(const cl_command_queue& cmd_queue, void* buff, std::size_t size, bool blocking)
{
  assert(size <= this->num_bytes_);
  int status = MEM_FAILURE;
  status= clEnqueueReadBuffer(cmd_queue,
                              this->buffer_,
                              blocking,
                              0,
                              size,
                              buff,
                              0,
                              nullptr,
                              &ceEvent_);
  if (!check_val(status,MEM_FAILURE,"clEnqueueReadBuffer failed: " + this->id_ + error_to_string(status)))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

//: write to command queue
bool bocl_mem::write_to_buffer_async(const cl_command_queue& cmd_queue)
{
  if (!is_gl_) {
    cl_int status = MEM_FAILURE;
    status = clEnqueueWriteBuffer(cmd_queue,
                                  this->buffer_,
                                  CL_FALSE,          //True=BLocking, False=NonBlocking
                                  0,
                                  this->num_bytes_,
                                  this->cpu_buf_,
                                  0,                //cl_uint num_events_in_wait_list
                                  nullptr,
                                  &ceEvent_);
    if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer (async) failed: " + this->id_))
      return MEM_FAILURE;
    return MEM_SUCCESS;
  }
  return true;
}

//: finish write to buffer using clWaitForEvent
bool bocl_mem::finish_write_to_buffer(const cl_command_queue&  /*cmd_queue*/)
{
  if (!is_gl_) {
    cl_int status = MEM_FAILURE;
    status = clWaitForEvents(1, &event_);
    if (!check_val(status,MEM_FAILURE,"clWaitForEvents failed: " + this->id_ + error_to_string(status)))
      return MEM_FAILURE;
    return MEM_SUCCESS;
  }
  return true;
}

void* bocl_mem::enqueue_map(const cl_command_queue& cmd_queue)
{
  int status = MEM_FAILURE;
  void* ptr = clEnqueueMapBuffer(cmd_queue,this->buffer_,CL_TRUE,CL_MAP_READ,
                                 0,this->num_bytes_,0,nullptr,&ceEvent_,&status);
  if (!check_val(status,MEM_FAILURE,"clEnqueueMapBuffer failed: " + this->id_ + error_to_string(status)))
    return nullptr;
  return ptr;
}

bool bocl_mem::enqueue_unmap(const cl_command_queue& cmd_queue, void* mapped_ptr)
{
  int status = MEM_FAILURE;
  status = clEnqueueUnmapMemObject(cmd_queue,this->buffer_, mapped_ptr, 0, nullptr, &ceEvent_);
  if (!check_val(status,MEM_FAILURE,"clEnqueueUnmapMemObject failed: " + this->id_ + error_to_string(status)))
    return false;
  return true;
}


//: THIS REQUIRES the queue to be finished
float bocl_mem::exec_time()
{
  cl_ulong tend, tstart;
  int status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,nullptr);
  status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,nullptr);
  if ( !check_val(status,CL_SUCCESS,"clFinish/ProfilingInfo failed (" + id_ + ") " +error_to_string(status)) )
    return false;

  //store execution time
  return 1e-6f*float(tend - tstart);
}

//: declare static var (for linking)
std::map<std::string, bocl_kernel*> bocl_mem::set_kernels_;

//: Get set kernel
bocl_kernel* bocl_mem::get_set_kernel(cl_device_id dev_id, cl_context context, const std::string& type)
{
  std::string opts = " -D TYPE=" + type;

  // check to see if this device has compiled kernels already
  std::stringstream ss; ss<<opts<<'_'<<dev_id<<'_'<<context;
  std::string identifier = ss.str();
  if (set_kernels_.find(identifier) != set_kernels_.end())
    return set_kernels_[identifier];

  //if not, compile and cache them
  std::cout<<"compiling set kernel for device_id: "<<dev_id<<std::endl;

  // OpenCL source code
  std::string oclSrc =
       "__kernel void set(__global TYPE* buffer,"
       "                  __global TYPE* var,"
       "                  __global uint* len){"
       "      uint n = get_global_id(0);"
       "      if (n < *len)"
       "        buffer[n] = *var;"
       "}";

  //compilation options - default opts should be " -D TYPE float "
  auto* setKernel = new bocl_kernel();
  setKernel->create_kernel(context, &dev_id, oclSrc, "set", opts, "set "+type+" kernel");

  //cache in map
  set_kernels_[identifier] = setKernel;
  return set_kernels_[identifier];
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
