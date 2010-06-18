#ifndef ray_bundle_test_driver_txx_
#define ray_bundle_test_driver_txx_

#include "ray_bundle_test_driver.h"

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
ray_bundle_test_driver<T>::setup_cl()
{
  // Create command queue
  cl_int status = CL_SUCCESS;

  cl_device_id* device_p = cl_manager_->devices();
  if (device_p) {
    command_queue_ = clCreateCommandQueue(cl_manager_->context(),
                                          device_p[0],
                                          CL_QUEUE_PROFILING_ENABLE,
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
bool ray_bundle_test_driver<T>::setup_image_cam_data(){
  if(!cl_manager_->setup_image_cam_arrays())
    return false;
  if(cl_manager_->setup_image_cam_buffers()!=SDK_SUCCESS)
    return false;
  return true;
}
template <class T>
bool ray_bundle_test_driver<T>::clean_image_cam_data(){
  if(!cl_manager_->clean_image_cam_arrays())
    return false;
  if(cl_manager_->clean_image_cam_buffers()!=SDK_SUCCESS)
    return false;
  return true;
}

template <class T>
int ray_bundle_test_driver<T>::create_kernel(vcl_string const& kernel_name)
{
  if (cl_manager_->create_kernel(kernel_name)!=SDK_SUCCESS)
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
int ray_bundle_test_driver<T>::set_tree_args()
{
  cl_int   status;
  // -- Set appropriate arguments to the kernel --
  // the array of tree cells
  status = clSetKernelArg(cl_manager_->kernel(),
                          0,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->cell_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (input_cell_array)"))
    return SDK_FAILURE;
  // the array of tree data
  status = clSetKernelArg(cl_manager_->kernel(),
                          1,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->data_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (input_data_array)"))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
int ray_bundle_test_driver<T>::set_basic_test_args(vcl_string arg_setup_spec)
{
  cl_int   status;
  if(arg_setup_spec == "basic"){
    status = clSetKernelArg(cl_manager_->kernel(), 3,
                            (this->n_rays_in_bundle() * sizeof(cl_uchar)), NULL);  
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clSetKernelArg failed. (local bundle pointer array)"))
      return SDK_FAILURE;
  }else{
    status = clSetKernelArg(cl_manager_->kernel(), 3,
                            (this->n_rays_in_bundle() * sizeof(cl_uchar4)), NULL);  
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clSetKernelArg failed. (local bundle pointer array)"))
      return SDK_FAILURE;
  }

  status = clSetKernelArg(cl_manager_->kernel(), 4, 
                          ( this->n_rays_in_bundle() * sizeof(cl_int)), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local exit point array)"))
    return SDK_FAILURE;



  status = clSetKernelArg(cl_manager_->kernel(), 5, 
                          (this->n_rays_in_bundle() * sizeof(cl_short4)), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local cached loc_code array)"))
    return SDK_FAILURE;



  status = clSetKernelArg(cl_manager_->kernel(), 6, 
                          (this->n_rays_in_bundle() * sizeof(cl_float16)), NULL);  

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local cached data array)"))
    return SDK_FAILURE;
  if(arg_setup_spec == "include_image_array"){
       status = clSetKernelArg(cl_manager_->kernel(), 7, 
                            (this->n_rays_in_bundle() * sizeof(cl_float4)), NULL);  

    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clSetKernelArg failed. (local cached data array)"))
      return SDK_FAILURE;
    status = clSetKernelArg(cl_manager_->kernel(), 8, 
                            (this->n_rays_in_bundle() * sizeof(cl_float4)), NULL);  

    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clSetKernelArg failed. (local cached data array)"))
      return SDK_FAILURE;
  }
  return SDK_SUCCESS;
}

template <class T>
int ray_bundle_test_driver<T>::set_image_cam_args()
{
  cl_int   status;

  status = clSetKernelArg(cl_manager_->kernel(),
                          3,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->n_levels_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (n_levels buffer)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(),
                          4,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->ray_origin_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (camera center)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(),
                          5,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->camera_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (camera pseudo inverse svd)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(),
                          6,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->roi_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (image_roi)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(),
                          7,
                          sizeof(cl_mem),
                          (void *)&cl_manager_->global_bbox_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. ( bounding box )"))
    return SDK_FAILURE;


  status = clSetKernelArg(cl_manager_->kernel(), 8,
                          3*sizeof(cl_float16), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. ( camera pseudo-inverse cache)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 9, 
                          sizeof(cl_float4), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (camera center cache)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 10, 
                          sizeof(cl_float4), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (bounding box cache)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 11, 
                          sizeof(cl_uint4), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (roi cache)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 12,
                          (this->n_rays_in_bundle() * sizeof(cl_uchar)), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local bundle pointer array)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 13, 
                          (3 * this->n_rays_in_bundle() * sizeof(cl_float)), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local exit point array)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 14, 
                          (this->n_rays_in_bundle() * sizeof(cl_short4)), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local cached loc_code array)"))
    return SDK_FAILURE;


  status = clSetKernelArg(cl_manager_->kernel(), 15, 
                          (this->n_rays_in_bundle() * sizeof(cl_float16)), NULL);  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (local cached data array)"))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}

template <class T>
int ray_bundle_test_driver<T>::run_bundle_test_kernels()
{
  cl_int   status;
  cl_event events[2];

  ////clear output array
  cl_manager_->clear_tree_results();

  cl_mem result_buf = clCreateBuffer(cl_manager_->context(),
                                     CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,
                                     cl_manager_->tree_result_size() * sizeof(cl_int4),
                                     cl_manager_->tree_results(),
                                     &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer failed. (tree_results)"))
    return SDK_FAILURE;

  if(this->set_tree_args()!=SDK_SUCCESS)
    return SDK_FAILURE;


  // the returned array test result
  status = clSetKernelArg(cl_manager_->kernel(),
                          2,
                          sizeof(cl_mem),
                          (void *)&result_buf);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (result_buf)"))
    return SDK_FAILURE;
#if 0
  if (this->set_ray_bundle_args()!=SDK_SUCCESS)
    return SDK_FAILURE;
#endif
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

  vcl_size_t globalThreads[]= {this->work_space_ni(), this->work_space_nj()};
  vcl_size_t localThreads[] = {this->bundle_ni(), this->bundle_nj()};

  if (used_local_memory_ > cl_manager_->total_local_memory())
    {
      vcl_cout << "Unsupported: Insufficient local memory on device.\n";
      return SDK_FAILURE;
    }
  vcl_cout << "Local memory used: " << used_local_memory_ << '\n';
  cl_event ceEvent;

  status = clEnqueueNDRangeKernel(command_queue_,
                                  cl_manager_->kernel(),
                                  2,
                                  NULL,
                                  globalThreads,
                                  localThreads,
                                  0,
                                  NULL,
                                  &ceEvent);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clEnqueueNDRangeKernel failed."))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clFinish failed."))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  float gpu_time_= (double)1.0e-6 * (tend - tstart); // convert nanoseconds to milliseconds 
  //vcl_cout<<"GPU time is "<<gpu_time_<< "ms"<<vcl_endl;

  // Enqueue readBuffers
  status = clEnqueueReadBuffer(command_queue_,
                               result_buf,
                               CL_TRUE,
                               0,
                               cl_manager_->tree_result_size()*sizeof(cl_int4),
                               cl_manager_->tree_results(),
                               0,
                               NULL,
                               &events[0]);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clEnqueueBuffer (tree_results)failed."))
    return SDK_FAILURE;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clWaitForEvents failed."))
    return SDK_FAILURE;

  clReleaseEvent(events[0]);

  status = clReleaseMemObject(result_buf);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
bool ray_bundle_test_driver<T>::init_work_image(vcl_string mode)
{
  if(mode == "4x4_uniform"||mode == "4x4_gauss")
    {
      cl_manager_->set_ni(4);
      cl_manager_->set_nj(4);
      if(!cl_manager_->setup_work_image())
        return false;
      cl_float* image = cl_manager_->ray_results();
      if(!image)
        return false;
      for(unsigned j = 0; j<4; ++j)
        for(unsigned i = 0; i<4; ++i)
          {
            *(image++) = 0.16667f*(i+j);
            *(image++) = 0.0f;/* skip alpha integral slot */
            *(image++) = 1.0f; /* vis_inf */
            *(image++) = (float)(i+j); /* pre */
          }
      return true;
    }
  return false;
}
template <class T>
int ray_bundle_test_driver<T>::run_norm_kernel()
{
  cl_int   status;
  cl_event events[2];

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

  vcl_size_t globalThreads[]= {cl_manager_->ni(),
                               cl_manager_->nj()};
  vcl_size_t localThreads[] = {this->bundle_ni(), this->bundle_nj()};

  if (used_local_memory_ > cl_manager_->total_local_memory())
    {
      vcl_cout << "Unsupported: Insufficient local memory on device.\n";
      return SDK_FAILURE;
    }
  vcl_cout << "Local memory used: " << used_local_memory_ << '\n';

  status = clEnqueueNDRangeKernel(command_queue_,
                                  cl_manager_->kernel(),
                                  2,
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

  status = clEnqueueReadBuffer(command_queue_,cl_manager_->image_buf(),CL_TRUE,
                                   0,cl_manager_->n_rays()*sizeof(cl_float4),
                                   cl_manager_->ray_results(),
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (ray_results)failed."))
    return false;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clReleaseEvent(events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return false;

  status = clReleaseMemObject(cl_manager_->image_buf());
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseMemObject failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


template <class T>
int ray_bundle_test_driver<T>::build_program()
{
  if (cl_manager_->build_kernel_program()!=CL_SUCCESS)
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
int ray_bundle_test_driver<T>::release_kernel()
{
  return cl_manager_->release_kernel();
}

template <class T>
int ray_bundle_test_driver<T>::cleanup_bundle_test()
{
  cl_int status = SDK_SUCCESS;

  //cl_manager_->cleanup_tree_processing();

  status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseCommandQueue failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}
template <class T>
bool ray_bundle_test_driver<T>::setup_norm_data(vcl_string mode, 
                                                bool use_uniform,
                                                float mean,
                                                float sigma)
{
  if(!this->init_work_image(mode))
    return false;
  if(cl_manager_->setup_work_img_buffer()!=SDK_SUCCESS)
    return false;
  if(!cl_manager_->setup_app_density(use_uniform, mean, sigma))
    return false;
  if(cl_manager_->setup_app_density_buffer()!=SDK_SUCCESS)
    return false;
  return true;
}
template <class T>
int ray_bundle_test_driver<T>::set_norm_args()
{
  cl_int status = SDK_SUCCESS;
  status = clSetKernelArg(cl_manager_->kernel(), 0, 
                          sizeof(cl_mem), (void *)&cl_manager_->image_buf());  

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (image array)"))
    return SDK_FAILURE;

  status = clSetKernelArg(cl_manager_->kernel(), 1, 
                          sizeof(cl_mem), (void *)&cl_manager_->app_density());  
  
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clSetKernelArg failed. (remote surface appearance)"))
    return SDK_FAILURE;
  return SDK_SUCCESS;

}
template <class T>
bool ray_bundle_test_driver<T>::clean_norm_data(){
  if(!cl_manager_->clean_work_image())
    return false;
#if 0/* done in execute function */
  if(cl_manager_->clean_work_img_buffer()!=SDK_SUCCESS)
    return false;
#endif
  if(!cl_manager_->clean_app_density())
    return false;
  if(cl_manager_->clean_app_density_buffer()!=SDK_SUCCESS)
    return false;
  return true;
}
template <class T>
ray_bundle_test_driver<T>::~ray_bundle_test_driver()
{
}

template <class T>
void ray_bundle_test_driver<T>::print_kernel_usage_info()
{
  vcl_cout << "Used Local Memory: " << used_local_memory_ << '\n'
           << "Kernel Vetted Work Group Size: " << kernel_work_group_size_ << vcl_endl;
}

#undef RAY_BUNDLE_TEST_DRIVER_INSTANTIATE
#define RAY_BUNDLE_TEST_DRIVER_INSTANTIATE(T) \
template class ray_bundle_test_driver<T >

#endif // ray_bundle_test_driver_txx_
