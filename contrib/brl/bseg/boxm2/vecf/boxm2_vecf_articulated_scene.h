#pragma once
#include <boxm2/boxm2_scene.h>
#include <vcl_compiler.h>
#include <iostream>
#include <string>
#include "boxm2_vecf_articulated_params.h"
#include <boxm2/boxm2_data_traits.h>
//smart pointer
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//:
// \file
// \brief  Parent class for articulated anatomy scenes
//
// \author O. T. Biris
// \date   2 Jul 2015
//
typedef  boxm2_data_traits<BOXM2_MOG3_GREY>::datatype gray_APM;
typedef  boxm2_data_traits<BOXM2_GAUSS_RGB>::datatype color_APM;
typedef vnl_vector_fixed<double,8> double8;
typedef vnl_vector_fixed<unsigned char,8> uchar8;
class boxm2_vecf_articulated_scene;
#define LERP(w1,w2,p,p1,p2) (w1 * (p2 - p) + w2 * (p-p1))/(p2 - p1)
typedef vbl_smart_ptr<boxm2_vecf_articulated_scene> boxm2_vecf_articulated_scene_sptr;

class boxm2_vecf_articulated_scene : public vbl_ref_count{
 public:
  boxm2_vecf_articulated_scene(){ base_model_ = 0; has_background_ = false; is_single_instance_ = false; color_apm_id_ = "frontalized";}

  boxm2_vecf_articulated_scene(std::string scene_file,std::string color_apm_id = "frontalized"){
    base_model_ = new boxm2_scene(scene_file);
    has_background_ = false;
    color_apm_id_=color_apm_id;
}
  virtual bool set_params(boxm2_vecf_articulated_params const& params)=0;
  virtual void map_to_target(boxm2_scene_sptr target_scene)=0;
  virtual void set_target_background(bool has_background){ has_background_ = has_background;}
  virtual void clear_target(boxm2_scene_sptr target_scene);
  boxm2_scene_sptr scene(){return base_model_;}
  std::string color_apm_id() {return color_apm_id_;}

static  double8 interp_generic_double8(vgl_point_3d<double>* neighbors, double8* probs, vgl_point_3d<double> p );
static  double interp_generic_double(vgl_point_3d<double>* neighbors, double* probs, vgl_point_3d<double> p );
  double8 to_double8(color_APM& apm){
    double8 apm_f; apm_f.fill(0);
    apm_f[0] = (double) apm[0]; apm_f[1] =(double) apm[1] ; apm_f[2] =(double) apm[2];
    return apm_f;
  }
  color_APM to_apm_t(double8& apm_f){
    color_APM apm; apm.fill(0);
    apm[0] =(unsigned char) apm_f[0]; apm[1] = (unsigned char) apm_f[1] ; apm[2] = (unsigned char) apm_f[2];
    return apm;
  }


 protected:
  static double gauss(double d, double sigma);
  bool has_background_;
  bool is_single_instance_;
  boxm2_scene_sptr base_model_;
  std::string color_apm_id_;
};
