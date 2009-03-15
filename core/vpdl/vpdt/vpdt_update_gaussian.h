// This is core/vpdl/vpdt/vpdt_update_gaussian.h
#ifndef vpdt_update_gaussian_h_
#define vpdt_update_gaussian_h_
//:
// \file
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date March 7, 2009
// \brief Iterative updating of Gaussians


#include <vpdl/vpdt/vpdt_field_traits.h>
#include <vpdl/vpdt/vpdt_gaussian.h>


//==============================================================================
// A helper class used to update the various covariance types

//: Specialized classes to update covariance of different types
// \note the \c Disambiguate template parameter is a dummy used to
//  prevent ambiguous instantiations
template<class F, class Covar, class Disambiguate= void>
struct vpdt_update_covariance {};

//: Specialized classes to update covariance of different types
template<class F>
struct vpdt_update_covariance<F, typename vpdt_eigen_sym_matrix_gen<F>::type,
                              typename vpdt_field_traits<F>::type_is_vector>
{
  //: the data type used for covariance
  typedef typename vpdt_eigen_sym_matrix_gen<F>::type Covar;
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;

  //: update the covariance matrix with a weighted vector difference
  static inline void increment(Covar& c, const T& s, const vector& d)
  {
    // FIXME this could be more efficient if we could iteratively update the eigen decomposition
    typename vpdt_field_traits<F>::matrix_type m;
    c.form_matrix(m);
    m += s * outer_product(d,d);
    c.set_matrix(m);
  }

  //: enforce a minimum covariance value in all dimensions
  static inline void enforce_min(Covar& c, const T& min_var)
  {
    vector ev = c.eigenvalues();
    const unsigned int dim = vpdt_size(ev);
    for (unsigned int i=0; i<dim; ++i){
      T& evi = vpdt_index(ev,i);
      if (evi < min_var)
        evi = min_var;
    }
    c.set_eigenvalues(ev);
  }
};


//: Specialized classes to update covariance of different types
template<class F>
struct vpdt_update_covariance<F, typename vpdt_field_traits<F>::vector_type,
                              typename vpdt_field_traits<F>::type_is_vector>
{
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;

  //: update the covariance matrix with a weighted vector difference
  static inline void increment(vector& c, const T& s, const vector& d)
  {
    c += s * element_product(d,d);
  }

  //: enforce a minimum covariance value in all dimensions
  static inline void enforce_min(vector& c, const T& min_var)
  {
    const unsigned int dim = vpdt_size(c);
    for (unsigned int i=0; i<dim; ++i){
      T& ci = vpdt_index(c,i);
      if (ci < min_var)
        ci = min_var;
    }
  }
};


//: Specialized classes to update covariance of different types
template<class F>
struct vpdt_update_covariance<F, typename vpdt_field_traits<F>::scalar_type,
                              typename vpdt_field_traits<F>::type_is_vector>
{
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;
  //: the data type used for vectors
  typedef typename vpdt_field_traits<F>::vector_type vector;

  //: update the covariance matrix with a weighted vector difference
  static inline void increment(T& c, const T& s, const vector& d)
  {
    c += s * dot_product(d,d);
  }

  //: enforce a minimum covariance value in all dimensions
  static inline void enforce_min(T& c, const T& min_var)
  {
    if (c<min_var)
      c = min_var;
  }
};


//: Specialized classes to update covariance of different types
template<class F>
struct vpdt_update_covariance<F, typename vpdt_field_traits<F>::scalar_type,
                              typename vpdt_field_traits<F>::type_is_scalar>
{
  //: the data type used for scalars
  typedef typename vpdt_field_traits<F>::scalar_type T;

  //: update the covariance matrix with a weighted vector difference
  static inline void increment(T& c, const T& s, const T& d)
  {
    c += s*d*d;
  }

  //: enforce a minimum covariance value in all dimensions
  static inline void enforce_min(T& c, const T& min_var)
  {
    if (c<min_var)
      c = min_var;
  }
};


//==============================================================================
// The Gaussian update functions


//: Update the statistics given a 1D Gaussian distribution and a learning rate
// \note if rho = 1/(num observations) then this just an online cumulative average
template <class F, class Covar>
void vpdt_update_gaussian(vpdt_gaussian<F,Covar>& gaussian,
                          typename vpdt_field_traits<F>::scalar_type rho,
                          const F& sample )
{
  typedef typename vpdt_field_traits<F>::scalar_type T;
  typedef typename vpdt_field_traits<F>::vector_type vector;

  // the complement of rho (i.e. rho+rho_comp=1.0)
  T rho_comp = T(1) - rho;
  // the difference vector between the sample and the mean
  vector diff = sample - gaussian.mean;

  // update the covariance
  vpdt_update_covariance<F,Covar>::increment(gaussian.covar, rho, diff);
  gaussian.covar *= rho_comp;

  // update the mean
  gaussian.mean += (rho * diff);
}


//: Update the statistics given a Gaussian distribution and a learning rate
// \param min_var forces the variance to stay above this limit
// \note If the same sample is observed repeatedly, the variances will
// converge to the minimum value parameter rather than zero.
template <class F, class Covar>
void vpdt_update_gaussian(vpdt_gaussian<F,Covar>& gaussian,
                          typename vpdt_field_traits<F>::scalar_type rho,
                          const F& sample,
                          typename vpdt_field_traits<F>::scalar_type min_var)
{
  vpdt_update_gaussian(gaussian, rho, sample);
  vpdt_update_covariance<F,Covar>::enforce_min(gaussian.covar, min_var);
}


#endif // vpdt_update_gaussian_h_
