// This is brl/bbas/bsta/bsta_kent.h
#ifndef bsta_kent_h_
#define bsta_kent_h_
//:
// \file
// \brief  Kent distribution function implementation
// \author Gamze Tunali (gtunali@brown.edu)
// \date   March 5, 2010
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim

#include "bsta_distribution.h"
#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_plane_3d.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

template <class T>
class bsta_kent// : public bsta_distribution<T,1>
{
 public:
  //: default construtor
  bsta_kent(T kappa, T beta, vnl_vector_fixed<T,3> gamma1, 
    vnl_vector_fixed<T,3> gamma2, vnl_vector_fixed<T,3> gamma3) 
    : kappa_(kappa), beta_(beta), gamma1_(gamma1), gamma2_(gamma2), gamma3_(gamma3) {}

  //: constructs kent distr. from a set of planes
  bsta_kent(vcl_vector<vgl_plane_3d<T> > planes);

  ~bsta_kent() {}

  T kappa() { return kappa_; }
  T beta() { return beta_; }

  //: true if unimodal, false if bimodal
  bool unimodal() { if ((kappa_/beta_) >= T(2))
                      return true; 
                    else 
                      return false; }

  //: pre: 
  T prob_density(vnl_vector_fixed<T,3> x);

  vnl_vector_fixed<T,3> mean() const { return gamma1_;  }


private:
  T kappa_;  // concentration or spread of the distribution (SHOULD BE > 0)
  T beta_;   // ellipticity of the contours of equal probability (ovalness parameter)
  vnl_vector_fixed<T,3> gamma1_;
  vnl_vector_fixed<T,3> gamma2_;
  vnl_vector_fixed<T,3> gamma3_;

  //: normalizing constant method
  //vnl_vector_fixed<T,3>
  T  normalizing_const(T kappa, T beta);
};

template <class T>
inline vcl_ostream& operator<< (vcl_ostream& os,
                                bsta_kent<T> const& b)
{
  return os << "kent: (kappa,beta) = (" << b.kappa() << "  " << b.beta() << ")\n";
}

#endif
