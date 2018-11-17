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
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include "../boxm2_vecf_articulated_scene.h"
#include "../boxm2_vecf_articulated_params.h"
#include "boxm2_vecf_ocl_transform_scene.h"
typedef vnl_vector_fixed<float,4> float4;

class boxm2_vecf_ocl_head_model : public boxm2_vecf_articulated_scene
{
 public:
  boxm2_vecf_ocl_head_model(std::string const& scene_file,const bocl_device_sptr& device,const boxm2_opencl_cache_sptr& opencl_cache, bool optimize = false, std::string color_apm_ident = "frontalized");


  //: map eye data to the target scene
  void map_to_target(boxm2_scene_sptr target_scene) override;

  void clear_target(boxm2_scene_sptr target_scene) override;

  void set_scale(vgl_vector_3d<double> scale);

  void set_intrinsic_change( bool change){intrinsic_change_ = change;}

  bool set_params(boxm2_vecf_articulated_params const& params) override{return true;} //blank for now
  bool optimize_;
friend class boxm2_vecf_ocl_appearance_extractor; //the appearance extractor needs to signal a change to the original model when its apm is updated

  void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) override ;
  int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) override ;
  bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const override;
  bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) override ;

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
