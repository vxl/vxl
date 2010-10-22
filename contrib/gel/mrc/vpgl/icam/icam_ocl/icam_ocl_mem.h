#ifndef icam_ocl_mem_h_
#define icam_ocl_mem_h_
//:
// \file
// \brief  A wrapper class for cl_mem buffers for OpenCL
// \author Gamze Tunali gtunali@brown.edu
// \date  October 20, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "icam_ocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>

#define MEM_SUCCESS 0
#define MEM_FAILURE 1

class icam_ocl_mem
{
 public:
  //: constructor that takes the context to start with
  icam_ocl_mem(const cl_context& context);

  //: creates the memory for buffer
  bool create_buffer(const cl_mem_flags& flags,  vcl_size_t size,  void *host_ptr);

  //: sets the buffer int the kernel as the idxth argument
  bool set_kernel_arg(const cl_kernel& kernel, cl_uint idx);

  //: releases buffer memory
  bool release_memory();

  //: returns a reference to the buffer
  cl_mem& buffer() { return buffer_; }

 private:
  cl_mem buffer_;
  const cl_context& context_;

  int check_val(cl_int status, cl_int result, vcl_string message);
};

#endif
