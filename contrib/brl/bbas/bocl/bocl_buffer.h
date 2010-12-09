#ifndef bocl_buffer_h_
#define bocl_buffer_h_
//:
// \file
// \brief  A wrapper class for cl_mem buffers for OpenCL
// \author Gamze Tunali gtunali@brown.edu
// \date   October 20, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "bocl_cl.h"
#include <vcl_string.h>
#include <vcl_cstddef.h>

#define MEM_SUCCESS 1
#define MEM_FAILURE 0

class bocl_buffer
{
 public:
  //: constructor that takes the context to start with
  bocl_buffer(const cl_context& context);

  //: creates the memory for buffer
  bool create_buffer(const cl_mem_flags& flags,  vcl_size_t size,  void *host_ptr);

  bool create_image2D(const cl_mem_flags& flags, const cl_image_format *format,
                      vcl_size_t width, vcl_size_t height, vcl_size_t row_pitch, void *host_ptr);
#if 0
  bool set_mem(cl_mem* buffer) { buffer_=&buffer; return true; }
#endif
  //: releases buffer memory
  bool release_memory();

  //: returns a reference to the buffer
  cl_mem& mem() { return buffer_; }

 private:
  cl_mem buffer_;
  const cl_context& context_;

  int check_val(cl_int status, cl_int result, vcl_string message);
};

#endif
