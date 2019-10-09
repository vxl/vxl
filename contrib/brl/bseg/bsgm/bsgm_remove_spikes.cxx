#include "bsgm_remove_spikes.h"
#include <vgl/vgl_point_3d.h>
#include <math.h>
#include <vnl/vnl_math.h>
#include <algorithm>


// given a vector of z height values find a set of clusters with values within the cluster tolerance
std::vector<std::pair<size_t, float> > bsgm_remove_spikes::cluster_centers(std::vector<float> zvals, size_t max_k){
  std::vector<std::pair<size_t, float> > ret; 
  size_t n = zvals.size();
  if(n == 1){
    ret.emplace_back(1,zvals[0]);
    return ret;
  }
  std::vector<float> temp = zvals;
  size_t k = 1;
  while(k <= max_k){
    n = temp.size();
    if(n == 0)
      break;
    float cent = temp[0];
    float csize = 1.0f;
    std::vector<size_t> to_remove;
    to_remove.push_back(0);
    for(size_t i = 1; i<n; ++i)
      if(fabs(temp[i]-cent)<params_.cluster_tol_){
        csize += 1.0f;
        cent = (cent*(csize-1.0f) + temp[i])/csize;
        to_remove.push_back(i);
      }
    std::vector<float> temp2;
    for(size_t i = 0; i<n; ++i){
        std::vector<size_t>::iterator iit = std::find(to_remove.begin(), to_remove.end(), i);
      if(iit != to_remove.end())
        continue;
      temp2.push_back(temp[i]);
    }
    temp = temp2;
    ret.emplace_back(to_remove.size(),cent);
    k++;
  }
  return ret;
}
// for each i,j position extract a neighborhood of z values and deterimine if a peak is present at
// a specific i,j
bool bsgm_remove_spikes::replace_spikes_with_local_z(bool smooth){
  if(!expand_input_img())
    return false;
  int r = params_.nbrhd_radius_;
  size_t ni = expanded_input_img_.ni(), nj = expanded_input_img_.nj();
  if(ni == 0 || nj == 0){
    std::cout << "Null input image" << std::endl;
    return false;
  }
  filtered_img_.set_size(input_img_.ni(), input_img_.nj());
  filtered_img_.fill(NAN);
  n_k_.set_size(ni, nj);
  n_k_.fill(NAN);
  std::vector<float> zvals;
  float center_z = 0.0f;
  size_t fi = 0, fj = 0; //filtered image pixel location
  for(int j = r; j<(nj-r);++j){
    fi = 0;
    for(int i = r; i<(ni-r);++i){
      float center_z = expanded_input_img_(i, j);
      zvals.clear();
      if(prt_&& i==id_ && j == jd_)
        std::cout << "peak neighbors c = " << center_z << std::endl;
      std::vector<float> dists;
      for(int jj = -r; jj<=r; ++jj)
        for(int ii = -r; ii<=r; ++ii){
          float v = expanded_input_img_(i + ii, j + jj);
          if (v!=invalid_z_&&vnl_math::isfinite(v)) {
            zvals.push_back(v);
            if (prt_&& i==id_ && j == jd_)
              std::cout << ii << ' ' << jj << ' ' << v << std::endl;
          }
        }
      std::vector<std::pair<size_t, float> > cluster_cent = this->cluster_centers(zvals, params_.max_k_);
      n_k_(i,j) = 100.0f*cluster_cent.size();
      size_t nk = cluster_cent.size();
      if (prt_ && i == id_ && j == jd_)
       std::cout << "k clusters " << std::endl;
      size_t max_n_cent = 0;
      float max_c = 0.0f;
      for (size_t q = 0; q < nk; ++q) {
          if (cluster_cent[q].first > max_n_cent) {
              max_n_cent = cluster_cent[q].first;
              max_c = cluster_cent[q].second;
          }
          if (prt_ && i == id_ && j == jd_) 
              std::cout << cluster_cent[q].second << ' '<< cluster_cent[q].first << std::endl;
      }
      float d = 0.0f;
      if (center_z!=invalid_z_&&vnl_math::isfinite(center_z) && max_n_cent > 1) {
        if(smooth) // always replace z with max_c
          filtered_img_(fi,fj) = max_c;
        else{// set z  max_c only if not close to largest cluster mean (i.e. a spike)
          d = fabs(center_z - max_c);
          if (d < params_.cluster_tol_)
              filtered_img_(fi, fj) = center_z;
          else 
              filtered_img_(fi, fj) = max_c;
        }
      }
      if (prt_ && i == id_ && j == jd_)
        std::cout << "max result " << max_c << ' ' << max_n_cent << ' '<< d<< std::endl;
      
      fi++;
    }
    fj++;
  }
  return true;
}
// expand the input image using nearest neighbor values to fill in a border of width nbrhd_radius
bool bsgm_remove_spikes::expand_input_img(){
  size_t r = params_.nbrhd_radius_;
  size_t dim_expand = 2*r;
  int ni = input_img_.ni(), nj = input_img_.nj();
  if(ni == 0 || nj == 0){
    std::cout << "Null input image" << std::endl;
    return false;
  }
  expanded_input_img_.set_size(ni+ dim_expand, nj+ dim_expand);
  expanded_input_img_.fill(NAN);
  //index into expanded image
  int ie = 0, je = 0;
  // end indices for expanded image
  int endj = nj + r;
  int endi = ni + r;
  //the nearest image position to an expanded image position
  int inear = 0, jnear = 0;
  //iterate through the expanded image
  for(int j=-r; j<endj; ++j){
    if (j >= 0 && j <= (nj - 1)) jnear = j;
    else if (j < 0) jnear = 0;
    else if (j > (nj - 1)) jnear = (nj - 1);
    int ie = 0;
    for(int i = -r; i<endi; i++){
      if (i>= 0 && i <= (ni - 1)) inear = i;
      else if (i < 0) inear = 0;
      else if (i > (ni - 1)) inear = (ni - 1);
      float v = input_img_(inear,jnear);
      if (v!=invalid_z_&&vnl_math::isfinite(v))
        expanded_input_img_(ie, je) = v;
      ie++;
    }
    je++;
  }   
  return true;
}

void bsgm_remove_spikes::enable_print_spike_neighborhood(size_t i , size_t j){
  prt_ = true;
  id_ = i; jd_ = j;
}
void bsgm_remove_spikes::disable_print_spike_neighborhood(){
  prt_ = false;
}
