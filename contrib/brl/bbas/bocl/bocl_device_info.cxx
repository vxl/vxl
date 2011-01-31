#include "bocl_device_info.h"
#include "bocl_utils.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h> // for std::malloc() and std::free()

bocl_device_info::bocl_device_info(cl_device_id* device)
{
  device_ = device;
  int status;

  //get platform associated with device
  cl_platform_id platform;
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_PLATFORM,
                           sizeof(platform),
                           (void*) &platform,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_PLATFORM failed."))
    return;
  char platform_name[512];
  status = clGetPlatformInfo(platform,CL_PLATFORM_NAME,sizeof(platform_name),platform_name,NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_PLATFORM_NAME failed."))
    return;
  platform_name_ = vcl_string(platform_name);

  // Get device specific information
  char vendor[512];
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_VENDOR,
                           sizeof(vendor),
                           (void*) vendor,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_VENDOR failed."))
    return;
  device_vendor_ = vcl_string(vendor);

  //get Device Type
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_TYPE,
                           sizeof(device_type_),
                           (void*) &device_type_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_TYPE failed."))
    return;

  //Get device max work gropu size
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_MAX_WORK_GROUP_SIZE,
                           sizeof(vcl_size_t),
                           (void*)&max_work_group_size_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_GROUP_SIZE failed."))
    return;
  max_work_group_size_ = max_work_group_size_/sizeof(vcl_size_t);


  //get max work item dimensions
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
                           sizeof(cl_uint),
                           (void*)&max_dimensions_,
                           NULL);
  if (!check_val(status,CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS failed."))
    return;

  //get max work item sizes
  max_work_item_sizes_ = (vcl_size_t*)vcl_malloc(max_dimensions_ * sizeof(vcl_size_t));
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_MAX_WORK_ITEM_SIZES,
                           sizeof(vcl_size_t) * max_dimensions_,
                           (void*)max_work_item_sizes_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_SIZES failed."))
    return;

  //get device local memory size (for each compute unit...)
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_LOCAL_MEM_SIZE,
                           sizeof(cl_ulong),
                           (void *)&total_local_memory_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_LOCAL_MEM_SIZE failed."))
    return;


  //get device global memory size
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_GLOBAL_MEM_SIZE,
                           sizeof(cl_ulong),
                           (void *)&total_global_memory_,
                           NULL);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_GLOBAL_MEM_SIZE failed."))
    return;

  //get device maximum compute units
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_MAX_COMPUTE_UNITS,
                           sizeof(cl_uint),
                           (void *)&max_compute_units_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_COMPUTE_UNITS failed."))
    return;

  //get preferred vector width
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT,
                           sizeof(cl_uint),
                           (void *)&vector_width_short_,
                           NULL);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT failed."))
    return;


  //get device preferred vector width float
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT,
                           sizeof(cl_uint),
                           (void *)&vector_width_float_,
                           NULL);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT failed."))
    return;


  //get device max clock freq
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_MAX_CLOCK_FREQUENCY,
                           sizeof(cl_uint),
                           (void *)&max_clock_freq_,
                           NULL);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_MAX_CLOCK_FREQUENCY failed."))
    return;

  //get device image support
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_IMAGE_SUPPORT,
                           sizeof(cl_bool),
                           (void *)&image_support_,
                           NULL);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_IMAGE_SUPPORT failed."))
    return;

  //get device image2d max width
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_IMAGE2D_MAX_WIDTH,
                           sizeof(vcl_size_t),
                           (void *)&image2d_max_width_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE2D_MAX_WIDTH failed."))
    return;

  //get device image2d max height
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_IMAGE2D_MAX_HEIGHT,
                           sizeof(vcl_size_t),
                           (void *)&image2d_max_height_,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE2D_MAX_HEIGHT failed."))
    return;

  //get device extension list
  char extensions[512];
  status = clGetDeviceInfo(*device_,
                           CL_DEVICE_EXTENSIONS,
                           sizeof(extensions),
                           (void*) extensions,
                           NULL);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_EXTENSIONS failed."))
    return;

  extensions_supported_ = vcl_string(extensions);
}


//destructorrr
bocl_device_info::~bocl_device_info()
{
  if (max_work_item_sizes_)
  {
    vcl_free(max_work_item_sizes_);
    max_work_item_sizes_ = NULL;
  }
}

vcl_ostream& operator <<(vcl_ostream &s, bocl_device_info& info)
{
  unsigned size = sizeof(vcl_size_t);
  s  << " Device Description\n"
     << " Device vendor: " << info.device_vendor_ << '\n'
     << " Device extensions: " << info.extensions_supported_ << 'n'
     << " Number of compute units: " << info.max_compute_units_ << '\n'
     << " Maximum clock frequency: " << info.max_clock_freq_/1000.0 << " GHz\n"
     << " Total global memory: "<< info.total_global_memory_/ 1073741824.0 /* 2^30 */ << " GBytes\n"
     << " Total local memory: "<< info.total_local_memory_/1024.0 << " KBytes\n"
     << " Maximum work group size: " << info.max_work_group_size_ << '\n'
     << " Maximum work item sizes: (" << (cl_uint) info.max_work_item_sizes_[0]/size << ','
     << (cl_uint) info.max_work_item_sizes_[1]/size << ','
     << (cl_uint) info.max_work_item_sizes_[2]/size << ")\n"
     << " Preferred short vector length: " << info.vector_width_short_ << '\n'
     << " Preferred float vector length: " << info.vector_width_float_ << '\n'
     << " image support " << info.image_support_ << '\n'
     << " Max 2D image width  " << info.image2d_max_width_ << '\n'
     << " Max 2D image height  " << info.image2d_max_height_ << '\n'
  ;
  return s;
}


