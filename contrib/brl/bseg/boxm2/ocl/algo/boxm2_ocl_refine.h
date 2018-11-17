#ifndef boxm2_ocl_refine_h_
#define boxm2_ocl_refine_h_
//: \file
// \brief  GPU accelerated refinement of a boxm2 scene
//
// \author Andrew Miller
// \date Jan 6, 2012
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

//: refine class
class boxm2_ocl_refine {
public:
  static int refine_scene(const bocl_device_sptr& device, const boxm2_scene_sptr& scene,
                          const boxm2_opencl_cache_sptr& cache, float thresh);

  static int refine_scene(const bocl_device_sptr& device, boxm2_scene_sptr scene,
                          const boxm2_opencl_cache_sptr& opencl_cache, float thresh,
                          std::vector<std::string> prefixes);

private:
  // compile kernels and place in static map
  static bocl_kernel *get_refine_tree_kernel(const bocl_device_sptr& device,
                                             const std::string& opts = "");
  static bocl_kernel *get_refine_data_kernel(const bocl_device_sptr& device,
                                             const std::string& opts = "");

  // map of paint kernel by device
  static std::map<std::string, bocl_kernel *> tree_kernels_, data_kernels_;

  // return option string by datasize
  static std::string get_option_string(int datasize);
};

#endif // boxm2_ocl_refine_h_
