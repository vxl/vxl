#ifndef boxm2_multi_update_h
#define boxm2_multi_update_h
//:
// \file
#include <boxm2_multi_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <brdb/brdb_value.h>


//: boxm2_multi_cache - example realization of abstract cache class
class boxm2_multi_update
{
  public:
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;

    //multi render header
    static float update(boxm2_multi_cache& cache, const vil_image_view<float>& img, vpgl_camera_double_sptr cam );

  private:

    //-------------------------------------------------------------------
    //static opencl kernel compilation functions
    //-------------------------------------------------------------------
    //map keeps track of all kernels compiled and cached
    static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels_;

    //compile kernels and cache
    static vcl_vector<bocl_kernel*>& get_kernels(bocl_device_sptr device, vcl_string opts);
};

#endif
