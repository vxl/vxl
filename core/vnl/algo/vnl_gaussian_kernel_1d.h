//-*- c++ -*-------------------------------------------------------------------
#ifndef vnl_gaussian_kernel_1d_h_
#define vnl_gaussian_kernel_1d_h_
#ifdef __GNUC__
#pragma interface
#endif
//
// Class : vnl_gaussian_kernel_1d
//
// .SECTION Description
//    vnl_gaussian_kernel_1d is a class that holds one half of a discretely
//    sampled 1D gaussian distribution.
//
// .NAME        vnl_gaussian_kernel_1d - 1D discrete Gaussian.
// .LIBRARY     vnl
// .HEADER	Numerics Package
// .INCLUDE     vnl/vnl_gaussian_kernel_1d.h
// .FILE        vnl/vnl_gaussian_kernel_1d.cxx
// .SECTION Author
//     Andrew W. Fitzgibbon, Oxford RRG, 07 Aug 97
//
//-----------------------------------------------------------------------------

#include <vnl/vnl_vector.h>

class vnl_gaussian_kernel_1d {
public:
  // Constructors/Destructors--------------------------------------------------
  
  vnl_gaussian_kernel_1d(double sigma, double cutoff = 0.5/256.0);

  double G(double x) const;
  
  int width() const { return vec_.size(); }
  double operator [] (int i) const { return vec_[i]; }
  
protected:
  // Data Members--------------------------------------------------------------
  vnl_vector<double> vec_;
  double inscale_;

  // Helpers-------------------------------------------------------------------
};

#endif   // DO NOT ADD CODE AFTER THIS LINE! END OF DEFINITION FOR CLASS vnl_gaussian_kernel_1d.

