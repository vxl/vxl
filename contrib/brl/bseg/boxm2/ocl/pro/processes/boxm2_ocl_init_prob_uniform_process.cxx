// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_init_prob_uniform_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for initializing uniform probability along Z.
//
// \author Vishal Jain
// \date Mar 30, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_init_prob_uniform_process_globals
{
    constexpr unsigned n_inputs_ = 3;
    constexpr unsigned n_outputs_ = 0;
    std::size_t local_threads[2] = { 8, 8 };
    static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
    std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts)
    {
        std::string identifier = device->device_identifier() + opts;
        if (kernels_.find(identifier) != kernels_.end())
            return kernels_[identifier];
        std::vector<bocl_kernel*> vec_kernels;
        //gather all render sources... seems like a lot for rendering...
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "pixel_conversion.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "backproject.cl");
        src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");
        src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
        //set kernel options
        std::string options = "-D INIT_UNIFORM_PROB -D STEP_CELL=step_cell_init_prob(aux_args,data_ptr,tfar-tblockfixed,d)";
        //have kernel construct itself using the context and device
        auto * ray_trace_kernel = new bocl_kernel();
        ray_trace_kernel->create_kernel(&device->context(),
                                         device->device_id(),
                                         src_paths,
                                         "init_prob_main",   //kernel name
                                         options,              //options
                                         "boxm2 opencl initialize probability"); //kernel identifier (for error checking)
        vec_kernels.push_back(ray_trace_kernel);
        kernels_[identifier] = vec_kernels;
        return kernels_[identifier];
    }
}

bool boxm2_ocl_init_prob_uniform_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_init_prob_uniform_process_globals;
    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_init_prob_uniform_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_init_prob_uniform_process_globals;
    if (pro.n_inputs() < n_inputs_) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
        return false;
    }
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    //get the inputs
    unsigned i = 0;
    bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    vgl_box_3d<double> bbox = scene->bounding_box();
    //get x and y size from scene
    std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    float xint = 0.0f;
    float yint = 0.0f;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        float num_octree_cells = std::pow(2.0f, (float)mdata.max_level_ - 1);
        xint = mdata.sub_block_dim_.x() / num_octree_cells;
        yint = mdata.sub_block_dim_.y() / num_octree_cells;
    }
    auto ni = (unsigned int)std::ceil(bbox.width() / xint);
    auto nj = (unsigned int)std::ceil(bbox.height() / yint);
    std::cout << "Size of the image " << ni << ',' << nj << std::endl;
    float z = bbox.max_z();
    std::string identifier = device->device_identifier();

    //: create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(), *(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE, &status);
    if (status != 0)return false;

    std::vector<bocl_kernel*>& kernels = get_kernels(device, "");
    float scene_origin[4];
    scene_origin[0] = bbox.min_x();
    scene_origin[1] = bbox.min_y();
    scene_origin[2] = bbox.min_z();
    scene_origin[3] = 1.0;
    unsigned cl_ni = RoundUp(ni, local_threads[0]);
    unsigned cl_nj = RoundUp(nj, local_threads[1]);
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    float ray_dx = 0, ray_dy = 0, ray_dz = -1;
    // initialize ray origin buffer, first and last return buffers
    int count = 0;
    for (unsigned int j = 0; j<cl_nj; ++j) {
        for (unsigned int i = 0; i<cl_ni; ++i) {
            int count4 = count * 4;
            ray_origins[count4 + 0] = scene_origin[0] + ((float)i + 0.15f)*(xint);
            ray_origins[count4 + 1] = scene_origin[1] + ((float)j + 0.15f)*(yint);
            ray_origins[count4 + 2] = z + 1.0f;
            ray_origins[count4 + 3] = 0.0;
            ray_directions[count4 + 0] = 0.0;
            ray_directions[count4 + 1] = 0.0;
            ray_directions[count4 + 2] = -1.0;
            ray_directions[count4 + 3] = 0.0;
            ++count;
        }
    }
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_directions, "ray_directions buffer");
    ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = ni;
    img_dim_buff[3] = nj;
    bocl_mem_sptr exp_img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int) * 4, "image dims");
    exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output = new bocl_mem(device->context(), output_arr, sizeof(float) * 100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    //2. set workgroup size
    std::size_t lThreads[] = { 8, 8 };
    std::size_t gThreads[] = { cl_ni, cl_nj };
    float subblk_dim = 0.0;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern = kernels[0];
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        bocl_mem* prob_init = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("prob_init"), 0, false);
        bocl_mem * blk_info = opencl_cache->loaded_block_info();
        transfer_time += (float)transfer.all();
        ////3. SET args
        kern->set_arg(blk_info);
        kern->set_arg(blk);
        kern->set_arg(prob_init);
        kern->set_arg(ray_o_buff.ptr());
        kern->set_arg(ray_d_buff.ptr());
        kern->set_arg(exp_img_dim.ptr());
        kern->set_arg(cl_output.ptr());
        kern->set_arg(lookup.ptr());
        kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar16));
        kern->set_local_arg(local_threads[0] * local_threads[1] * 10 * sizeof(cl_uchar));
        //execute kernel
        kern->execute(queue, 2, lThreads, gThreads);
        clFinish(queue);
        gpu_time += kern->exec_time();
        prob_init->read_to_buffer(queue);
        // clear render kernel args so it can reset em on next execution
        kern->clear_args();
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("prob_init"), true);
    }
    clReleaseCommandQueue(queue);

    delete[] ray_origins;
    delete[] ray_directions;

    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    return true;
}
