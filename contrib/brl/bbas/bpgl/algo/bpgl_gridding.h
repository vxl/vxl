// This is bbas/bpgl/algo/bpgl_gridding.h
#ifndef bpgl_gridding_h_
#define bpgl_gridding_h_
//:
// \file
// \brief Transform irregular data to gridded 2D format (e.g. DSMs)
// \author Dan Crispell
// \date Nov 26, 2018
//

#include <iostream>
#include <stdexcept>
#include <vector>
#include <bvgl/bvgl_k_nearest_neighbors_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bpgl_gridding
{

template<class T, class DATA_T>
class inverse_distance_interp
{
public:
  inverse_distance_interp( T max_dist=vnl_numeric_traits<T>::maxval,
                           DATA_T invalid_val=DATA_T(NAN) )
    : max_dist_(max_dist), invalid_val_(invalid_val)
  {}
  DATA_T operator() ( vgl_point_2d<T> loc,
                      std::vector<vgl_point_2d<T>> const& neighbor_locs,
                      std::vector<DATA_T> const& neighbor_vals ) const
  {
    T weight_sum(0);
    T val_sum(0);
    const T eps(1e-6);
    const unsigned num_neighbors = neighbor_locs.size();
    for (unsigned i=0; i<num_neighbors; ++i) {
      T dist = (neighbor_locs[i] - loc).length();
      if (dist <= max_dist_) {
        if (dist < eps) {
          dist = eps;
        }
        T weight = 1.0 / dist;
        weight_sum += weight;
        val_sum += weight*neighbor_vals[i];
      }
    }
    if (weight_sum == T(0)) {
      return invalid_val_;
    }
    return val_sum / weight_sum;
  }
private:
  T max_dist_;
  DATA_T invalid_val_;
};


template<class T, class DATA_T>
class linear_interp
{
public:
  linear_interp( T max_dist=vnl_numeric_traits<T>::maxval,
                 DATA_T invalid_val=DATA_T(NAN) )
    : max_dist_(max_dist), invalid_val_(invalid_val)
  {}
  DATA_T operator() ( vgl_point_2d<T> loc,
                      std::vector<vgl_point_2d<T>> const& neighbor_locs,
                      std::vector<DATA_T> const& neighbor_vals ) const
  {
    T weight_sum(0);
    T val_sum(0);
    const T eps(1e-6);
    const unsigned num_neighbors = neighbor_locs.size();
    vnl_matrix<T> A(num_neighbors,3);
    vnl_vector<T> b(num_neighbors);
    int num_valid_neighbors = 0;
    for (unsigned i=0; i<num_neighbors; ++i) {
      vgl_point_2d<T> const& neighbor_loc(neighbor_locs[i]);
      T dist = (neighbor_locs[i] - loc).length();
      T weight = 0;
      if (dist <= max_dist_) {
        if (dist < eps) {
          dist = eps;
        }
        weight = 1.0 / dist;
        ++num_valid_neighbors;
      }
      A[i][0] = weight * neighbor_loc.x();
      A[i][1] = weight * neighbor_loc.y();
      A[i][2] = weight;
      b[i] = weight * neighbor_vals[i];
    }
    if (num_valid_neighbors < 3) {
      return invalid_val_;
    }
    // employ Tikhonov Regularization to cope with degenerate point configurations
    vnl_matrix<T> R(3, 3, 0);
    const T reg_const = 1e-3;
    for (int d=0; d<3; ++d) {
      R[d][d] = reg_const;
    }
    vnl_matrix<T> A_transpose = A.transpose();
    vnl_vector<T> f = vnl_matrix_inverse<T>(A_transpose*A + R.transpose()*R)*A_transpose * b;
    DATA_T value = f[0]*loc.x() + f[1]*loc.y() + f[2];
    return value;
  }
private:
  T max_dist_;
  DATA_T invalid_val_;
};



template<class T, class DATA_T, class INTERP_T>
vil_image_view<DATA_T>
grid_data_2d(std::vector<vgl_point_2d<T>> const& data_in_loc,
             std::vector<DATA_T> const& data_in,
             vgl_point_2d<T> out_upper_left,
             size_t out_ni, size_t out_nj,
             T step_size,
             INTERP_T &interp_fun,
             unsigned num_nearest_neighbors,
             double out_theta_radians=0.0)
{
  // validate input
  if (data_in_loc.size() != data_in.size()) {
    throw std::runtime_error("Input location and data arrays not equal size");
  }

  // create knn instance
  bvgl_k_nearest_neighbors_2d<T> knn(data_in_loc);
  if (!knn.is_valid()) {
    throw std::runtime_error("KNN initialization failure");
  }

  vgl_vector_2d<T> i_vec(std::cos(out_theta_radians), std::sin(out_theta_radians));
  vgl_vector_2d<T> j_vec(std::sin(out_theta_radians), -std::cos(out_theta_radians));

  // loop across all grid values
  vil_image_view<DATA_T> gridded(out_ni, out_nj);
  for (unsigned j=0; j<out_nj; ++j) {
    for (unsigned i=0; i<out_ni; ++i) {

      // interpolation point
      vgl_point_2d<T> loc = out_upper_left +
                          + i*step_size*i_vec
                          + j*step_size*j_vec;

      // nearest neighbors
      std::vector<vgl_point_2d<T> > neighbor_locs;
      std::vector<unsigned> neighbor_indices;
      if (!knn.knn(loc, num_nearest_neighbors, neighbor_locs, neighbor_indices)) {
        throw std::runtime_error("KNN failed to return neighbors");
        continue;
      }

      // neighbor values for interpolation
      std::vector<DATA_T> neighbor_vals;
      for (auto index : neighbor_indices) {
        neighbor_vals.push_back(data_in[index]);
      }

      // interpolate
      T val = interp_fun(loc, neighbor_locs, neighbor_vals);
      gridded(i,j) = val;
    }
  }
  return gridded;
}

 template<class pointT, class pixelT>
void  pointset_from_grid(vil_image_view<pixelT> const& grid, vgl_point_2d<pointT> const& upper_left, pointT step_size,
                         std::vector<vgl_point_3d<pointT> >& ptset, double out_theta_radians = 0.0){
  ptset.clear();
  vgl_vector_2d<pointT> i_vec(std::cos(out_theta_radians), std::sin(out_theta_radians));
  vgl_vector_2d<pointT> j_vec(std::sin(out_theta_radians), -std::cos(out_theta_radians));
  pointT xul = upper_left.x(), yul = upper_left.y();
  size_t ni = grid.ni(), nj = grid.nj();
  for(size_t j = 0; j<nj; ++j)
    for(size_t i = 0; i<ni; ++i){
      vgl_point_2d<pixelT> loc = upper_left +
        i*step_size*i_vec + j*step_size*j_vec;
      pointT z = grid(i,j);
      if(!vnl_math::isfinite(z))
        continue;
      ptset.emplace_back(loc.x(), loc.y(), z);
    }
}


}
#endif
