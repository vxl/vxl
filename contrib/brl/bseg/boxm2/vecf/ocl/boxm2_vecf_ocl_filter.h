#ifndef boxm2_vecf_ocl_filter_h
#define boxm2_vecf_ocl_filter_h
//:
// \file
// \brief A function to filter a block using the six neigborhood
// \author J.L. Mundy
// \date October 25, 2014
//
#include <iostream>
#include <vector>
#include <vbl/vbl_ref_count.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/io/boxm2_cache.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>

//: Map a scene with Euclidean and anisotropic scale transforms.
// the input transform is the inverse so that the target scene voxels
// are mapped backwards to extract the data from the source
// thus each target voxel is defined if the inverse transform lands on a
// valid source voxel.
class boxm2_vecf_ocl_filter
{
 public:
  //: Constructor.
  boxm2_vecf_ocl_filter(boxm2_scene_sptr& source_scene,
                                 boxm2_scene_sptr& temp_scene,
                                 const boxm2_opencl_cache_sptr& ocl_cache);

  ~boxm2_vecf_ocl_filter();


  //: transform a scene of arbitrary size, block by block
  // no interpolation
  bool filter(std::vector<float> const& weights, unsigned num_iterations = 1);

 protected:
  bool compile_filter_kernel();
  bool init_ocl_filter();
  bool get_scene_appearance(const boxm2_scene_sptr& scene,
                            std::string&      options);

  boxm2_opencl_cache_sptr  opencl_cache_;
  boxm2_scene_sptr temp_scene_;
  boxm2_scene_sptr source_scene_;
  bocl_device_sptr device_;
  int apptypesize_;//size of the appearance model in bytes
  std::string app_type_; //type of appearance

  //filter kernel and args
  bocl_kernel * filter_kern;
  bocl_mem_sptr centerX;
  bocl_mem_sptr centerY;
  bocl_mem_sptr centerZ;
  bocl_mem_sptr lookup;
  bocl_mem_sptr output;

  bocl_mem* ocl_depth;
  bocl_mem* ocl_n_iter;
  bocl_mem* blk_info_temp;
  bocl_mem* blk_info_source;
  boxm2_scene_info* info_buffer;
  bocl_mem* blk_temp;
  bocl_mem* alpha_temp;
  bocl_mem * mog_temp;
  boxm2_scene_info* info_buffer_source;
  bocl_mem* blk_source;
  bocl_mem* alpha_source;
  bocl_mem* mog_source;
  bocl_mem* filter_weights;
  cl_command_queue queue;
};

#endif
