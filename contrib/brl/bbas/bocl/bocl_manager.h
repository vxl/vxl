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
#include "bocl_device.h"
#include "bocl_device_info.h"
#include <vcl_cstddef.h>
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
  static T* instance();

  //: Queries found platforms, creates a list of CPU and GPU devices
  bool initialize_cl();

  //: Initialise the opencl environment
  void clear_cl();

  //: available devices
  vcl_vector<bocl_device*> gpus_;
  vcl_vector<bocl_device*> cpus_;
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
  vcl_size_t group_size()         const { return curr_device_->info().max_work_group_size_; }
  cl_ulong total_local_memory()   const { return curr_device_->info().total_local_memory_; }
  cl_bool image_support()         const { return curr_device_->info().image_support_; }
  vcl_size_t image2d_max_width()  const { return curr_device_->info().image2d_max_width_; }
  vcl_size_t image2d_max_height() const { return curr_device_->info().image2d_max_height_; }
  vcl_string platform_name()      const { return curr_device_->info().platform_name_; }
  cl_device_type device_type()    const { return curr_device_->info().device_type_; }
  //////////////////////////////////////////////////////////////////////////////

 protected:

  //: Constructor
  bocl_manager() {}

  //Singleton instance of the manager
  static T* instance_;


////////////////////////////////////////////////////////////////////////////////
// OLD helper methods/ deprecated
////////////////////////////////////////////////////////////////////////////////
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
  vcl_string program_source() const { return prog_; }

  //: initialize context from a device
  cl_context create_context(cl_device_id* device, int num_devices);

  //build kernel program:
  int build_kernel_program(cl_program & program, vcl_string options);
};

class bocl_manager_child: public bocl_manager<bocl_manager_child>, public vbl_ref_count
{
 public:
  bocl_manager_child() : bocl_manager<bocl_manager_child>() {}
  ~bocl_manager_child() {}
};

typedef vbl_smart_ptr<bocl_manager_child> bocl_manager_child_sptr;
//: Binary write boxm2_scene scene to stream
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child const& scene);
void vsl_b_write(vsl_b_ostream& os, const bocl_manager_child* &p);
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child_sptr& sptr);
void vsl_b_write(vsl_b_ostream& os, bocl_manager_child_sptr const& sptr);

//: Binary load boxm2_scene scene from stream.
void vsl_b_read(vsl_b_istream& is, bocl_manager_child &scene);
void vsl_b_read(vsl_b_istream& is, bocl_manager_child* p);
void vsl_b_read(vsl_b_istream& is, bocl_manager_child_sptr& sptr);
void vsl_b_read(vsl_b_istream& is, bocl_manager_child_sptr const& sptr);
#endif // bocl_manager_h_
