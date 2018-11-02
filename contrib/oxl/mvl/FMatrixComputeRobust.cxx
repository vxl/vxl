#include <iostream>
#include <cmath>
#include "FMatrixComputeRobust.h"
//:
// \file
#include <mvl/Probability.h>
#include <mvl/HomgNorm2D.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/HomgInterestPointSet.h>
#include <mvl/FMatrixCompute7Point.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3x3.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

FMatrixComputeRobust::FMatrixComputeRobust() = default;

FMatrixComputeRobust::~FMatrixComputeRobust() = default;
//-----------------------------------------------------------------------------
//
//: Compute a robust fundamental matrix.
//
// \returns false if the calculation fails.
//
//-----------------------------------------------------------------------------
bool FMatrixComputeRobust::compute(PairMatchSetCorner& matches, FMatrix *F)
{
  inliers_.resize(0);
  residuals_.resize(0);

  // Copy matching points from matchset.
  // Set up some initial variables
  HomgInterestPointSet const* points1 = matches.get_corners1();
  HomgInterestPointSet const* points2 = matches.get_corners2();
  std::vector<HomgPoint2D> point1_store, point2_store;
  std::vector<int> point1_int, point2_int;
  matches.extract_matches(point1_store, point1_int, point2_store, point2_int);
  data_size_ = matches.count();
  std::vector<HomgPoint2D> point1_image(data_size_), point2_image(data_size_);

  // Store the image points
  for (int a = 0; a < data_size_; a++) {
    vnl_double_2 temp1;
    temp1 = points1->get_2d(point1_int[a]);
    point1_image[a] = HomgPoint2D(temp1[0], temp1[1], 1.0);
  }

  for (int a = 0; a < data_size_; a++) {
    vnl_double_2 temp2;
    temp2 = points2->get_2d(point2_int[a]);
    point2_image[a] = HomgPoint2D(temp2[0], temp2[1], 1.0);
  }

  FMatrix Fs;
  double Ds = 1e+10;
  int count = 0;
  std::vector<bool> inlier_list(data_size_, false);
  std::vector<double> residualsF(data_size_, 100.0);

  // 150 random samples from the points set
  for (int i = 0; i < 100; i++) {
    std::vector<int> index(7);

    // Take the minimum sample of seven points for the F Matrix calculation
    index = Monte_Carlo(point1_store, point1_int,  8, 7);
    std::vector<HomgPoint2D> seven1(7);
    std::vector<HomgPoint2D> seven2(7);
    for (int j = 0; j < 7; j++) {
      vnl_double_2 t1 = points1->get_2d(index[j]);
      seven1[j] = HomgPoint2D(t1[0], t1[1], 1.0);
      int other = matches.get_match_12(index[j]);
      vnl_double_2 t2 = points2->get_2d(other);
      seven2[j] = HomgPoint2D(t2[0], t2[1], 1.0);
    }

    // Set up a new FMatrix 7 point Computor
    // Note the conditioning and de-conditioning is done internally
    FMatrixCompute7Point Computor(true, rank2_truncate_);

    // Compute F
    std::vector<FMatrix*> F_temp;
    if (!Computor.compute(seven1, seven2, F_temp))
      std::cerr << "Seven point failure\n";

    for (auto & k : F_temp) {
      int temp_count = 0;
      std::vector<bool> list(data_size_);
      std::vector<double> residuals = calculate_residuals(point1_image, point2_image, k);
      double term_error = calculate_term(residuals, list, temp_count);
      if (term_error < Ds) {
        Fs = *k;
        Ds = term_error;
        basis_ = index;
        inlier_list = list;
        residualsF = residuals;
        count = temp_count;
      }
    }

    for (auto & k : F_temp)
      delete k;
  }
  std::cerr << "Final Figures...\n"
           << "Ds : " << Ds << '\n';
  vnl_double_3x3 sample = Fs.get_matrix();
  HomgPoint2D one, two;
  Fs.get_epipoles(&one, &two);
  vnl_double_2 o = one.get_double2();
  vnl_double_2 t = two.get_double2();
  HomgPoint2D c1(o[0], o[1], 1.0);
  HomgPoint2D c2(t[0], t[1], 1.0);
  std::cerr << "Epipole 1 : " << c1 << " Epipole 2 : " << c2 << "\n\n";
  epipole1_ = c1;
  epipole2_ = c2;
  sample /= sample.get(2, 2);
  std::cerr << "FMatrix : " << sample << '\n';
  F->set(Fs.get_matrix());

  int inlier_count = count;
  double std_in = 0.0;

  for (int k = 0; k < data_size_; k++) {
    if (inlier_list[k]) {
      std_in += residualsF[k];
    }
  }
  std_in /= inlier_count;
  std_in = std::sqrt(std_in);

  // Update the inliers in the PairMatchSet object
  matches.set(inlier_list, point1_int, point2_int);
#if 0
  for (int z=0, k=0; z < inlier_list.size(); z++)
    if (inlier_list[z]) {
      std::cerr << "residualsF[" << z << "] : " << residualsF[z] << '\n';
               << k++ << '\n';
    }
#endif
  inliers_ = inlier_list;
  residuals_ = residualsF;
  std::cerr << "Inlier -\n"
           << "         std : " << std_in << '\n'
           << "         " << inlier_count << '/' << data_size_ << '\n';
  return true;
}

//: Calculate all the residuals for a given relation
std::vector<double> FMatrixComputeRobust::calculate_residuals(std::vector<vgl_homg_point_2d<double> >& one,
                                                             std::vector<vgl_homg_point_2d<double> >& two,
                                                             FMatrix* F)
{
  std::vector<double> ret(data_size_);
  for (int i = 0; i < data_size_; i++) {
    double val = calculate_residual(one[i], two[i], F);
      ret[i] = val;
  }
  return ret;
}

//: Calculate all the residuals for a given relation
std::vector<double> FMatrixComputeRobust::calculate_residuals(std::vector<HomgPoint2D>& one,
                                                             std::vector<HomgPoint2D>& two,
                                                             FMatrix* F)
{
  std::vector<double> ret(data_size_);
  for (int i = 0; i < data_size_; i++) {
    double val = calculate_residual(one[i], two[i], F);
      ret[i] = val;
  }
  return ret;
}

//: Find the standard deviation of the residuals
double FMatrixComputeRobust::stdev(std::vector<double>& residuals)
{
  double ret = 0.0;
  for (int i = 0; i < data_size_; i++)
    ret += residuals[i];

  ret /= residuals.size();
  ret = std::sqrt(ret);
  return ret;
}

//:
// \todo not yet implemented
double FMatrixComputeRobust::calculate_term(std::vector<double>& /*residuals*/,
                                            std::vector<bool>& /*inlier_list*/,
                                            int& /*count*/)
{
  std::cerr << "FMatrixComputeRobust::calculate_term() not yet implemented\n";
  return 10000.0;
}

//:
// \todo not yet implemented
double FMatrixComputeRobust::calculate_residual(vgl_homg_point_2d<double>& /*one*/,
                                                vgl_homg_point_2d<double>& /*two*/,
                                                FMatrix* /*F*/)
{
  std::cerr << "FMatrixComputeRobust::calculate_residual() not yet implemented\n";
  return 100.0;
}

//:
// \todo not yet implemented
double FMatrixComputeRobust::calculate_residual(HomgPoint2D& /*one*/,
                                                HomgPoint2D& /*two*/,
                                                FMatrix* /*F*/)
{
  std::cerr << "FMatrixComputeRobust::calculate_residual() not yet implemented\n";
  return 100.0;
}
