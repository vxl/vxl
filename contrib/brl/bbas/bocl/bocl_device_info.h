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
    vcl_size_t max_work_group_size_;        //!< Max allowed work-items in a group
    cl_uint max_dimensions_;                //!< Max group dimensions allowed
    vcl_size_t * max_work_item_sizes_;      //!< Max work-items sizes in each dimension
    cl_ulong total_local_memory_;           //!< Max local memory allowed
    cl_ulong total_global_memory_;          //!< Max global memory allowed
    cl_uint max_compute_units_;             //!< Max compute units
    cl_uint vector_width_short_;            //!< Ideal short vector size
    cl_uint vector_width_float_;            //!< Ideal float vector size
    cl_uint max_clock_freq_;                //!< Maximum clock frequency
    cl_bool image_support_;                 //!< image support
    vcl_size_t image2d_max_width_;          //!< Ideal float vector size
    vcl_size_t image2d_max_height_;         //!< Ideal float vector size
    vcl_string device_vendor_; 
    vcl_string extensions_supported_; 
    vcl_string platform_name_; 
};

//:  output stream
vcl_ostream& operator <<(vcl_ostream &s, bocl_device_info& info);

#endif
