// This is brl/bbas/bocl/bocl_manager.h
#ifndef bocl_manager_h_
#define bocl_manager_h_
//:
// \file
// \brief A parent class for singleton opencl managers
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
#include "bocl_device_info.h"
#include <vcl_cstddef.h>
#if !defined(__APPLE__)
#include <malloc.h>
#endif
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
//#define GROUP_SIZE 64
#define VECTOR_SIZE 4

template <class T>
class bocl_manager
{
 public:
  cl_context context_;                        //!< GPU context
  cl_context cpu_context_;                    //!< CPU context

  //: Destructor
  virtual ~bocl_manager();

  //: Use this instead of constructor
  static T* instance();

  //: Queries found platforms, creates a list of CPU and GPU devices
  bool initialize_cl();

  //: initialize context from a device
  cl_context create_context(cl_device_id* device, int num_devices);

  //: Initialise the opencl environment
  void clear_cl();

  //: available devices
  cl_device_id* devices() {return devices_;}
  cl_device_id* cpus() {return cpus_;}
  cl_device_id* gpus() {return gpus_;}
  int numCPUs() { return numCPUs_; }
  int numGPUs() { return numGPUs_; }

  //get for current manager information..
  cl_context& context() {return context_;}
  vcl_size_t group_size() const {return curr_info_.max_work_group_size_;}
  cl_ulong total_local_memory() const {return  curr_info_.total_local_memory_;}
  cl_bool image_support(){return curr_info_.image_support_;}
  vcl_size_t image2d_max_width(){ return curr_info_.image2d_max_width_; }
  vcl_size_t image2d_max_height(){ return curr_info_.image2d_max_height_; }
  vcl_string platform_name(){ return curr_info_.platform_name_; }
  cl_device_type device_type() { return curr_info_.device_type_; }

 protected:

  //: Constructor
  bocl_manager() :
    context_(0),
    cpu_context_(0),
    devices_(0),
    gpus_(0),
    cpus_(0),
    numGPUs_(0),
    numCPUs_(0) {}

  //Singleton instance of the manager
  static T* instance_;

  //: OpenCL Current Device Info
  bocl_device_info curr_info_;
  //: OpenCL number of devices
  vcl_size_t number_devices_;
  //: OpenCL list of devices associated with the current context
  cl_device_id* devices_;

  //store gpus and cpus
  cl_device_id* gpus_;              //!< CL GPU device list
  unsigned    numGPUs_;
  cl_device_id* cpus_;              //!< CL CPU device list
  unsigned    numCPUs_;


  //////////////////////////////////////////////////////////////////////////////
  // OLD helper methods/ deprecated
  //////////////////////////////////////////////////////////////////////////////
  //Malloc and Free Helper methods
  bool free_buffer(void* buffer);
  bool create_buffer(void** buffer,vcl_string type,int elm_size,int length);
    //: program source
  vcl_string prog_;

 public:
  //: Allocate host memory for use with clCreateBuffer (aligned if necessary)
  void* allocate_host_mem(vcl_size_t size);
  bool load_kernel_source(vcl_string const& path);
  bool append_process_kernels(vcl_string const& path);
  bool write_program(vcl_string const& path);
  vcl_string program_source() const {return prog_;}

  //build kernel program:
  int build_kernel_program(cl_program & program, vcl_string options);
};

#endif // bocl_manager_h_
