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

#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cstddef>
#include "bocl_cl.h"
#include "bocl_device.h"
#include "bocl_device_info.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#if !defined(__APPLE__)
#include <malloc.h>
#endif
#define SDK_SUCCESS 0
#define SDK_FAILURE 1
//#define GROUP_SIZE 64
#define VECTOR_SIZE 4

//makes a bocl_mem a sptr
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <vsl/vsl_binary_io.h>

template <class T>
class bocl_manager
{
 public:

  //: Destructor
  virtual ~bocl_manager();

  //: Use this instead of constructor
  static T& instance();

  //: available devices
  std::vector<bocl_device_sptr> gpus_;
  std::vector<bocl_device_sptr> cpus_;
  int numCPUs() const { return cpus_.size(); }
  int numGPUs() const { return gpus_.size(); }

  //////////////////////////////////////////////////////////////////////////////
  //: current device (defaults to last GPU), used for old methods
  bocl_device_sptr curr_device_;
  cl_device_id* devices() { return curr_device_->device_id(); }

  //get for current manager information..
  cl_context context_;
  cl_context& context() { return context_; }

  //: current device info...
  std::size_t group_size()         const { return curr_device_->info().max_work_group_size_; }
  cl_ulong total_local_memory()   const { return curr_device_->info().total_local_memory_; }
  cl_bool image_support()         const { return curr_device_->info().image_support_; }
  std::size_t image2d_max_width()  const { return curr_device_->info().image2d_max_width_; }
  std::size_t image2d_max_height() const { return curr_device_->info().image2d_max_height_; }
  std::string platform_name()      const { return curr_device_->info().platform_name_; }
  cl_device_type device_type()    const { return curr_device_->info().device_type_; }
  //////////////////////////////////////////////////////////////////////////////

  static bool is_instantiated() { return instantiated_; }

  //: Clear the opencl environment smart pointers
  void clear_cl();

 protected:

  //: Constructor
  bocl_manager();

  //: Queries found platforms, creates a list of CPU and GPU devices
  bool initialize_cl();

////////////////////////////////////////////////////////////////////////////////
// OLD helper methods/ deprecated
////////////////////////////////////////////////////////////////////////////////
  //Malloc and Free Helper methods
  bool free_buffer(void* buffer);
  bool create_buffer(void** buffer,std::string type,int elm_size,int length);
    //: program source
  std::string prog_;

  static bool instantiated_;

 public:
  //: Allocate host memory for use with clCreateBuffer (aligned if necessary)
  void* allocate_host_mem(std::size_t size);
  bool load_kernel_source(std::string const& path);
  bool append_process_kernels(std::string const& path);
  bool write_program(std::string const& path);
  std::string program_source() const { return prog_; }

  //: initialize context from a device
  cl_context create_context(cl_device_id* device, int num_devices);

  //build kernel program:
  int build_kernel_program(cl_program & program, std::string options);

 private:
  // prevent users from making copies of the singleton.
  //: Copy constructor
  bocl_manager(bocl_manager<T> const& og) {}
  //: assignment operator
  bocl_manager& operator = (bocl_manager<T> const& rhs) {return *this;}
};

class bocl_manager_child: public bocl_manager<bocl_manager_child>
{
  // friend class to allow the constructor to be called
  friend class bocl_manager<bocl_manager_child>;
 private:
  bocl_manager_child() : bocl_manager<bocl_manager_child>() {}
  ~bocl_manager_child() override = default;
};

//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child const& scene);
void vsl_b_write(vsl_b_ostream& os, const bocl_manager_child* &p);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bocl_manager_child &scene);
void vsl_b_read(vsl_b_istream& is, bocl_manager_child* p);
#endif // bocl_manager_h_
