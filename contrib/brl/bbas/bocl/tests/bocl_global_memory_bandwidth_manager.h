#ifndef bocl_global_memory_bandwidth_manager_h_
#define bocl_global_memory_bandwidth_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_kernel.h>

class bocl_global_memory_bandwidth_manager : public bocl_manager<bocl_global_memory_bandwidth_manager>
{
 public:

  bocl_global_memory_bandwidth_manager()
    : program_(0),time_in_secs_(0.0f) {}

  ~bocl_global_memory_bandwidth_manager();

  unsigned array_size() const { return len_; }
  bocl_mem* array_buf() { return array_buf_; }

  vcl_string program_source() const { return prog_; }
  cl_program program() { return program_; }

  bool setup_array(unsigned len);
  void clean_array();
  bool setup_result_array();
  void clean_result_array();
  void create_buffers();
  void release_buffers();
  bool run_kernel();

  bool run_kernel_using_image();

  //: diff from run_kernel allocated array of float4 in local memory
  bool run_kernel_prefetch();

  int create_kernel(vcl_string const& kernel_name, vcl_string src_path, vcl_string options);

  float time_taken() const  { return time_in_secs_; }
  bocl_kernel kernel()      { return kernel_; }

  cl_int * result_flag()    { return result_flag_; }
  cl_float * result_array() { return result_array_; }

 protected:

  cl_program program_;

  cl_command_queue command_queue_;
  bocl_kernel kernel_;
  cl_float* array_;
  cl_float* result_array_;
  cl_uint * cl_len_;
  cl_int * result_flag_;
  bocl_mem* array_buf_;
  bocl_mem* result_array_buf_;
  bocl_mem* cl_len_buf_;
  bocl_mem* result_flag_buf_;
  float time_in_secs_;
  cl_image_format inputformat;
  unsigned len_;
};

#endif // bocl_global_memory_bandwidth_manager_h_
