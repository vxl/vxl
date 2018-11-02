#include <iostream>
#include "bocl_buffer.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bocl_buffer::bocl_buffer(const cl_context& context)
: context_(context)
{
}

bool bocl_buffer::create_buffer(const cl_mem_flags& flags,  std::size_t size,  void *host_ptr)
{
  cl_int status = MEM_FAILURE;
  // Create and initialize memory objects
  buffer_ = clCreateBuffer(this->context_,flags, size, host_ptr, &status);
  if (!check_val(status, MEM_FAILURE, "clCreateBuffer (input array) failed."))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_buffer::create_image2D(const cl_mem_flags& flags, const cl_image_format *format,
                                 std::size_t width, std::size_t height, std::size_t row_pitch, void *host_ptr)
{
  cl_int status = MEM_FAILURE;
  // Create and initialize memory objects
  buffer_ = clCreateImage2D(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, format,
                            width, height, row_pitch, host_ptr, &status);
 if (!check_val(status, MEM_FAILURE, "clCreateBuffer (input array) failed."))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

bool bocl_buffer::release_memory()
{
  cl_int status = clReleaseMemObject(buffer_);
  if (!check_val(status,MEM_FAILURE,"clReleaseMemObject (array_buf_) failed."))
    return MEM_FAILURE;
  return MEM_SUCCESS;
}

// Check for error returns
int bocl_buffer::check_val(cl_int status, cl_int result, std::string message)
{
  if (status != result) {
    std::cout << message << '\n';
    return 0;
  }
  return 1;
}
