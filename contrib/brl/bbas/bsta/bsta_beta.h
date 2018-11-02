// This is brl/bbas/bsta/bsta_beta.h
#ifndef bsta_beta_h_
#define bsta_beta_h_
//:
// \file
// \brief  Beta distribution function implementation
// \author Gamze Tunali (gtunali@brown.edu)
// \date   November 13, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include <vector>
#include <iostream>
#include "bsta_distribution.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class bsta_beta : public bsta_distribution<T,1>
{
 public:
  // default constructor
  bsta_beta() : alpha_(T(0.00001)), beta_(T(0.00001)) {}

  //: pre: alpha>0 and beta>0
  bsta_beta(T alpha, T beta) : alpha_(alpha), beta_(beta)
  { if (alpha_< 0) alpha_=T(0.00001); if (beta_<0) beta_=T(0.00001); }

  //: constructs from a set of sample values
  bsta_beta(std::vector<T> x);

  static bool bsta_beta_from_moments(T mean, T var, T& alpha, T& beta);

  T alpha() const { return alpha_; }

  T beta() const { return beta_; }

  void set_alpha_beta(T alpha, T beta)
  {
    alpha_=alpha; beta_=beta;
    if (alpha_ < 0)
      alpha_=T(0.00001);
    if (beta_ <0)
      beta_=T(0.00001);
  }

  //: pre: x should be in [0,1]
  T prob_density(T x) const;

  T cum_dist_funct(T x) const;

  T distance(T x) const;
  T mean() const { return alpha_/(alpha_+beta_); }

  T var() const { T t=alpha_+beta_; return (alpha_*beta_)/(t*t*(t+1)); }

 private:
  T alpha_;
  T beta_;
};

template <class T>
inline std::ostream& operator<< (std::ostream& os,
                                bsta_beta<T> const& b)
{
  return
  os << "beta: (alpha,beta) = (" << b.alpha() << "  " << b.beta() <<' '<<b.mean()<<' '<<b.var()<< ")\n";
}

#endif
