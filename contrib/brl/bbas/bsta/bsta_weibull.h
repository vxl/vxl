// This is brl/bbas/bsta/bsta_weibull.h
#ifndef bsta_weibull_h_
#define bsta_weibull_h_
//:
// \file
// \brief A Weibull distribution
// \author Joseph L. Mundy
// \date November 8, 2008
//  The three-parameter, shifted,  Weibull distribution has the
// form
//                                           _    _ k
//                                          | x-mu |
//                             _    _ k-1  -|------|
//                       k    | x-mu |      |lambda|
//  p(x;lamba, k, mu)= ------ |------|    e  -     -
//                     lambda |lambda|
//                             -     -
// where lambda is called the scale parameter, k is the shape parameter and
// mu is the location parameter.
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include "bsta_distribution.h"
#include <vnl/vnl_gamma.h>
#include <vcl_cassert.h>
#include <vcl_iosfwd.h>

// A Weibull distribution does not have a natural, unique extension to
// multi-dimensional variables. However, various approaches do exist
// and could be implemented. Thus, it does make sense to still template over n.
// However, only the case for n = 1 will be implemented now.
template <class T>
class bsta_weibull : public bsta_distribution<T,1>
{
  typedef typename bsta_distribution<T,1>::vector_type vector_;
  typedef typename bsta_distribution<T,1>::vector_type covar_type_;

 public:
  bsta_weibull();
  //: two parameter form
  bsta_weibull(vector_ const& lambda, vector_ const& k);

  //: three parameter form (the "shifted" Weibull)
  bsta_weibull(vector_ const& lambda, vector_ const& k, vector_ const& mu);

  //: destructor
  ~bsta_weibull(){}

  //: the scale parameter
  vector_ lambda() const {return lambda_;}

  //: the shape parameter
  vector_ k() const {return k_;}

  //: the location parameter
  vector_ mu() const {return mu_;}

  //: The mean of the distribution, for 1-d the vector_ is typedefed to T
  vector_ mean() const
  {
    double dk = static_cast<double>(k_);
    assert(dk>0);
    double la = static_cast<double>(lambda_);
    assert(la>0);
    double m = static_cast<double>(mu_);
    return static_cast<vector_>(m+la*vnl_gamma(1.0+1/dk));
  }

  //: The variance of the distribution
  covar_type_ var() const
  {
    double dk = static_cast<double>(k_);
    assert(dk>0);
    double la = static_cast<double>(lambda_);
    assert(la>0);
    double m = vnl_gamma(1.0+1/dk);
    double v = vnl_gamma(1.0+2/dk);
    double ret = la*la*(v-m*m);
    return static_cast<vector_>(ret);
  }
  //: The co_variance of the distribution same as variance for 1-d case
  covar_type_ covar() const
    {return this->var();}

  //: The probability density at this sample
  T prob_density(const vector_& pt) const;

  //: The probability integrated over a box
  T probability(const vector_& min_pt,
                const vector_& max_pt) const;
 protected:
  vector_ lambda_;
  vector_ mu_;
  vector_ k_;
};

template <class T >
inline vcl_ostream& operator<< (vcl_ostream& os,
                                bsta_weibull<T> const& w)
{
  os << "weibull:lambda(" << w.lambda() << ")\n"
     << "weibull:k(" << w.k() << ")\n"
     << "weibull:mu(" << w.mu() << ")\n";
  return os;
}

#endif // bsta_weibull_h_
