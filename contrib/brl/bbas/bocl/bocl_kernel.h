#ifndef bocl_kernel_h_
#define bocl_kernel_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstddef.h> // for std::size_t
#include "bocl_cl.h"
#include "bocl_buffer.h"

#include "bocl_utils.h"

class bocl_kernel
{
 public:
  bocl_kernel(){}

  ~bocl_kernel();

  bool create_kernel(cl_program const& program, vcl_string const& kernel_name, cl_int& status);

  //: sets the buffer int the kernel as the idx'th argument
  bool set_arg(int arg_id, vcl_string name);

  bool set_local_arg(int arg_id, vcl_size_t arg_size);

  bool set_args();

  void release_kernel(cl_int& status);

  cl_kernel& kernel() { return kernel_; }

 private:
  cl_kernel kernel_;
};

#endif
