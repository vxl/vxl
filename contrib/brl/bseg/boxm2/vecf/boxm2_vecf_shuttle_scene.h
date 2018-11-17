#ifndef boxm2_vecf_shuttle_scene_h_
#define boxm2_vecf_shuttle_scene_h_
//:
// \file
// \brief  A model of the space shuttle for testing particle filtering
//
// \author J.L. Mundy
// \date   16 Jan 2016
//
#include <iostream>
#include <string>
#include <vector>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_data.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vpgl/vpgl_camera_double_sptr.h>
class boxm2_vecf_particle{
 public:
 boxm2_vecf_particle(): p_(0.0f), cam_(nullptr), tx_(0.0), ty_(0.0), s_(0.0),
    rx_(0.0),ry_(0.0),rz_(0.0){}
 boxm2_vecf_particle(float p, vpgl_camera_double_sptr const& cam): p_(p), cam_(cam),
    tx_(0.0), ty_(0.0), s_(0.0), rx_(0.0),ry_(0.0),rz_(0.0){}
  bool operator () (boxm2_vecf_particle const& p0, boxm2_vecf_particle const& p1);
  void print() { std::cout << "(" << tx_ << ' ' << ty_ << ' ' << s_ << ' ' << rx_ << ' ' << ry_ << ' ' << rz_ << " p: " << p_ << std::endl;}
  void print_txty() { std::cout << '{' << tx_ << ',' << ty_ << ',' << p_ << '}'<< std::endl;}
  void print_txs() { std::cout << '{' << tx_ << ',' << s_ << ',' << p_ << '}'<< std::endl;}
  void print_txry() { std::cout << '{' << tx_ << ',' << ry_ << ',' << p_ << '}'<< std::endl;}
  void print_txrz() { std::cout << '{' << tx_ << ',' << rz_ << ',' << p_ << '}'<< std::endl;}
  float p_;
  vpgl_camera_double_sptr cam_;
  double tx_;
  double ty_;
  double s_;
  double rx_;
  double ry_;
  double rz_;
};

class boxm2_vecf_shuttle_scene : public boxm2_scene
{
 public:
 boxm2_vecf_shuttle_scene(): alpha_data_(nullptr), app_data_(nullptr), nobs_data_(nullptr){}

  boxm2_vecf_shuttle_scene(std::string const& scene_file, std::string const& imesh_file);

  boxm2_vecf_shuttle_scene(std::string const& scene_file);

  boxm2_scene_sptr scene(){ return base_model_;}

  vil_image_view<float> silhouette(vpgl_camera_double_sptr const& cam, unsigned ni, unsigned nj) const;
  void generate_particles(const vpgl_camera_double_sptr& cam, double txmin, double txmax, double dtx);
  void generate_particles_txy(const vpgl_camera_double_sptr& cam,
                          double txmin, double txmax, double dtx,
                          double tymin, double tymax, double dty);
  void generate_particles_txs(vpgl_camera_double_sptr cam,
                          double txmin, double txmax, double dtx,
                          double smin, double smax, double ds);
  void generate_particles_txry(const vpgl_camera_double_sptr& cam,
                               double txmin, double txmax, double dtx,
                               double rymin, double rymax, double dry);
  void generate_particles_txrz(const vpgl_camera_double_sptr& cam,
                               double txmin, double txmax, double dtx,
                               double rzmin, double rzmax, double drz);
  void evaluate_particles(vil_image_view<vxl_byte> const& observed_img, vgl_box_2d<int> const& bkg_box);
  void save_silhouettes(std::string const& base_dir);
  void save_cameras(std::string const& base_dir);
 private:
  boxm2_scene_sptr base_model_;
  //:scan spherical shell
  //: set up pointers to source block databases
  void extract_block_data();
  boxm2_block* blk_;                     // the source block
  boxm2_block* target_blk_;              // the target block
  boxm2_data<BOXM2_ALPHA>* alpha_data_;  // source alpha database
  boxm2_data<BOXM2_MOG3_GREY>* app_data_;// source appearance database
  boxm2_data<BOXM2_NUM_OBS>* nobs_data_; // source nobs database
  std::vector<boxm2_vecf_particle> particles_;
  std::vector<vil_image_view<float> > silhouettes_;
  double bk_var_;
  double bk_mean_;
  double total_var_;
  double total_mean_;
  double log_p_uniform_;
};
#endif // boxm2_vecf_shuttle_scene_h_
