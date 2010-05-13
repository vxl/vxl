// This is brl/bseg/boxm/opt/open_cl/boxm_opencl_manager.txx
#ifndef boxm_opencl_manager_txx_
#define boxm_opencl_manager_txx_

#include "boxm_opencl_manager.h"
//:
// \file

#include <vcl_utility.h>


//: Insure only one instance is created
template <class T>
T* boxm_opencl_manager<T>::instance()
{
  if (!instance_) {
    instance_ = new T();
    instance_->initialize_cl();
  }
  return boxm_opencl_manager::instance_;
}

template <class T>
void boxm_opencl_manager<T>::clear_cl()
{
  clReleaseContext(context_);
  if (devices_)
  {
    free(devices_);
    devices_ = NULL;
  }

  if (max_work_item_sizes_)
  {
    free(max_work_item_sizes_);
    max_work_item_sizes_ = NULL;
  }
}

//: Destructor
template <class T>
boxm_opencl_manager<T>::~boxm_opencl_manager()
{
  this->clear_cl();
}


template <class T>
bool boxm_opencl_manager<T>::initialize_cl()
{
  cl_int status = CL_SUCCESS;
  cl_uint num_platforms = 0;
  // Check the number of  available platforms
  status = clGetPlatformIDs(0,NULL,&num_platforms);
  if (status != CL_SUCCESS) {
    vcl_cerr << "boxm_opencl_manager: clGetPlatformIDs (call 1) returned " << status << '\n';
    return false;
  }
  if (num_platforms == 0) {
    vcl_cerr << "boxm_opencl_manager: 0 OpenCL platforms found!\n";
    return false;
  }
  if (num_platforms > 1) {
    vcl_cerr << "boxm_opencl_manager: warning: found " << num_platforms << "OpenCL platforms. Using the first\n";
  }
  // Get the first platform ID
  cl_platform_id platform_id[2];
  status = clGetPlatformIDs (2, platform_id, NULL);
  if (status != CL_SUCCESS) {
    vcl_cerr << "boxm_opencl_manager: clGetPlatformIDs (call 2) returned " << status << '\n';
    return false;
  }
  bool gpu_found=false;
  bool cpu_found=false;

  cl_device_id device;
  //: First checking for GPU
  for(unsigned i=0;i<num_platforms;i++)
  {
	  if( clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_GPU, 1, &device, NULL)== CL_SUCCESS)
	  {
		gpu_found=true;
		break;
	  }

  }
  //: If GPU not found then look for CPU
  if(!gpu_found)
  {
    for(unsigned i=0;i<num_platforms;i++)
  {
	  if( clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_CPU, 1, &device, NULL)== CL_SUCCESS)
	  {
	  cpu_found=true;
		break;
	  }

  }
  }
  // get an available GPU device from the the platform
  // should we be using all if more than one avaiable?

   if(!gpu_found && !cpu_found)
	return false;
  //Create a context from the device ID
  context_ = clCreateContext(0, 1, &device, NULL, NULL, &status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateContextFromType failed.")) {
    return false;
  }


  vcl_size_t device_list_size = 0;
  // First, get the size of device list data
  status = clGetContextInfo(context_,
                            CL_CONTEXT_DEVICES,
                            0,
                            NULL,
                            &device_list_size);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetContextInfo failed."))
    return false;
  number_devices_ = device_list_size/sizeof(cl_device_id);

  // Now allocate memory for device list based on the size we got earlier
  devices_ = (cl_device_id *)malloc(device_list_size);
  if (devices_==NULL) {
    vcl_cout << "Failed to allocate memory (devices).\n";
    return false;
  }

  // Now, get the device list data
  status = clGetContextInfo(context_,
                            CL_CONTEXT_DEVICES,
                            device_list_size,
                            devices_,
                            NULL);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetGetContextInfo failed."))
    return false;

  vcl_size_t max_work_group_size = 0;
  // Get device specific information
  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_MAX_WORK_GROUP_SIZE,
                           sizeof(vcl_size_t),
                           (void*)&max_work_group_size,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_MAX_WORK_GROUP_SIZE failed."))
    return false;

  max_work_group_size_ = max_work_group_size/sizeof(vcl_size_t);

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                           sizeof(cl_uint),
                           (void*)&max_dimensions_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS failed."))
    return false;


  max_work_item_sizes_ = (vcl_size_t*)malloc(max_dimensions_ * sizeof(vcl_size_t));

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_MAX_WORK_ITEM_SIZES,
                           sizeof(vcl_size_t) * max_dimensions_,
                           (void*)max_work_item_sizes_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_SIZES failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_LOCAL_MEM_SIZE,
                           sizeof(cl_ulong),
                           (void *)&total_local_memory_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_LOCAL_MEM_SIZE failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_GLOBAL_MEM_SIZE,
                           sizeof(cl_ulong),
                           (void *)&total_global_memory_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_GLOBAL_MEM_SIZE failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_MAX_COMPUTE_UNITS,
                           sizeof(cl_uint),
                           (void *)&max_compute_units_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_MAX_COMPUTE_UNITS failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
                           sizeof(cl_uint),
                           (void *)&vector_width_short_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
                           sizeof(cl_uint),
                           (void *)&vector_width_float_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_MAX_CLOCK_FREQUENCY,
                           sizeof(cl_uint),
                           (void *)&max_clock_freq_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_MAX_CLOCK_FREQUENCY failed."))
    return false;

  status = clGetDeviceInfo(devices_[0],
                           CL_DEVICE_IMAGE_SUPPORT,
                           sizeof(cl_bool),
                           (void *)&image_support_,
                           NULL);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clGetDeviceInfo CL_DEVICE_IMAGE_SUPPORT failed."))
    return false;
  unsigned size = sizeof(vcl_size_t);
  vcl_cout << "Context Description\n"
           << " Number of devices: " << number_devices_ << '\n'
           << " Number of compute units: " << max_compute_units_ << '\n'
           << " Maximum clock frequency: " << max_clock_freq_/1000.0 << " GHz\n"
           <<" Total global memory: "<<total_global_memory_/ 1073741824.0 /* 2^30 */ << " GBytes\n"
           <<" Total local memory: "<< total_local_memory_/1024.0 << " KBytes\n"
           << " Maximum work group size: " << max_work_group_size_ << '\n'
           << " Maximum work item sizes: (" << (cl_uint)max_work_item_sizes_[0]/size << ','
           << (cl_uint)max_work_item_sizes_[1]/size << ','
           << (cl_uint)max_work_item_sizes_[2]/size << ")\n"
           << " Preferred short vector length: " << vector_width_short_ << '\n'
           << " Preferred float vector length: " << vector_width_float_ << '\n'
           << " image support " << image_support_ << '\n';
  for (unsigned id = 0; id<number_devices_; ++id)
    vcl_cout << " Device id [" << id << "]: " << devices_[id] << '\n';
  return true;
}

template <class T>
void* boxm_opencl_manager<T>::allocate_host_mem(vcl_size_t size)
{
#if defined (_WIN32)
  return _aligned_malloc(size, 16);
#elif defined(__APPLE__)
  return malloc(size);
#else
  return memalign(16, size);
#endif
}


#undef BOXM_OPENCL_MANAGER_INSTANTIATE
#define BOXM_OPENCL_MANAGER_INSTANTIATE(T) \
template <class T > T* boxm_opencl_manager<T >::instance_ = 0; \
template class boxm_opencl_manager<T >


#endif // boxm_opencl_manager_txx_
