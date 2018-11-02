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

#include <string>
#include <iostream>
#include <cstddef>
#include "bocl_cl.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define MEM_SUCCESS 1
#define MEM_FAILURE 0

class bocl_buffer
{
 public:
  //: constructor that takes the context to start with
  bocl_buffer(const cl_context& context);

  //: creates the memory for buffer
  bool create_buffer(const cl_mem_flags& flags,  std::size_t size,  void *host_ptr);

  bool create_image2D(const cl_mem_flags& flags, const cl_image_format *format,
                      std::size_t width, std::size_t height, std::size_t row_pitch, void *host_ptr);
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

  int check_val(cl_int status, cl_int result, std::string message);
};

#endif
