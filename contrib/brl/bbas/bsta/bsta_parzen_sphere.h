// This is brl/bbas/bsta/bsta_parzen_sphere.h
#ifndef bsta_parzen_sphere_h_
#define bsta_parzen_sphere_h_
//:
// \file
// \brief A distribution based on a number of samples, independent vector elements with equal variance
// \author Joseph L. Mundy
// \date October 12, 2008
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//
// Do not remove the following statement
// Approved for Public Release, Distribution Unlimited (DISTAR Case 12529)
//

#include "bsta_parzen.h"
#include <vcl_vector.h>
#include <vcl_algorithm.h>

//: forward declare vnl_matrix_fixed and vnl_vector_fixed
template<class T, unsigned n, unsigned m> class vnl_matrix_fixed;
template<class T, unsigned n>             class vnl_vector_fixed;

//: A parzen distribution
// the Gaussian sphere is used as a component of the mixture
template <class T, unsigned n>
class bsta_parzen_sphere : public bsta_parzen<T,n>
{
 public:

  //: the covariance type
  typedef vnl_matrix_fixed<T,n,n> covar_type;
  typedef typename bsta_distribution<T,n>::vector_type vect_t;
  typedef typename bsta_distribution<T,n>::math_type math_t;
  bsta_parzen_sphere(): bandwidth_(T(1)),
    bandwidth_adapted_(false) {}// no samples

  bsta_parzen_sphere(typename bsta_parzen<T,n>::sample_vector const& samples, T bandwidth = T(1)) :
    bsta_parzen<T,n>(samples), bandwidth_(bandwidth),
    bandwidth_adapted_(false){}

  ~bsta_parzen_sphere() {}

  //: kernel bandwidth
  T bandwidth() const {return bandwidth_;}

  void set_bandwidth(T bandwidth) { bandwidth_ = bandwidth; }

  bool bandwidth_adapted() const { return bandwidth_adapted_; }

  void set_bandwidth_adapted(bool bandwidth_adapted)
    {bandwidth_adapted_=bandwidth_adapted;}

  //: The mean of the distribution (just the sample mean)
  vnl_vector_fixed<T,n> mean() const;

  //: The covariance of the distribution (the sample covariance + bandwidth*I)
  covar_type covar() const;

  //: The probability density at sample pt
  T prob_density(vect_t const& pt) const;

  //: The probability density integrated over a box (returns a probability)
  T probability(vect_t const& min_pt,
                vect_t const& max_pt) const;

  //: The distance and index of the nearest sample
  T nearest_sample(const vect_t& pt, unsigned & index) const;

 protected:
  T bandwidth_;
  bool bandwidth_adapted_;
};

//: specialize to the scalar case, needed due to differences in computing the covariance matrix
template <class T >
class bsta_parzen_sphere<T,1> : public bsta_parzen<T,1>
{
 public:
  //actually a scalar
  typedef typename bsta_distribution<T,1>::vector_type vect_t;

  bsta_parzen_sphere(): bandwidth_(T(1)),
    bandwidth_adapted_(false) {}// no samples

  bsta_parzen_sphere(typename bsta_parzen<T,1>::sample_vector const& samples,
                     T bandwidth = T(1)): bsta_parzen<T,1>(samples),
    bandwidth_(bandwidth), bandwidth_adapted_(false){}

  ~bsta_parzen_sphere() {}

  //: kernel bandwidth
  T bandwidth() const {return bandwidth_;}

  void set_bandwidth(T bandwidth) { bandwidth_ = bandwidth; }

  bool bandwidth_adapted() const { return bandwidth_adapted_; }

  void set_bandwidth_adapted(bool bandwidth_adapted)
    {bandwidth_adapted_=bandwidth_adapted;}

  //: The mean of the distribution (just the sample mean)
  T mean() const;

  //: The covariance of the distribution (the sample covariance + bandwidth*I)
  T covar() const;

  //: The probability density at sample pt
  T prob_density(vect_t const& pt) const;

  //: The probability density integrated over a box (returns a probability)
  T probability(vect_t const& min_pt,
                vect_t const& max_pt) const;

  //: the distance and index of the nearest sample
  T nearest_sample(const vect_t& pt, unsigned & index) const;
 protected:
  T bandwidth_;
  bool bandwidth_adapted_;
};

#endif // bsta_parzen_sphere_h_
