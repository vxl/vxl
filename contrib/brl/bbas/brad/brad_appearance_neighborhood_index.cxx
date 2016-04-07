#include "brad_appearance_neighborhood_index.h"
#include <vpgl/vpgl_camera.h>
#include <vnl/vnl_math.h>
#include <bsta/algo/bsta_mog3_grey.h>
#include <cstdlib> // for rand
#include <algorithm>
#include <iostream>
#include <iomanip>
#define BRAD_INIT_SIGMA 0.15f
#define BRAD_MIN_SIGMA 0.10f
brad_appearance_neighborhood_index::brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata)
{
    std::vector<vgl_vector_3d<double > > local_view_dirs;
    std::vector<vgl_vector_3d<double > > local_illum_dirs;
	
	for(std::vector<brad_image_metadata_sptr>::const_iterator mit = metadata.begin(); mit !=metadata.end(); ++mit){  
    //convert to sun illumination to standard spherical coordinates
    double sun_sphere_az = 90.0 - (*mit)->sun_azimuth_ + 360.0;
    if (sun_sphere_az>=360.0) sun_sphere_az -= 360.0;
    double sun_sphere_el = 90.0 - (*mit)->sun_elevation_;
    double sun_sphere_az_rad = vnl_math::pi_over_180*sun_sphere_az;
    double sun_sphere_el_rad = vnl_math::pi_over_180*sun_sphere_el;
    // convert to Cartesian
    double s = vcl_sin(sun_sphere_el_rad);
    double x = s*vcl_cos(sun_sphere_az_rad);
    double y = s*vcl_sin(sun_sphere_az_rad);
    double z = vcl_cos(sun_sphere_el_rad);
    vgl_vector_3d<double> ill_dir(x, y, z);
    local_illum_dirs.push_back(-ill_dir);

    //convert view direction to standard spherical coordinates
    double view_sphere_az = 90.0 - (*mit)->view_azimuth_ + 360.0;
    if (view_sphere_az>=360.0) view_sphere_az -= 360.0;
    double view_sphere_el = 90.0 - (*mit)->view_elevation_;
    double view_sphere_az_rad = vnl_math::pi_over_180*view_sphere_az;
    double view_sphere_el_rad = vnl_math::pi_over_180*view_sphere_el;
    // convert to Cartesian
    s = vcl_sin(view_sphere_el_rad);
    x = s*vcl_cos(view_sphere_az_rad);
    y = s*vcl_sin(view_sphere_az_rad);
    z = vcl_cos(view_sphere_el_rad);
    vgl_vector_3d<double> view_dir(x, y, z);
    local_view_dirs.push_back(-view_dir);
  }
  (*this) = brad_appearance_neighborhood_index(local_illum_dirs, local_view_dirs);
}


brad_appearance_neighborhood_index::brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata,
                                                                       std::map<unsigned, std::map<unsigned, double> > const& overlap){
  *this = brad_appearance_neighborhood_index(metadata);
  view_overlap_ = overlap;
}

// the appearance clusters determine a set of illumination directions and viewpoints
// that satsify the following criteria:
//
//   1) the cluster target image is distinct from other cluster targets. That is the
//      the viewpoint of each cluster target must differ by a specified orientation threshold
//   2) the other appearance images in the cluster must be distinct from the target image
///     still have a significant overlap with the target image
//   3) each appearance image must have illumination and viewpoint that is likely to
//      accurately predict the appearance of the target image.
//
#if 0 // use random shuffle instead
unsigned brad_appearance_neighborhood_index::random_index(unsigned n) const{
  return static_cast<unsigned>(std::rand()) % n;
}
#endif

void brad_appearance_neighborhood_index::fill_near_view_dir_map(){
  unsigned n = this->n_dirs();
  bool full_overlap = !this->overlap_is_specified();
  for(unsigned i = 0; i<n; ++i){
    for(unsigned j = 0; j<n; ++j){
      if(j == i)
        continue;
      double dp = dot_product(view_dirs_[i], view_dirs_[j]);
      double sang = std::sqrt(1.0-(dp*dp));
      if(sang<max_view_dir_thresh_&&full_overlap)
        near_view_dir_map_[i].push_back(j);
      else if(!full_overlap && sang<max_view_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_)
        near_view_dir_map_[i].push_back(j);
    }
  }
}
#if 1
// note! the view direction neighbors must be computed before calling this method
void brad_appearance_neighborhood_index::fill_near_illum_dir_map(){
  unsigned n = this->n_dirs();
  bool full_overlap = !this->overlap_is_specified();
  for(unsigned i = 0; i<n; ++i){
    const std::vector<unsigned>& view_nbrs = near_view_dir_map_[i];
    for(unsigned j = 0; j<n; ++j){
      if(j == i)
        continue;
    std::vector<unsigned>::const_iterator nit;
    nit = std::find(view_nbrs.begin(), view_nbrs.end(), j);
    if(nit == view_nbrs.end())
      continue;
    double dp = dot_product(illumination_dirs_[i], illumination_dirs_[j]);
	double sang = std::sqrt(1.0-(dp*dp));
      if(sang<max_illum_dir_thresh_ && full_overlap)
        near_illum_dir_map_[i].push_back(j);
      else if(!full_overlap && sang<max_illum_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_)
        near_illum_dir_map_[i].push_back(j);
    }
  }
}
#else
// note! the view direction neighbors must be computed before calling this method
void brad_appearance_neighborhood_index::fill_near_illum_dir_map(){
  unsigned n = this->n_dirs();
  bool full_overlap = !this->overlap_is_specified();
  for(unsigned i = 0; i<n; ++i){
    vgl_vector_3d<double> i_illum_dir = illumination_dirs_[i];
    for(unsigned j = 0; j<n; ++j){
      if(i==j)
        continue;
      double dp = dot_product(i_illum_dir, illumination_dirs_[j]);
	double sang = std::sqrt(1.0-(dp*dp));
        if(sang<max_illum_dir_thresh_ && full_overlap)
          near_illum_dir_map_[i].push_back(j);
        else if(!full_overlap && sang<max_illum_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_)
          near_illum_dir_map_[i].push_back(j);
    }
  }
}
#endif
void brad_appearance_neighborhood_index::compute_index(){
  // full overlap can occur in the case of cropped satellite images of 
  // a scene where every view provides full coverage of scene surfaces
  // modulo occlusion
  near_view_dir_map_.clear();
  near_illum_dir_map_.clear();
  index_.clear();
  unsigned n = n_dirs();
  this->fill_near_view_dir_map();
  this->fill_near_illum_dir_map();
  std::vector<unsigned> img_index(n);
  for(unsigned i = 0; i<n; ++i)
    img_index[i]=i;
  std::random_shuffle( img_index.begin(), img_index.end());
#if 0
  std::cout << "shuffle(";
  for(unsigned i = 0; i<n; ++i)
    std::cout << img_index[i] << ' ';
  std::cout << ")\n";
#endif
  std::vector<unsigned> targets;
  // iterate over the image collection until all target candidates are exhausted
  unsigned ptr = 0;
  while(ptr<n){
    unsigned img_idx = img_index[ptr];
    unsigned nillum = static_cast<unsigned>(near_illum_dir_map_[img_idx].size());
    if(nillum < num_illum_neighbors_thresh_){
      ptr++;
      continue;
    }
    //check if proposed target is close in viewpoint to an existing target
    const std::vector<unsigned>& view_nbrs = near_view_dir_map_[img_idx];
    bool near = false;
    for(unsigned i=0; i<static_cast<unsigned>(targets.size())&&!near; ++i){
      std::vector<unsigned>::const_iterator nit;
      nit = std::find(view_nbrs.begin(), view_nbrs.end(), targets[i]);
      if(nit != view_nbrs.end())
         near = true;
    }  
    if(near){
      ptr++;
      continue;
    }
    targets.push_back(img_idx);
	ptr++;
  }
  // the set of targets is determined
  unsigned ntarg = static_cast<unsigned>(targets.size());
  if(!ntarg)
    return;

  for(unsigned i = 0; i<ntarg; ++i)
    index_[targets[i]]=near_illum_dir_map_[targets[i]];
  std::cout << "APPEARANCE INDEX ***\n";
  this->print_index();
  std::cout << "********\n";
}

void brad_appearance_neighborhood_index::print_index() const{
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit = index_.begin();
      iit != index_.end(); ++iit){
    unsigned i_idx = iit->first;
    const std::vector<unsigned>& nbrs = iit->second;
    std::cout << "t: " << i_idx << "[ ";
    for(std::vector<unsigned>::const_iterator nit = nbrs.begin();
        nit != nbrs.end(); ++nit)
      std::cout << *nit << ' ';
    std::cout << "]\n";
  }
}
void brad_appearance_neighborhood_index::print_index_angles() const{
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit = index_.begin();
      iit != index_.end(); ++iit){
    unsigned i_idx = iit->first;
    vgl_vector_3d<double> view_dir_i = view_dirs_[i_idx];
    std::cout << "Target separation[" << i_idx << "]:( ";
    for(std::map<unsigned, std::vector<unsigned> >::const_iterator jit = index_.begin();
        jit != index_.end(); ++jit){
      unsigned j_idx = jit->first;
      if(i_idx == j_idx)
        continue;
      vgl_vector_3d<double> view_dir_j = view_dirs_[j_idx];
      std::cout << j_idx << "<>" << std::setprecision(3) << 180.0*angle(view_dir_i, view_dir_j)/3.14159 << ' ';
    }
    std::cout << ")\n";
  }

  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit = index_.begin();
      iit != index_.end(); ++iit){
    unsigned target_idx = iit->first;
    vgl_vector_3d<double> view_dir = view_dirs_[target_idx];
    vgl_vector_3d<double> illum_dir = illumination_dirs_[target_idx];
    const std::vector<unsigned>& illum_neighbors = iit->second;
    std::map<unsigned, std::vector<unsigned> >::const_iterator kit;
    kit = near_view_dir_map_.find(target_idx);
    const std::vector<unsigned>& view_neighbors = kit->second;
    std::cout << "\nTarget v(" << view_dir.x() << ' ' << view_dir.y() << ' ' << view_dir.z() << "):I("
              << illum_dir.x() << ' ' << illum_dir.y() << ' ' << illum_dir.z() << ")\n";
    std::cout << " view angles: ";
    for(std::vector<unsigned>::const_iterator vit = view_neighbors.begin();
        vit != view_neighbors.end(); vit++)
      std::cout << 180.0*angle(view_dir, view_dirs_[*vit])/3.14159 << ' ';
    std::cout << '\n';
    std::cout << " illumination angles: ";
    for(std::vector<unsigned>::const_iterator nit = illum_neighbors.begin();
        nit != illum_neighbors.end(); nit++)
      std::cout << 180.0*angle(illum_dir, illumination_dirs_[*nit])/3.14159 << ' ';
    std::cout << '\n';
  }
}
bool brad_appearance_neighborhood_index::pixel_intensity(vil_image_view<float> const& img, vpgl_camera_double_sptr const& cam,
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
void brad_appearance_neighborhood_index::project_intensities(vgl_point_3d<double> const& p){
  intensity_.clear();
  nbr_intensities_.clear();
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit = index_.begin();
      iit != index_.end(); ++iit){
    unsigned img_index = iit->first;
    const std::vector<unsigned>& nbrs = iit->second;
    float Itarg = 0.0f;
    if(!pixel_intensity(imgs_[img_index], cams_[img_index], p, Itarg))
      continue;
    intensity_[img_index]=Itarg;
    for(std::vector<unsigned>::const_iterator nit = nbrs.begin();
        nit != nbrs.end(); ++nit){
      float Inbr = 0.0f;
      if(!pixel_intensity(imgs_[*nit], cams_[*nit], p, Inbr))
        continue;
      nbr_intensities_[img_index].push_back(Inbr);
    }
  }
}
void brad_appearance_neighborhood_index::print_intensities() const{
  std::map<unsigned, double>::const_iterator inti = intensity_.begin();
  std::map<unsigned, std::vector<double> >::const_iterator ninti = nbr_intensities_.begin();
  for(; inti!=intensity_.end(); ++inti, ++ninti){
    double tint = inti->second;
    const std::vector<double>& nbr_intens = ninti->second;
    std::cout << "t["<< inti->first <<  "]: " << tint << " n:( ";
    for(std::vector<double>::const_iterator nit = nbr_intens.begin();
        nit != nbr_intens.end(); ++nit)
      std::cout << *nit << ' ';
    std::cout << ")\n";
  }
}
void brad_appearance_neighborhood_index::print_view_neighbors() const{
  std::cout << "View neighbors\n";
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator vit =  near_view_dir_map_.begin();
      vit !=  near_view_dir_map_.end(); vit++){
    unsigned i_idx = vit->first;
    const std::vector<unsigned>& nbrs = vit->second;
    std::cout << "view: " << i_idx << " [ ";
    for(std::vector<unsigned>::const_iterator nit = nbrs.begin();
        nit != nbrs.end(); ++nit)
      std::cout << *nit << ' ';
    std::cout << "]\n";
  }
}    

void brad_appearance_neighborhood_index::print_illum_neighbors() const{
  std::cout << "Illumination neighbors\n";
  for(std::map<unsigned, std::vector<unsigned> >::const_iterator iit =  near_illum_dir_map_.begin();
      iit !=  near_illum_dir_map_.end(); iit++){
    unsigned i_idx = iit->first;
    const std::vector<unsigned>& nbrs = iit->second;
    std::cout << "illum: " << i_idx << " [ ";
    for(std::vector<unsigned>::const_iterator nit = nbrs.begin();
        nit != nbrs.end(); ++nit)
      std::cout << *nit << ' ';
    std::cout << "]\n";
  }
}
void brad_appearance_neighborhood_index::test_appearance_update(){
  // must have called ::print_intensities first!
  std::map<unsigned, double>::iterator inti = intensity_.begin();
  std::map<unsigned, std::vector<double> >::iterator ninti = nbr_intensities_.begin();
  for(; inti!=intensity_.end(); ++inti, ++ninti){
    float tint = static_cast<float>(inti->second);
    vnl_vector_fixed<unsigned char, 8> mog3(static_cast<unsigned char>(0));
    vnl_vector_fixed<float, 4> nobs(0.0f);
    std::vector<double>& nbr_intens = ninti->second;
    unsigned n_nbr = static_cast<unsigned>(nbr_intens.size());
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
