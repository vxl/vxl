#include <cstdlib>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include "brad_appearance_neighborhood_index.h"
#include <vnl/vnl_math.h>

brad_appearance_neighborhood_index::brad_appearance_neighborhood_index(std::vector<brad_image_metadata_sptr> const& metadata)
{
    std::vector<vgl_vector_3d<double > > local_view_dirs;
    std::vector<vgl_vector_3d<double > > local_illum_dirs;

        for(const auto & mit : metadata){
    //convert to sun illumination to standard spherical coordinates
    double sun_sphere_az = 90.0 - mit->sun_azimuth_ + 360.0;
    if (sun_sphere_az>=360.0) sun_sphere_az -= 360.0;
    double sun_sphere_el = 90.0 - mit->sun_elevation_;
    double sun_sphere_az_rad = vnl_math::pi_over_180*sun_sphere_az;
    double sun_sphere_el_rad = vnl_math::pi_over_180*sun_sphere_el;
    // convert to Cartesian
    double s = std::sin(sun_sphere_el_rad);
    double x = s*std::cos(sun_sphere_az_rad);
    double y = s*std::sin(sun_sphere_az_rad);
    double z = std::cos(sun_sphere_el_rad);
    vgl_vector_3d<double> ill_dir(x, y, z);
    local_illum_dirs.push_back(-ill_dir);

    //convert view direction to standard spherical coordinates
    double view_sphere_az = 90.0 - mit->view_azimuth_ + 360.0;
    if (view_sphere_az>=360.0) view_sphere_az -= 360.0;
    double view_sphere_el = 90.0 - mit->view_elevation_;
    double view_sphere_az_rad = vnl_math::pi_over_180*view_sphere_az;
    double view_sphere_el_rad = vnl_math::pi_over_180*view_sphere_el;
    // convert to Cartesian
    s = std::sin(view_sphere_el_rad);
    x = s*std::cos(view_sphere_az_rad);
    y = s*std::sin(view_sphere_az_rad);
    z = std::cos(view_sphere_el_rad);
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
#if 0
void brad_appearance_neighborhood_index::fill_view_dir_maps(){
  unsigned n = this->n_dirs();
  bool full_overlap = !this->overlap_is_specified();
  for(unsigned i = 0; i<n; ++i){
    for(unsigned j = 0; j<n; ++j){
      if(j == i)
        continue;
      double dp = dot_product(view_dirs_[i], view_dirs_[j]);
      double sang = std::sqrt(1.0-(dp*dp));
      if(sang<max_view_dir_thresh_&&full_overlap)
        valid_view_dir_map_[i].push_back(j);
      else if(!full_overlap && sang<max_view_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_)
        valid_view_dir_map_[i].push_back(j);
    }
  }
}
#else
void brad_appearance_neighborhood_index::fill_view_dir_maps(){
  unsigned n = this->n_dirs();
  bool full_overlap = !this->overlap_is_specified();
  for(unsigned i = 0; i<n; ++i){
    for(unsigned j = 0; j<n; ++j){
      if(j == i)
        continue;
      double dp = dot_product(view_dirs_[i], view_dirs_[j]);
      double sang = std::sqrt(1.0-(dp*dp));
      if(sang<min_view_dir_thresh_ && full_overlap){
        close_view_dir_map_[i].push_back(j);
        continue;
      }
      if (!full_overlap && sang<min_view_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_){
        close_view_dir_map_[i].push_back(j);
        continue;
      }
      if(sang>min_view_dir_thresh_ && sang < max_view_dir_thresh_ && full_overlap){
        valid_view_dir_map_[i].push_back(j);
        continue;
      }
      if (!full_overlap && sang<max_view_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_)
        valid_view_dir_map_[i].push_back(j);
    }
  }
}
#endif
// note! the view direction neighbors must be computed before calling this method
void brad_appearance_neighborhood_index::fill_near_illum_dir_map(){
  unsigned n = this->n_dirs();
  bool full_overlap = !this->overlap_is_specified();
  for(unsigned i = 0; i<n; ++i){
    const std::vector<unsigned>& view_nbrs = valid_view_dir_map_[i];
    const std::vector<unsigned>& close_nbrs = close_view_dir_map_[i];
    for(unsigned j = 0; j<n; ++j){
      if(j == i)
        continue;
      std::vector<unsigned>::const_iterator nit;
      // determine if j is a close neighbor, if so, skip
      nit = std::find(close_nbrs.begin(), close_nbrs.end(), j);
      if(nit != close_nbrs.end())
        continue;
      // determine if j is a view neighbor
      nit = std::find(view_nbrs.begin(), view_nbrs.end(), j);
      if(nit == view_nbrs.end())
        continue; // if not then don't bother testing illumination
      double dp = dot_product(illumination_dirs_[i], illumination_dirs_[j]);
      double sang = std::sqrt(1.0-(dp*dp));
      if(sang<illum_dir_thresh_ && full_overlap)
        near_illum_dir_map_[i].push_back(j);
      else if(!full_overlap && sang<illum_dir_thresh_ && view_overlap_[i][j]>min_overlap_thresh_)
        near_illum_dir_map_[i].push_back(j);
    }
  }
}

void brad_appearance_neighborhood_index::compute_index(){
  // full overlap can occur in the case of cropped satellite images of
  // a scene where every view provides full coverage of scene surfaces
  // modulo occlusion
  valid_view_dir_map_.clear();
  near_illum_dir_map_.clear();
  index_.clear();
  unsigned n = n_dirs();
  this->fill_view_dir_maps();
  this->fill_near_illum_dir_map();
  std::vector<unsigned> img_index(n);
  for(unsigned i = 0; i<n; ++i)
    img_index[i]=i;
  std::random_shuffle( img_index.begin(), img_index.end());
  std::vector<unsigned> targets;
  // iterate over the image collection until all target candidates are exhausted
  unsigned ptr = 0;
  while(ptr<n){
    unsigned img_idx = img_index[ptr];
    auto nillum = static_cast<unsigned>(near_illum_dir_map_[img_idx].size());
    if(nillum < num_illum_neighbors_thresh_){
      ptr++;
      continue;
    }
    //check if proposed target is in close views or valid views of an existing target
    const std::vector<unsigned>& close_nbrs = close_view_dir_map_[img_idx];
    const std::vector<unsigned>& valid_nbrs = valid_view_dir_map_[img_idx];
    bool near = false;
    for(unsigned i=0; i<static_cast<unsigned>(targets.size())&&!near; ++i){
      std::vector<unsigned>::const_iterator nit;
      nit = std::find(close_nbrs.begin(), close_nbrs.end(), targets[i]);
      if(nit != close_nbrs.end())
         near = true;
    }
    if(near){
      ptr++;
      continue;
    }
    near = false;
    for(unsigned i=0; i<static_cast<unsigned>(targets.size())&&!near; ++i){
      std::vector<unsigned>::const_iterator nit;
      nit = std::find(valid_nbrs.begin(), valid_nbrs.end(), targets[i]);
      if(nit != valid_nbrs.end())
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
  auto ntarg = static_cast<unsigned>(targets.size());
  if(!ntarg)
    return;

  for(unsigned i = 0; i<ntarg; ++i)
    index_[targets[i]]=near_illum_dir_map_[targets[i]];
}

bool brad_appearance_neighborhood_index::force_single_index(unsigned img_idx){
  valid_view_dir_map_.clear();
  near_illum_dir_map_.clear();
  index_.clear();
  this->fill_view_dir_maps();
  this->fill_near_illum_dir_map();
  auto nillum = static_cast<unsigned>(near_illum_dir_map_[img_idx].size());
  if(nillum < num_illum_neighbors_thresh_)
    return false;
  index_[img_idx]=near_illum_dir_map_[img_idx];
  return true;
}

std::vector<unsigned> brad_appearance_neighborhood_index::index(unsigned indx) const{
  std::vector<unsigned> null_ret;
  std::map<unsigned, std::vector<unsigned> >::const_iterator iit;
  iit = near_illum_dir_map_.find(indx);
  if(iit == near_illum_dir_map_.end())
    return null_ret;
  return iit->second;
}
unsigned brad_appearance_neighborhood_index::most_nadir_view() const{
  auto n = static_cast<unsigned>(view_dirs_.size());
  vgl_vector_3d<double> down_z_dir(0.0, 0.0, -1.0);
  double max_dot = 0.0;
  unsigned ret = 0;
  for(unsigned i = 0; i<n; ++i){
    double dp = dot_product(view_dirs_[i], down_z_dir);
    if(dp>max_dot){
      max_dot = dp;
      ret = i;
    }
  }
  return ret;
}
//predicate functor operator for sorting
bool brad_appearance_neighborhood_index::operator () (unsigned va, unsigned vb) const{
  auto n = static_cast<unsigned>(view_dirs_.size());
  vgl_vector_3d<double> down_z_dir(0.0, 0.0, -1.0);
  if(va>=n || vb>=n)
    return false;
  double dpa = dot_product(view_dirs_[va], down_z_dir);
  double dpb = dot_product(view_dirs_[vb], down_z_dir);
  return dpa>dpb;
}
std::vector<unsigned> brad_appearance_neighborhood_index::views_in_nadir_order() const{
  auto n = static_cast<unsigned>(view_dirs_.size());
  std::vector<unsigned> views(n);
  for(unsigned i = 0; i<n; ++i)
    views[i]=i;
  std::sort(views.begin(), views.end(), (*this));
  return views;
}
void brad_appearance_neighborhood_index::print_index() const{
  for(const auto & iit : index_){
    unsigned i_idx = iit.first;
    const std::vector<unsigned>& nbrs = iit.second;
    std::cout << "t: " << i_idx << "[ ";
    for(unsigned int nbr : nbrs)
      std::cout << nbr << ' ';
    std::cout << "]\n";
  }
}
void brad_appearance_neighborhood_index::print_index_angles() const{
  for(auto iit = index_.begin();
      iit != index_.end(); ++iit){
    unsigned i_idx = iit->first;
    vgl_vector_3d<double> view_dir_i = view_dirs_[i_idx];
    std::cout << "Target separation[" << i_idx << "]:( ";
    for(const auto & jit : index_){
      unsigned j_idx = jit.first;
      if(i_idx == j_idx)
        continue;
      vgl_vector_3d<double> view_dir_j = view_dirs_[j_idx];
      std::cout << j_idx << "<>" << std::setprecision(3) << 180.0*angle(view_dir_i, view_dir_j)/3.14159 << ' ';
    }
    std::cout << ")\n";
  }

  for(const auto & iit : index_){
    unsigned target_idx = iit.first;
    vgl_vector_3d<double> view_dir = view_dirs_[target_idx];
    vgl_vector_3d<double> illum_dir = illumination_dirs_[target_idx];
    const std::vector<unsigned>& illum_neighbors = iit.second;
    std::map<unsigned, std::vector<unsigned> >::const_iterator kit;
    kit = valid_view_dir_map_.find(target_idx);
    const std::vector<unsigned>& view_neighbors = kit->second;
    std::cout << "\nTarget v(" << view_dir.x() << ' ' << view_dir.y() << ' ' << view_dir.z() << "):I("
              << illum_dir.x() << ' ' << illum_dir.y() << ' ' << illum_dir.z() << ")\n";
    std::cout << " view angles: ";
    for(unsigned int view_neighbor : view_neighbors)
      std::cout << 180.0*angle(view_dir, view_dirs_[view_neighbor])/3.14159 << ' ';
    std::cout << '\n';
    std::cout << " illumination angles: ";
    for(unsigned int illum_neighbor : illum_neighbors)
      std::cout << 180.0*angle(illum_dir, illumination_dirs_[illum_neighbor])/3.14159 << ' ';
    std::cout << '\n';
  }
}
void brad_appearance_neighborhood_index::print_view_neighbors() const{
  std::cout << "View neighbors\n";
  for(const auto & vit : valid_view_dir_map_){
    unsigned i_idx = vit.first;
    const std::vector<unsigned>& nbrs = vit.second;
    std::cout << "view: " << i_idx << " [ ";
    for(unsigned int nbr : nbrs)
      std::cout << nbr << ' ';
    std::cout << "]\n";
  }
}

void brad_appearance_neighborhood_index::print_illum_neighbors() const{
  std::cout << "Illumination neighbors\n";
  for(const auto & iit : near_illum_dir_map_){
    unsigned i_idx = iit.first;
    const std::vector<unsigned>& nbrs = iit.second;
    std::cout << "illum: " << i_idx << " [ ";
    for(unsigned int nbr : nbrs)
      std::cout << nbr << ' ';
    std::cout << "]\n";
  }
}
