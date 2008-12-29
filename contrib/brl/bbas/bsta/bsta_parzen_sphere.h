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
#include <vnl/vnl_math.h>
#include <vcl_vector.h>
#include <vcl_algorithm.h>


//: A Parzen_Sphere distribution
// used as a component of the mixture
template <class T, unsigned n>
class bsta_parzen_sphere : public bsta_parzen<T,n>
{
  typedef typename bsta_distribution<T,n>::vector_type vector_;
  typedef vcl_vector<vector_> sample_vector;
 public:
  bsta_parzen_sphere(): bandwidth_(T(1)),
    bandwidth_adapted_(false) {}// no samples

  bsta_parzen_sphere(sample_vector const& samples, T bandwidth = T(1)) :
    bsta_parzen<T,n>(samples), bandwidth_(bandwidth),
    bandwidth_adapted_(false){}

  //: kernel bandwidth
  T bandwidth() const {return bandwidth_;}

  void set_bandwidth(T bandwidth) {bandwidth_ = bandwidth ;}

  bool bandwidth_adapted() {return bandwidth_adapted_;}

  void set_bandwidth_adapted(bool bandwidth_adapted)
    {bandwidth_adapted_=bandwidth_adapted;}

  //: The mean of the distribution
  virtual vector_ mean() const{
    vector_ sum; sum*=T(0);
    sample_vector::const_iterator sit = samples_.begin();
    for(;sit != samples_.end(); ++sit)
      sum += (*sit);
    sum /= static_cast<T>(samples_.size());
    return sum;
  }
  //: The probability density at sample pt
  virtual T prob_density(const vnl_vector_fixed<T,n>& pt) const;

  //: The probability density integrated over a box (returns a probability)
  virtual T probability(const vnl_vector_fixed<T,n>& min_pt,
                        const vnl_vector_fixed<T,n>& max_pt) const;

 protected:
  T bandwidth_;
  bool bandwidth_adapted_;
};

#endif // bsta_parzen_sphere_h_
