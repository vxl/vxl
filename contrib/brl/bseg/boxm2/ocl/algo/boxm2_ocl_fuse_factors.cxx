// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_fuse_factors.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_fuse_factors.h"
//:
// \file
// \brief  A process for combining the factors
//
// \author Vishal Jain
// \date Nov 24, 2015

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>

#include <vil/vil_image_view.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: Map of kernels should persist between process executions

std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_fuse_factors::fuse_factors_kernels_;
bool boxm2_ocl_fuse_factors::fuse_factors(const boxm2_scene_sptr&         scene,
                                          const bocl_device_sptr&         device,
                                          const boxm2_opencl_cache_sptr&  opencl_cache,
                                          std::vector<std::string>   factors_ident,
                                          std::vector<float>   weights)
{
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    std::size_t local_threads[1] = { 64 };
    std::size_t global_threads[1] = { 64 };
    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout << "Update MBs in cache: " << binCache / (1024.0*1024.0) << std::endl;
    // create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                    *(device->device_id()),
                                                    CL_QUEUE_PROFILING_ENABLE,
                                                    &status);
    if (status != 0)
        return false;
    std::vector<boxm2_block_id> blks_order;
    blks_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator  id;
    // compile the kernel if not already compiled
    //: Initialize Cumulative factor
    bocl_kernel * kern = get_fuse_factors_kernels(device)[0];
    float weight_buf[] = { 0.0 };
    bocl_mem * weight = new bocl_mem(device->context(), weight_buf, sizeof(float), "weight buffer");

    for (id = blks_order.begin(); id != blks_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
        blk_info->write_to_buffer((queue));
        local_threads[0] = 64;
        global_threads[0] = RoundUp(info_buffer->data_buffer_length, local_threads[0]);
        //grab an appropriately sized AUX data buffer
        bocl_mem *aux0_cum = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("cum"), 0, false);
        bocl_mem* prob_init = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("prob_init"), 0, false);
        transfer_time += (float)transfer.all();
        kern->set_arg(blk_info);
        kern->set_arg(prob_init);
        kern->set_arg(aux0_cum);
        //execute kernel
        kern->execute(queue, 1, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        aux0_cum->read_to_buffer(queue);
        kern->clear_args();
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("prob_init"), true);
    }
    kern = get_fuse_factors_kernels(device)[1];
    for (id = blks_order.begin(); id != blks_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
        blk_info->write_to_buffer((queue));
        local_threads[0] = 64;
        global_threads[0] = RoundUp(info_buffer->data_buffer_length, local_threads[0]);
        //grab an appropriately sized AUX data buffer
        bocl_mem *aux0_cum = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("cum"), 0, false);
        weight->create_buffer(CL_MEM_READ_ONLY, queue);
        for (unsigned int j = 0; j < factors_ident.size(); j++)
        {

            weight_buf[0] = weights[j];
            weight->write_to_buffer(queue);
            bocl_mem *aux0_factor = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix(factors_ident[j]), 0, false);

            kern->set_arg(blk_info);
            kern->set_arg(aux0_factor);
            kern->set_arg(aux0_cum);
            kern->set_arg(weight);
            //execute kernel
            kern->execute(queue, 1, local_threads, global_threads);
            int status = clFinish(queue);
            if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                return false;
            gpu_time += kern->exec_time();
            aux0_cum->read_to_buffer(queue);
            kern->clear_args();
        }

    }
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    kern = get_fuse_factors_kernels(device)[2];
    for (id = blks_order.begin(); id != blks_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
        blk_info->write_to_buffer((queue));
        //grab an appropriately sized AUX data buffer
        bocl_mem *aux0_cum = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("cum"), 0, false);

        //set workspace
        std::size_t ltr[] = { 4, 4, 4 };
        std::size_t gtr[] = { RoundUp(mdata.sub_block_num_.x(), ltr[0]),
                             RoundUp(mdata.sub_block_num_.y(), ltr[1]),
                             RoundUp(mdata.sub_block_num_.z(), ltr[2]) };
        kern->set_arg(blk_info);
        kern->set_arg(blk);
        kern->set_arg(alpha);
        kern->set_arg(aux0_cum);
        kern->set_arg(lookup.ptr());
        kern->set_local_arg(ltr[0] * ltr[1] * ltr[2] * 10 * sizeof(cl_uchar));
        kern->set_local_arg(ltr[0] * ltr[1] * ltr[2] * sizeof(cl_uchar16));

        //execute kernel
        kern->execute(queue, 3, ltr, gtr);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        alpha->read_to_buffer(queue);
        kern->clear_args();
        //opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_ALPHA>::prefix(), true);
    }
    clFinish(queue);

    return true;
}



std::vector<bocl_kernel*>& boxm2_ocl_fuse_factors::get_fuse_factors_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (fuse_factors_kernels_.find(identifier) != fuse_factors_kernels_.end())
        return fuse_factors_kernels_[identifier];

    //otherwise compile the kernels
    std::cout << "=== boxm2_ocl_update_process::compiling kernels on device " << identifier << "===" << std::endl;
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_kernels.cl");
    src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;

    //compilation options
    std::string options = "-D INIT_CUM";
    auto* init_cum = new bocl_kernel();
    std::string init_cum_opts = options;
    init_cum->create_kernel(&device->context(), device->device_id(), src_paths, "init_cum_main", init_cum_opts, "update::init_cum");
    vec_kernels.push_back(init_cum);

    options = "-D FUSE_FACTORS";
    auto* fusefactors = new bocl_kernel();
    std::string fusefactors_opts = options;
    fusefactors->create_kernel(&device->context(), device->device_id(), src_paths, "fuse_factors_main", fusefactors_opts, "update::fuse_factors");
    vec_kernels.push_back(fusefactors);

    options = "-D EVALUATE_ALPHA";
    auto* evaluate_alpha = new bocl_kernel();
    std::string evaluate_alpha_opts = options;
    evaluate_alpha->create_kernel(&device->context(), device->device_id(), src_paths, "evaluate_alpha_main", evaluate_alpha_opts, "update::evaluate_alpha");
    vec_kernels.push_back(evaluate_alpha);
    //store and return
    fuse_factors_kernels_[identifier] = vec_kernels;
    return fuse_factors_kernels_[identifier];
}
