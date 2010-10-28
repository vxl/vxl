// This is algo/bapl/bapl_affine_transform.cxx
//:
// \file

#include "bapl_affine_transform.h"
#include <vnl/vnl_identity.h> 
#include <vnl/algo/vnl_svd.h>

//: Constructor
bapl_affine_transform::bapl_affine_transform()
  :  t_(0.0, 0.0) 
{
  A_.set_identity();
}

//: Constructor
bapl_affine_transform::bapl_affine_transform(const vnl_double_2x2& A, const vnl_double_2& t)
  :  A_(A), t_(t) 
{}

//: Constructor
bapl_affine_transform::bapl_affine_transform(double a11, double a12,
                                             double a21, double a22,
                                             double tx,  double ty )
  : t_(vnl_double_2(tx,ty)) 
{
  A_(0,0) = a11;  A_(0,1) = a12;
  A_(1,0) = a21;  A_(1,1) = a22;
}

//: Compute and return the inverse matrix
bapl_affine_transform 
bapl_affine_transform::inverse() const
{
  vnl_svd<double> svd_A( A_ );
  vnl_matrix< double > A_inv( svd_A.inverse() );
  vnl_double_2 t_inv = (A_inv*t_)*-1;
  return bapl_affine_transform(A_inv,t_inv);
}
