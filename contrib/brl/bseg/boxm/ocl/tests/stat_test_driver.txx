#ifndef stat_test_driver_txx_
#define stat_test_driver_txx_

#include "stat_test_driver.h"

#include <vcl_fstream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>

template <class T>
int
stat_test_driver<T>::setup_cl()
{
  // Create command queue
  cl_int status = CL_SUCCESS;

  cl_device_id* device_p = cl_manager_->devices();
  if (device_p) {
    command_queue_ = clCreateCommandQueue(cl_manager_->context(),
                                          device_p[0],
                                          0,
                                          &status);
  }
  else {
    return SDK_FAILURE;
  }

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateCommandQueue failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
bool stat_test_driver<T>::setup_result_data(unsigned rsize){
  cl_manager_->set_result_size(rsize);
  if(!cl_manager_->setup_stat_results())
    return false;
  if(cl_manager_->setup_stat_results_buffer()!=SDK_SUCCESS)
    return false;
  return true;
}
template <class T>
bool stat_test_driver<T>::clean_io_data(){
  if(!cl_manager_->clean_stat_input())
    return false;
  if(cl_manager_->clean_stat_input_buffer()!=SDK_SUCCESS)
    return false;
  if(!cl_manager_->clean_stat_results())
    return false;
  if(cl_manager_->clean_stat_results_buffer()!=SDK_SUCCESS)
    return false;
  return true;
}

template <class T>
int stat_test_driver<T>::create_kernel(vcl_string const& kernel_name)
{
  if (cl_manager_->create_kernel(kernel_name)!=SDK_SUCCESS)
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
int stat_test_driver<T>::set_stat_args(vcl_string arg_setup_spec)
{
  if(arg_setup_spec != "basic")
    return SDK_FAILURE;
  cl_int   status;
  // -- Set appropriate arguments to the kernel --
  // the input buffer
  status = clSetKernelArg(cl_manager_->kernel(),
                          0,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->input_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (input_buffer)"))
    return SDK_FAILURE;
  // the test result buffer
  status = clSetKernelArg(cl_manager_->kernel(),
                          1,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->results_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (results_buf)"))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}

template <class T>
int stat_test_driver<T>::run_stat_kernels()
{
  cl_int   status;
  

  if(this->set_stat_args()!=SDK_SUCCESS)
    return SDK_FAILURE;

  status = clGetKernelWorkGroupInfo(cl_manager_->kernel(),
                                    cl_manager_->devices()[0],
                                    CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),
                                    &used_local_memory_,
                                    NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    {
      return SDK_FAILURE;
    }

  status = clGetKernelWorkGroupInfo(cl_manager_->kernel(),
                                    cl_manager_->devices()[0],
                                    CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),
                                    &kernel_work_group_size_,
                                    NULL);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    {
      return SDK_FAILURE;
    }

  vcl_size_t globalThreads[]= {1};
  vcl_size_t localThreads[] = {1};

  if (used_local_memory_ > cl_manager_->total_local_memory())
    {
      vcl_cout << "Unsupported: Insufficient local memory on device.\n";
      return SDK_FAILURE;
    }
  vcl_cout << "Local memory used: " << used_local_memory_ << '\n';

  status = clEnqueueNDRangeKernel(command_queue_,
                                  cl_manager_->kernel(),
                                  1,
                                  NULL,
                                  globalThreads,
                                  localThreads,
                                  0,
                                  NULL,
                                  NULL);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clEnqueueNDRangeKernel failed."))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clFinish failed."))
    return SDK_FAILURE;

  if(!this->read_stat_results())
    return SDK_FAILURE;

  return SDK_SUCCESS;
}
template<class T>
bool stat_test_driver<T>:: read_stat_results()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,
                                   cl_manager_->results_buf(),CL_TRUE, 0,
                                   cl_manager_->result_size()*sizeof(cl_float),
                                   cl_manager_->stat_results(),
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueReadBuffer (stat_results)failed."))
    return false;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clReleaseEvent(events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return false;

   return true;
}

template <class T>
int stat_test_driver<T>::build_program()
{
  if (cl_manager_->build_kernel_program()!=CL_SUCCESS)
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
int stat_test_driver<T>::release_kernel()
{
  return cl_manager_->release_kernel();
}

template <class T>
int stat_test_driver<T>::cleanup_queue()
{
  cl_int status = SDK_SUCCESS;


  status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseCommandQueue failed."))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

template <class T>
bool stat_test_driver<T>::cleanup_stat_test(){
  if(!this->clean_io_data())
    return false;

  if(this->cleanup_queue()!=SDK_SUCCESS)
    return false;
  return true;
}

template <class T>
stat_test_driver<T>::~stat_test_driver()
{
}

template <class T>
void stat_test_driver<T>::print_kernel_usage_info()
{
  vcl_cout << "Used Local Memory: " << used_local_memory_ << '\n'
           << "Kernel Vetted Work Group Size: " << kernel_work_group_size_ << vcl_endl;
}

#undef STAT_TEST_DRIVER_INSTANTIATE
#define STAT_TEST_DRIVER_INSTANTIATE(T) \
template class stat_test_driver<T >

#endif // stat_test_driver_txx_
