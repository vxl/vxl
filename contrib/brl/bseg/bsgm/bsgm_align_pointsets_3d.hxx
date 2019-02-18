// This is brl/bseg/bsgm/bsgm_align_pointsets_3d.hxx
#include "bsgm_align_pointsets_3d.h"
#include <vgl/vgl_distance.h>
#include <vul/vul_timer.h>

template <typename T>
size_t bsgm_align_pointsets_3d<T>::consensus(T tx, T ty, T tz, T c_tol)
{
  size_t ncon = 0;
  size_t n = ptset_0_.size();
  size_t n_skip = n/params_.min_n_pts_;
  if(n_skip == 0)
	  n_skip = 1;
  for(size_t i = 0; i<n; i+=n_skip){
    const vgl_point_3d<T>& p = ptset_0_.p(i);
    // translate a point from ptset_0
    vgl_point_3d<T> tp(p.x()+tx, p.y()+ty, p.z()+tz), cp;
    // find the closest point from ptset_1
    if(!knn_.closest_point(tp, cp)){
      std::cout << "KNN index failed to find neighbors - fatal" << std::endl;
      return -1;
    }
    // is the closest point within c_tol?
    if(vgl_distance(tp, cp)<= c_tol)
      ncon++;
  }
  return ncon;
}

template <typename T>
bool bsgm_align_pointsets_3d<T>::search(T rad, T inc, T tx0, T ty0, T tz0, T con_tol,
                                        T& max_tx, T& max_ty, T& max_tz, size_t& max_con,
                                        bool verbose)
{
  size_t max_consensus = 0;
  max_tx = rad; max_ty = rad; max_tz = rad;
  for (T tx = tx0-rad; tx <= tx0+rad; tx += inc) {
    for (T ty = ty0-rad; ty <= ty0+rad; ty += inc) {
      size_t max_z_con = 0;
      for (T tz = tz0-rad; tz <= tz0+rad; tz += inc) {
        size_t  con = this->consensus(tx, ty, tz, con_tol);
        if (con == -1)
          return false;
        if (con > max_consensus) {
          max_consensus = con;
          max_tx = tx;  max_ty = ty; max_tz = tz;
        }
        if (con > max_z_con)
          max_z_con = con;
      }
      if(verbose) std::cout << max_z_con << ' ';
    }
    if(verbose) std::cout << std::endl;
  }
  return true;
}

template <typename T>
bool bsgm_align_pointsets_3d<T>::find_translation(bool verbose)
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
  T max_tx = c_rad, max_ty = c_rad, max_tz = c_rad;
  T tx0 = T(0),  ty0 = T(0), tz0 = T(0);
  size_t max_consensus = 0;
  if(!search(c_rad, c_inc, tx0, ty0, tz0, c_con_tol, max_tx, max_ty, max_tz, max_consensus, verbose))
    return false;
  if(max_tx == -c_rad || max_tx == c_rad ||
     max_ty == -c_rad || max_ty == c_rad ||
     max_tz == -c_rad || max_tz == c_rad){
    std::cout << "Coarse search finished at search bounds" << std::endl;
    return false;
  }
  // fine search
  if(verbose) std::cout << "\nfine search" << std::endl;
  T max_ftx = max_tx, max_fty = max_ty, max_ftz = max_tz;
  if(!search(f_rad, f_inc, max_tx, max_ty, max_tz, f_con_tol, max_ftx, max_fty, max_ftz, max_consensus, verbose))
    return false;

  // ultra fine search
  if(verbose) std::cout << "\nultra fine search" << std::endl;
  max_consensus = 0;
  T u_max_ftx = max_ftx, u_max_fty = max_fty, u_max_ftz = max_ftz;
  if(!search(uf_rad, uf_inc, max_ftx, max_fty, max_ftz, f_con_tol, u_max_ftx, u_max_fty, u_max_ftz, max_consensus, verbose))
    return false;

  std::cout << "\ntranslation:0->1 ( " << u_max_ftx << ' ' << u_max_fty << ' ' << u_max_ftz << ")" << std::endl;
  translation_.set(u_max_ftx, u_max_fty, u_max_ftz);
  std::cout << "find pairwise dsm translation in " << t.real()/1000.0f << " secs." << std::endl;
  return true;
}

#define BSGM_ALIGN_POINTSETS_3D_INSTANIATE(T) \
  template class bsgm_align_pointsets_3d<T>
