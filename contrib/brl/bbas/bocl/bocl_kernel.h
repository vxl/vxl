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
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstddef.h> // for std::size_t
#include "bocl_cl.h"
#include "bocl_mem.h"
#include "bocl_utils.h"

//:  High level wrapper for an Opencl cl_kernel object.
//  keeps ref to the context and device_id for which this kernel is created
//  can create a kernel from source and device like the following:
//  bocl_kernel render(context, device_id, vcl_vector<string> render_srcs, "render_kernel", "-DNVIDIA", "render_kernel");
//  for each arg... render.set_arg(bocl_mem_buffer)
//  for each local arg... render.set_local_arg(bocl_mem_buffer)
//  render.execute(cmdQueue, localThreads, globalThreads);
//
//  NOTE: local memory arguments must come after bocl_mem arguments
//        this could be fixed with a vcl_map<bocl_mem*, vcl_size_t> object
//        that just interprets null keys as local memory args instead of the two
//        vcl_vectors currently implemented

class bocl_kernel
{
  public:

    bocl_kernel() : kernel_(0), program_(0) {}
    ~bocl_kernel();

    //: create kernel from a list of sources, and a kernel name, and an ID
    bool create_kernel( cl_context* context,
                        cl_device_id* device,
                        vcl_vector<vcl_string> src_paths,
                        vcl_string const& kernel_name,
                        vcl_string options,
                        vcl_string id);

    //: execute this kernel on given command queue with given workspace size
    bool execute(cl_command_queue& cmdQueue, vcl_size_t* localThreads, vcl_size_t* globalThreads);

    //: set a bocl_mem buffer arg (pushes it onthe back
    bool set_arg(bocl_mem* buffer);

    //: sets a local arg of size "size"
    bool set_local_arg(vcl_size_t size);

    bool clear_args() {
      args_.clear();
      local_args_.clear();
      return true;
    }

    //: will return a reference to the cl_kernel this class wraps
    cl_kernel& kernel() { return kernel_; }

    //: returns the number of arguments that you've given this kernel
    int arg_cnt() { return args_.size() + local_args_.size(); }

    //: returns GPU time
    float exec_time();

  private:

    //: cl kernel object
    cl_kernel kernel_;

    //: cl program object
    cl_program program_;

    //: kernels event object
    cl_event  ceEvent_;

    //: pointers to bocl_mem buffers that this kernel was most recently passed
    vcl_vector<bocl_mem*> args_;

    //: list of local args
    vcl_vector<vcl_size_t> local_args_;

    //: ID for error printing
    vcl_string id_;

    //: source string
    vcl_string prog_;

    //: OpenCL context (reference)
    cl_context* context_;

    //: OpenCL device (reference)
    cl_device_id* device_;

    //: for creating kernels from a list of sources
    bool load_kernel_source(vcl_string const& path);
    bool append_process_kernels(vcl_string const& path);
    bool build_kernel_program(cl_program &program, vcl_string options);
};

#endif
