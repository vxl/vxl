#ifndef boxm2_ocl_filter_process_h_
#define boxm2_ocl_filter_process_h_
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <bprb/bprb_func_process.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vcl_where_root_dir.h>


bool boxm2_ocl_filter_process_cons(bprb_func_process& pro);
bool boxm2_ocl_filter_process(bprb_func_process& pro);

namespace boxm2_ocl_filter_process_globals
{
    constexpr unsigned n_inputs_ = 3;
    constexpr unsigned n_outputs_ = 0;

    void compile_filter_kernel(const bocl_device_sptr& device,bocl_kernel * refine_data_kernel);
    static std::map<std::string,bocl_kernel* > kernels;
}

#endif // boxm2_ocl_filter_process_h_
