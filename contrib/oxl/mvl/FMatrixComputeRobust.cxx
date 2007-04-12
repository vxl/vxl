#include "FMatrixComputeRobust.h"
#include <mvl/Probability.h>
#include <mvl/HomgNorm2D.h>
#include <mvl/HomgOperator2D.h>
#include <mvl/HomgInterestPointSet.h>
#include <mvl/FMatrixCompute7Point.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3x3.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>

FMatrixComputeRobust::FMatrixComputeRobust() {}

FMatrixComputeRobust::~FMatrixComputeRobust() {}
//-----------------------------------------------------------------------------
//
// - Compute a robust fundamental matrix.
//
// Return false if the calculation fails.
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
  vcl_vector<HomgPoint2D> point1_store, point2_store;
  vcl_vector<int> point1_int, point2_int;
  matches.extract_matches(point1_store, point1_int, point2_store, point2_int);
  data_size_ = matches.count();
  vcl_vector<HomgPoint2D> point1_image(data_size_), point2_image(data_size_);

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
  vcl_vector<bool> inlier_list(data_size_, false);
  vcl_vector<double> residualsF(data_size_, 100.0);

  // 150 random samples from the points set
  for (int i = 0; i < 100; i++) {
    vcl_vector<int> index(7);

    // Take the minimum sample of seven points for the F Matrix calculation
    index = Monte_Carlo(point1_store, point1_int,  8, 7);
    vcl_vector<HomgPoint2D> seven1(7);
    vcl_vector<HomgPoint2D> seven2(7);
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
    vcl_vector<FMatrix*> F_temp;
    if (!Computor.compute(seven1, seven2, F_temp))
      vcl_cerr << "Seven point failure\n";

    for (unsigned int k = 0; k < F_temp.size(); k++) {
      int temp_count = 0;
      vcl_vector<bool> list(data_size_);
      vcl_vector<double> residuals = calculate_residuals(point1_image, point2_image, F_temp[k]);
      double term_error = calculate_term(residuals, list, temp_count);
      if (term_error < Ds) {
        Fs = *F_temp[k];
        Ds = term_error;
        basis_ = index;
        inlier_list = list;
        residualsF = residuals;
        count = temp_count;
      }
    }

    for (unsigned int k = 0; k < F_temp.size(); k++)
      delete F_temp[k];
  }
  vcl_cerr << "Final Figures...\n";
  vcl_cerr << "Ds : " << Ds << vcl_endl;
  vnl_double_3x3 sample = Fs.get_matrix();
  HomgPoint2D one, two;
  Fs.get_epipoles(&one, &two);
  vnl_double_2 o = one.get_double2();
  vnl_double_2 t = two.get_double2();
  HomgPoint2D c1(o[0], o[1], 1.0);
  HomgPoint2D c2(t[0], t[1], 1.0);
  vcl_cerr << "Epipole 1 : " << c1 << " Epipole 2 : " << c2 << vcl_endl;
  vcl_cerr << vcl_endl;
  epipole1_ = c1;
  epipole2_ = c2;
  sample /= sample.get(2, 2);
  vcl_cerr << "FMatrix : " << sample << vcl_endl;
  F->set(Fs.get_matrix());

  int inlier_count = count;
  double std_in = 0.0;

  for (int k = 0; k < data_size_; k++) {
    if (inlier_list[k] == true) {
      std_in += residualsF[k];
    }
  }
  std_in /= inlier_count;
  std_in = vcl_sqrt(std_in);

  // Update the inliers in the PairMatchSet object
  matches.set(inlier_list, point1_int, point2_int);
#if 0
  for (int z=0, k=0; z < inlier_list.size(); z++)
    if (inlier_list[z] == true) {
      vcl_cerr << "residualsF[" << z << "] : " << residualsF[z] << vcl_endl;
      vcl_cerr << k++ << vcl_endl;
    }
#endif
  inliers_ = inlier_list;
  residuals_ = residualsF;
  vcl_cerr << "Inlier -\n";
  vcl_cerr << "         std : " << std_in << vcl_endl;
  vcl_cerr << "         " << inlier_count << "/" << data_size_ << vcl_endl;
  return true;
}

// Calculate all the residuals for a given relation
vcl_vector<double> FMatrixComputeRobust::calculate_residuals(vcl_vector<vgl_homg_point_2d<double> >& one,
                                                             vcl_vector<vgl_homg_point_2d<double> >& two,
                                                             FMatrix* F) {
  vcl_vector<double> ret(data_size_);
  for (int i = 0; i < data_size_; i++) {
    double val = calculate_residual(one[i], two[i], F);
      ret[i] = val;
  }
  return ret;
}

// Calculate all the residuals for a given relation
vcl_vector<double> FMatrixComputeRobust::calculate_residuals(vcl_vector<HomgPoint2D>& one,
                                                             vcl_vector<HomgPoint2D>& two,
                                                             FMatrix* F) {
  vcl_vector<double> ret(data_size_);
  for (int i = 0; i < data_size_; i++) {
    double val = calculate_residual(one[i], two[i], F);
      ret[i] = val;
  }
  return ret;
}

// Find the standard deviation of the residuals
double FMatrixComputeRobust::stdev(vcl_vector<double>& residuals) {
  double ret = 0.0;
  for (int i = 0; i < data_size_; i++)
    ret += residuals[i];

  ret /= residuals.size();
  ret = vcl_sqrt(ret);
  return ret;
}

// Implement Me!!! TODO
double FMatrixComputeRobust::calculate_term(vcl_vector<double>& /*residuals*/,
                                            vcl_vector<bool>& /*inlier_list*/,
                                            int& /*count*/) {
  vcl_cerr << "FMatrixComputeRobust::calculate_term() not yet implemented\n";
  return 10000.0;
}

// Implement Me!!! TODO
double FMatrixComputeRobust::calculate_residual(vgl_homg_point_2d<double>& /*one*/,
                                                vgl_homg_point_2d<double>& /*two*/,
                                                FMatrix* /*F*/) {
  vcl_cerr << "FMatrixComputeRobust::calculate_residual() not yet implemented\n";
  return 100.0;
}

// Implement Me!!! TODO
double FMatrixComputeRobust::calculate_residual(HomgPoint2D& /*one*/,
                                                HomgPoint2D& /*two*/,
                                                FMatrix* /*F*/) {
  vcl_cerr << "FMatrixComputeRobust::calculate_residual() not yet implemented\n";
  return 100.0;
}
