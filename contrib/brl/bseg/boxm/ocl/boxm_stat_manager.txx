// This is brl/bseg/boxm/ocl/boxm_stat_manager.txx
#ifndef boxm_stat_manager_txx_
#define boxm_stat_manager_txx_
#include "boxm_stat_manager.h"

#include <vcl_fstream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <vul/vul_timer.h>
#include "boxm_ocl_utils.h"


template<class T>
void boxm_stat_manager<T>::clear_stat_input()
{
  if (stat_input_) {
    unsigned n = this->input_size();
    for (unsigned i = 0; i<n; ++i)
      stat_input_[i]=0.0f;
  }
}

template<class T>
bool boxm_stat_manager<T>::setup_stat_input()
{
#if defined (_WIN32)
  stat_input_ = (cl_float*)_aligned_malloc(this->input_size()* sizeof(cl_float), 16);
#elif defined(__APPLE__)
  stat_input_ = (cl_float*)malloc(this->input_size() * sizeof(cl_float));
#else
  stat_input_ = (cl_float*)memalign(16, this->input_size() * sizeof(cl_float));
#endif

  if (stat_input_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (stat_input)\n";
    return SDK_FAILURE;
  }
  this->clear_stat_input();

  return true;
}

template<class T>
bool boxm_stat_manager<T>::clean_stat_input()
{
  if (stat_input_)
  {
#ifdef _WIN32
    _aligned_free(stat_input_);
#else
    free(stat_input_);
#endif
    stat_input_ = NULL;
  }
  input_size_ = 0;
  return true;
}

template<class T>
void boxm_stat_manager<T>::clear_stat_data()
{
  if (stat_input_) {
    unsigned n = this->data_size();
    for (unsigned i = 0; i<n; ++i)
      stat_data_[i]=0.0f;
  }
}

template<class T>
bool boxm_stat_manager<T>::setup_stat_data(vcl_vector<float> const& data)
{
  data_size_ = data.size()+1;
#if defined (_WIN32)
  stat_data_ = (cl_float*)_aligned_malloc(this->data_size()* sizeof(cl_float), 16);
#elif defined(__APPLE__)
  stat_data_ = (cl_float*)malloc(this->data_size() * sizeof(cl_float));
#else
  stat_data_ = (cl_float*)memalign(16, this->data_size() * sizeof(cl_float));
#endif

  if (stat_data_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (stat_data)\n";
    return SDK_FAILURE;
  }
  this->clear_stat_data();
  stat_data_[0]=static_cast<float>(data.size());
  for (unsigned i = 1; i<this->data_size(); ++i)
    stat_data_[i] = data[i-1];
  return true;
}

template<class T>
bool boxm_stat_manager<T>::clean_stat_data()
{
  if (stat_data_)
  {
#ifdef _WIN32
    _aligned_free(stat_data_);
#else
    free(stat_data_);
#endif
    stat_data_ = NULL;
  }
  data_size_ = 0;
  return true;
}


template<class T>
void boxm_stat_manager<T>::clear_stat_results()
{
  if (stat_results_) {
    unsigned n = this->result_size();
    for (unsigned i = 0; i<n; ++i)
      stat_results_[i]=0.0f;
  }
}

template<class T>
bool boxm_stat_manager<T>::setup_stat_results()
{
#if defined (_WIN32)
  stat_results_ = (cl_float*)_aligned_malloc(this->result_size()* sizeof(cl_float), 16);
#elif defined(__APPLE__)
  stat_results_ = (cl_float*)malloc(this->result_size() * sizeof(cl_float));
#else
  stat_results_ = (cl_float*)memalign(16, this->result_size() * sizeof(cl_float));
#endif

  if (stat_results_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (stat_results)\n";
    return SDK_FAILURE;
  }
  this->clear_stat_results();

  return true;
}

template<class T>
bool boxm_stat_manager<T>::clean_stat_results()
{
  if (stat_results_)
  {
#ifdef _WIN32
    _aligned_free(stat_results_);
#else
    free(stat_results_);
#endif
    stat_results_ = NULL;
  }
  result_size_ = 0;
  return true;
}

template<class T>
int boxm_stat_manager<T>::setup_stat_input_buffer()
{
  cl_int status = CL_SUCCESS;
  stat_input_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, input_size_*sizeof(cl_float),stat_input_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (stat input) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_stat_manager<T>::clean_stat_input_buffer()
{
  cl_int status = clReleaseMemObject(stat_input_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (stat_input_buf_) failed."))
    return SDK_FAILURE;
  else {
    stat_input_buf_=0;
    return SDK_SUCCESS;
  }
}

template<class T>
int boxm_stat_manager<T>::setup_stat_data_buffer()
{
  cl_int status = CL_SUCCESS;
  stat_data_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, data_size_*sizeof(cl_float),stat_data_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (stat data) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_stat_manager<T>::clean_stat_data_buffer()
{
  cl_int status = clReleaseMemObject(stat_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (stat_data_buf_) failed."))
    return SDK_FAILURE;
  else {
    stat_data_buf_=0;
    return SDK_SUCCESS;
  }
}

template <class T>
int boxm_stat_manager<T>::setup_stat_results_buffer()
{
  cl_int   status;
  this->clear_stat_results();
  stat_results_buf_ = clCreateBuffer(this->context(),
                                     CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                                     result_size_* sizeof(cl_float),
                                     stat_results_,
                                     &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer failed. (stat_results)"))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_stat_manager<T>::clean_stat_results_buffer()
{
  cl_int status = clReleaseMemObject(stat_results_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (stat_results_buf_) failed."))
    return SDK_FAILURE;
  else {
    stat_results_buf_=0;
    return SDK_SUCCESS;
  }
}

template<class T>
int boxm_stat_manager<T>::build_kernel_program(bool useimage)
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

  program_ = clCreateProgramWithSource(this->context_,
                                       1,
                                       &source,
                                       sourceSize,
                                       &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  vcl_string options="";

  if (useimage)
    options+="-D USEIMAGE";

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_,
                          1,
                          this->devices_,
                          options.c_str(),
                          NULL,
                          NULL);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program_, this->devices_[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_stat_manager<T>::create_kernel(vcl_string const& kernel_name)
{
  cl_int status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_ = clCreateKernel(program_,kernel_name.c_str(),&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int boxm_stat_manager<T>::release_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseKernel failed."))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}


template<class T>
void boxm_stat_manager<T>::set_gauss_1d(float mean, float sigma)
{
  this->clean_stat_input();
  this->set_input_size(2);
  this->setup_stat_input();
  stat_input_[0]=mean;
  stat_input_[1]=sigma;
  this->setup_stat_input_buffer();
}

template<class T>
void boxm_stat_manager<T>::set_init_gauss_1d(float mean, float sigma,
                                             float min_sigma, float rho)
{
  this->clean_stat_input();
  this->set_input_size(4);
  this->setup_stat_input();
  stat_input_[0]=mean;
  stat_input_[1]=sigma;
  stat_input_[2]=min_sigma;
  stat_input_[3]=rho;
  this->setup_stat_input_buffer();
}

template<class T>
void boxm_stat_manager<T>::
set_gauss_3_mixture_1d(float mu0, float sigma0, float w0,
                       float mu1, float sigma1, float w1,
                       float mu2, float sigma2, float w2)
{
  this->clean_stat_input();
  this->set_input_size(9);
  this->setup_stat_input();
  stat_input_[0]=mu0;  stat_input_[1]=sigma0;  stat_input_[2]=w0;
  stat_input_[3]=mu1;  stat_input_[4]=sigma1;  stat_input_[5]=w1;
  stat_input_[6]=mu2;  stat_input_[7]=sigma2;  stat_input_[8]=w2;
  this->setup_stat_input_buffer();
}

template<class T>
void boxm_stat_manager<T>::
set_insert_gauss_3_mix_1d(float init_weight, float init_sigma,
                          float mu0, float sigma0, float w0,
                          float mu1, float sigma1, float w1,
                          float mu2, float sigma2, float w2 )
{
  this->clean_stat_input();
  this->set_input_size(11);
  this->setup_stat_input();
  stat_input_[0]=init_weight;   stat_input_[1]= init_sigma;
  stat_input_[2]=mu0;  stat_input_[3]=sigma0;  stat_input_[4]=w0;
  stat_input_[5]=mu1;  stat_input_[6]=sigma1;  stat_input_[7]=w1;
  stat_input_[8]=mu2;  stat_input_[9]=sigma2;  stat_input_[10]=w2;
  this->setup_stat_input_buffer();
}

template<class T>
void boxm_stat_manager<T>::
set_update_gauss_3_mix_1d(float weight, float init_sigma,
                          float min_sigma, float t_match,
                          float mu0, float sigma0, float w0,
                          float mu1, float sigma1, float w1,
                          float mu2, float sigma2, float w2)
{
  this->clean_stat_input();
  this->set_input_size(13);
  this->setup_stat_input();
  stat_input_[0]=weight;   stat_input_[1]= init_sigma;
  stat_input_[2]=min_sigma;   stat_input_[3]=t_match;
  stat_input_[4]=mu0;  stat_input_[5]=sigma0;  stat_input_[6]=w0;
  stat_input_[7]=mu1;  stat_input_[8]=sigma1;  stat_input_[9]=w1;
  stat_input_[10]=mu2;  stat_input_[11]=sigma2;  stat_input_[12]=w2;
  this->setup_stat_input_buffer();
}

template<class T>
void boxm_stat_manager<T>::print_results()
{
  for (unsigned i = 0; i<result_size_; ++i)
    vcl_cout << "r[" << i << "] " << (float)stat_results_[i] << '\n';
}


#define BOXM_STAT_MANAGER_INSTANTIATE(T) \
  template class boxm_stat_manager<T >

#endif
