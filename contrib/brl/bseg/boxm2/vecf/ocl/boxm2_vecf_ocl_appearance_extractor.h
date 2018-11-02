#pragma once
#include <iostream>
#include <algorithm>
#include <boxm2/vecf/boxm2_vecf_articulated_scene.h>
#include <boxm2/vecf/boxm2_vecf_appearance_extractor.h>
#include <boxm2/vecf/boxm2_vecf_orbit_scene.h>
#include <boxm2/vecf/ocl/boxm2_vecf_ocl_composite_head_model.h>
#include <boxm2/io/boxm2_lru_cache.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
class boxm2_vecf_ocl_appearance_extractor{
 public:
  boxm2_vecf_ocl_appearance_extractor(boxm2_scene_sptr target_scene, boxm2_vecf_ocl_composite_head_model& source_model):scene_transformer_(target_scene,source_model.opencl_cache_,"",source_model.color_apm_id_,false,true),head_model_(source_model)
  {

    target_scene_ = target_scene;
    device_ = source_model.device_;
    opencl_cache_ = source_model.opencl_cache_;
#ifdef  USE_ORBIT_CL
  this->compile_kernels();
#endif
  }
  void extract_appearance(bool faux=false,bool individual_color=true,bool head_only=false){
    this->faux_ = faux;
    this->individual_appearance_ = individual_color;
    this->head_only_ = head_only;
    vis_cells_.clear();
    this->extract_head_appearance();
    if(!head_only)
      this->extract_orbit_appearance();
    head_model_.intrinsic_change_ = true;
  }
  uchar8 rgb2yuv(uchar8& color) {
    int R = static_cast<int>(color[0]);
    int G = static_cast<int>(color[1]);
    int B = static_cast<int>(color[2]);
    uchar8 ret_val; ret_val.fill(0);
    int   Y = ( (  66 * R + 129 * G +  25 * B + 128) >> 8) +  16;
    int   U = ( ( -38 * R -  74 * G + 112 * B + 128) >> 8) + 128;
    int   V = ( ( 112 * R -  94 * G -  18 * B + 128) >> 8) + 128;

    ret_val[0] = (unsigned char) Y; ret_val[1] = (unsigned char) U ; ret_val[2] = (unsigned char) V;
    return ret_val;
  }
#define clip(val) std::min ( 255, std::max (0, val) );
  uchar8 yuv2rgb(uchar8& color) {
    int Y = static_cast<int>(color[0]);
    int U = static_cast<int>(color[1]);
    int V = static_cast<int>(color[2]);
    int C = Y - 16;
    int D = U - 128;
    int E = V - 128;
    uchar8 ret_val; ret_val.fill(0);

   int R = clip(( 298 * C           + 409 * E + 128) >> 8);
   int G = clip(( 298 * C - 100 * D - 208 * E + 128) >> 8);
   int B = clip(( 298 * C + 516 * D           + 128) >> 8);

    ret_val[0] = (unsigned char) B; ret_val[1] = (unsigned char) G ; ret_val[2] = (unsigned char) R;
    return ret_val;
  }


 private:
  bool faux_,individual_appearance_,head_only_;
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
  void compile_kernels();
  bool compute_mean_anatomical_appearance(bool is_right);
  bool extract_appearance_one_pass(bool is_right);
  float8 weight_intesities(float& vis_a,float& vis_b,float8 int_A,float8 int_B, float8 mean,float8 meanB);
  bool extract_data(boxm2_scene_sptr scene,boxm2_block_id& block,float * &alpha,gray_APM* &gray_app, color_APM* &color_app);
  boxm2_vecf_ocl_transform_scene scene_transformer_;
  color_APM to_apm_t(float8 apm_f){
    color_APM apm; apm.fill(0);
    apm[0] =(unsigned char) apm_f[0]; apm[1] = (unsigned char) apm_f[1] ; apm[2] = (unsigned char) apm_f[2];
    return apm;
  }

  float8 to_float8(color_APM& apm){
    float8 apm_f; apm_f.fill(0);
    apm_f[0] = (float) apm[0]; apm_f[1] =(float) apm[1] ; apm_f[2] =(float) apm[2];
    return apm_f;
  }
  cl_command_queue queue_;
  boxm2_scene_sptr target_scene_;
  color_APM left_upper_lid_app_, right_upper_lid_app_, right_lower_lid_app_,left_lower_lid_app_, left_eyelid_crease_app_, right_eyelid_crease_app_, left_iris_app_,right_iris_app_, left_sclera_app_,right_sclera_app_,left_pupil_app_,right_pupil_app_;
  float8 total_sclera_app_,total_pupil_app_,total_iris_app_,total_upper_lid_app_,total_lower_lid_app_,total_eyelid_crease_app_;
  float vis_sclera_,vis_pupil_,vis_iris_,vis_upper_lid_,vis_lower_lid_,vis_eyelid_crease_;
  float max_t_color;
  vis_score_t * current_vis_score_;
  boxm2_vecf_ocl_composite_head_model& head_model_;
  std::vector<unsigned> vis_cells_;
  boxm2_opencl_cache_sptr opencl_cache_;
  bocl_device_sptr device_;
  std::vector<bocl_kernel*> kernels_;
};
