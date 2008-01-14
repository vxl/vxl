// This is brcv/seg/bsta/bsta_gaussian.h
#ifndef bsta_gaussian_h_
#define bsta_gaussian_h_

//:
// \file
// \brief A Gaussian distribution for use in a mixture model 
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/25/06
//
// \verbatim
//  Modifications
// \endverbatim


#include "bsta_distribution.h"
#include <vnl/vnl_math.h>

//: forward declare vnl_vector_fixed
template<class T, unsigned n> class vnl_vector_fixed;


template<unsigned n>
struct two_pi_power
{
  static inline double value()
  { return 2.0*vnl_math::pi*two_pi_power<n-1>::value(); }
};

VCL_DEFINE_SPECIALIZATION
struct two_pi_power<0>
{
  static inline double value() { return 1.0; }
};



//: A Gaussian distribution 
// used as a component of the mixture
template <class T, unsigned n>
class bsta_gaussian : public bsta_distribution<T,n>
{
  public:

    //: The mean of the distribution
    const vnl_vector_fixed<T,n>& mean() const { return mean_; }

    //: Set the mean of the distribution
    void set_mean(const vnl_vector_fixed<T,n>& mean) { mean_ = mean; }

  protected:
    bsta_gaussian() : mean_(T(0)) {}
    bsta_gaussian(const vnl_vector_fixed<T,n>& mean) : mean_(mean) {}

    //: The mean
    vnl_vector_fixed<T,n> mean_;
};


//: A Gaussian distribution 1D
// warning: this is partial specialization, some compilers may complain
template <class T>
class bsta_gaussian<T,1> : public bsta_distribution<T,1>
{
  public:

    //: The mean of the distribution
    const T& mean() const { return mean_; }

    //: Set the mean of the distribution
    void set_mean(const T& mean) { mean_ = mean; }

  protected:
    bsta_gaussian() : mean_(T(0)) {}
    bsta_gaussian(const T& mean) : mean_(mean) {}

    //: The mean
    T mean_;
};




#endif // bsta_gaussian_h_
