#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "bocl_device_info.h"
#include "bocl_utils.h"
//:
// \file
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bocl_device_info::bocl_device_info(cl_device_id* device)
{
  is_nvidia_device_ = false;
  device_ = device;
  int status;

  //get platform associated with device
  cl_platform_id platform;
  status = clGetDeviceInfo(*device_, CL_DEVICE_PLATFORM, sizeof(platform), (void*) &platform, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_PLATFORM failed."))
    return;
  char platform_name[512];
  status = clGetPlatformInfo(platform,CL_PLATFORM_NAME,sizeof(platform_name),platform_name,nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_PLATFORM_NAME failed."))
    return;
  platform_name_ = std::string(platform_name);
  status = clGetPlatformInfo(platform,CL_PLATFORM_VERSION,sizeof(platform_name),platform_name,nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_PLATFORM_VERSION failed."))
    return;
  platform_version_ = std::string(platform_name);

  //get device name
  char device_string[1024];
  status = clGetDeviceInfo(*device_, CL_DEVICE_NAME, sizeof(device_string), &device_string, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_NAME failed."))
    return;
  device_name_ = std::string(device_string);

  //get Device Type
  status = clGetDeviceInfo(*device_,CL_DEVICE_TYPE,sizeof(device_type_),(void*) &device_type_,nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_TYPE failed."))
    return;

  //get device vendor
  status = clGetDeviceInfo(*device_, CL_DEVICE_VENDOR, sizeof(device_string), &device_string, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_VENDOR failed."))
    return;
  device_vendor_ = std::string(device_string);

  //store driver version
  status = clGetDeviceInfo(*device_, CL_DRIVER_VERSION, sizeof(device_string), &device_string, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_VENDOR failed."))
    return;
  driver_version_ = std::string(device_string);


  //Get device max work gropu size
  status = clGetDeviceInfo(*device_,CL_DEVICE_MAX_WORK_GROUP_SIZE,sizeof(std::size_t),(void*)&max_work_group_size_,nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_GROUP_SIZE failed."))
    return;
  max_work_group_size_ = max_work_group_size_/sizeof(std::size_t);
  //get max work item dimensions
  status = clGetDeviceInfo(*device_,CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, sizeof(cl_uint),  (void*)&max_dimensions_, nullptr);
  if (!check_val(status,CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS failed."))
    return;
  //get max work item sizes
  status = clGetDeviceInfo(*device_, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(std::size_t) * 3, (void*)max_work_item_sizes_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_WORK_ITEM_SIZES failed."))
    return;

  //get device local memory size (for each compute unit...)
  status = clGetDeviceInfo(*device_, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(cl_ulong), (void *)&total_local_memory_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_LOCAL_MEM_SIZE failed."))
    return;

  //get device global memory size
  status = clGetDeviceInfo(*device_, CL_DEVICE_GLOBAL_MEM_SIZE, sizeof(cl_ulong), (void *)&total_global_memory_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_GLOBAL_MEM_SIZE failed."))
    return;

  //store max buffer alloc size
  status = clGetDeviceInfo(*device_, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(cl_ulong), (void *)&max_mem_alloc_size_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_MAX_MEM_ALLOC_SIZE failed."))
    return;

  //store max param size
  status = clGetDeviceInfo(*device_, CL_DEVICE_MAX_PARAMETER_SIZE, sizeof(cl_ulong), (void *)&max_parameter_size_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_MAX_PARAMETER_SIZE failed."))
    return;

  //address bits (pointer size on device)
  status = clGetDeviceInfo(*device_, CL_DEVICE_ADDRESS_BITS, sizeof(addr_bits_), &addr_bits_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_ADDRESS_BITS failed."))
    return;

  //get device maximum compute units
  status = clGetDeviceInfo(*device_, CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(cl_uint), (void *)&max_compute_units_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_MAX_COMPUTE_UNITS failed."))
    return;

  //get preferred vector width
  status = clGetDeviceInfo(*device_, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, sizeof(cl_uint), (void *)&vector_width_short_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT failed."))
    return;

  //get device preferred vector width float
  status = clGetDeviceInfo(*device_, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, sizeof(cl_uint), (void *)&vector_width_float_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT failed."))
    return;

  //get device max clock freq
  status = clGetDeviceInfo(*device_, CL_DEVICE_MAX_CLOCK_FREQUENCY, sizeof(cl_uint), (void *)&max_clock_freq_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_MAX_CLOCK_FREQUENCY failed."))
    return;

  //get device image support
  status = clGetDeviceInfo(*device_, CL_DEVICE_IMAGE_SUPPORT, sizeof(cl_bool), (void *)&image_support_, nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetDeviceInfo CL_DEVICE_IMAGE_SUPPORT failed."))
    return;

  //get device image2d max width
  status = clGetDeviceInfo(*device_,CL_DEVICE_IMAGE2D_MAX_WIDTH,sizeof(std::size_t),(void *)&image2d_max_width_,nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE2D_MAX_WIDTH failed."))
    return;
  //get device image2d max height
  status = clGetDeviceInfo(*device_, CL_DEVICE_IMAGE2D_MAX_HEIGHT, sizeof(std::size_t), (void *)&image2d_max_height_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE2D_MAX_HEIGHT failed."))
    return;
  //get device image3d max width
  status = clGetDeviceInfo(*device_, CL_DEVICE_IMAGE3D_MAX_WIDTH, sizeof(std::size_t), (void *)&image3d_max_width_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE3D_MAX_WIDTH failed."))
    return;
  //get device image3d max height
  status = clGetDeviceInfo(*device_, CL_DEVICE_IMAGE3D_MAX_HEIGHT, sizeof(std::size_t), (void *)&image3d_max_height_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE3D_MAX_HEIGHT failed."))
    return;
  //get device image3d max width
  status = clGetDeviceInfo(*device_, CL_DEVICE_IMAGE3D_MAX_DEPTH, sizeof(std::size_t), (void *)&image3d_max_depth_, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_IMAGE3D_MAX_DEPTH failed."))
    return;

  //get device extension list
  char extensions[2000];
  status = clGetDeviceInfo(*device_, CL_DEVICE_EXTENSIONS,  sizeof(extensions), (void*) extensions, nullptr);
  if (!check_val(status, CL_SUCCESS, "clGetDeviceInfo CL_DEVICE_EXTENSIONS failed."))
    return;
  extensions_supported_ = std::string(extensions);

  //see if it is an NVIDIA device
  is_nvidia_device_ = std::strstr(extensions_supported_.c_str(), "cl_nv_device_attribute_query") != nullptr;

  //if it's an nvidia device, get nvidia specific attributes
  if(is_nvidia_device_)
  {
      clGetDeviceInfo(*device_, CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, sizeof(cl_uint), &compute_capability_major_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, sizeof(cl_uint), &compute_capability_minor_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_REGISTERS_PER_BLOCK_NV, sizeof(cl_uint), &regs_per_block_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_WARP_SIZE_NV, sizeof(cl_uint), &warp_size_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_GPU_OVERLAP_NV, sizeof(cl_bool), &gpu_overlap_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV, sizeof(cl_bool), &exec_timeout_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_INTEGRATED_MEMORY_NV, sizeof(cl_bool), &integrated_memory_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_PCI_BUS_ID_NV, sizeof(cl_uint), &bus_id_, nullptr);
      clGetDeviceInfo(*device_, CL_DEVICE_PCI_SLOT_ID_NV, sizeof(cl_uint), &slot_id_, nullptr);
  }


#if 0  //other properties not yet stored
  // CL_DEVICE_MAX_MEM_ALLOC_SIZE
  cl_ulong max_mem_alloc_size;
  clGetDeviceInfo(device, CL_DEVICE_MAX_MEM_ALLOC_SIZE, sizeof(max_mem_alloc_size), &max_mem_alloc_size, NULL);
  shrLogEx(iLogMode, 0, "  CL_DEVICE_MAX_MEM_ALLOC_SIZE:\t\t%u MByte\n", (unsigned int)(max_mem_alloc_size / (1024 * 1024)));

  // CL_DEVICE_ERROR_CORRECTION_SUPPORT
  cl_bool error_correction_support;
  clGetDeviceInfo(device, CL_DEVICE_ERROR_CORRECTION_SUPPORT, sizeof(error_correction_support), &error_correction_support, NULL);
  shrLogEx(iLogMode, 0, "  CL_DEVICE_ERROR_CORRECTION_SUPPORT:\t%s\n", error_correction_support == CL_TRUE ? "yes" : "no");

  // CL_DEVICE_LOCAL_MEM_TYPE
  cl_device_local_mem_type local_mem_type;
  clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_TYPE, sizeof(local_mem_type), &local_mem_type, NULL);
  shrLogEx(iLogMode, 0, "  CL_DEVICE_LOCAL_MEM_TYPE:\t\t%s\n", local_mem_type == 1 ? "local" : "global");

  // CL_DEVICE_LOCAL_MEM_SIZE
  clGetDeviceInfo(device, CL_DEVICE_LOCAL_MEM_SIZE, sizeof(mem_size), &mem_size, NULL);
  shrLogEx(iLogMode, 0, "  CL_DEVICE_LOCAL_MEM_SIZE:\t\t%u KByte\n", (unsigned int)(mem_size / 1024));

  // CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE
  clGetDeviceInfo(device, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, sizeof(mem_size), &mem_size, NULL);
  shrLogEx(iLogMode, 0, "  CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE:\t%u KByte\n", (unsigned int)(mem_size / 1024));

  // CL_DEVICE_QUEUE_PROPERTIES
  cl_command_queue_properties queue_properties;
  clGetDeviceInfo(device, CL_DEVICE_QUEUE_PROPERTIES, sizeof(queue_properties), &queue_properties, NULL);
  if( queue_properties & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE )
      shrLogEx(iLogMode, 0, "  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE");
  if( queue_properties & CL_QUEUE_PROFILING_ENABLE )
      shrLogEx(iLogMode, 0, "  CL_DEVICE_QUEUE_PROPERTIES:\t\t%s\n", "CL_QUEUE_PROFILING_ENABLE");
#endif

}

//destructorrr
bocl_device_info::~bocl_device_info()
{

#if 0
  if (max_work_item_sizes_)
  {
    delete[] max_work_item_sizes_; //std::free(max_work_item_sizes_);
    max_work_item_sizes_ = NULL;
  }
#endif
}

std::ostream& operator <<(std::ostream &s, bocl_device_info& info)
{
  unsigned size = sizeof(std::size_t);
  s  << " Device Description: \n"
     << " Device ID : " << (long) (*info.device_) << '\n'
     << " Device Name : " << info.device_name_ << '\n'
     << " Device Vendor: " << info.device_vendor_ << '\n'
     << " Device Platform: " << info.platform_name_ << '\n'
     << " Device Platform Version: " << info.platform_version_ << '\n'
     << " Device Driver Version: " << info.driver_version_ << '\n'
     << " Device Type: ";
  if( info.device_type_ & CL_DEVICE_TYPE_CPU )
      s << "CL_DEVICE_TYPE_CPU \n";
  if( info.device_type_ & CL_DEVICE_TYPE_GPU )
      s << "CL_DEVICE_TYPE_GPU \n";
  if( info.device_type_ & CL_DEVICE_TYPE_ACCELERATOR )
      s << "CL_DEVICE_TYPE_ACCELERATOR \n";
  if( info.device_type_ & CL_DEVICE_TYPE_DEFAULT )
      s << "CL_DEVICE_TYPE_DEFAULT \n";

  s  << " Number of compute units: " << info.max_compute_units_ << '\n'
     << " Maximum clock frequency: " << info.max_clock_freq_/1000.0 << " GHz\n"
     << " Total global memory: "<< info.total_global_memory_/1073741824.0 /* 2^30 */ << " GBytes\n"
     << " Total local memory: "<< info.total_local_memory_/1024.0 << " KBytes\n"
     << " Maximum mem object size: "<< info.max_mem_alloc_size_/1073741824.0 /*2^30 */ << " GBytes\n"
     << " Maximum param size : "<< info.max_parameter_size_/1073741824.0 /*2^30*/ << " GBytes\n"
     << " Device Address Bits: "<< info.addr_bits_ << " bits\n"
     << " Maximum work group size: " << info.max_work_group_size_ << '\n'
     << " Maximum work item sizes: (" << (cl_uint) info.max_work_item_sizes_[0]/size << ','
     << (cl_uint) info.max_work_item_sizes_[1]/size << ','
     << (cl_uint) info.max_work_item_sizes_[2]/size << ")\n"
     << " Preferred short vector length: " << info.vector_width_short_ << '\n'
     << " Preferred float vector length: " << info.vector_width_float_ << '\n'
     << " image support " << info.image_support_ << '\n'
     << " Max 2D image dim: (" << info.image2d_max_width_ << ',' << info.image2d_max_height_ << ")\n"
     << " Max 3D image dim: (" << info.image3d_max_width_ << ',' << info.image3d_max_height_ << ',' << info.image3d_max_depth_ << ")\n"
     << " Device extensions: " << info.extensions_supported_ << '\n'
  ;

  if(info.is_nvidia_device_)
  {
    s << " NVIDIA Specific Device Properties: \n"
      << "   CL_DEVICE_COMPUTE_CAPABILITY_NV: " << info.compute_capability_major_ << "." << info.compute_capability_minor_ << '\n'
      << "   NUMBER OF MULTIPROCESSORS: " << info.max_compute_units_ << '\n'
      << "   CL_DEVICE_REGISTERS_PER_BLOCK_NV: " << info.regs_per_block_ << '\n'
      << "   CL_DEVICE_WARP_SIZE_NV: " << info.warp_size_ << '\n'
      << "   CL_DEVICE_GPU_OVERLAP_NV: " << ((info.gpu_overlap_ == CL_TRUE) ? "CL_TRUE" : "CL_FALSE") << '\n'
      << "   CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV: " << ((info.exec_timeout_ == CL_TRUE) ? "CL_TRUE" : "CL_FALSE") << '\n'
      << "   CL_DEVICE_INTEGRATED_MEMORY_NV: " << ((info.integrated_memory_ == CL_TRUE) ? "CL_TRUE" : "CL_FALSE") << '\n'
      << "   CL_DEVICE_PCI_BUS_ID_NV: " << info.bus_id_ << '\n'
      << "   CL_DEVICE_PCI_SLOT_ID_NV: " << info.slot_id_ << '\n'
    ;
  }

  return s;
}
