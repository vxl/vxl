#ifndef icam_ocl_kernel_h_
#define icam_ocl_kernel_h_

#include <vcl_string.h>
#include <vcl_vector.h>
#include "icam_ocl_cl.h"
#include "icam_ocl_mem.h"

#include "icam_ocl_utils.h"

class icam_ocl_kernel 
{
public:
  icam_ocl_kernel(){}

  ~icam_ocl_kernel();

  bool create_kernel(cl_program const& program, vcl_string const& kernel_name, cl_int& status);

  bool create_in_buffers(const cl_context& context, vcl_vector<void*> data, vcl_vector<unsigned> sizes);

  bool create_out_buffers(const cl_context& context, vcl_vector<void*> data, vcl_vector<unsigned> sizes);

  bool enqueue_write_buffer(const cl_command_queue& queue, int idx,  cl_bool block_write,
                            size_t  offset,size_t  cnb, const void*  data, cl_uint num_events,
                            const cl_event* ev1, cl_event* ev2);

  bool enqueue_read_buffer(const cl_command_queue& queue, int idx, cl_bool block_read,
                           size_t  offset,size_t  cnb, void*  data, cl_uint num_events,
                           const cl_event* ev1, cl_event *ev2);

  bool release_buffers();

  bool set_args();

  void release_kernel(cl_int& status);

  cl_kernel& kernel() { return kernel_; }

  int buffer_cnt() { return buffers_.size();}

private:
  cl_kernel kernel_;
  vcl_vector<icam_ocl_mem*> buffers_;
  
};

#endif

