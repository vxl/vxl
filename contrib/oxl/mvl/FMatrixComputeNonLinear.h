// This is oxl/mvl/FMatrixComputeNonLinear.h
#ifndef FMatrixComputeNonLinear_h_
#define FMatrixComputeNonLinear_h_
//:
// \file
//    FMatrixComputeNonLinear is a class that contains the functions required for
//  two differing Non-Linear minimisations of the F Matrix:
//  - Zhengyou Zhang's 36 different rank 2 parametrisations of the F Matrix
//  - Phil Torr's augmentation of a given 7 points basis
//
// \author
//     David McKinnon, U.Q. 2/1/01
//
//-----------------------------------------------------------------------------

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_least_squares_function.h>
#include <vgl/vgl_homg_point_2d.h>
#include <mvl/AffineMetric.h>
#include <mvl/HomgNorm2D.h>
#include <mvl/PairMatchSetCorner.h>
#include <mvl/FMatrix.h> // needed since there is an FMatrix data member

class FMatrixComputeNonLinear : public vnl_least_squares_function
{
 public:

  // Constructors/Destructors--------------------------------------------------
  FMatrixComputeNonLinear(PairMatchSetCorner* matches);

  // Computations--------------------------------------------------------------
  // Calling this function results in the 36 parametrisations
  bool compute(FMatrix* F);
  // Calling this function results in the augmentation of the basis
  bool compute_basis(FMatrix* F, std::vector<int> basis);

  // The virtual function from vnl_levenberg_marquardt
  void f(const vnl_vector<double>& x, vnl_vector<double>& fx) override;

 private:
  // Data Members--------------------------------------------------------------
  int data_size_;
  PairMatchSetCorner& matches_;
  int p_, q_, r_;
  FMatrix F_orig_;
  bool one_;
  std::vector<vgl_homg_point_2d<double> > basis1_;
  std::vector<vgl_homg_point_2d<double> > basis2_;
  std::vector<vgl_homg_point_2d<double> > points1_;
  std::vector<vgl_homg_point_2d<double> > points2_;

  // Helpers-------------------------------------------------------------------
  void fmatrix_to_params(const FMatrix& F, vnl_vector<double>& params);
  FMatrix params_to_fmatrix(const vnl_vector<double>& params);
  void get_plan(int &r1, int &c1, int &r2, int &c2);
  vnl_vector<double> calculate_residuals(FMatrix* F);
};

#endif // FMatrixComputeNonLinear_h_
