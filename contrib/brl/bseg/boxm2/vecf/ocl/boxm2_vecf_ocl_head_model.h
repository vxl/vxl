#ifndef boxm2_vecf_ocl_head_model_h_
#define boxm2_vecf_ocl_head_model_h_
//:
// \file
// \brief  boxm2_vecf_ocl_head_model static model of human head (no eyes)
//
#include <iostream>
#include <string>
#include <vector>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <vcl_compiler.h>
#include "../boxm2_vecf_articulated_scene.h"
#include "../boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_ocl_transform_scene.h"
typedef vnl_vector_fixed<float,4> float4;

class boxm2_vecf_ocl_head_model : public boxm2_vecf_articulated_scene
{
 public:
  boxm2_vecf_ocl_head_model(std::string const& scene_file,bocl_device_sptr device,boxm2_opencl_cache_sptr opencl_cache, bool optimize = false, std::string color_apm_ident = "frontalized");


  //: map eye data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene);

  void clear_target(boxm2_scene_sptr target_scene);

  void set_scale(vgl_vector_3d<double> scale);

  void set_intrinsic_change( bool change){intrinsic_change_ = change;}

  virtual bool set_params(boxm2_vecf_articulated_params const& params){return true;} //blank for now
  bool optimize_;
friend class boxm2_vecf_ocl_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated
 protected:
  bool intrinsic_change_;
  vgl_vector_3d<double> scale_;
  boxm2_opencl_cache_sptr opencl_cache_;
  boxm2_vecf_ocl_transform_scene scene_transformer_;
  bocl_device_sptr device_;
private:
  bool get_data(boxm2_scene_sptr scene, boxm2_block_id const& blk_id,
                boxm2_data_base **alpha_data,
                boxm2_data_base **app_data,
                boxm2_data_base **nobs_data);
};
#endif // boxm2_vecf_ocl_head_model_h_
