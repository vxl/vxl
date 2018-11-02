// This is core/vpdl/vpdt/vpdt_norm_metric.h
#ifndef vpdt_norm_metric_h_
#define vpdt_norm_metric_h_
//:
// \file
// \author Matthew Leotta
// \date March 5, 2009
// \brief Metrics derived from norms
//
// \verbatim
//  Modifications
//   <None yet>
// \endverbatim


#include <limits>
#include <cmath>
#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_access.h>
#include <vpdl/vpdt/vpdt_eigen_sym_matrix.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//: A metric in field \c F with metric tensor \c Tensor
// \note the \c Disambiguate template parameter is a dummy used to
//  prevent ambiguous instantiations
template<class F, class Tensor, class Disambiguate= void>
struct vpdt_norm_metric;


//: A metric in field \c F with vpdt_eigen_sym_matrix covariance
template<class F>
struct vpdt_norm_metric<F, typename vpdt_eigen_sym_matrix_gen<F>::type,
                        typename vpdt_field_traits<F>::type_is_vector >
{
  //: the data type used for the metric tensor
  typedef typename vpdt_eigen_sym_matrix_gen<F>::type covar_type;
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<F>::matrix_type matrix;

  //: Compute the Mahalanobis distance between two points
  static inline T distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    return std::sqrt(sqr_distance(pt1,pt2,c));
  }

  //: Compute the square Mahalanobis distance between two points
  static inline T sqr_distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    // F must provide operator-(F,F) that returns (or can cast to) the vector type
    vector d(pt1-pt2);
    return c.inverse_quad_form(d);
  }

  //: Compute the square Mahalanobis distance and also the derivative \a g wrt \a pt1
  static inline T sqr_distance_deriv(const F& pt1, const F& pt2,
                                     const covar_type& c, vector& g)
  {
    // F must provide operator-(F,F) that returns (or can cast to) the vector type
    vector d(pt1-pt2);
    c.inverse_product(d,g);
    T sqr_dist = dot_product(d,g);
    g *= 2;
    return sqr_dist;
  }

  //: Compute the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline void compute_covar(matrix& covar, const F& /*pt*/, const covar_type& c)
  {
    c.form_matrix(covar);
  }

  //: Compute the determinant of the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline T covar_det(const F& /*pt*/, const covar_type& c)
  {
    return c.determinant();
  }
};


//: A metric in field \c F with vector (diagonal matrix) covariance.
//  In this case each dimension is independent
template<class F>
struct vpdt_norm_metric<F, typename vpdt_field_traits<F>::vector_type,
                           typename vpdt_field_traits<F>::type_is_vector >
{
  //: the data type used for the metric tensor
  typedef typename vpdt_field_traits<F>::vector_type covar_type;
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<F>::matrix_type matrix;

  //: Compute the Mahalanobis distance between two points
  static inline T distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    return std::sqrt(sqr_distance(pt1,pt2,c));
  }

  //: Compute the square Mahalanobis distance between two points
  static inline T sqr_distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    const unsigned int d = vpdt_size(c);
    assert(vpdt_size(pt1) == d);
    assert(vpdt_size(pt2) == d);
    T val = T(0);
    for (unsigned int i=0; i<d; ++i)
    {
      T tmp = (vpdt_index(pt1,i)-vpdt_index(pt2,i));
      val += tmp*tmp/vpdt_index(c,i);
    }
    return val;
  }

  //: Compute the square Mahalanobis distance and also the derivative \a g wrt \a pt1
  static inline T sqr_distance_deriv(const F& pt1, const F& pt2,
                                     const covar_type& c, vector& g)
  {
    const unsigned int d = vpdt_size(c);
    vpdt_set_size(g,d);
    assert(vpdt_size(pt1) == d);
    assert(vpdt_size(pt2) == d);
    T val = T(0);
    for (unsigned int i=0; i<d; ++i)
    {
      T tmp = (vpdt_index(pt1,i)-vpdt_index(pt2,i));
      T& g_i = vpdt_index(g,i) = tmp/vpdt_index(c,i);
      val += tmp*g_i;
      g_i *= 2;
    }
    return val;
  }

  //: Compute the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline void compute_covar(matrix& covar, const F& /*pt*/, const covar_type& c)
  {
    const unsigned int d = c.size();
    vpdt_set_size(covar,d);
    assert(vpdt_size(covar) == d);
    for (unsigned int i=0; i<d; ++i)
    {
      vpdt_index(covar,i,i) = vpdt_index(c,i);
      for (unsigned int j=i+1; j<d; ++j)
        vpdt_index(covar,i,j) = vpdt_index(covar,j,i) = T(0);
    }
  }

  //: Compute the determinant of the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline T covar_det(const F& pt, const covar_type& c)
  {
    const unsigned int d = c.size();
    double det = 1.0;
    for (unsigned int i=0; i<d; ++i)
      det *= vpdt_index(c,i);
    return static_cast<T>(det);
  }
};


//: A metric in field \c F with scalar (scaled identity matrix) covariance.
//  In this case the metric is a scaled L2 norm
template<class F>
struct vpdt_norm_metric<F, typename vpdt_field_traits<F>::scalar_type,
                           typename vpdt_field_traits<F>::type_is_vector >
{
  //: the data type used for the metric tensor
  typedef typename vpdt_field_traits<F>::scalar_type covar_type;
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<F>::matrix_type matrix;

  //: Compute the Mahalanobis distance between two points
  static inline T distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    return std::sqrt(sqr_distance(pt1,pt2,c));
  }

  //: Compute the square Mahalanobis distance between two points
  static inline T sqr_distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    const unsigned int d = vpdt_size(pt1);
    assert(vpdt_size(pt2) == d);
    T val = T(0);
    for (unsigned int i=0; i<d; ++i)
    {
      T tmp = (vpdt_index(pt1,i)-vpdt_index(pt2,i));
      val += tmp*tmp/c;
    }
    return val;
  }

  //: Compute the square Mahalanobis distance and also the derivative \a g wrt \a pt1
  static inline T sqr_distance_deriv(const F& pt1, const F& pt2,
                                     const covar_type& c, vector& g)
  {
    const unsigned int d = vpdt_size(pt1);
    vpdt_set_size(g,d);
    assert(vpdt_size(pt2) == d);
    T val = T(0);
    for (unsigned int i=0; i<d; ++i)
    {
      T tmp = (vpdt_index(pt1,i)-vpdt_index(pt2,i));
      T& g_i = vpdt_index(g,i) = tmp/c;
      val += tmp*g_i;
      g_i *= 2;
    }
    return val;
  }

  //: Compute the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline void compute_covar(matrix& covar, const F& pt, const covar_type& c)
  {
    const unsigned int d = vpdt_size(pt);
    vpdt_set_size(covar,d);
    assert(vpdt_size(covar) == d);
    for (unsigned int i=0; i<d; ++i)
    {
      vpdt_index(covar,i,i) = c;
      for (unsigned int j=i+1; j<d; ++j)
        vpdt_index(covar,i,j) = vpdt_index(covar,j,i) = T(0);
    }
  }

  //: Compute the determinant of the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline T covar_det(const F& pt, const covar_type& c)
  {
    const unsigned int d = vpdt_size(pt);
    double det = 1.0;
    for (unsigned int i=0; i<d; ++i)
      det *= c;
    return static_cast<T>(det);
  }
};


//: A metric in field \c F with scalar variance.
//  In this case the metric is a scaled L2 norm
template<class F>
struct vpdt_norm_metric<F, typename vpdt_field_traits<F>::scalar_type,
                           typename vpdt_field_traits<F>::type_is_scalar >
{
  //: the data type used for the metric tensor
  typedef typename vpdt_field_traits<F>::scalar_type covar_type;
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;
  //: the data type used for matrices
  typedef typename vpdt_field_traits<F>::matrix_type matrix;

  //: Compute the Mahalanobis distance between two points
  static inline T distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    return std::sqrt(sqr_distance(pt1,pt2,c));
  }

  //: Compute the square Mahalanobis distance between two points
  static inline T sqr_distance(const F& pt1, const F& pt2, const covar_type& c)
  {
    T tmp = pt1-pt2;
    return tmp*tmp/c;
  }

  //: Compute the square Mahalanobis distance and also the derivative \a g wrt \a pt1
  static inline T sqr_distance_deriv(const F& pt1, const F& pt2,
                                     const covar_type& c, vector& g)
  {
    T tmp = pt1-pt2;
    g = 2*tmp/c;
    return tmp*tmp/c;
  }

  //: Compute the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline void compute_covar(matrix& covar, const F& /*pt*/, const covar_type& c)
  {
    covar = c;
  }

  //: Compute the determinant of the covariance matrix (metric tensor) at a point
  // \note this metric is independent of the point
  static inline T covar_det(const F& /*pt*/, const covar_type& c)
  {
    return c;
  }
};


#endif // vpdt_norm_metric_h_
