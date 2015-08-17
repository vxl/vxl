#pragma once
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_orbit_scene.h>
#include <boxm2/vecf/boxm2_vecf_composite_head_model.h>
#include <boxm2/io/boxm2_lru_cache.h>
typedef  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype gray_APM;
typedef  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype color_APM;
typedef  boxm2_data_traits<BOXM2_VIS_SCORE>::datatype vis_score_t;
typedef vnl_vector_fixed<float,8> float8;
typedef vnl_vector_fixed<unsigned char,8> uchar8;

static vcl_string gray_APM_prefix = boxm2_data_traits<BOXM2_MOG3_GREY>::prefix();
static vcl_string color_APM_prefix = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix();
static vcl_string vis_score_prefix = boxm2_data_traits<BOXM2_VIS_SCORE>::prefix();

class boxm2_vecf_appearance_extractor{
 public:
 boxm2_vecf_appearance_extractor(boxm2_scene_sptr target_scene, boxm2_vecf_composite_head_model& source_model):head_model_(source_model){
    target_scene_ = target_scene;
  }
  void extract_appearance(bool faux=false){
    this->faux_ = faux;
    vis_cells_.clear();
    this->extract_head_appearance();
    this->extract_orbit_appearance();
    head_model_.intrinsic_change_ = true;
  }
 private:
  bool faux_;
  void extract_orbit_appearance();
  void extract_head_appearance();
  void extract_eye_appearance(bool is_right , bool extract);
  void extract_lower_lid_appearance(bool is_right , bool extract);
  void extract_upper_lid_appearance(bool is_right , bool extract);
  void extract_eyelid_crease_appearance(bool is_right , bool extract);
  void extract_pupil_appearance(bool is_right , bool extract);
  void extract_iris_appearance(bool is_right , bool extract);
  void bump_up_vis_scores();
  void reset(bool is_right);
  bool extract_data(boxm2_scene_sptr scene,boxm2_block_id& block,float * &alpha,gray_APM* &gray_app, color_APM* &color_app);

  color_APM to_apm_t(float8& apm_f){
    color_APM apm; apm.fill(0);
    apm[0] =(unsigned char) apm_f[0]; apm[1] = (unsigned char) apm_f[1] ; apm[2] = (unsigned char) apm_f[2];
    return apm;
  }

  float8 to_float8(color_APM& apm){
    float8 apm_f; apm_f.fill(0);
    apm_f[0] = (float) apm[0]; apm_f[1] =(float) apm[1] ; apm_f[2] =(float) apm[2];
    return apm_f;
  }
  boxm2_scene_sptr target_scene_;
  color_APM left_upper_lid_app_, right_upper_lid_app_, right_lower_lid_app_,left_lower_lid_app_, left_eyelid_crease_app_, right_eyelid_crease_app_, left_iris_app_,right_iris_app_, left_sclera_app_,right_sclera_app_,left_pupil_app_,right_pupil_app_;
  vis_score_t * current_vis_score_;
  boxm2_vecf_composite_head_model& head_model_;
  vcl_vector<unsigned> vis_cells_;
};
