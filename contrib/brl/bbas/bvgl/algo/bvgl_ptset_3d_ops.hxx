#ifndef bvgl_ptset_3d_ops_hxx_
#define bvgl_ptset_3d_ops_hxx_

#include "bvgl_ptset_3d_ops.h"
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <limits>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_det.h>
const bool bvgl_debug = true;
template <class T>
void bvgl_ptset_3d_ops<T>::compute_roughness(T radius, size_t n_nbrs){
  ptset_with_scalar_result_.clear();
  size_t n = ptset_.size();
  for(size_t i =0; i<n; ++i){
    const vgl_point_3d<T>& p = ptset_.p(i);
    vgl_pointset_3d<T> nbrs;
    if(!knn_.knn(p, n_nbrs, nbrs)){
      ptset_with_scalar_result_.add_point_with_scalar(p, NAN);
      continue;
    }
    std::vector<vgl_homg_point_3d<T> > points;
    for(size_t k = 0; k<n_nbrs; ++k){
        T d = vgl_distance(nbrs.p(k), p);
        if(d<radius)
          points.emplace_back(nbrs.p(k));
    }
    if(points.size() < 4){
      ptset_with_scalar_result_.add_point_with_scalar(p, NAN);
      continue;
    }
    vgl_fit_plane_3d<T> pfit(points);
    T var = pfit.fit();
    ptset_with_scalar_result_.add_point_with_scalar(p, sqrt(var));
  }
}
template <class T>
T bvgl_ptset_3d_ops<T>::average_distance(T frac_ptset){
  vnl_random rand;
  T avg_d = T(0);
  size_t n = ptset_.size();
  if(n == 0)
    return std::numeric_limits<T>::max();
  if(n == 2)
    return vgl_distance<T>(ptset_.p(0), ptset_.p(1));
  T f = frac_ptset;
  if(n < 1000)
    f = T(1);
  size_t fn = static_cast<size_t>(f*n);
  for(size_t i = 0; i<fn; ++i){
    size_t k = rand(fn);
    vgl_point_3d<T> p = ptset_.p(k), cp;
    vgl_pointset_3d<T> nbrs;
    knn_.knn(p, 2, nbrs);
    if (nbrs.size() == 2) {
      cp = nbrs.p(0);
      if (cp == p)
        cp = nbrs.p(1);
    }
    else continue;
    avg_d += vgl_distance<T>(p, cp);
  }
  avg_d /= fn;
  return avg_d;
}
template <class T>
void bvgl_ptset_3d_ops<T>::remove_noise(T radius, T dist_tol, size_t n_nbrs){
  ptset_result_.clear();
  size_t n = ptset_.size();
  for(size_t i =0; i<n; ++i){
    const vgl_point_3d<T>& p = ptset_.p(i);
    vgl_pointset_3d<T> nbrs;
    if(!knn_.knn(p, n_nbrs, nbrs)){
      continue;
    }
    std::vector<vgl_homg_point_3d<T> > points;
    for(size_t k = 0; k<n_nbrs; ++k){
        T d = vgl_distance(nbrs.p(k), p);
        if(d<radius)
          points.emplace_back(nbrs.p(k));
    }
    if(points.size() < 4){
      continue;
    }
    vgl_fit_plane_3d<T> pfit(points);
    T error = sqrt(pfit.fit());
    if(error>T(20)*dist_tol)
      continue;
    vgl_homg_plane_3d<T>& plh = pfit.get_plane();
    T pd = vgl_distance<T>(vgl_homg_point_3d<T>(p), plh);
    if(pd > dist_tol)
      continue;
    ptset_result_.add_point(p);
  }
}
template <class T>
bool bvgl_ptset_3d_ops<T>::covariance_matrix(vgl_pointset_3d<T> const& ptset, vnl_matrix_fixed<T, 3, 3>& C, T frac_ptset){
  vnl_random rand;
  size_t n = ptset.size();
  if (n < 100) {
    std::cerr << "pointset too small to compute covariance accurately = " << n << std::endl;
    return false;
  }
  T f = frac_ptset;
  if (n < 10000)
    f = T(1);
  size_t fn = static_cast<size_t>(f * n);
  //compute mean position
  T mean_x = 0.0, mean_y = 0.0, mean_z = 0.0;
  for (size_t i = 0; i < fn; ++i) {
    size_t k = rand(fn);
    vgl_point_3d<T> p = ptset.p(k);
    mean_x += p.x();
    mean_y += p.y();
    mean_z += p.z();
  }
  mean_x /= fn;   mean_y /= fn;   mean_z /= fn;
  // compute the covariance matrix
  T C00 = 0.0, C01 = 0.0, C02 = 0.0;
  T C11 = 0.0, C12 = 0.0, C22 = 0.0;
  for (size_t i = 0; i < fn; ++i) {
    size_t k = rand(fn);
    vgl_point_3d<T> p = ptset.p(k);
    T vx = p.x() - mean_x;
    T vy = p.y() - mean_y;
    T vz = p.z() - mean_z;
    C00 += vx * vx; C01 += vx * vy; C02 += vx * vz;
    C11 += vy * vy; C12 += vy * vz; C22 += vz * vz;
  }
  C00 /= fn; C01 /= fn; C02 /= fn;
  C11 /= fn; C12 /= fn; C22 /= fn;

  C[0][0] = C00;   C[0][1] = C01; C[1][0] = C01; C[0][2] = C02; C[2][0] = C02;
  C[1][1] = C11;   C[1][2] = C12; C[2][1] = C12; C[2][2] = C22;
  return true;
}
template <class T>
void bvgl_ptset_3d_ops<T>::R_to_diagonalize_covar(vnl_matrix_fixed<T, 3, 3> C, vnl_matrix_fixed<T,3,3>& R, std::vector<T>& lambda){
//cast to double for numerical accuracy
  vnl_matrix<double> Cd(3, 3);
  for (size_t r = 0; r < 3; ++r)
    for (size_t c = 0; c < 3; c++)
    Cd[r][c] = C[r][c];

  vnl_symmetric_eigensystem<double> eigen(Cd);
  vnl_matrix_fixed<T, 3, 3> m;
  for (size_t r = 0; r < 3; r++)
    for (size_t c = 0; c < 3; c++) {
      m[r][c] = static_cast<T>(eigen.V[c][r]); // V^t
    }
  // for checks using fixed matrices and vectors
  vnl_matrix_fixed<T, 3, 3> Cf;
  for (size_t r = 0; r < 3; ++r)
    for (size_t c = 0; c < 3; ++c)
      Cf[r][c] = static_cast<T>(C[r][c]);
  T det = vnl_det(m);
  if(bvgl_debug) std::cout << "det R " << det << std::endl;

  lambda.resize(3);
  for (size_t i = 0; i < 3; ++i)
    lambda[i] = static_cast<T>(eigen.D[i]);
  if(bvgl_debug) std::cout << "lambda " << lambda[0] << ' ' << lambda[1] << ' ' << lambda[2] << std::endl;
  if (det > 0)
    R = m;
  else {
    for(size_t c = 0; c<3; ++c)
      m[0][c] *= -T(1);
    R = m;
  }
  if(bvgl_debug) std::cout << "check if R diagonalizes C \n" << m * Cf * m.transpose() << std::endl;
}
template <class T>
vgl_pointset_3d<T>  bvgl_ptset_3d_ops<T>::Rtrans(vgl_pointset_3d<T> const& ptset, vnl_matrix_fixed<T,3,3> const& R){
  vgl_pointset_3d<T> ret;
  size_t n = ptset.size();
  for(size_t i = 0; i<n; ++i){
    const vgl_point_3d<T>& p = ptset.p(i);
    vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.z()), v_rot;
    v_rot = R*v;
    if(!ptset.has_normals() && !ptset.has_scalars()){
      ret.add_point(vgl_point_3d<T>(v_rot[0], v_rot[1], v_rot[2]));
      continue;
    }
    if(ptset.has_normals()){
      const vgl_vector_3d<T>& norm = ptset.n(i);
      vnl_vector_fixed<T, 3> nv(norm.x(), norm.y(), norm.z()), norm_rot;
      norm_rot = R*nv;
      if(!ptset.has_scalars()){
        ret.add_point_with_normal(vgl_point_3d<T>(v_rot[0], v_rot[1], v_rot[2]),
                                  vgl_vector_3d<T>(norm_rot[0], norm_rot[1], norm_rot[2]));
      }else{
        T sc = ptset.sc(i);
        ret.add_point_with_normal_and_scalar(vgl_point_3d<T>(v_rot[0], v_rot[1], v_rot[2]),
                                             vgl_vector_3d<T>(norm_rot[0], norm_rot[1], norm_rot[2]), sc);
      }
      continue;
    }
    if(ptset.has_scalars()){
     T sc = ptset.sc(i);
     ret.add_point_with_scalar(vgl_point_3d<T>(v_rot[0], v_rot[1], v_rot[2]),sc);
     continue;
    }
  }
  return ret;
}
template <class T>
vgl_pointset_3d<T> bvgl_ptset_3d_ops<T>::Strans(vgl_pointset_3d<T> const& ptset, T scale, vnl_matrix_fixed<T,3,3> const& R,vnl_vector_fixed<T,3> const& t, bool translate_first){
  vgl_pointset_3d<T> ret;
  size_t n = ptset.size();
  for(size_t i = 0; i<n; ++i){
    const vgl_point_3d<T>& p = ptset.p(i);
    vnl_vector_fixed<T, 3> v(p.x(), p.y(), p.z()), v_sim;

    if(translate_first)
      v_sim = scale*R*(v+t);
    else
      v_sim = scale*R*v +t;

    if(!ptset.has_normals() && !ptset.has_scalars()){
      ret.add_point(vgl_point_3d<T>(v_sim[0], v_sim[1], v_sim[2]));
      continue;
    }
    if(ptset.has_normals()){
      const vgl_vector_3d<T>& norm = ptset.n(i);
      // no need to scale or translate the normal
      vnl_vector_fixed<T, 3> nv(norm.x(), norm.y(), norm.z()), norm_sim;
      norm_sim = R*nv;
      if(!ptset.has_scalars()){
        ret.add_point_with_normal(vgl_point_3d<T>(v_sim[0], v_sim[1], v_sim[2]),
                                  vgl_vector_3d<T>(norm_sim[0], norm_sim[1], norm_sim[2]));
      }else{
        T sc = ptset.sc(i);
        ret.add_point_with_normal_and_scalar(vgl_point_3d<T>(v_sim[0], v_sim[1], v_sim[2]),
                                             vgl_vector_3d<T>(norm_sim[0], norm_sim[1], norm_sim[2]), sc);
      }
      continue;
    }
    if(ptset.has_scalars()){
     T sc = ptset.sc(i);
     ret.add_point_with_scalar(vgl_point_3d<T>(v_sim[0], v_sim[1], v_sim[2]),sc);
     continue;
    }
  }
  return ret;
}

#define BVGL_PTSET_3D_OPS_INSTANTIATE(T)        \
template class bvgl_ptset_3d_ops<T>

#endif //bvgl_ptset_3d_ops_hxx_
