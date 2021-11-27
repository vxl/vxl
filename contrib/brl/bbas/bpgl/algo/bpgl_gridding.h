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
#include <numeric>
#include <bvgl/bvgl_k_nearest_neighbors_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
#include "bpgl_surface_type.h"
#include <vil/vil_save.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bpgl_gridding
{


//: Inverse distance interpolation class
// invalid_val: Value to return when interpolation is not appropriate
// dist_eps: The smallest meaningful distance of input points. Must be > 0.
template<class T, class DATA_T>
class inverse_distance_interp
{
 public:

  // constructors
  inverse_distance_interp() = default;

  inverse_distance_interp(
      DATA_T invalid_val,
      T dist_eps) :
    invalid_val_(invalid_val),
    dist_eps_(dist_eps)
  {}

  // accessors
  DATA_T invalid_val() const { return invalid_val_; }
  void invalid_val(DATA_T x) { invalid_val_ = x; }

  T dist_eps() const { return dist_eps_; }
  void dist_eps(T x) { dist_eps_ = x; }

  // interpolation operator
  DATA_T operator() (
      vgl_point_2d<T> interp_loc,
      std::vector<vgl_point_2d<T> > const& neighbor_locs,
      std::vector<DATA_T> const& neighbor_vals,
      T max_dist = vnl_numeric_traits<T>::maxval
      ) const
  {
    T weight_sum(0);
    T val_sum(0);
    const unsigned num_neighbors = neighbor_locs.size();
    for (unsigned i=0; i<num_neighbors; ++i) {
      T dist = (neighbor_locs[i] - interp_loc).length();
      if (dist <= max_dist) {
        if (dist < this->dist_eps_) {
          dist = this->dist_eps_;
        }
        T weight = 1.0 / dist;
        weight_sum += weight;
        val_sum += weight*neighbor_vals[i];
      }
    }
    if (weight_sum == T(0)) {
      return this->invalid_val_;
    }
    return val_sum / weight_sum;
  }

 private:

  // parameters with defaults
  DATA_T invalid_val_ = DATA_T(NAN);
  T dist_eps_ = 1e-5;

};


//: Linear interpolation class
// invalid_val: Value to return when interpolation is not appropriate
// dist_eps: The smallest meaningful distance of input points. Must be > 0.
// dist_iexp: neighbor weight is proportional to (1/dist)^dist_iexp
// regularization_lambda: Larger regularization values will bias the solution
//    towards "flatter" functions.  Very large values will result in weighted
//    averages of neighbor values.
// rcond_thresh: Threshold for inverse matrix conditioning. Must be > 0
// relative_interp: interpolation relative to neighbor centroids
//
// Note: internals are represented at double precision to ensure
// accuracy of the final result.  Only on output is the resulting
// interpolated value cast back to DATA_T
template<class T, class DATA_T>
class linear_interp
{
 public:

  // constructors
  linear_interp() = default;

  linear_interp(
      DATA_T invalid_val,
      T dist_eps) :
    invalid_val_(invalid_val),
    dist_eps_(dist_eps)
  {}

  // accessors
  DATA_T invalid_val() const { return invalid_val_; }
  void invalid_val(DATA_T x) { invalid_val_ = x; }

  T dist_eps() const { return dist_eps_; }
  void dist_eps(T x) { dist_eps_ = x; }

  int dist_iexp() const { return dist_iexp_; }
  void dist_iexp(int x) { dist_iexp_ = x; }

  double regularization_lambda() const { return regularization_lambda_; }
  void regularization_lambda(double x) { regularization_lambda_ = x; }

  double rcond_thresh() const { return rcond_thresh_; }
  void rcond_thresh(double x) { rcond_thresh_ = x; }

  bool relative_interp() const { return relative_interp_; }
  void relative_interp(bool x) { relative_interp_ = x; }

  // interpolation operator
  DATA_T operator() (
      vgl_point_2d<T> interp_loc,
      std::vector<vgl_point_2d<T> > const& neighbor_locs,
      std::vector<DATA_T> const& neighbor_vals,
      T max_dist = vnl_numeric_traits<T>::maxval
      ) const
  {
    const unsigned num_neighbors = neighbor_locs.size();

    // vectors of valid neighbor data
    std::vector<double> X,Y,V,W;
    int num_valid_neighbors = 0;

    for (unsigned i=0; i<num_neighbors; ++i) {
      T dist = (neighbor_locs[i] - interp_loc).length();
      if (dist <= max_dist) {
        if (dist < this->dist_eps_) {
          dist = this->dist_eps_;
        }

        // neighbor weight
        double dist_d = static_cast<double>(dist);
        double weight = 1.0 / std::pow(dist_d, dist_iexp_);

        // save to internal storage
        X.emplace_back(static_cast<double>(neighbor_locs[i].x()));
        Y.emplace_back(static_cast<double>(neighbor_locs[i].y()));
        V.emplace_back(static_cast<double>(neighbor_vals[i]));
        W.emplace_back(weight);
        num_valid_neighbors++;
      }
    }

    // check for sufficent neighbors
    if (num_valid_neighbors < 3) {
      // std::cerr << "insufficent neighbors" << std::endl;
      return this->invalid_val_;
    }

    // weight normalization
    double weight_norm = std::accumulate(W.begin(), W.end(), 0.0);
    for (auto& w : W) {
      w /= weight_norm;
    }

    // absolute interpolation: origin at 0
    // relative interpolation: origin at neighbor loc/val centroid
    double x_origin = 0, y_origin = 0, v_origin = 0;
    if (relative_interp_) {
      double N = static_cast<double>(num_valid_neighbors);
      x_origin = std::accumulate(X.begin(), X.end(), 0.0) / N;
      y_origin = std::accumulate(Y.begin(), Y.end(), 0.0) / N;
      v_origin = std::accumulate(V.begin(), V.end(), 0.0) / N;
    }

    // system matrices
    vnl_matrix<double> A(num_valid_neighbors,3);
    vnl_vector<double> b(num_valid_neighbors);
    for (unsigned i=0; i<(unsigned)num_valid_neighbors; ++i) {
      A[i][0] = W[i] * (X[i] - x_origin);
      A[i][1] = W[i] * (Y[i] - y_origin);
      A[i][2] = W[i];
      b[i] = W[i] * (V[i] - v_origin);
    }

    // employ Tikhonov Regularization to cope with degenerate point configurations
    // f = inv(AT * A + lambda * I) * AT * b
    vnl_matrix<double> lambdaI(3, 3, 0);
    for (int d=0; d<3; ++d) {
      lambdaI[d][d] = regularization_lambda_;
    }

    // matrix processing
    vnl_matrix<double> At = A.transpose();
    vnl_matrix<double> AtA_reg = At*A + lambdaI;
    vnl_matrix_inverse<double> inv_AtA_reg(AtA_reg.as_ref());

    // check reciprocal condition number
    auto rcond = inv_AtA_reg.well_condition();
    if (rcond < rcond_thresh_) {
      std::cerr << "matrix has poor condition (" << rcond << ")\n" << std::endl;
      return this->invalid_val_;
    }

    // final solution
    vnl_vector<double> f = inv_AtA_reg.as_matrix() * At * b;
    double x = static_cast<double>(interp_loc.x());
    double y = static_cast<double>(interp_loc.y());
    double value = f[0]*(x - x_origin) + f[1]*(y - y_origin) + f[2] + v_origin;

    // cast as data type
    DATA_T value_return = static_cast<DATA_T>(value);
    return value_return;
  }

 private:

  // parameters with defaults
  DATA_T invalid_val_ = DATA_T(NAN);
  T dist_eps_ = 1e-5;
  int dist_iexp_ = 2;
  double regularization_lambda_ = 1e-3;
  double rcond_thresh_ = 1e-8;
  bool relative_interp_ = true;

};



template<class T, class DATA_T, class INTERP_T>
vil_image_view<DATA_T>
grid_data_2d(
    INTERP_T const& interp_fun,
    std::vector<vgl_point_2d<T>> const& data_in_loc,
    std::vector<DATA_T> const& data_in,
    vgl_point_2d<T> out_upper_left,
    size_t out_ni,
    size_t out_nj,
    T step_size,
    unsigned min_neighbors = 3,
    unsigned max_neighbors = 5,
    T max_dist = vnl_numeric_traits<T>::maxval,
    double out_theta_radians = 0.0)
{
  // total number of points
  size_t npts = data_in_loc.size();

  // validate input
  if (npts != data_in.size()) {
    throw std::runtime_error("Input location and data arrays not equal size");
  }

  // validate min/max neighbor range
  if (size_t(min_neighbors) > npts) {
    throw std::runtime_error("Fewer points than minimum number of neighbors");
  }
  if (size_t(max_neighbors) > npts) {
    max_neighbors = unsigned(npts);
  }
  if (min_neighbors > max_neighbors) {
    throw std::runtime_error("Invalid neighbor range");
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
      vgl_point_2d<T> loc = out_upper_left
                          + i*step_size*i_vec
                          + j*step_size*j_vec;

      // retrieve at most max_neighbors within max_dist of interpolation point
      std::vector<vgl_point_2d<T> > neighbor_locs;
      std::vector<unsigned> neighbor_indices;
      if (!knn.knn(loc, max_neighbors, neighbor_locs, neighbor_indices, max_dist)) {
        throw std::runtime_error("KNN failed to return neighbors");
      }

      // check for at least min_neighbors
      if (neighbor_indices.size() < min_neighbors) {
        gridded(i,j) = interp_fun.invalid_val();
        continue;
      }

      // neighbor values for interpolation
      std::vector<DATA_T> neighbor_vals;
      for (auto nidx : neighbor_indices) {
        neighbor_vals.push_back(data_in[nidx]);
      }

      // interpolate via non-virtual method
      T val = interp_fun(loc, neighbor_locs, neighbor_vals, max_dist);
      gridded(i,j) = val;
    }
  }
  return gridded;
}
// map surface types from disparity space to dsm grid space using
// the disparity pixel index attached to the vector index of
// each 2-d point in the input, data_in_loc
 template<class T>
void grid_surface_type_2d(
    std::vector<vgl_point_2d<T> > const& data_in_loc,
    bpgl_surface_type const& disparity_stype,
    std::map<size_t, std::pair<size_t, size_t> >& pt_indx_to_pix,
    bpgl_surface_type& heightmap_stype,
    vgl_point_2d<T> out_upper_left,
    T step_size,
    unsigned min_neighbors = 3,
    unsigned max_neighbors = 5,
    T max_dist = vnl_numeric_traits<T>::maxval)
{
  std::vector<bpgl_surface_type::stype>& styps = heightmap_stype.stypes();
  size_t dni = disparity_stype.ni(), dnj = disparity_stype.nj();
  size_t hni = heightmap_stype.ni(), hnj = heightmap_stype.nj();
 
 // total number of points
 size_t npts = data_in_loc.size();

 // validate input
 if (npts != pt_indx_to_pix.size()) {
   throw std::runtime_error("Input locations and pix index not equal size");
 }

 // validate min/max neighbor range
 if (size_t(min_neighbors) > npts) {
   throw std::runtime_error("Fewer points than minimum number of neighbors");
 }
 if (size_t(max_neighbors) > npts) {
   max_neighbors = unsigned(npts);
 }
 if (min_neighbors > max_neighbors) {
   throw std::runtime_error("Invalid neighbor range");
 }

  // create knn instance
 bvgl_k_nearest_neighbors_2d<T> knn(data_in_loc);
 if (!knn.is_valid()) {
   throw std::runtime_error("KNN initialization failure");
 }
 size_t out_ni = heightmap_stype.ni(), out_nj = heightmap_stype.nj();
 vgl_vector_2d<T> i_vec(T(1), T(0));
 vgl_vector_2d<T> j_vec(T(0), -T(1));//spatial y coordinate is negated image j coordinate
 
 for (unsigned j=0; j<hnj; ++j) {
     for (unsigned i = 0; i < hni; ++i) {

         // grid point
         vgl_point_2d<T> loc = out_upper_left + i * step_size * i_vec + j * step_size * j_vec;

         // retrieve at most max_neighbors within max_dist of interpolation point
         std::vector<vgl_point_2d<T> > neighbor_locs;
         std::vector<unsigned> neighbor_indices;
         if (!knn.knn(loc, max_neighbors, neighbor_locs, neighbor_indices, max_dist)) {
             throw std::runtime_error("KNN failed to return neighbors");
         }

         // check for at least min_neighbors
         if (neighbor_indices.size() < min_neighbors) {
             heightmap_stype.p(i, j, bpgl_surface_type::INVALID_DATA) = 1.0f;
             continue;
         }
         // only consider neighbors within circumscribed circle around grid cell
         // that touches the center of adjacent cells
         std::vector<size_t> reduced_indices;
         size_t iidx = 0;
         float ccirc_radius = step_size * vnl_math::sqrt2;
         for (auto p : neighbor_locs) {
             if (vgl_distance(p, loc) <= ccirc_radius)
               reduced_indices.push_back(neighbor_indices[iidx]);
             ++iidx;
         }
         if (reduced_indices.size() == 0) {
             heightmap_stype.p(i, j, bpgl_surface_type::INVALID_DATA) = 1.0f;
             continue;
         }
         

         // pix values in disparity space surface type
         // take min probabilty as characteristic of grid cell.
         // except for the case of shadow, then take max
         for (auto t : styps) {
           float min_p = 1.0f, max_p = 0.0f;
           for (auto nidx : reduced_indices) {
             std::pair<size_t, size_t> pix = pt_indx_to_pix[nidx];
             size_t di = pix.first, dj = pix.second;
             if (di >= dni || dj >= dnj)
               continue;
             float p = disparity_stype.const_p(di, dj, t);
             if (p < min_p) min_p = p;
             if (p > max_p) max_p = p;
           }
           if(t == bpgl_surface_type::SHADOW)
             heightmap_stype.p(i, j, t) = max_p;
           else
             heightmap_stype.p(i, j, t) = min_p;
         }
     }//end i,
 }//end j
}// end grid
 

template<class pointT, class pixelT>
void pointset_from_grid(vil_image_view<pixelT> const& grid,
                        vgl_point_2d<pointT> const& upper_left,
                        pointT step_size,
                        std::vector<vgl_point_3d<pointT> >& ptset,
                        double out_theta_radians = 0.0)
{
  ptset.clear();
  vgl_vector_2d<pointT> i_vec(std::cos(out_theta_radians), std::sin(out_theta_radians));
  vgl_vector_2d<pointT> j_vec(std::sin(out_theta_radians), -std::cos(out_theta_radians));
  pointT xul = upper_left.x(), yul = upper_left.y();
  size_t ni = grid.ni(), nj = grid.nj();
  for (size_t j = 0; j<nj; ++j) {
    for (size_t i = 0; i<ni; ++i) {
      vgl_point_2d<pixelT> loc = upper_left +
        i*step_size*i_vec + j*step_size*j_vec;
      pointT z = grid(i,j);
      if(!vnl_math::isfinite(z))
        continue;
      ptset.emplace_back(loc.x(), loc.y(), z);
    }
  }
}


}
#endif
