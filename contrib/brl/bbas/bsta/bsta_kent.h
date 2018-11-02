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

#include <vector>
#include <iostream>
#include "bsta_distribution.h"
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#if 0
#include <vgl/vgl_plane_3d.h>
#endif
#include <vsl/vsl_binary_io.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class T>
class bsta_kent // : public bsta_distribution<T,1>
{
 public:
  //: default constructor
  bsta_kent()
   : kappa_(0), beta_(0), gamma1_(vnl_vector_fixed<T,3>(0,0,0)),
     gamma2_(vnl_vector_fixed<T,3>(0,0,0)), gamma3_(vnl_vector_fixed<T,3>(0,0,0)) {}

  bsta_kent(T kappa, T beta, vnl_vector_fixed<T,3> const& gamma1,
            vnl_vector_fixed<T,3> const& gamma2, vnl_vector_fixed<T,3> const& gamma3)
   : kappa_(kappa), beta_(beta), gamma1_(gamma1), gamma2_(gamma2), gamma3_(gamma3) {}

  int version() const { return 1; }

#if 0
  //: constructs kent distr. from a set of planes
  bsta_kent(std::vector<vgl_plane_3d<T> > const& planes);
#endif

  //: construct from a 3x3 matrix
  bsta_kent(vnl_matrix_fixed<T,3,3> const& m);

  ~bsta_kent() = default;

  T kappa() const { return kappa_; }
  T beta() const { return beta_; }
  vnl_vector_fixed<T,3> minor_axis() const { return gamma1_; }
  vnl_vector_fixed<T,3> major_axis() const { return gamma2_; }
  vnl_vector_fixed<T,3> mean_direction() const { return gamma3_; }

  //: true if unimodal, false if bimodal
  bool unimodal() const { return kappa_/beta_ >= T(2); }

  //: pre:
  T prob_density(vnl_vector_fixed<T,3> const& x);

  vnl_vector_fixed<T,3> mean() const { return gamma1_; /* std::pow(kappa_,beta_); ??? */ }

 private:
  T kappa_;  // concentration or spread of the distribution (SHOULD BE > 0)
  T beta_;   // ellipticity of the contours of equal probability (ovalness parameter)
  vnl_vector_fixed<T,3> gamma1_;
  vnl_vector_fixed<T,3> gamma2_;
  vnl_vector_fixed<T,3> gamma3_;

  //: normalizing constant method
  T normalizing_const(T kappa=0, T beta=0);
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, bsta_kent<T> const& b);

template <class T>
void vsl_b_write(vsl_b_ostream & os, bsta_kent<T> const* &b);

template <class T>
void vsl_b_read(vsl_b_istream & is, bsta_kent<T> &b);

template <class T>
void vsl_b_read(vsl_b_istream & is, bsta_kent<T> *&b);

template <class T>
std::ostream& operator<< (std::ostream& os, bsta_kent<T> & b);

#endif
