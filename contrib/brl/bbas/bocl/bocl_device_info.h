#ifndef bocl_device_info_h_
#define bocl_device_info_h_
//:
// \file
// \brief  A wrapper for relevant device information in opencl
// \author Andrew Miller acm@computervisiongroup.com
// \date  Jan 25, 2011
//
// \verbatim
//  Modifications
//   Yi Dong Dec, 2015 -- enlarge array size to avoid CL_DEVICE_EXTENSIONS query failure
// \endverbatim
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>
#include <iosfwd>
#include "bocl_cl.h"
#include "bocl_utils.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
  #define CL_DEVICE_PCI_BUS_ID_NV                     0x4008
  #define CL_DEVICE_PCI_SLOT_ID_NV                    0x4009
#endif


//:  High level wrapper OpenCL Device information
class bocl_device_info
{
  public:
    bocl_device_info() = default;
    bocl_device_info(cl_device_id* device);
    ~bocl_device_info();

    //:Store a pointer to the cl_device_id
    cl_device_id* device_;

    //: device info
    std::string device_name_;
    std::string device_vendor_;
    std::string extensions_supported_;
    std::string platform_name_;
    std::string platform_version_;
    std::string driver_version_;
    cl_device_type device_type_;
    cl_uint addr_bits_;                     //!< Device Address Bits (pointer size)
    std::size_t max_work_group_size_;        //!< Max allowed work-items in a group
    cl_uint max_dimensions_;                //!< Max group dimensions allowed
    std::size_t max_work_item_sizes_[3];      //!< Max work-items sizes in each dimension
    cl_ulong max_mem_alloc_size_;           //!< Max memory alloc size for a buffer
    cl_ulong max_parameter_size_;           //!< Max buffer size that can be passed to a kernel
    cl_ulong total_local_memory_;           //!< Max local memory allowed
    cl_ulong total_global_memory_;          //!< Max global memory allowed
    cl_uint max_compute_units_;             //!< Max compute units
    cl_uint vector_width_short_;            //!< Ideal short vector size
    cl_uint vector_width_float_;            //!< Ideal float vector size
    cl_uint max_clock_freq_;                //!< Maximum clock frequency
    cl_bool image_support_;                 //!< image support
    std::size_t image2d_max_width_;          //!< Image2d Max Width
    std::size_t image2d_max_height_;         //!< Image2d Max Height
    std::size_t image3d_max_width_;          //!< Image3d Max Width
    std::size_t image3d_max_height_;         //!< Image3d Max Height
    std::size_t image3d_max_depth_;          //!< Image3d Max Depth


    //NVIDIA Specific Properties
    bool is_nvidia_device_;
    cl_uint compute_capability_major_, compute_capability_minor_;
    cl_uint regs_per_block_;
    cl_uint warp_size_;
    cl_bool gpu_overlap_;
    cl_bool exec_timeout_;
    cl_bool integrated_memory_;
    cl_uint bus_id_;
    cl_uint slot_id_;
};

//:  output stream
std::ostream& operator <<(std::ostream &s, bocl_device_info& info);

#endif
