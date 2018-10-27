#include "brad_belief_prop_utils.h"
#include <vpgl/vpgl_camera.h>
#include <bsta/algo/bsta_mog3_grey.h>
#include <vil/vil_save.h>
#define BRAD_INIT_SIGMA 0.1f
#define BRAD_MIN_SIGMA 0.05f
#define BRAD_FIXED_SIGMA 0.02f
//#define BRAD_INIT_SIGMA 0.2f
//#define BRAD_MIN_SIGMA 0.15f
bool brad_belief_prop_utils::pixel_intensity(vil_image_view<float> const& img, vpgl_camera_double_sptr const& cam,
                                                         vgl_point_3d<double> p, float& I) const{
  I = -1.0f;
  double ud, vd;
  cam->project(p.x(), p.y(), p.z(), ud, vd );
  int ni = static_cast<int>(img.ni()), nj = static_cast<int>(img.nj());
  int i = static_cast<int>(ud), j = static_cast<int>(vd);
  if (i<0||i>=ni||j<0||j>=nj) return false;
  I = img(i,j);
  return true;
}

void brad_belief_prop_utils::project_intensities(vgl_point_3d<double> const& p){
  intensity_.clear();
  nbr_intensities_.clear();
  const std::map<unsigned, std::vector<unsigned> >& index = app_index_.const_index();
  for(const auto & iit : index){
    unsigned img_index = iit.first;
    const std::vector<unsigned>& nbrs = iit.second;
    float Itarg = 0.0f;
    if(!pixel_intensity(imgs_[img_index], cams_[img_index], p, Itarg))
      continue;
    intensity_[img_index]=Itarg;
    for(unsigned int nbr : nbrs){
      float Inbr = 0.0f;
      if(!pixel_intensity(imgs_[nbr], cams_[nbr], p, Inbr))
        continue;
      nbr_intensities_[img_index].push_back(Inbr);
    }
  }
}

void brad_belief_prop_utils::print_intensities() const{
  auto inti = intensity_.begin();
  auto ninti = nbr_intensities_.begin();
  for(; inti!=intensity_.end(); ++inti, ++ninti){
    double tint = inti->second;
    const std::vector<double>& nbr_intens = ninti->second;
    std::cout << "t["<< inti->first <<  "]: " << tint << " n:( ";
    for(double nbr_inten : nbr_intens)
      std::cout << nbr_inten << ' ';
    std::cout << ")\n";
  }
}
void brad_belief_prop_utils::update_mog(std::vector<double> const& intens, vnl_vector_fixed<unsigned char, 8> & mog3,
                                        vnl_vector_fixed<float, 4>& nobs, bool fixed_sigma) const{
  for(double inten : intens)
      bsta_mog3_grey::update_gauss_mixture_3(mog3, nobs, static_cast<float>(inten), 1.0f, BRAD_INIT_SIGMA, BRAD_MIN_SIGMA);
  if(fixed_sigma)
    bsta_mog3_grey::force_mog3_sigmas_to_value(mog3, BRAD_FIXED_SIGMA);
}

double brad_belief_prop_utils::p_mog(double intensity, vnl_vector_fixed<unsigned char, 8> & mog3) const{
  return bsta_mog3_grey::prob_density(mog3, static_cast<float>(intensity));
}

  void brad_belief_prop_utils::print_mog(vnl_vector_fixed<unsigned char, 8> const& mog3, vnl_vector_fixed<float, 4> const&  /*nobs*/) const{
    float mu0 = static_cast<float>(mog3[0])/255.0f, mu1 = static_cast<float>(mog3[3])/255.0f, mu2 = static_cast<float>(mog3[6])/255.0f;
    float w0 = static_cast<float>(mog3[2])/255.0f, w1 = static_cast<float>(mog3[5])/255.0f, w2 = 1.0f-(w0+w1);
    float sigma0 = static_cast<float>(mog3[1])/255.0f, sigma1 = static_cast<float>(mog3[4])/255.0f, sigma2 =static_cast<float>(mog3[7])/255.0f;
    std::cout << "mog: mean(" << mu0 << ' '<< mu1 << ' '<< mu2 <<") w("<< w0 << ' '<< w1 << ' ' << w2 <<") sigma("
              << sigma0 << ' ' << sigma1 << ' ' << sigma2 << ")\n";
}

void brad_belief_prop_utils::test_appearance_update(){
  // must have called ::print_intensities first!
  auto inti = intensity_.begin();
  auto ninti = nbr_intensities_.begin();
  for(; inti!=intensity_.end(); ++inti, ++ninti){
    auto tint = static_cast<float>(inti->second);
    vnl_vector_fixed<unsigned char, 8> mog3(static_cast<unsigned char>(0));
    vnl_vector_fixed<float, 4> nobs(0.0f);
    std::vector<double>& nbr_intens = ninti->second;
    auto n_nbr = static_cast<unsigned>(nbr_intens.size());
    for(unsigned i = 0; i<n_nbr; ++i)
      bsta_mog3_grey::update_gauss_mixture_3(mog3, nobs, static_cast<float>(nbr_intens[i]), 1.0f, BRAD_INIT_SIGMA, BRAD_MIN_SIGMA);

    float mu0 = static_cast<float>(mog3[0])/255.0f, mu1 = static_cast<float>(mog3[3])/255.0f, mu2 = static_cast<float>(mog3[6])/255.0f;
    float w0 = static_cast<float>(mog3[2])/255.0f, w1 = static_cast<float>(mog3[5])/255.0f, w2 = 1.0f-(w0+w1);
    float sigma0 = static_cast<float>(mog3[1])/255.0f, sigma1 = static_cast<float>(mog3[4])/255.0f, sigma2 =static_cast<float>(mog3[7])/255.0f;
    std::cout << "Updated mog[" << inti->first << "]: mean(" << mu0 << ' '<< mu1 << ' '<< mu2 <<") w("<< w0 << ' '<< w1 << ' ' << w2 <<") sigma("
              << sigma0 << ' ' << sigma1 << ' ' << sigma2 << ")\n";
    // below is done if mog_var is >0 which is bad!!!
    //sigma0 = 0.01f*255.0f;     sigma1 = 0.01f*255.0f;     sigma2 = 0.01f*255.0f;
    //    mog3[1] = static_cast<unsigned char>(sigma0);     mog3[4] = static_cast<unsigned char>(sigma1); mog3[7] = static_cast<unsigned char>(sigma2);
    std::cout << "P(" << tint << ") = " << bsta_mog3_grey::prob_density(mog3, tint) << '\n';
  }
}
std::vector<unsigned> brad_belief_prop_utils::index(unsigned indx) const{
  return app_index_.index(indx);
}

std::vector<double> brad_belief_prop_utils::ref_intensities(std::vector<unsigned> const& ref_indices, vgl_point_3d<double> const& p) const{
  std::vector<double> ret;
  for(unsigned int ref_indice : ref_indices){
    float I = 0.0f;
    if(!pixel_intensity(imgs_[ref_indice], cams_[ref_indice], p, I))
      continue;
    ret.push_back(I);
  }
  return ret;
}
void brad_belief_prop_utils::init_zray(double Iray, std::vector<unsigned> const& ref_indices, vgl_point_3d<double> const& p,
                                       double dzmin,  bool fixed_sigma){
  std::vector<double> ref_ints = this->ref_intensities(ref_indices, p);
  zray_.min_z_ = p.z()+dzmin;
  zray_.max_z_ = p.z()+(100.0+dzmin);
  zray_.init_ray();
  auto n = static_cast<unsigned>((zray_.max_z_-zray_.min_z_)/zray_.dim_);
  for(unsigned i=n; i>=1; i--){
    double z = zray_.ray_data_[i-1].cell_z_;
    vgl_point_3d<double> pray(p.x(), p.y(), z);
    std::vector<double> ref_ints = this->ref_intensities(ref_indices, pray);
    this->update_mog(ref_ints, zray_.ray_data_[i-1].mog3_, zray_.ray_data_[i-1].nobs_, fixed_sigma);
    zray_.ray_data_[i-1].pI_ = p_mog(Iray, zray_.ray_data_[i-1].mog3_);
  }
}

void brad_belief_prop_utils::zray_pre(){
  auto n = static_cast<unsigned>((zray_.max_z_-zray_.min_z_)/zray_.dim_);
  double pre = 0.0;
  for(unsigned i=n; i>=1; i--){
    zray_.ray_data_[i-1].pre_ = pre;
    pre +=  zray_.ray_data_[i-1].pI_*zray_.ray_data_[i-1].vis_*zray_.ray_data_[i-1].PinS_;
  }
}

void brad_belief_prop_utils::zray_post(){
  auto n = static_cast<unsigned>((zray_.max_z_-zray_.min_z_)/zray_.dim_);
  double post = 0.0;
  // reverse scan for post
  for(unsigned i=1; i<=n; i++){
    double pins = zray_.ray_data_[i-1].PinS_;
    post = (1.0-pins)*post + zray_.ray_data_[i-1].pI_*pins;
    zray_.ray_data_[i-1].post_ = post;
  }
}
void brad_belief_prop_utils::update_PinS(){
auto n = static_cast<unsigned>((zray_.max_z_-zray_.min_z_)/zray_.dim_);
  // reverse scan for post
  for(unsigned i=1; i<=n; i++){
    double pins = zray_.ray_data_[i-1].PinS_;
    double pre = zray_.ray_data_[i-1].pre_;
    double post = zray_.ray_data_[i-1].post_;
    double vis =  zray_.ray_data_[i-1].vis_;
    double temp = vis*pins*zray_.ray_data_[i-1].pI_;
    temp += pre;
    double prob_r = zray_.ray_data_[i-1].init_ratio_;
    zray_.ray_data_[i-1].PinS_=1.0/(1.0 + (prob_r*post/temp));
  }
}
void brad_belief_prop_utils::update_vis(){
  double vis = 1.0;
  auto n = static_cast<unsigned>((zray_.max_z_-zray_.min_z_)/zray_.dim_);
  for(unsigned i=n; i>=1; i--){
    zray_.ray_data_[i-1].vis_=vis;
    vis *= (1.0-zray_.ray_data_[i-1].PinS_);
  }
}
void brad_belief_prop_utils::print_zray(unsigned ni) const{
  zray_.ray_data_[ni-1].print();
}

double brad_belief_prop_utils::expected_depth() const{
  auto n = static_cast<unsigned>((zray_.max_z_-zray_.min_z_)/zray_.dim_);
  double sum = 0.0;
  double sump = 0.0;
  for(unsigned i=n; i>=1; i--){
    double vis = zray_.ray_data_[i-1].vis_;
    double pins = zray_.ray_data_[i-1].PinS_;
    double z = zray_.ray_data_[i-1].cell_z_;
    sum += z*vis*pins;
    sump += vis*pins;
  }
  return sum/sump;
}
bool brad_belief_prop_utils::force_single_index(unsigned indx){
  return app_index_.force_single_index(indx);
}

void brad_belief_prop_utils::compute_depth_map(vgl_box_2d<double>const& region, double cell_len, double z0){
  unsigned mnv = this->most_nadir_view();
  std::vector<unsigned> ref_views =this->index(mnv);
  double min_x = region.min_x(), max_x = region.max_x();
  double min_y = region.min_y(), max_y = region.max_y();
  auto ni = static_cast<unsigned>((max_x-min_x)/cell_len), nj = static_cast<unsigned>((max_y-min_y)/cell_len);
  depth_.set_size(ni, nj);
  depth_.fill(0.0f);
  double y = min_y;
  for(unsigned j = 0; j<nj; ++j){
    double x = min_x;
    for(unsigned i = 0; i<ni; ++i){
      vgl_point_3d<double> pi(x, y, z0);
      float Iray =0.0f;
      this->pixel_intensity(imgs_[mnv], cams_[mnv], pi, Iray);
      this->init_zray(Iray, ref_views, pi, 0.0, true);
      this->zray_pre();
      this->zray_post();
      this->update_PinS();
      this->update_vis();
      auto ed = static_cast<float>(this->expected_depth());
      depth_(i,j) = ed;
      x += cell_len;
    }
    y += cell_len;
  }
}

bool brad_belief_prop_utils::save_depth_map(std::string const& depth_path) const{
  return vil_save(depth_, depth_path.c_str());
}
