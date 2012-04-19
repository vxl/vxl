#ifndef bocl_device_info_h_
#define bocl_device_info_h_
//:
// \file
// \brief  A wrapper for relevant device information in opencl
// \author Andrew Miller acm@computervisiongroup.com
// \date  Jan 25, 2011
//
#include "bocl_cl.h"
#include "bocl_utils.h"
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstddef.h> // for std::size_t
#include <vcl_iosfwd.h>

//Just in case NVIDIA extensions are not defined
#ifndef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
  /* cl_nv_device_attribute_query extension - no extension #define since it has no functions */
  #define CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV       0x4000
  #define CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV       0x4001
  #define CL_DEVICE_REGISTERS_PER_BLOCK_NV            0x4002
  #define CL_DEVICE_WARP_SIZE_NV                      0x4003
  #define CL_DEVICE_GPU_OVERLAP_NV                    0x4004
  #define CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV            0x4005
  #define CL_DEVICE_INTEGRATED_MEMORY_NV              0x4006
#endif


//:  High level wrapper OpenCL Device information
class bocl_device_info
{
  public:
    bocl_device_info() {}
    bocl_device_info(cl_device_id* device);
    ~bocl_device_info();
  
    //:Store a pointer to the cl_device_id
    cl_device_id* device_; 
  
    //: device info
    vcl_string device_name_; 
    vcl_string device_vendor_; 
    vcl_string extensions_supported_; 
    vcl_string platform_name_; 
    vcl_string platform_version_;
    vcl_string driver_version_; 
    cl_device_type device_type_; 
    cl_uint addr_bits_;                     //!< Device Address Bits (pointer size)
    vcl_size_t max_work_group_size_;        //!< Max allowed work-items in a group
    cl_uint max_dimensions_;                //!< Max group dimensions allowed
    vcl_size_t max_work_item_sizes_[3];      //!< Max work-items sizes in each dimension
    cl_ulong max_mem_alloc_size_;           //!< Max memory alloc size for a buffer
    cl_ulong max_parameter_size_;           //!< Max buffer size that can be passed to a kernel
    cl_ulong total_local_memory_;           //!< Max local memory allowed
    cl_ulong total_global_memory_;          //!< Max global memory allowed
    cl_uint max_compute_units_;             //!< Max compute units
    cl_uint vector_width_short_;            //!< Ideal short vector size
    cl_uint vector_width_float_;            //!< Ideal float vector size
    cl_uint max_clock_freq_;                //!< Maximum clock frequency
    cl_bool image_support_;                 //!< image support
    vcl_size_t image2d_max_width_;          //!< Image2d Max Width
    vcl_size_t image2d_max_height_;         //!< Image2d Max Height
    vcl_size_t image3d_max_width_;          //!< Image3d Max Width
    vcl_size_t image3d_max_height_;         //!< Image3d Max Height
    vcl_size_t image3d_max_depth_;          //!< Image3d Max Depth
    
    
    //NVIDIA Specific Properties
    bool is_nvidia_device_; 
    cl_uint compute_capability_major_, compute_capability_minor_;
    cl_uint regs_per_block_;
    cl_uint warp_size_;
    cl_bool gpu_overlap_;
    cl_bool exec_timeout_;
    cl_bool integrated_memory_;

};

//:  output stream
vcl_ostream& operator <<(vcl_ostream &s, bocl_device_info& info);

#endif
