// This is brl/bbas/bsta/bsta_gauss.h
#ifndef bsta_gauss_h_
#define bsta_gauss_h_
//:
// \file
// \brief 1-d and 2-d Gaussian smoothing for Parzen window calculations
// \author Joseph Mundy
// \date 5/19/04
//
// A very simple Gaussian smoothing process to be used by gauss
// Keeps the link dependence very light
//
// \verbatim
//  Modifications
// \endverbatim
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
class bsta_gauss
{
 public:
  ~bsta_gauss(){}
  static double bsta_gaussian(const double x, const double sigma);

  static void bsta_1d_gaussian_kernel(const double sigma, const double fuzz,
                                      int& radius,
                                      vcl_vector<double>& kernel);

  static void bsta_1d_gaussian(const double sigma,
                               vcl_vector<double> const& in_buf,
                               vcl_vector<double>& out_buf);

  static void bsta_2d_gaussian(const double sigma,
                               vbl_array_2d<double> const& in_buf,
                               vbl_array_2d<double>& out_buf);
 private:
  //only static methods
  bsta_gauss();
};

#endif // bsta_gauss_h_
