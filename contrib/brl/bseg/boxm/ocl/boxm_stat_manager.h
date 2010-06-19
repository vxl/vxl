#ifndef boxm_stat_manager_h_
#define boxm_stat_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <bocl/bocl_cl.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>
#include "boxm_ocl_utils.h"


template <class T_data>
class boxm_stat_manager : public bocl_manager<boxm_stat_manager<T_data> >
{
 public:
  boxm_stat_manager()
    : program_(0),
    stat_input_(0),
    stat_data_(0),
    stat_results_(0),
    input_size_(0),
    data_size_(0),
    result_size_(0)
    {}

  ~boxm_stat_manager(){  if (program_)
    clReleaseProgram(program_);}

  unsigned input_size(){return input_size_;}
  void set_input_size(unsigned isize){input_size_=isize;}

  unsigned data_size(){return data_size_;}
  void set_data_size(unsigned dsize){data_size_=dsize;}

  unsigned result_size(){return result_size_;}
  void set_result_size(unsigned rsize){result_size_=rsize;}


  cl_float* stat_results() {return stat_results_;}

  

  cl_mem& input_buf() {return stat_input_buf_;}
  cl_mem& data_buf() {return stat_data_buf_;}
  cl_mem& results_buf() {return stat_results_buf_;}

  void clear_stat_input();
  bool setup_stat_input();
  bool clean_stat_input();

  void clear_stat_data();
  bool setup_stat_data(vcl_vector<float> const& data);
  bool clean_stat_data();

  void clear_stat_results();
  bool setup_stat_results();
  bool clean_stat_results();

  cl_program program() {return program_;}

  int setup_stat_input_buffer();
  int clean_stat_input_buffer();

  int setup_stat_data_buffer();
  int clean_stat_data_buffer();

  int setup_stat_results_buffer();
  int clean_stat_results_buffer();


  int build_kernel_program(bool useimage=false);

  int create_kernel(vcl_string const& name);
  int release_kernel();

  cl_kernel kernel() {return kernel_;}

  void set_gauss_1d(float mean, float sigma);
  void set_init_gauss_1d(float mean, float sigma, float min_sigma, float rho);
  void set_gauss_3_mixture_1d(float mu0, float sigma0, float w0,
                              float mu1, float sigma1, float w1,
                              float mu2, float sigma2, float w2);
  void set_insert_gauss_3_mix_1d(float init_weight, float init_sigma,
                                 float mu0, float sigma0, float w0, 
                                 float mu1, float sigma1, float w1,
                                 float mu2, float sigma2, float w2);
void set_update_gauss_3_mix_1d(float weight, float init_sigma,
                               float min_sigma, float t_match,
                               float mu0, float sigma0, float w0, 
                               float mu1, float sigma1, float w1,
                               float mu2, float sigma2, float w2);
  void print_results();
 protected:
  cl_program program_;
  cl_command_queue command_queue_;
  cl_kernel kernel_;
  unsigned input_size_;
  unsigned data_size_;
  unsigned result_size_;
  cl_float* stat_input_;
  cl_float* stat_data_;
  cl_float* stat_results_;
  cl_mem   stat_input_buf_;
  cl_mem   stat_data_buf_;
  cl_mem   stat_results_buf_;
  cl_image_format inputformat;
};

#endif // boxm_stat_manager_h_
