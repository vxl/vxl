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

    void compile_filter_kernel(bocl_device_sptr device,bocl_kernel * refine_data_kernel)
    {
        vcl_vector<vcl_string> src_paths;
        vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "basic/linked_list.cl");
        src_paths.push_back(source_dir + "basic/sort_vector.cl");
        src_paths.push_back(source_dir + "bit/filter_block.cl"); 

        vcl_string options = " -D LIST_TYPE=float4 "; 
        refine_data_kernel->create_kernel( &device->context(), device->device_id(),
                                           src_paths, "filter_block", options,
                                           "boxm2 ocl filter kernel");
    }

    static vcl_map<vcl_string,bocl_kernel* > kernels;
}

#endif // boxm2_ocl_filter_process_h_
