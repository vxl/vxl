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

#include "bsta_distribution.h"
#include <vnl/vnl_math.h>
#include <vnl/vnl_beta.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

template <class T>
class bsta_beta : public bsta_distribution<T,1>
{
 public:
  //: default construtor
  bsta_beta() : alpha_(T(0.00001)), beta_(T(0.00001)) {}

  //: pre: alpha>0 and beta>0
  bsta_beta(T alpha, T beta) : alpha_(alpha), beta_(beta)
     { if (alpha_< 0) alpha_=T(0.00001); if (beta_<0) beta_=T(0.00001); }

  //: constructs from a set of sample values
  bsta_beta(vcl_vector<T> x);

  static bool bsta_beta_from_moments(T mean, T var, T& alpha, T& beta);

  T alpha() const { return alpha_; }

  T beta() const { return beta_; }

  void set_alpha_beta(T alpha, T beta)
  { alpha_=alpha; beta_=beta;
    if (alpha_ < 0)
      alpha_=T(0.00001);
    if (beta_ <0)
      beta_=T(0.00001);
  }

  //: pre: x should be in [0,1]
  T prob_density(T x) const;

  T cum_dist_funct(T x) const;

  T mean() const { return alpha_/(alpha_+beta_); }

  T var() const { T t=alpha_+beta_; return (alpha_*beta_)/(t*t*(t+1)); }

 private:
  T alpha_;
  T beta_;
};

template <class T>
inline vcl_ostream& operator<< (vcl_ostream& os,
                                bsta_beta<T> const& b)
{
  return
  os << "beta: (alpha,beta) = (" << b.alpha() << "  " << b.beta() << ")\n";
}

#endif
