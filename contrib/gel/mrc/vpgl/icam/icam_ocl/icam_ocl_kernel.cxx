#include "icam_ocl_kernel.h"
#include <vcl_iostream.h>


bool icam_ocl_kernel::create_kernel(cl_program const& program, vcl_string const& kernel_name, cl_int& status)
{
  status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_ = clCreateKernel(program,kernel_name.c_str(),&status);

  if (!check_val(status,CL_SUCCESS,error_to_string(status)))
    return SDK_FAILURE;
  return  SDK_SUCCESS;
}

icam_ocl_kernel::~icam_ocl_kernel()
{
  cl_int status;
  release_kernel(status);
}

void icam_ocl_kernel::release_kernel(cl_int& status)
{
  status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;

  // delete all the buffers
  for (unsigned int i=0; i<buffers_.size(); i++)
    delete buffers_[i];
  buffers_.clear();

  if (!check_val(status,CL_SUCCESS,"clReleaseKernel failed."))
    status =  SDK_FAILURE;
  else
    status = SDK_SUCCESS;
}

bool icam_ocl_kernel::create_in_buffers(const cl_context& context,
                                     vcl_vector<void*> data, 
                                     vcl_vector<unsigned> sizes)
{
  if (data.size() != sizes.size())
    return SDK_FAILURE;

  // add the new ones at the next available place
  for (unsigned i=0; i<data.size(); i++) {
    icam_ocl_mem* buf = new icam_ocl_mem(context);
    if (!buf->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizes[i], data[i]))
      return SDK_FAILURE;
    buffers_.push_back(buf);
  }
  return SDK_SUCCESS;
}

bool icam_ocl_kernel::create_out_buffers(const cl_context& context,
                                     vcl_vector<void*> data, 
                                     vcl_vector<unsigned> sizes)
{
  if (data.size() != sizes.size())
    return SDK_FAILURE;
  for (unsigned i=0; i<data.size(); i++) {
    icam_ocl_mem* buf = new icam_ocl_mem(context);
    if (!buf->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizes[i], data[i]))
      return SDK_FAILURE;
    buffers_.push_back(buf);
  }
  return SDK_SUCCESS;
}

bool icam_ocl_kernel::set_local_arg(int arg_id, size_t arg_size)
{
  //  set local variable
  cl_int status = clSetKernelArg(kernel_,arg_id,arg_size,0);
  if(status!=SDK_SUCCESS){
	  vcl_cout << "Set local kernel arg failed\n";
	  return SDK_FAILURE;
  }
  return SDK_SUCCESS;
}

bool icam_ocl_kernel::enqueue_write_buffer(const cl_command_queue& queue, int idx, 
                                    cl_bool block_write,
                                    size_t  offset          /* offset */,
                                    size_t  cnb             /* cb */,
                                    const void*  data       /* ptr */,
                                    cl_uint num_events      /* num_events_in_wait_list */,
                                    const cl_event *  ev1   /* event_wait_list */,
                                    cl_event *   ev2)
{
  cl_int status = clEnqueueWriteBuffer(queue,buffers_[idx]->buffer(),block_write,
                                       offset,cnb,data,num_events,ev1,ev2);

  if (!check_val(status,CL_SUCCESS,"clCreateBuffer ( rotation ) failed."))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

bool icam_ocl_kernel::enqueue_read_buffer(const cl_command_queue& queue, int idx, cl_bool block_read,
                           size_t  offset,size_t  cnb, void*  data, cl_uint num_events,
                           const cl_event* ev1, cl_event *ev2)
{
  cl_int status = clEnqueueReadBuffer(queue,buffers_[idx]->buffer(),block_read,
    offset,cnb,data,num_events,ev1,ev2);

  if (!check_val(status,CL_SUCCESS,"clCreateBuffer ( rotation ) failed."))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

bool icam_ocl_kernel::release_buffers()
{
  for (unsigned i=0; i<buffers_.size(); i++) 
    if (!buffers_[i]->release_memory())    
      return SDK_FAILURE;
  return SDK_SUCCESS;
}

bool icam_ocl_kernel::set_args()
{
  for (unsigned i=0; i<buffers_.size(); i++) {
    if (!buffers_[i]->set_kernel_arg(kernel_,i)) 
      return SDK_FAILURE;
  }
  return SDK_SUCCESS;
}
