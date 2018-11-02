#pragma once
#include <iostream>
#include <boxm2/boxm2_data_traits.h>
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_vector_fixed.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/io/boxm2_lru_cache.h>
#include <boxm2/boxm2_util.h>
#include <bvpl/kernels/bvpl_kernel.h>

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef vnl_vector_fixed<uchar, 16> uchar16;
typedef vnl_vector_fixed<uchar, 8> uchar8;
typedef vnl_vector_fixed<ushort, 4> ushort4;
typedef vnl_vector_fixed<int, 16> int16;

class boxm2_ocl_refine_scene_around_geometry{

public:
  boxm2_ocl_refine_scene_around_geometry(boxm2_scene_sptr scene,
                                         boxm2_opencl_cache_sptr cache,
                                         bocl_device_sptr device,
                                         bvpl_kernel_vector_sptr filter_vector,
                                         int num_times,
                                         float p_thresh,
                                         bool refine_gpu) :
    p_thresh_(p_thresh),
    refine_gpu_(refine_gpu),
    filter_vector_(filter_vector),
    num_times_(num_times),
    cache_(cache),
    scene_(scene),
    device_(device)
  {
  std::vector<std::string> valid_types;
  valid_types.emplace_back("boxm2_mog6_view_compact");
  valid_types.emplace_back("boxm2_mog3_grey");
  valid_types.emplace_back("boxm2_gauss_rgb_view");
  if (!boxm2_util::verify_appearance(*scene_,valid_types,app_type_,app_type_size_))
    {
    std::cout << "scene doesn't have the correct appearance type - "
             << "only boxm2_gauss_rgb_view and mog6_view compact allowed!!" << std::endl;
    }
  this->compile_kernel();
  }

  bool refine();
  float p_thresh_;
  bool refine_gpu_;
  bvpl_kernel_vector_sptr filter_vector_;

private:

  bool label_cells_for_refinement();
  int num_times_;
  bool compile_kernel();
  bool refine_cpp();
  bool refine_gpu();
  int appTypeSize_;
  boxm2_opencl_cache_sptr cache_;
  boxm2_scene_sptr scene_;
  bocl_device_sptr device_;
  std::vector<bocl_kernel*> kerns_;
  std::string app_type_;
  int app_type_size_;

};
