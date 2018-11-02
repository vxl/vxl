// This is bvpl_taylor_basis_sample.h
#ifndef bvpl_taylor_basis_sample_h
#define bvpl_taylor_basis_sample_h

//:
// \file
// \brief A data structure that contains vectors and matrices needed for taylor approximation
// \author Isabel Restrepo mir@lems.brown.edu
// \date  10-Feb-2011.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_io.h>


//: Data structure needed for 2-degree taylor approximation of 3-dimesional volumes
class bvpl_taylor_basis2_sample
{
public:
  bvpl_taylor_basis2_sample()
  {
    I0 = 0.0f;
    //empty - to save space
    G = vnl_double_3(0.0,0.0,0.0);
    H = vnl_double_3x3(0.0);
  }

  bvpl_taylor_basis2_sample(double zeroth_dev, vnl_double_3 grad, vnl_double_3x3 hess): I0(zeroth_dev), G(grad), H(hess){}

  short version_no() const{ return 1; }
  //: Approximation to zeroth derivative
  double I0;
  //: Approximation to the Gradient Vector
  vnl_double_3 G;
  //: Approximation to the Hessian
  vnl_double_3x3 H;
};

std::ostream& operator << (std::ostream& os, const bvpl_taylor_basis2_sample& sample);

void vsl_b_write(vsl_b_ostream & os, bvpl_taylor_basis2_sample const &sample);

void vsl_b_write(vsl_b_ostream & os, bvpl_taylor_basis2_sample const * &sample);

void vsl_b_read(vsl_b_istream & is, bvpl_taylor_basis2_sample &sample);

void vsl_b_read(vsl_b_istream & is, bvpl_taylor_basis2_sample *&sample);

#endif
