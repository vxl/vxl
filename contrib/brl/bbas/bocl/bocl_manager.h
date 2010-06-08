// This is brl/bbas/bocl/bocl_manager.h
#ifndef bocl_manager_h_
#define bocl_manager_h_
//:
// \file
// \brief
//  A parent class for singleton opencl managers
// \author J. Mundy
// \date November 13, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_iostream.h>
#include <vcl_string.h>
#include "bocl_cl.h"
#include <vcl_cstddef.h>
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
//#define GROUP_SIZE 64
#define VECTOR_SIZE 4


template <class T>
class bocl_manager
{
 protected:
  vcl_size_t number_devices_;
  vcl_size_t max_work_group_size_;   //!< Max allowed work-items in a group
  cl_uint max_dimensions_;           //!< Max group dimensions allowed
  vcl_size_t * max_work_item_sizes_; //!< Max work-items sizes in each dimension
  cl_ulong total_local_memory_;      //!< Max local memory allowed
  cl_ulong total_global_memory_;     //!< Max global memory allowed
  cl_uint max_compute_units_;        //!< Max compute units
  cl_uint vector_width_short_;       //!< Ideal short vector size
  cl_uint vector_width_float_;       //!< Ideal float vector size
  cl_uint max_clock_freq_;           //!< Maximum clock frequency
  cl_bool image_support_;            //!< image support
  cl_context context_;               //!< CL context
  cl_device_id *devices_;            //!< CL device list
  vcl_size_t image2d_max_width_;       //!< Ideal float vector size
  vcl_size_t image2d_max_height_;       //!< Ideal float vector size

 public:

  //: Destructor
  virtual ~bocl_manager();

  //: Use this instead of constructor
  static T* instance();

  //: Initialise the opencl environment
  void clear_cl();

  //: Initialise the opencl environment
  bool initialize_cl();

  //: Check for error returns
  int check_val(cl_int status, cl_int result, std::string message) {
    if (status != result) {
      vcl_cout << message << '\n';
      return 0;
    }
    return 1;
  }
  vcl_size_t group_size() const {return max_work_group_size_;}
  cl_ulong total_local_memory() const {return total_local_memory_;}
  cl_context context() {return context_;}
  cl_device_id * devices() {return devices_;}

  //: Allocate host memory for use with clCreateBuffer (aligned if necessary)
  void* allocate_host_mem(vcl_size_t size);
  bool load_kernel_source(vcl_string const& path);
  bool append_process_kernels(vcl_string const& path);
  bool write_program(vcl_string const& path);
  vcl_string program_source() const {return prog_;}

  cl_bool image_support(){return image_support_;}
 protected:

  //: Constructor
  bocl_manager() : devices_(0),max_work_item_sizes_(0) {}

  static T* instance_;

  vcl_string prog_;

};

#endif // bocl_manager_h_
