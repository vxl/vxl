//:
// \file

#include "HMatrix2DComputeRobust.h"
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <mvl/Probability.h>
#include <mvl/AffineMetric.h>
#include <mvl/HomgInterestPointSet.h>

HMatrix2DComputeRobust::HMatrix2DComputeRobust() {}

HMatrix2DComputeRobust::~HMatrix2DComputeRobust() {}


HMatrix2D HMatrix2DComputeRobust::compute(PairMatchSetCorner& matches)
{
  HMatrix2D H;
  if (compute(matches, &H))
    return H;
  else
    return HMatrix2D();
}

HMatrix2D HMatrix2DComputeRobust::compute(vcl_vector<HomgPoint2D>& points1, vcl_vector<HomgPoint2D>& points2)
{
  if (points1.size() != points2.size())
    vcl_cerr << __FILE__ ": Point vectors are not of equal length\n";
  assert(points1.size() <= points2.size());
  HomgInterestPointSet p1(points1,0);
  HomgInterestPointSet p2(points2,0);
  PairMatchSetCorner matches(&p1, &p2);
  int count = matches.size();
  vcl_vector<bool> inliers(count, true);
  vcl_vector<int> ind1(count), ind2(count);
  for (int i = 0; i < count; i++)  ind1[i] = ind2[i] = i;
  matches.set(inliers, ind1, ind2);

  HMatrix2D H;
  if (compute(matches, &H))
    return H;
  else
    return HMatrix2D();
}

HMatrix2D HMatrix2DComputeRobust::compute(vcl_vector<vgl_homg_point_2d<double> >& points1,
                                          vcl_vector<vgl_homg_point_2d<double> >& points2)
{
  if (points1.size() != points2.size())
    vcl_cerr << __FILE__ ": Point vectors are not of equal length\n";
  assert(points1.size() <= points2.size());
  HomgInterestPointSet p1(points1,0);
  HomgInterestPointSet p2(points2,0);
  PairMatchSetCorner matches(&p1, &p2);
  int count = matches.size();
  vcl_vector<bool> inliers(count, true);
  vcl_vector<int> ind1(count), ind2(count);
  for (int i = 0; i < count; i++)  ind1[i] = ind2[i] = i;
  matches.set(inliers, ind1, ind2);

  HMatrix2D H;
  if (compute(matches, &H))
    return H;
  else
    return HMatrix2D();
}

//-----------------------------------------------------------------------------
//
//: Compute a robust homography.
//
// Return false if the calculation fails.
//
bool HMatrix2DComputeRobust::compute(PairMatchSetCorner& matches, HMatrix2D *H)
{
  // Copy matching points from matchset.
  // Set up some initial variables
  HomgInterestPointSet const* points1 = matches.get_corners1();
  HomgInterestPointSet const* points2 = matches.get_corners2();
  vcl_vector<HomgPoint2D> point1_store, point2_store;
  vcl_vector<int> point1_int, point2_int;
  matches.extract_matches(point1_store, point1_int, point2_store, point2_int);
  data_size_ = matches.count();
  vcl_vector<HomgPoint2D> point1_image(data_size_), point2_image(data_size_);

  for (int a = 0; a < data_size_; a++)
  {
    vnl_double_2 temp1;
    temp1 = points1->get_2d(point1_int[a]);
    point1_image[a] = HomgPoint2D(temp1[0], temp1[1], 1.0);
  }

  for (int a = 0; a < data_size_; a++)
  {
    vnl_double_2 temp2;
    temp2 = points2->get_2d(point2_int[a]);
    point2_image[a] = HomgPoint2D(temp2[0], temp2[1], 1.0);
  }

  HMatrix2D Hs;
  double Ds = 1e+10;
  int count;
  vcl_vector<bool> inlier_list(data_size_);
  vcl_vector<double> residualsH(data_size_, 100.0);
  // 300 random samples from the points set
  for (int i = 0; i < 100; i++)
  {
    vcl_vector<int> index(4);
    // Take the minimum sample of seven points for the F Matrix calculation
    index = Monte_Carlo(point1_store, point1_int, 8, 4);
    vcl_vector<HomgPoint2D> four1_homg(4);
    vcl_vector<HomgPoint2D> four2_homg(4);
    for (int j = 0; j < 4; j++)
    {
      int ind = index[j];
      vnl_double_2 p1 = points1->get_2d(ind);
      four1_homg[j] = HomgPoint2D(p1[0], p1[1], 1.0);
      int other = matches.get_match_12(ind);
      vnl_double_2 p2 = points2->get_2d(other);
      four2_homg[j] = HomgPoint2D(p2[0], p2[1], 1.0);
    }

    // Set up a new HMatrix 4 point Computor
    HMatrix2DCompute4Point Computor;

    // Compute H with preconditioned points
//  HMatrix2D* H_temp_homg = new HMatrix2D();
//  HMatrix2D* H_temp = new HMatrix2D();

    if (!Computor.compute(four1_homg, four2_homg, &Hs))
      vcl_cerr << "HMatrix2DCompute4Point - failure!\n";

    // De-condition H
//    H_temp = new HMatrix2D(metric.homg_to_image_H(*H_temp_homg, metric, metric));

    // Now to for each relation calculate the MLE estimate and corresponding vector
    // of error terms
    int temp_count = 0;
    vcl_vector<bool> list(data_size_);
    vcl_vector<double> residuals = calculate_residuals(point1_image, point2_image, &Hs);
    double mle_error = calculate_term(residuals, list, temp_count);
    if (mle_error < Ds)
    {
//    HMatrix2D Hs_homg = *H_temp_homg;
      Ds = mle_error;
      basis_ = index;
      inlier_list = list;
      residualsH = residuals;
      count = temp_count;
      vcl_cerr << "Minimum so far... : " << Ds << vcl_endl;
      vcl_cerr << "Inliers : " << count << vcl_endl;
      vcl_cerr << "HMatrix2D : " << Hs.get_matrix() << vcl_endl;
    }
  }
  vcl_cerr << "Final Figures...\n";
  vcl_cerr << "Ds : " << Ds << vcl_endl;
  vcl_cerr << "HMatrix2D : " << Hs << vcl_endl;
  H->set(Hs.get_matrix());

  int inlier_count = count;
  double std_in = stdev(residualsH);

  matches.set(inlier_list, point1_int, point2_int);
  inlier_count = matches.compute_match_count();
  inliers_ = inlier_list;
  residuals_ = residualsH;
  vcl_cerr << "Residuals Variance : " << std_in << vcl_endl;
  vcl_cerr << "Inlier -\n";
  vcl_cerr << "         " << inlier_count << vcl_endl;

  return true;
}

double HMatrix2DComputeRobust::stdev(vcl_vector<double>& residuals)
{
  double ret = 0.0;
  for (int i = 0; i < data_size_; i++)
    ret += residuals[i];

  ret /= residuals.size();
  ret = vcl_sqrt(ret);
  return ret;
}

vcl_vector<double> HMatrix2DComputeRobust::calculate_residuals(vcl_vector<vgl_homg_point_2d<double> >& one,
                                                               vcl_vector<vgl_homg_point_2d<double> >& two,
                                                               HMatrix2D* H)
{
  vcl_vector<double> ret(data_size_);
  for (int i = 0; i < data_size_; i++)
  {
    ret[i] = calculate_residual(one[i], two[i], H);
  }
  return ret;
}

vcl_vector<double> HMatrix2DComputeRobust::calculate_residuals(vcl_vector<HomgPoint2D>& one,
                                                               vcl_vector<HomgPoint2D>& two,
                                                               HMatrix2D* H)
{
  vcl_vector<double> ret(data_size_);
  for (int i = 0; i < data_size_; i++)
  {
    ret[i] = calculate_residual(one[i], two[i], H);
  }
  return ret;
}

// TODO
double HMatrix2DComputeRobust::calculate_term(vcl_vector<double>& /*residuals*/,
                                              vcl_vector<bool>& /*inlier_list*/,
                                              int& /*count*/)
{
  vcl_cerr << "HMatrix2DComputeRobust::calculate_term() not yet implemented\n";
  return 10000.0;
}

// TODO
double HMatrix2DComputeRobust::calculate_residual(vgl_homg_point_2d<double>& /*one*/,
                                                  vgl_homg_point_2d<double>& /*two*/,
                                                  HMatrix2D* /*H*/)
{
  vcl_cerr << "HMatrix2DComputeRobust::calculate_residual() not yet implemented\n";
  return -1.0;
}

// TODO
double HMatrix2DComputeRobust::calculate_residual(HomgPoint2D& /*one*/,
                                                  HomgPoint2D& /*two*/,
                                                  HMatrix2D* /*H*/)
{
  vcl_cerr << "HMatrix2DComputeRobust::calculate_residual() not yet implemented\n";
  return 100.0;
}
