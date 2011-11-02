#ifndef boxm2_ocl_paint_batch_h_
#define boxm2_ocl_paint_batch_h_
//:
// \file
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//boxm2 includes
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/io/boxm2_cache.h>
#include <boxm2/io/boxm2_stream_cache.h>

//bsta sigma normalizer...
#include <bsta/algo/bsta_sigma_normalizer.h>

//: boxm2_ocl_paint_batch class
class boxm2_ocl_paint_batch
{
  public:
    static void paint_block( boxm2_scene_sptr           scene,
                             bocl_device_sptr           device,
                             boxm2_stream_cache_sptr    str_cache,
                             boxm2_opencl_cache_sptr    opencl_cache,
                             cl_command_queue           &queue,
                             vcl_string                 data_type,
                             boxm2_block_id             id,
                             bsta_sigma_normalizer_sptr n_table ); 

  private:

    //compile kernels and place in static map
    static bocl_kernel* compile_kernels( bocl_device_sptr device, vcl_string opts="" ); 

    //map of paint kernel by device
    static vcl_map<vcl_string, bocl_kernel*> kernels_; 

    static void weighted_mean_var(float* obs, float* vis, int numSamples, float& mean, float& var); 

};

#endif // boxm2_ocl_paint_batch_h_
