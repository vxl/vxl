#pragma once
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_orbit_scene.h>
#include <boxm2/vecf/boxm2_vecf_composite_head_model.h>
#include <boxm2/boxm2_scene.h>
typedef  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype color_APM;
typedef  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype gray_APM;

static vcl_string gray_APM_prefix = boxm2_data_traits<BOXM2_MOG3_GREY>::prefix();
static vcl_string color_APM_prefix = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix();

class boxm2_vecf_appearance_extractor{
 public:
 boxm2_vecf_appearance_extractor(boxm2_scene_sptr target_scene, boxm2_vecf_composite_head_model& source_model):head_model_(source_model){
    target_scene_ = target_scene;
  }
  void extract_appearance(){
    this->extract_head_appearance();
    this->extract_orbit_appearance();
    head_model_.intrinsic_change_ = true;
  }
 private:
  void extract_orbit_appearance();
  void extract_head_appearance();
  void extract_eye_appearance(bool is_right);
  void extract_lower_lid_appearance(bool is_right);
  void extract_upper_lid_appearance(bool is_right);
  void extract_eyelid_crease_appearance(bool is_right);
  bool extract_data(boxm2_scene_sptr scene,boxm2_block_id& block,float * &alpha,gray_APM* &gray_app, color_APM* &color_app);
  boxm2_scene_sptr target_scene_;
  boxm2_vecf_composite_head_model& head_model_;
};
