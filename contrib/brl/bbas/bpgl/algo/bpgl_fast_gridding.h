// This is bbas/bpgl/algo/bpgl_fast_gridding.h
#ifndef bpgl_fast_gridding_h_
#define bpgl_fast_gridding_h_
//:
// \file
// \brief Transform irregular data to gridded 2D format (e.g. DSMs) with arrays
// \author J.L. Mundy
// \date Sept 17, 2019
//

#include <iostream>
#include <stdexcept>
#include <vector>
#include <numeric>
#include <tuple>
#include <bvgl/bvgl_k_nearest_neighbors_2d.h>
#include <vnl/vnl_numeric_traits.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_distance.h>
#include "bpgl_surface_type.h"
#include <vil/vil_save.h>
#include <immintrin.h>
#include <cstdint>
#include <vgl/vgl_pointset_3d.h>
#include <fstream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
namespace bpgl_fast_gridding
{
  template<class T, class DATA_T> 
    inline DATA_T weighted_sum(std::vector<DATA_T> const& data, std::vector<T> const& weights); 
  // overloading the function
  // with data value pair
  template<class T>
    inline std::pair<T, T> weighted_sum(std::vector<std::pair<T, T>> const& data, std::vector<T> const& weights) {
    size_t n = weights.size();
    std::pair<T, T> sum(0.0f, 0.0f);
    T weight_sum = 0.0f;
    
    for (size_t i = 0; i < n; ++i) {
      T w = weights[i];
      weight_sum += w;
      const std::pair<T, T>& dat = data[i];
      sum.first += w * dat.first;
      sum.second += w * dat.second;
    }
    
    if (weight_sum == T(0))
      return std::pair<T, T>(NAN, NAN);
    
    sum.first /= weight_sum;
    sum.second /= weight_sum;
    return sum;
  }
  // overloading the function
  // with single data value
  template<class T>
    inline T weighted_sum(std::vector<T> const& data, std::vector<T> const& weights) {
    size_t n = weights.size();
    T sum = 0.0;
    T weight_sum = 0.0f;
    
    for (size_t i = 0; i < n; ++i) {
      T w = weights[i];
      weight_sum += w;
      sum += w * data[i];
    }
    
    if (weight_sum == T(0))
      return T(NAN);
    
    sum /= weight_sum;
    return sum;
  }
    // overloading the function
  // with triple data and pair of weights
    template<class T>
    inline std::tuple<T, T, T> weighted_sum(std::vector<std::tuple<T, T, T>> const& data, std::vector<std::pair<T, T> > const& weights) {
        size_t n = weights.size();
        std::tuple<T, T, T> sum(0, 0, 0);
        std::pair<T, T>  weight_sum(0, 0);
        for (size_t i = 0; i < n; ++i) {
            const std::pair<T, T>& w = weights[i];
            const std::tuple<T, T, T>& dat = data[i];
            weight_sum.first += w.first; weight_sum.second += w.second;
            std::get<0>(sum) += w.first * std::get<0>(dat);
            std::get<1>(sum) += w.first * std::get<1>(dat);
            std::get<2>(sum) += w.second * std::get<2>(dat);
        }
        if (weight_sum.first == T(0) || weight_sum.second == T(0))
            return std::tuple<T, T, T>(NAN, NAN, NAN);
        std::get<0>(sum) /= weight_sum.first;
        std::get<1>(sum) /= weight_sum.first;
        std::get<2>(sum) /= weight_sum.second; std::get<2>(sum) = sqrt(std::get<2>(sum));
        return sum;
    }

  // predicate for sorting  
  static bool ptless(std::pair< uint32_t, float> const a, std::pair<uint32_t, float > const b) { return a.second < b.second; }
  
  // index for griding points
  template<class T, class DATA_T>
  inline  void grid_neighbor_indices_2d(std::vector<vgl_point_2d<T> > const& pts_2d,
         std::vector<DATA_T> const& vals, vgl_point_2d<T> const& upper_left,
         size_t ni, size_t nj, size_t max_nbrs, T step_size, int nbrhood_radius,
  std::vector<std::vector<std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> > > >& index) {
    // point locations around a point from which neighbors can be found
    // radius 1
    std::vector<std::pair<int, int > > nbrd_1 =
      { {-1, 0}, {0,0}, {1,0}, {0, -1}, {0, 1}, {-1, -1}, {-1, 1},{1, -1}, {1, 1} };
    // radius 2
    std::vector<std::pair<int, int > > nbrd_2 =
      { {-2, 0},{-1, 0}, {0,0}, {1,0}, {2,0}, {0,-2}, {0, -1},{0, 1}, {0, 2}, {-1, -1}, {-1, 1},{1, -1}, {1, 1} };
    std::vector<std::pair<int, int > > nbd;

    // set size of point grid index
    index.resize(nj, std::vector < std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> > >(ni));

    // upper left corner of the array
    float ulx = upper_left.x(), uly = upper_left.y();

    // determine the neigborhood domain
    nbd = nbrd_1;
    if (nbrhood_radius >= 2)
      nbd = nbrd_2;
    size_t n = pts_2d.size();

    vul_timer t;

    // set up grid 
    // points are (upper left_x + step_size(0,ni-1))  and (upper_left y - step_size*y(0,nj-1))
    for (size_t k = 0; k < pts_2d.size(); ++k) {
      const vgl_point_2d<T>& p = pts_2d[k];
      float x = p.x(), y = p.y();

      // determine center grid location corresponding to p
      int idx = (x - ulx) / step_size, idy = (uly - y) / step_size;
      // iterate over the neigborhood
      for(size_t r = 0; r<nbd.size(); ++r){
        int jj = idy + nbd[r].second;
        if (jj < 0 || jj >= nj)
          continue;
        int ii = idx + nbd[r].first;
        if (ii < 0 || ii >= ni)
          continue;
        std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> tup(k, p, vals[k]);
        index[jj][ii].push_back(tup);
      }
    }

    std::cout << "PHASE I COMPLETE " << t.real() << std::endl;
    t.mark();
    // second pass to keep n closest to center
    // points are now placed in grid cells but may be more than the
    // required number. Compute city block distance and retain
    // the required number closest to the cell center
    for(size_t j = 0; j<nj; ++j)
      for (size_t i = 0; i < ni; ++i) {
        float x0 = ulx + step_size * i, y0 = uly - step_size * j;
        std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> > indices_cp = index[j][i];
        size_t nnbrs = indices_cp.size();
        std::vector < std::pair< uint32_t, float> > closest;
        for (size_t k = 0; k < nnbrs; ++k) {
          std::tuple<uint32_t, vgl_point_2d<T>, DATA_T>& tup = indices_cp[k];
          float dx = std::get<1>(tup).x() - x0, dy = std::get<1>(tup).y() - y0;
          closest.emplace_back(k, fabs(dx) + fabs(dy));
        }
        if(nnbrs > max_nbrs)
          std::sort(closest.begin(), closest.end(), ptless);
        index[j][i].clear();
        for (size_t c = 0; c < max_nbrs && c < closest.size(); ++c)
          index[j][i].push_back(indices_cp[closest[c].first]);
      }
    std::cout << "PHASE I and PHASE II COMPLETE " << t.real() << std::endl;
  };
#if 0
  //: Inverse distance interpolation class
  // invalid_val: Value to return when interpolation is not appropriate
  // dist_eps: The smallest meaningful distance of input points. Must be > 0.
  template<class T, class DATA_T, class OUT_T>
    class tuple_inverse_distance_interp
  {
  public:
    
    // constructors
    tuple_inverse_distance_interp() = default;
    
  tuple_inverse_distance_interp(
                                T invalid_val,
                                T dist_eps) :
    invalid_val_(invalid_val),
      dist_eps_(dist_eps)
      {}
    
    // accessors
    float invalid_val() const { return invalid_val_; }
    void invalid_val(float x) { invalid_val_ = x; }
    
    float dist_eps() const { return dist_eps_; }
    void dist_eps(float x) { dist_eps_ = x; }
    
    // interpolation operator
    OUT_T operator() (
      vgl_point_2d<T> interp_loc,
      std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> > const& neighbor_data,
      float max_dist = vnl_numeric_traits<T>::maxval
                       ) const
    {
      T weight_sum(0);
      const unsigned num_neighbors = neighbor_data.size();
      std::vector<T> weights;
      std::vector<DATA_T> data;
      for (unsigned i = 0; i < num_neighbors; ++i) {
        // distance between the point and the cell center.
        float dist = (std::get<1>(neighbor_data[i]) - interp_loc).length();
        if (dist <= max_dist) {
          if (dist < this->dist_eps_) {
            dist = this->dist_eps_;
          }
          // assemble weights and data to be weighted
          float weight = 1.0 / dist;
          weights.push_back(weight);
          data.push_back(std::get<2>(neighbor_data[i]));
        }
      }
      return weighted_sum<T>(data, weights);
    }
    
  private:
    
    // parameters with defaults
    float dist_eps_ = 1e-5;
    
  };

  // class using parallel vector arithmetic
  template<class T, class DATA_T>
    class AVX_512_inverse_distance_interp
  {
  public:
    
    
    // constructors
    AVX_512_inverse_distance_interp() = default;
    
  AVX_512_inverse_distance_interp(
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
      unsigned num_neighbors = neighbor_locs.size();
      if (num_neighbors > 8)
        throw std::runtime_error("AVX 512 handles a maximum of 8 points_2d");
      
      // load neighbor locations and  values to be interpolated into registers
      const float* raw_locs = reinterpret_cast<const float*>(neighbor_locs.data());
      __m512 pdata = _mm512_loadu_ps(raw_locs);
      const float* raw_vals = reinterpret_cast<const float*>(neighbor_vals.data());
      __m512 vdata = _mm512_loadu_ps(raw_vals);
      
      // set mask for number of neighbors
      __mmask16 nbr_mask = static_cast<__mmask16>((1U << 2* num_neighbors) - 1);
      // zero out portions of the registers not occupied with data
      __m512 masked_nbrs = _mm512_maskz_mov_ps(nbr_mask, pdata);
      __m512 masked_vals = _mm512_maskz_mov_ps(nbr_mask, vdata);
      
      // fill in copies of the grid cell center location to subtract from neighbor locations
      alignas(64) float loc_data[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      for (int i = 0; i < num_neighbors; ++i) {
        loc_data[2 * i] = interp_loc.x(); // even index
        loc_data[2 * i + 1] = interp_loc.y(); // odd index
      }
      __m512 locd =  _mm512_load_ps(loc_data);
      
      // subtract the grid center from neighbor locations
      __m512 vresult = _mm512_sub_ps(pdata, locd);
      
      
      // Now collect the x and y locations as separate arrays for computing distance magnitude
      __m512i idx_a = _mm512_set_epi32(14, 12, 10, 8, 6, 4, 2, 0,
                                       14, 12, 10, 8, 6, 4, 2, 0);
      __m512i idx_b = _mm512_set_epi32(15, 13, 11, 9, 7, 5, 3, 1,
                                       15, 13, 11, 9, 7, 5, 3, 1);
      
      // vector a contains x coordinate differences and vector b contains y coordinate differences
      __m512 a = _mm512_permutexvar_ps(idx_a, vresult);
      __m512 b = _mm512_permutexvar_ps(idx_b, vresult);
      
      // Compute sqrt(a² + b²)
      __m512 a2 = _mm512_mul_ps(a, a);
      __m512 b2 = _mm512_mul_ps(b, b);
      __m512 sum = _mm512_add_ps(a2, b2);
      __m512 mag_result = _mm512_sqrt_ps(sum);
      
      // Clip the distance magnitudes within bounds
      __m512 mvlow = _mm512_set1_ps(dist_eps_);
      __m512 mvhi = _mm512_set1_ps(max_dist);
      __m512 clipped = _mm512_max_ps(mvlow, _mm512_min_ps(mag_result, mvhi));
      __m512 reciprocal = _mm512_rcp14_ps(clipped);
      
      // mask the result to the number of weights (neighbors)
      __mmask16 result_mask = static_cast<__mmask16>((1U << num_neighbors) - 1);
      __m512 masked_weight_result = _mm512_maskz_mov_ps(result_mask, reciprocal);
      // sum of the weights
      float total_weight = _mm512_reduce_add_ps(masked_weight_result);
      
      // multiply the weights by the values to be interpolated
      __m512 weighted_vals = _mm512_mul_ps(masked_weight_result, vdata);
      
      // sum the result
      float total_weighted_sum = _mm512_reduce_add_ps(weighted_vals);
      
      // the interpolated value
      return total_weighted_sum / total_weight;
    }
    
  private:
    
    // parameters with defaults
    DATA_T invalid_val_ = DATA_T(NAN);
    T dist_eps_ = 1e-5;
  };
#endif


  template <class T, class DATA_T, class OUT_T>
  struct interp;

  template <class T, class DATA_T>
  struct interp<T, DATA_T, DATA_T> {
      DATA_T operator()(
          vgl_point_2d<T> interp_loc,
          std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> > const& neighbor_data
          ) {
          T weight_sum(0);
          const unsigned num_neighbors = neighbor_data.size();
          std::vector<T> weights, probs;
          std::vector<DATA_T> data;
          for (unsigned i = 0; i < num_neighbors; ++i) {
              // distance between the point and the cell center.
              float dist = (std::get<1>(neighbor_data[i]) - interp_loc).length();

              // assemble weights and data to be weighted
              float weight = 1.0 / dist;
              weights.push_back(weight);
              data.push_back(std::get<2>(neighbor_data[i]));

          }
          return weighted_sum<T>(data, weights);
      }
  };
// specialize to all three heightmaps at once
  template <class T>
  struct interp<T, std::pair<T, T>, std::tuple<T, T, T> > {
      std::tuple<T, T, T> operator() (
          vgl_point_2d<T> interp_loc,
          std::vector<std::tuple<uint32_t, vgl_point_2d<T>, std::pair<T, T> > > const& neighbor_data
          ) {
          const unsigned num_neighbors = neighbor_data.size();
          std::vector<std::pair<T, T> > weights;
          std::vector<std::tuple<T, T, T> > data;
          for (unsigned i = 0; i < num_neighbors; ++i) {
            // distance between the point and the cell center.
            float dist = (std::get<1>(neighbor_data[i]) - interp_loc).length();
            // z values and prob values
            std::pair<T, T> dat = std::get<2>(neighbor_data[i]);

            // assemble weights and data to be weighted
            // inverse dist and probablity
            float dweight = 1.0 / dist;
            weights.emplace_back(dweight, dat.second);
            // data to be interpolated: z, probability and radial distance sq
            data.emplace_back(dat.first, dat.second, dist*dist);
          }
          return weighted_sum<T>(data, weights);
      }
  };



  // output structure to hold multiple types of output data
  // e.g. a single image or two images (e.g., elevation and probability)
  template<class T, class DATA_T, class IDATA_T>
    struct image_data {
      IDATA_T image_dat_;
      void set_size(size_t ni, size_t nj) {}
      void set_data(DATA_T const& dat) {}
    };

  // specialization to a single image
  template<class T>
    struct image_data<T, T, vil_image_view<T> > {
    vil_image_view<T> image_dat_;
    void set_size(size_t ni, size_t nj) {
      image_dat_.set_size(ni, nj);
    }
    void set_data(size_t i, size_t j, T const& dat) {
      image_dat_(i, j) = dat;
    }
  };

  // specialization to a pair of images
  template<class T>
    struct image_data<T, std::pair<T, T>, std::pair<vil_image_view<T>, vil_image_view<T> > > {
    std::pair<vil_image_view<T>, vil_image_view<T> > image_dat_;
    
    void set_size(size_t ni, size_t nj) {
      image_dat_.first.set_size(ni, nj);
      image_dat_.second.set_size(ni, nj);
    }
    void set_data(size_t i, size_t j, std::pair<T, T> const& dat) {
      image_dat_.first(i, j) = dat.first;
      image_dat_.second(i, j) = dat.second;
    }
  };

  // specialization to a triple of images
  template<class T>
    struct image_data<T, std::tuple<T, T, T>, std::tuple<vil_image_view<T>, vil_image_view<T> , vil_image_view<T> > > {
    std::tuple<vil_image_view<T>, vil_image_view<T>, vil_image_view<T> > image_dat_;
    
    void set_size(size_t ni, size_t nj) {
      std::get<0>(image_dat_).set_size(ni, nj);
      std::get<1>(image_dat_).set_size(ni, nj);
      std::get<2>(image_dat_).set_size(ni, nj);
    }
    void set_data(size_t i, size_t j, std::tuple<T, T, T> const& dat) {
      std::get<0>(image_dat_)(i, j) = std::get<0>(dat);
      std::get<1>(image_dat_)(i, j) = std::get<1>(dat);
      std::get<2>(image_dat_)(i, j) = std::get<2>(dat);
    }
  };

  // the main function to grid the point data
  // templated over scalar type (e.g. float)
  // a pair or single version of point data (z or z and prob)
  // a single, pair or triple output image (IDATA_T)
  template<class T, class DATA_T, class OUT_T, class IDATA_T>
    inline image_data<T,  OUT_T, IDATA_T>
    grid_data_2d_array(
                       std::vector<vgl_point_2d<T>> const& data_in_loc,
                       std::vector<DATA_T> const& data_in,
                       vgl_point_2d<T> out_upper_left,
                       size_t out_ni,
                       size_t out_nj,
                       T step_size,
                       unsigned min_neighbors = 3,
                       unsigned max_neighbors = 5,
                       int nbrhood_radius = 2)
    {
      // total number of points
      size_t npts = data_in_loc.size();
      T max_dist = std::numeric_limits<T>::max();
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
      vul_timer tt;
      
      // the point grid index slightly faster than knn. Simpler to further parallelize
      std::vector<std::vector<std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> > > > index;
      
      grid_neighbor_indices_2d<T, DATA_T>(data_in_loc, data_in, out_upper_left,
              out_ni, out_nj, max_neighbors, step_size, nbrhood_radius, index);
      
      std::cout << "total index time " << tt.real() << std::endl;
      tt.mark();
      
      // loop across all grid values and setup data for interpolation
      image_data<T, OUT_T, IDATA_T> gridded;
      gridded.set_size(out_ni, out_nj);
      tt.mark();
      for (unsigned j=0; j<out_nj; ++j) {
        for (unsigned i=0; i<out_ni; ++i) {

       // interpolation point
       vgl_point_2d<T> loc(out_upper_left.x() + i * step_size,
                           out_upper_left.y() - j * step_size);
          
          std::vector<vgl_point_2d<T> >neighbor_locs_ar;
          std::vector<std::tuple<uint32_t, vgl_point_2d<T>, DATA_T> >& index_data = index[j][i];
          
          // interpolate 
          interp<T, DATA_T, OUT_T> intp;
          OUT_T val = intp(loc, index_data);
          gridded.set_data(i, j, val);
          
        }//end i
      }// end j
      std::cout << "Interpolation time " << tt.real() << std::endl;
      return gridded;
    }
  //// END ARRAY VERSION
  
} // end namespace
#endif
