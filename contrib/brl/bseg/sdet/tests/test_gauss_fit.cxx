// This is brl/bseg/sdet/tests/test_gauss_fit.cxx
#include <vcl_iostream.h>
#include <sdet/sdet_gauss_fit.h>
#include <testlib/testlib_test.h>
#include <vnl/vnl_math.h>
// for new matrix operations
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_determinant.h>
#include <vnl/algo/vnl_matrix_inverse.h>

void test_gauss_fit()
{
#if 0
  // Create 15x15 vector of image points with known parameters
  vcl_vector<vgl_point_3d<double> > img_pts;

  // the gaussian parameters
  double peak = 1500.;
  double plane = 300.;
  double x_bar = 6.4;
  double y_bar = 9.2;
  double var_x = 1.5;
  double var_y = 2.5;
  double covar = 0.0;


  // calculate the image points (1st try - no noise added)
  double pi = vnl_math:pi;
  vnl_matrix<double> V(2,2);        // the covariance matrix
  V(0,0) = var_x;                    // the x variance
  V(0,1) = covar;                    // the covariance (equal)
  V(1,0) = covar;                    // the covariance (equal)
  V(1,1) = var_y;                    // the y variance

  vcl_cout << "Covariance Matrix:\n"
           << V(0,0) << "  " << V(0,1) << "\n\n"
           << V(1,0) << "  " << V(1,1) << vcl_endl << vcl_endl;

  vnl_matrix<double> Vinv = vnl_matrix_inverse<double>(V);
  double Vdet = vnl_determinant(V);  // determinant of variance matrix
  double const_term = 1.0/((2.0*pi)*(2.0*pi)*Vdet);

  vcl_cout << "Covariance Matrix inverse:\n"
           << Vinv(0,0) << "  " << Vinv(0,1) << vcl_endl
           << Vinv(1,0) << "  " << Vinv(1,1) << "\n\n"
           << "Vdet = " << Vdet << ",  const term =" << const_term << vcl_endl << vcl_endl;

  for (double j=0.; j < 15.; j++)
    for (double i=0.; i<15.; i++)
    {
      double peak_delta = peak - plane;             //height of peak above bkgd

      vnl_matrix<double> D(2,1);                    // the pixel's xi-xbari matrix
      D(0,0) = i - x_bar;                            // pixel x loc - peak x loc
      D(1,0) = j - y_bar;                            // pixel y loc - peak y loc

      vnl_matrix<double> Dtrans = D.transpose();    // the xi-xbari matrix transpose
      double exponent = -((Dtrans * Vinv * D)(0,0))/2.0;                // exponent of e

      //value of gaussian at x,y, a matrix operation
      double exponential = vcl_exp(exponent);
//    double gaussian = peak_delta * const_term * exponential;
      double gaussian = peak_delta * exponential;

      vcl_cout << "i,j= [" << i <<',' << j << "], D matrix=[" << D(0,0) << ", " << D(1,0)
               << "], exp= " << exponent << ",  expntl=" << exponential
               <<  ",  gauss= " << gaussian  << vcl_endl;

      vgl_point_3d<double> pnt(i, j, (gaussian+plane));
      img_pts.push_back(pnt);        // value of pixel above "floor"
    }
  vcl_cout << "Finished creating test data, now fitting " << vcl_endl;

  // now fit this noisless distribution and observe the results

  vnl_vector<double> result = sdet_gauss_fit::adjust( img_pts );

  vcl_cout << "fit result; unknows =" << vcl_endl;
  for (int i=0; i<7; i++)
    vcl_cout << i << ",   " << result[i] << vcl_endl;

  // ---------------------------------------------------------------------------
  // Test fit of ellipse to known parameters
  vcl_cout << "\n---------------------------------------------------------------------\n"
           << "\n Test fit of ellipse at known degrees 0, 30, 90 " << vcl_endl;

  // type parameters the same as in fit class
  const double PI = vnl_math:pi;
  double theta = 0.;                        // angle from x axis to ellipse major axis
  double a = 0.;                            // ellipse major axis std deviation
  double b = 0.;                            // ellipse minor axis std deviation

  // ---------------------------------------------------------------------------
  // Test at theta=0 degrees, ellipse with a=3, b=1
  double sxx = 18.0/4.0;
  double syy = 2.0/4.0;
  double sxy = 0.0;

  result[4] = sxx;
  result[5] = syy;
  result[6] = sxy;

  vnl_vector<double> params =
  sdet_gauss_fit::calculate_ellipse( result, 0.0, 0.0, );

  // put params into proper variables
  theta = params[0];
  a = params[1];
  b = params[2];
  float angle = theta*90.0/pi;

  vcl_cout << "\n   Test fit of ellipse at 0 degrees\n"
           << "sin(theta)= " << vcl_sin(theta)
           << ",   cos(theta)= " << vcl_cos(theta) << vcl_endl
           << "theta= " << theta << ",   angle= " << angle
           << ",   a= " << a << ",   b= " << b << vcl_endl;

  // ---------------------------------------------------------------------------
  // now test at theta= 30 degrees
  const double rad = PI/6;
  const double sin30 = vcl_sin(rad);
  const double cos30 = vcl_cos(rad);
  result[4] = sxx = (18.0*cos30*cos30 + 2.0*sin30*sin30)/4.0;
  result[5] = syy = (18.0*sin30*sin30 + 2.0*cos30*cos30)/4.0;
  result[6] = sxy = (18.0*sin30*cos30 + 2.0*cos30*sin30)/4.0;

  params = sdet_gauss_fit::calculate_ellipse( result, 0.0, 0.0);

  // put params into proper variables
  theta = params[0];
  a = params[1];
  b = params[2];
  angle = theta*90.0/pi;

  vcl_cout << "\n   Test fit of ellipse at 30 degrees\n"
           << "sin(theta)= " << vcl_sin(theta)
           << ",   cos(theta)= " << vcl_cos(theta) << vcl_endl
           << "theta= " << theta << ",   angle= " << angle
           << ",   a= " << a << ",   b= " << b << vcl_endl;


  // ---------------------------------------------------------------------------
  // now test at theta= 90 degrees
  result[4] = sxx = 2.0/4.0;
  result[5] = syy = 18.0/4.0;
  result[6] = sxy = 0.0;

  params = sdet_gauss_fit::calculate_ellipse( result, 0.0, 0.0);

  // put params into proper variables
  theta = params[0];
  a = params[1];
  b = params[2];
  angle = theta*180.0/(2.0*pi);

  vcl_cout << "\n   Test fit of ellipse at 90 degrees\n"
           << "sin(theta)= " << vcl_sin(theta)
           << ",   cos(theta)= " << vcl_cos(theta) << vcl_endl
           << "theta= " << theta << ",   angle= " << angle
           << ",   a= " << a << ",   b= " << b << vcl_endl;
#endif //needs work
}

TESTMAIN(test_gauss_fit);
