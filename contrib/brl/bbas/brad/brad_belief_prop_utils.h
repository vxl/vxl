#ifndef __BRAD_BELIEF_PROP_UTILS_H
#define __BRAD_BELIEF_PROP_UTILS_H
//-----------------------------------------------------------------------------
//:
// \file
// \brief Utilities to study probabilistic volumetric modeling based on belief propagation
//
//
// \author J. L. Mundy
// \date April 7, 2016
//
//----------------------------------------------------------------------------
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include "brad_appearance_neighborhood_index.h"
struct brad_cell_data{
brad_cell_data()
    : vis_(1.0)
    , pre_(0.0)
    , post_(0.0)
    , init_ratio_(1.0)
    , mog3_(vnl_vector_fixed<unsigned char , 8>(static_cast<unsigned char>(0)))
    , nobs_(vnl_vector_fixed<float , 4>(0.0f))
  {}
  double cell_z_;
  double vis_;
  double PinS_;
  double pI_;
  double pre_;
  double post_;
  double init_ratio_;
  vnl_vector_fixed<unsigned char, 8> mog3_;
  vnl_vector_fixed<float, 4> nobs_;
  void print() const{ std::cout << "z: " << cell_z_ << " pI: " << pI_ << " pre: " << pre_<< " post: " << post_ << " vis: " << vis_ << " PinS: " << PinS_ << '\n';}
};
struct brad_ray{
brad_ray(): dim_(1.0), min_z_(0.0), max_z_(10.0){}
  double dim_;
  double min_z_;
  double max_z_;
  std::vector<brad_cell_data> ray_data_;
  void init_ray(){
    unsigned n = static_cast<unsigned>((max_z_-min_z_)/dim_);
    double nm1 = 1.0/static_cast<double>(n);
    ray_data_.clear();
    ray_data_.resize(n);
    double vis = 1.0;
    for(unsigned i=n; i>=1; i--){
      double z = i*dim_;
      ray_data_[i-1].cell_z_ = z+min_z_;
      ray_data_[i-1].vis_ = vis;
      double p = nm1/vis;
      ray_data_[i-1].PinS_ = p;
      vis *= (1.0-p);
      ray_data_[i-1].pI_ = 0.0;
      ray_data_[i-1].init_ratio_= (1.0-p)/p;
    }
  }
  void print () const{
    unsigned n = static_cast<unsigned>((max_z_-min_z_)/dim_);
    std::cout << "Zray (" << min_z_ << " --> " << max_z_ << "):dim = " << dim_ << '\n';
    for(unsigned i=n; i>=1; i--){
      std::cout << "cell[" << i << "]:";
         ray_data_[i-1].print();}
  }
};
  class brad_belief_prop_utils
{
 public:
 brad_belief_prop_utils()= default;
  void set_metadata(std::vector<brad_image_metadata_sptr> const& metadata){
    app_index_ = brad_appearance_neighborhood_index(metadata);
  }
  void set_images(std::vector<vil_image_view<float> > const& imgs){imgs_ = imgs;}
  void set_cams(std::vector<vpgl_camera_double_sptr> const& cams){ cams_ = cams;}
  //for debug purposes
  void project_intensities(vgl_point_3d<double> const& p);
  void print_intensities() const;
  void test_appearance_update();
  unsigned most_nadir_view() const{return app_index_.most_nadir_view();}
  std::vector<unsigned> index(unsigned indx) const;
  vgl_vector_3d<double> view_dir(unsigned indx) const{return app_index_.view_dir(indx);}
  std::vector<double> ref_intensities(std::vector<unsigned> const& ref_indices, vgl_point_3d<double> const& p) const;
  bool pixel_intensity(vil_image_view<float> const& img, vpgl_camera_double_sptr const& cam, vgl_point_3d<double> p, float& I) const;
  void update_mog(std::vector<double> const& intens, vnl_vector_fixed<unsigned char, 8> & mog3,
                  vnl_vector_fixed<float, 4>& nobsn, bool fixed_sigma = false) const;
  double p_mog(double intensity, vnl_vector_fixed<unsigned char, 8> & mog3) const;
  void print_mog(vnl_vector_fixed<unsigned char, 8> const& mog3, vnl_vector_fixed<float, 4> const& nobs) const;
  void init_zray(double Iray, std::vector<unsigned> const& ref_indices, vgl_point_3d<double> const& p, double dzmin = -20.0, bool fixed_sigma=false);
  void zray_pre();
  void zray_post();
  void update_PinS();
  void update_vis();
  double expected_depth() const;
  void compute_depth_map(vgl_box_2d<double>const& region, double cell_len, double z0);
  bool save_depth_map(std::string const& depth_path) const;
  void print_zray() const {zray_.print();}
  void print_zray(unsigned ni) const;
  bool force_single_index(unsigned indx);

 private:
  brad_ray zray_;
  std::vector<vil_image_view<float> > imgs_;
  std::vector<vpgl_camera_double_sptr> cams_;
  brad_appearance_neighborhood_index app_index_;
  std::map<unsigned, double> intensity_;
  std::map<unsigned, std::vector<double> > nbr_intensities_;
  vil_image_view<float> depth_;
};


#endif
