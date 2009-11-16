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

template <class T>
class bsta_beta : public bsta_distribution<T,1>
{
public:
  //: default construtor
  bsta_beta() : alpha_(T(0)), beta_(T(0)) {}

  //: pre: alpha>0 and beta>0
  bsta_beta(T alpha, T beta) : alpha_(alpha), beta_(beta) 
     { if (alpha_< 0) alpha_=T(0); if (beta_<0) beta_=T(0); }
  
  //: constructs from a set of sample values
  bsta_beta(vcl_vector<T> x);

  void set_alpha_beta(T alpha, T beta)
     { if (alpha_ = alpha < 0) alpha_=T(0); if (beta_ = beta <0) beta_=T(0); }

  //: pre: x should be in [0,1]
  T prob_density(T x) const 
     { if (x >=0 && x<=1) return (vcl_pow(x, alpha_-1)*vcl_pow(1-x,beta_-1))/vnl_beta(alpha_,beta_);
       else return T(0); }

  T mean() {return alpha_/(alpha_+beta_); }

  T var() { T t=alpha_+beta_; return (alpha_*beta_)/(t*t*(t+1)); }

private:
  T alpha_;
  T beta_;

};

#endif