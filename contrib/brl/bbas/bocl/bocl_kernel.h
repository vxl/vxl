#ifndef bocl_kernel_h_
#define bocl_kernel_h_
//:
// \file
// \brief  A wrapper class for cl_kernel objects for OpenCL
// \author Gamze Tunali gtunali@brown.edu
// \date  October 20, 2010
//
// \verbatim
//  Modifications
//   Andrew Miller - 7 Dec 2010 - moved to BOCL, added some more functionality
// \endverbatim
#include <string>
#include <vector>
#include <iostream>
#include <cstddef>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "bocl_cl.h"
#include "bocl_utils.h"

class bocl_mem;

//:  High level wrapper for an Opencl cl_kernel object.
//  - keeps ref to the context and device_id for which this kernel is created
//  - can create a kernel from source and device like the following:
//    bocl_kernel render(context, device_id, std::vector<string> render_srcs, "render_kernel", "-DNVIDIA", "render_kernel");
//  - for each arg... render.set_arg(bocl_mem_buffer)
//  - for each local arg... render.set_local_arg(bocl_mem_buffer)
//  - render.execute(cmdQueue, localThreads, globalThreads);
//
//  NOTE: local memory arguments must come after bocl_mem arguments;
//        this could be fixed with a std::map<bocl_mem*, std::size_t> object
//        that just interprets null keys as local memory args instead of the two
//        std::vectors currently implemented

class bocl_kernel
{
 public:

  bocl_kernel() : kernel_(nullptr), program_(nullptr) {}
  ~bocl_kernel();

  //: create kernel from a list of sources, and a kernel name, and an ID
  bool create_kernel( cl_context* context,
                      cl_device_id* device,
                      std::vector<std::string> src_paths,
                      std::string const& kernel_name,
                      std::string options,
                      std::string id);

  //: create kernel from string source
  bool create_kernel( const cl_context& context,
                      cl_device_id* device,
                      std::string const& src,
                      std::string const& kernel_name,
                      std::string options,
                      const std::string& id );

  //: execute this kernel on given command queue with given workspace size
  bool execute(const cl_command_queue& cmd_queue, cl_uint dim,
               std::size_t* local_threads,
               std::size_t* global_threads,
               std::size_t* global_offsets=nullptr);

  //: set a bocl_mem buffer arg (pushes it on the back)
  bool set_arg(bocl_mem* buffer);

  //: sets a local arg of size "size"
  bool set_local_arg(std::size_t size);

  bool clear_args() { args_.clear(); local_args_.clear(); return true; }

  //: will return a reference to the cl_kernel this class wraps
  cl_kernel& kernel() { return kernel_; }

  //: returns the string ID
  std::string id() const { return id_; }

  //: returns the number of arguments that you've given this kernel
  int arg_cnt() const { return (int)args_.size() + (int)local_args_.size(); }

  //: calls clReleaseEvent on ceEvent_
  bool release_current_event();

  ////////////////////////////////////////////////////////////////////////////
  // Profiling functions

  //: returns GPU time in miliseconds
  float exec_time();
  //: returns local memory used by a single instance of kernel
  unsigned long local_mem_size();
  //: returns maximal workgroup size for this kernel
  std::size_t workgroup_size();
  //: returns a string of the build log for this program.
  std::string build_log();
  //: returns program binary associated with kernel (NVIDIA case returns PTX string)
  std::string program_binaries();

  //: return program source code - makes interpreting build errors easier
  std::string source() { return prog_; }

 private:

  //: cl kernel object
  cl_kernel kernel_;

  //: cl program object
  cl_program program_;

  //: kernels event object
  cl_event  ceEvent_;

  //: pointers to bocl_mem buffers that this kernel was most recently passed
  std::vector<bocl_mem*> args_;

  //: list of local args
  std::vector<std::size_t> local_args_;

  //: ID for error printing
  std::string id_;

  //: source string
  std::string prog_;

  //: OpenCL context (reference)
  cl_context* context_;

  //: OpenCL device (reference)
  cl_device_id* device_;

  //: for creating kernels from a list of sources
  bool load_kernel_source(std::string const& path);
  bool append_process_kernels(std::string const& path);
  bool build_kernel_program(cl_program &program, const std::string& options);
};

#endif
