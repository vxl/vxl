#ifndef boxm2_ocl_refine_h_
#define boxm2_ocl_refine_h_
//: \file
// \brief  GPU accelerated refinement of a boxm2 scene
//
// \author Andrew Miller
// \date Jan 6, 2012
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: refine class
class boxm2_ocl_refine
{
  public:

    static int refine_scene( bocl_device_sptr device,
                                  boxm2_scene_sptr scene,
                                  boxm2_opencl_cache_sptr cache,
                                  float thresh);
  private:

    //compile kernels and place in static map
    static bocl_kernel* get_refine_tree_kernel(bocl_device_sptr device, vcl_string opts="" );
    static bocl_kernel* get_refine_data_kernel(bocl_device_sptr device, vcl_string opts="" );

    //map of paint kernel by device
    static vcl_map<vcl_string, bocl_kernel*> tree_kernels_, data_kernels_;

    //return option string by datasize
    static vcl_string get_option_string(int datasize);
};

#endif //boxm2_ocl_refine_h_
