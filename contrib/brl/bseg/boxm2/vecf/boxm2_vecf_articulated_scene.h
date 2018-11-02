#pragma once
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <cstddef>
#include <boxm2/boxm2_scene.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_block.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/boxm2_data.h>
#include <boxm2/boxm2_data_base.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>

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
typedef vnl_vector_fixed<unsigned char, 16> uchar16;

// structure for prerefining the target scene
struct unrefined_cell_info{
unrefined_cell_info():linear_index_(0), ix_(0), iy_(0), iz_(0){}
  unsigned linear_index_;
  unsigned ix_;
  unsigned iy_;
  unsigned iz_;
  vgl_point_3d<double> pt_;
};

class boxm2_vecf_articulated_scene;
#define LERP(w1,w2,p,p1,p2) (w1 * (p2 - p) + w2 * (p-p1))/(p2 - p1)
typedef vbl_smart_ptr<boxm2_vecf_articulated_scene> boxm2_vecf_articulated_scene_sptr;

class boxm2_vecf_articulated_scene : public vbl_ref_count{
 public:
 boxm2_vecf_articulated_scene(): blk_(nullptr), alpha_base_(nullptr), app_base_(nullptr),nobs_base_(nullptr), alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr),
    target_alpha_base_(nullptr),target_app_base_(nullptr),target_nobs_base_(nullptr), target_alpha_data_(nullptr), target_app_data_(nullptr),
    target_nobs_data_(nullptr), target_blk_(nullptr),sigma_(0.5f), source_model_exists_(false), base_model_(nullptr),
    has_background_(false), is_single_instance_(true), color_apm_id_("frontalized"), target_data_extracted_(false){
  }

 boxm2_vecf_articulated_scene(std::string scene_file,std::string color_apm_id = "frontalized"):
  blk_(nullptr), alpha_base_(nullptr), app_base_(nullptr), nobs_base_(nullptr), alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr), target_alpha_base_(nullptr),
    target_app_base_(nullptr),target_nobs_base_(nullptr), target_alpha_data_(nullptr), target_app_data_(nullptr), target_nobs_data_(nullptr), target_blk_(nullptr),
    is_single_instance_(true), sigma_(0.5f),source_model_exists_(false), has_background_(false), color_apm_id_(std::move(color_apm_id)),
    target_data_extracted_(false){
    base_model_ = new boxm2_scene(scene_file);
  }
  // member methods
  void extract_target_block_data(boxm2_scene_sptr target_scene);
  void extract_source_block_data();
  boxm2_scene_sptr scene(){return base_model_;}

  std::string color_apm_id() {return color_apm_id_;}

  static  double8 interp_generic_double8(vgl_point_3d<double>* neighbors, double8* probs, vgl_point_3d<double> p );
  static  double interp_generic_double(vgl_point_3d<double>* neighbors, const double* probs, vgl_point_3d<double> p );
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
  // functions required by the sub classes
  virtual bool set_params(boxm2_vecf_articulated_params const& params)=0;
  virtual void map_to_target(boxm2_scene_sptr target_scene)=0;
  virtual void inverse_vector_field_unrefined(std::vector<vgl_point_3d<double> > const& unrefined_target_pts) = 0;
  virtual int prerefine_target_sub_block(vgl_point_3d<double> const& sub_block_pt, unsigned pt_index) = 0;
  virtual bool inverse_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const = 0;
  virtual bool apply_vector_field(cell_info const& target_cell, vgl_vector_3d<double> const& inv_vf) = 0;

  // functions that may be implemented by subclass or by *this
  virtual void set_target_background(bool has_background){ has_background_ = has_background;}
  virtual void clear_target(boxm2_scene_sptr target_scene);
  virtual void prerefine_target(boxm2_scene_sptr target_scene);
  virtual bool coupled_vector_field(vgl_point_3d<double> const& target_pt, vgl_vector_3d<double>& inv_vf) const{return false;}

  // can be overriden by structural root anatomy component (e.g. head)
  // accounts for global mapping to a specific subject
  virtual void extract_unrefined_cell_info();
  //: tree subblock size in mm
 double subblock_len() const { if(blk_)return (blk_->sub_block_dim()).x(); return 0.0;}

 std::size_t target_linear_index(std::size_t ix, std::size_t iy, std::size_t iz) const{
   return (ix*targ_n_.y()+iy)*targ_n_.z() + iz;
 }
 vgl_box_3d<double> source_bounding_box() const {return source_bb_;}
 protected:
  float sigma_;
  static double gauss(double d, double sigma);
  bool has_background_;
  bool is_single_instance_;
  bool source_model_exists_;
  boxm2_scene_sptr base_model_;

  std::string color_apm_id_;

  // source block information
  boxm2_block_sptr blk_;              // the source block
  boxm2_block_id blk_id_;
  vgl_box_3d<double> source_bb_; // source bounding box
  ///// treat as read only !!
  ///// refers to source tree array (can't initialize a ref to a const)
  ///// but use reference for efficiency
  boxm2_array_3d<uchar16> trees_;
  ////////

  // block meta info
  float alpha_init_;
  vgl_vector_3d<unsigned> n_;
  vgl_vector_3d<double> dims_;
  vgl_point_3d<double> origin_;
  // source dbs
  boxm2_data_base* alpha_base_;
  boxm2_data_base* app_base_;
  boxm2_data_base* nobs_base_;
 // standard source data buffers
  boxm2_data<BOXM2_ALPHA>::datatype* alpha_data_;  // source alpha data
  boxm2_data<BOXM2_MOG3_GREY>::datatype* app_data_;// source appearance data
  boxm2_data<BOXM2_NUM_OBS>::datatype* nobs_data_; // source nobs data

  // target block information
  bool target_data_extracted_;
  boxm2_block_sptr target_blk_;              // the target block
  vgl_vector_3d<unsigned> targ_n_;
  vgl_vector_3d<double> targ_dims_;
  vgl_point_3d<double> targ_origin_;
 // target dbs
  boxm2_data_base* target_alpha_base_;
  boxm2_data_base* target_app_base_;
  boxm2_data_base* target_nobs_base_;
  boxm2_data<BOXM2_ALPHA>::datatype* target_alpha_data_;   //target alpha data
  boxm2_data<BOXM2_MOG3_GREY>::datatype* target_app_data_; //target appearance data
  boxm2_data<BOXM2_NUM_OBS>::datatype* target_nobs_data_;  //target nobs data

  // unrefined vector field info
  // computed by each component over the target tree cells
  std::vector<vgl_vector_3d<double> > vfield_unrefined_;
  std::vector<bool> valid_unrefined_;

  // unrefined target tree cell info
  // supplied by top level structural anatomical component
  std::vector<unrefined_cell_info> unrefined_cell_info_;

};
