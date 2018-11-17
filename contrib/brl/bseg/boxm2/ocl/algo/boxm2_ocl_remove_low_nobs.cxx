// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_remove_low_nobs.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_remove_low_nobs.h"
//:
// \file
// \brief  An open cl function for clearing cells with a low number of observations
//
// \author J.L. Mundy
// \date March 12, 2016

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

std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_remove_low_nobs::remove_low_nobs_kernels_;

bool boxm2_ocl_remove_low_nobs::remove_low_nobs(const boxm2_scene_sptr&         scene,
                                                const bocl_device_sptr&         device,
                                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                                float                    nobs_threshold_multiplier)

{
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    std::size_t local_threads[1] = { 64 };
    std::size_t global_threads[1] = { 64 };
    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout << "remove nobs MBs in cache: " << binCache / (1024.0*1024.0) << std::endl;
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
    bocl_kernel * kern = get_remove_low_nobs_kernels(device)[0];
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // histogram of total number of observations of each cell
    unsigned nbins = 50;
    std::vector<float> hist(nbins,0.0f);
    short interval= 2;
    for (id = blks_order.begin(); id != blks_order.end(); ++id){
      bocl_mem* nobs  = opencl_cache->get_data<BOXM2_NUM_OBS>(scene, *id);
      if(!nobs){
        std::cout << "Fatal! - no number of observation data assigned for block " << *id << '\n';
        return false;
      }
      unsigned nobs_size = static_cast<unsigned>(boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix()));
      unsigned n = static_cast<unsigned>(nobs->num_bytes())/nobs_size;
      auto* nobs_ptr = (cl_short4*) nobs->cpu_buffer();
      //upcount the matched histogram bins
      for(unsigned i = 0; i<n; ++i){
        // number of observations for each mog component
        short n0 = nobs_ptr[i].s[0], n1 = nobs_ptr[i].s[0], n2 = nobs_ptr[i].s[0];
        short v = n0 + n1 + n2;
        bool found = false;
        for(unsigned k = 0; k<nbins&&!found; k++){
          short bin = static_cast<short>(k+1)*interval;
          if(v<=bin){
            hist[k]+=1.0f;
            found = true;
          }
        }
      }
    }
    // normalize histogram
    float sum = 0.0f;
    for(unsigned i = 0; i< nbins; ++i)
      sum += hist[i];

    // find mean, but could be a more complex function of P(Nobs).
    float mean = 0.0f;
    for(unsigned i = 0; i< nbins; ++i){
      float bincnt = hist[i];
      hist[i] = bincnt/sum;
      mean += static_cast<float>(interval*i)*hist[i];
    }
    // compute threshold on nobs
    // nobs_threshold is multiple of mean
    float nobs_threshold = mean*nobs_threshold_multiplier;

    // display the histogram
    std::cout << "Histogram of nobs: mean = " << mean << " nobs threshold = " << nobs_threshold << '\n';
    std::cout << "Nobs  p(Nobs) \n";
    for(unsigned i = 0; i< nbins; ++i)
      if(hist[i]>0.0f)
        std::cout << i*interval << ' ' << hist[i] << '\n';
    std::cout << '\n';
    //
    // set up memory for kernel argument, nobs_threshold
    //
    cl_float num_obs_thresh[1];
    num_obs_thresh[0] = nobs_threshold;
    bocl_mem_sptr nobs_thresh_mem = new bocl_mem(device->context(), num_obs_thresh, sizeof(num_obs_thresh), "update alpha bool buffer");
    nobs_thresh_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    for (id = blks_order.begin(); id != blks_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        bocl_mem* nobs  = opencl_cache->get_data<BOXM2_NUM_OBS>(scene, *id);

        auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
        blk_info->write_to_buffer((queue));
        //set workspace
        std::size_t ltr[] = { 4, 4, 4 };
        std::size_t gtr[] = { RoundUp(mdata.sub_block_num_.x(), static_cast<int>(ltr[0])),
                             RoundUp(mdata.sub_block_num_.y(), static_cast<int>(ltr[1])),
                             RoundUp(mdata.sub_block_num_.z(), static_cast<int>(ltr[2])) };
        kern->set_arg(blk_info);
        kern->set_arg(blk);
        kern->set_arg(alpha);
        kern->set_arg(nobs);
        kern->set_arg(nobs_thresh_mem.ptr());
        kern->set_arg(lookup.ptr());
        kern->set_local_arg(ltr[0] * ltr[1] * ltr[2] * 10 * sizeof(cl_uchar));
        kern->set_local_arg(ltr[0] * ltr[1] * ltr[2] * sizeof(cl_uchar16));

        //execute kernel
        kern->execute(queue, 3, ltr, gtr);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "REMOVE LOW NOBS FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        alpha->read_to_buffer(queue);
        kern->clear_args();
    }
    clFinish(queue);

    return true;
}



std::vector<bocl_kernel*>& boxm2_ocl_remove_low_nobs::get_remove_low_nobs_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (remove_low_nobs_kernels_.find(identifier) != remove_low_nobs_kernels_.end())
        return remove_low_nobs_kernels_[identifier];

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

    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;
    std::string options = "-D REMOVE_LOW_NOBS";
    auto* remove_low_nobs = new bocl_kernel();
    const std::string& remove_low_nobs_opts = options;
    remove_low_nobs->create_kernel(&device->context(), device->device_id(), src_paths, "remove_low_nobs_main", remove_low_nobs_opts, "update::remove_low_nobs");
    vec_kernels.push_back(remove_low_nobs);
    //store and return
    remove_low_nobs_kernels_[identifier] = vec_kernels;
    return remove_low_nobs_kernels_[identifier];
}
