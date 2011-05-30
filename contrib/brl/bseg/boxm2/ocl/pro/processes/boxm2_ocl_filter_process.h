#ifndef boxm2_ocl_filter_process_h_
#define boxm2_ocl_filter_process_h_
#include <bprb/bprb_func_process.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <vcl_where_root_dir.h>


bool boxm2_ocl_filter_process_cons(bprb_func_process& pro);
bool boxm2_ocl_filter_process(bprb_func_process& pro);

namespace boxm2_ocl_filter_process_globals
{
    const unsigned n_inputs_ = 3;
    const unsigned n_outputs_ = 0;

    void compile_filter_kernel(bocl_device_sptr device,bocl_kernel * refine_data_kernel); 
    static vcl_map<vcl_string,bocl_kernel* > kernels;
}

#endif // boxm2_ocl_filter_process_h_
