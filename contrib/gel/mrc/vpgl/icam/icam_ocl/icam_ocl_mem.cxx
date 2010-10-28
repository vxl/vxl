#include "icam_ocl_mem.h"
#include <vcl_iostream.h>

icam_ocl_mem::icam_ocl_mem(const cl_context& context)
: context_(context)
{
}

bool icam_ocl_mem::create_buffer(const cl_mem_flags& flags,  vcl_size_t size,  void *host_ptr)
{
  cl_int status = MEM_FAILURE;
  // Create and initialize memory objects
  buffer_ = clCreateBuffer(this->context_,flags, size, host_ptr, &status);
  if (!check_val(status, MEM_FAILURE, "clCreateBuffer (input array) failed."))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool icam_ocl_mem::set_kernel_arg(const cl_kernel& kernel, cl_uint idx)
{
  cl_int status = clSetKernelArg(kernel,idx,sizeof(cl_mem),(void *)&buffer_);
  if (!check_val(status,MEM_FAILURE,"clSetKernelArg failed. (length of array)"))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool icam_ocl_mem::release_memory()
{
  cl_int status = clReleaseMemObject(buffer_);
  if (!check_val(status,MEM_FAILURE,"clReleaseMemObject (array_buf_) failed."))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

// Check for error returns
int icam_ocl_mem::check_val(cl_int status, cl_int result, std::string message)
{
  if (status != result) {
    vcl_cout << message << '\n';
    return 0;
  }
  return 1;
}
