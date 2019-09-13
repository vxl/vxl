#ifndef bvgl_ptset_3d_ops_hxx_
#define bvgl_ptset_3d_ops_hxx_

#include "bvgl_ptset_3d_ops.h"
#include <vgl/algo/vgl_fit_plane_3d.h>
#include <vgl/vgl_distance.h>
template <class T>
void bvgl_ptset_3d_ops<T>::compute_roughness(T radius, size_t n_nbrs){
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
#define BVGL_PTSET_3D_OPS_INSTANTIATE(T)    \
template class bvgl_ptset_3d_ops<T>

#endif //bvgl_ptset_3d_ops_hxx_
