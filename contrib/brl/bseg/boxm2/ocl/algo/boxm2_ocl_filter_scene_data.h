#pragma once
#include <iostream>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_util.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <boct/boct_bit_tree.h>
class boxm2_ocl_filter_scene_data{
public:
        boxm2_ocl_filter_scene_data()= default;
        boxm2_ocl_filter_scene_data(boxm2_opencl_cache_sptr opencl_cache, bocl_device_sptr device,
                bvpl_kernel_vector_sptr filter_vector,std::string appType,std::size_t appTypeSize,cl_command_queue* queue){
                filter_vector_=filter_vector;
                opencl_cache_=opencl_cache;
                device_=device;
                appType_=appType;
                appTypeSize_=appTypeSize;
                scene_=opencl_cache_->get_cpu_cache()->get_scenes()[0];
                this->compile_kernels();
                queue_=queue;

        }
        bool apply_filter(int i);

private:
        void compile_kernels();
        bvpl_kernel_vector_sptr filter_vector_;
        boxm2_opencl_cache_sptr opencl_cache_;
        bocl_device_sptr device_;
        std::string appType_;
        std::size_t appTypeSize_;
        boxm2_scene_sptr scene_;
        std::vector<bocl_kernel*> kernels;
        cl_command_queue* queue_;
};
