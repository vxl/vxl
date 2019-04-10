// This is brl/bseg/bsgm/bsgm_prob_align_pointsets_3d.hxx
#include "bsgm_prob_align_pointsets_3d.h"
#include <map>
#include <vector>
#include <algorithm>
#include <vgl/vgl_distance.h>
#include <vul/vul_timer.h>

template <typename T>
void bsgm_prob_align_pointsets_3d<T>::descending_prob_sort(vgl_pointset_3d<T>& prob_ptset)
{
  std::vector<std::pair<size_t, T> > indexed_probs;
  size_t n = prob_ptset.size();
  size_t max_n = params_.max_n_pts_;
  if(max_n > (n-1))
    max_n = (n-1);
  for(size_t i = 0; i<n; ++i){
    T pr = prob_ptset.sc(i);
    indexed_probs.emplace_back(i, pr);
  }
  std::sort(indexed_probs.begin(), indexed_probs.end(), bsgm_prob_align_pointsets_3d<T>::prob_greater);
  std::vector<vgl_point_3d<T> > pts;
  std::vector<T> probs;
  for(size_t i = 0; i<=max_n; ++i){
    const vgl_point_3d<T>& p =prob_ptset.p(indexed_probs[i].first);
    pts.emplace_back(p.x(), p.y(), p.z());
    probs.push_back(indexed_probs[i].second);
  }
  prob_ptset.clear();
  prob_ptset.set_points_with_scalars(pts, probs);
}

template <typename T>
T bsgm_prob_align_pointsets_3d<T>::total_prob(T tx, T ty, T tz, T c_tol)
{
  float total_prob = 0;
  size_t n = ptset_0_.size();
  for(size_t i = 0; i<n; ++i){
    const vgl_point_3d<T>& p = ptset_0_.p(i);
    // translate a point from ptset_0
    vgl_point_3d<T> tp(p.x()+tx, p.y()+ty, p.z()+tz);
    // find the closest point from ptset_1
    unsigned int indx = 0;
    if(!knn_.closest_index(tp, indx)){
      std::cout << "KNN index failed to find neighbors - fatal" << std::endl;
      return -1;
    }
    const vgl_point_3d<T>& cp = ptset_1_.p(indx);
    T prob_nbr = ptset_1_.sc(indx);
    // is the closest point within c_tol?
    if(vgl_distance(tp, cp)<= c_tol){
      total_prob += prob_nbr;
    }
  }
  return total_prob;
}

template <typename T>
bool bsgm_prob_align_pointsets_3d<T>::search(T rad, T inc, T tx0, T ty0, T tz0, T con_tol,
                                             T& max_tx, T& max_ty, T& max_tz, T& max_prob,
                                             bool verbose)
{
  max_prob = 0.0f;
  max_tx = rad; max_ty = rad; max_tz = rad;
  for (T tx = tx0-rad; tx <= tx0+rad; tx += inc) {
    for (T ty = ty0-rad; ty <= ty0+rad; ty += inc) {
      float max_z_prob = 0.0f;
      for (T tz = tz0-rad; tz <= tz0+rad; tz += inc) {
        float tprob = this->total_prob(tx, ty, tz, con_tol);
        if (tprob > max_prob) {
          max_prob = tprob;
          max_tx = tx;  max_ty = ty; max_tz = tz;
        }
        if (tprob > max_z_prob)
          max_z_prob = tprob;
      }
      if(verbose) std::cout << max_z_prob << ' ';
    }
    if(verbose) std::cout << std::endl;
  }
  return true;
}

template <typename T>
bool bsgm_prob_align_pointsets_3d<T>::find_translation(bool verbose)
{
  vul_timer t;
  T sd = params_.point_sample_dist_;
  T cdm = params_.consen_dist_mul_;
  T c_inc = sd*params_.coarse_inc_mul_;
  T f_inc = sd*params_.fine_inc_mul_;
  T c_rad = sd*params_.coarse_radius_mul_;
  T f_rad = sd*params_.fine_radius_mul_;
  T f_con_tol = cdm*f_inc;
  T c_con_tol = cdm*c_inc;
  // ultra fine search parameters
  T uf_inc = f_inc*T(1)/T(4);
  T uf_rad = f_inc*T(5)/T(4);
  // coarse search
  if(verbose) std::cout << "coarse search" << std::endl;
  if(verbose) std::cout << "max pts = "<< params_.max_n_pts_ << " ctol = " << c_con_tol << std::endl;
  T max_tx = c_rad, max_ty = c_rad, max_tz = c_rad;
  T tx0 = T(0),  ty0 = T(0), tz0 = T(0);
  T max_prob = 0.0f;
  if(!search(c_rad, c_inc, tx0, ty0, tz0, c_con_tol, max_tx, max_ty, max_tz, max_prob, verbose))
    return false;
  if(max_tx == -c_rad || max_tx == c_rad ||
     max_ty == -c_rad || max_ty == c_rad ||
     max_tz == -c_rad || max_tz == c_rad){
    std::cout << "Coarse search finished at search bounds" << std::endl;
    return false;
  }
  if(verbose) std::cout << "coarse rad = " << c_rad << " coarse inc = " << c_inc << std::endl;
  if(verbose) std::cout << "trans( " << max_tx << ' ' << max_ty << ' ' << max_tz << ")" <<std::endl;
  if(verbose) std::cout << "max prob " << max_prob << std::endl;
  // fine search
  max_prob = 0.0f;
  if(verbose) std::cout << "\nfine search" << std::endl;
  if(verbose) std::cout << " ctol = " << f_con_tol << std::endl;
  T max_ftx = max_tx, max_fty = max_ty, max_ftz = max_tz;
  if(!search(f_rad, f_inc, max_tx, max_ty, max_tz, f_con_tol, max_ftx, max_fty, max_ftz, max_prob, verbose))
    return false;

  if(verbose) std::cout << "fine rad = " << f_rad << " fine inc = " << f_inc << std::endl;
  if(verbose) std::cout << "trans( " << max_ftx << ' ' << max_fty << ' ' << max_ftz << ")" <<std::endl;
  if(verbose) std::cout << "max prob " << max_prob << std::endl;
  // ultra fine search
  if(verbose) std::cout << "\nultra fine search" << std::endl;
  if(verbose) std::cout << " ctol = " << f_con_tol << std::endl;
  max_prob = 0.0f;
  T u_max_ftx = max_ftx, u_max_fty = max_fty, u_max_ftz = max_ftz;
  if(!search(uf_rad, uf_inc, max_ftx, max_fty, max_ftz, f_con_tol, u_max_ftx, u_max_fty, u_max_ftz, max_prob, verbose))
    return false;

  if(verbose) std::cout << "ultra fine rad = " << uf_rad << " ultra fine inc = " << uf_inc << std::endl;
  std::cout << "\ntranslation:0->1 ( " << u_max_ftx << ' ' << u_max_fty << ' ' << u_max_ftz << ")" << std::endl;
  if(verbose) std::cout << "max prob " << max_prob << std::endl;
  translation_.set(u_max_ftx, u_max_fty, u_max_ftz);
  std::cout << "find pairwise dsm translation in " << t.real()/1000.0f << " secs." << std::endl;
  return true;
}

#define BSGM_PROB_ALIGN_POINTSETS_3D_INSTANIATE(T) \
  template class bsgm_prob_align_pointsets_3d<T>
