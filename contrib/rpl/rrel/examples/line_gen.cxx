// This is rpl/rrel/examples/line_gen.cxx

//:
// \file
//  Simple program to generate line data plus outliers.
//

#include <vcl_iostream.h>
#include <vnl/vnl_random.h>

int
main()
{
  int x0 = 0;
  int xd = 90;
  int x1 = 100;

  // Params for line 1
  double a0 = 10, a1 = 0.2;

  // Params for line 2
  double b0 = 20, b1 = -0.01;

  // Small scale ("normal") noise
  double sigma = 1.0;

  // Large scale ("outlier") noise
  double outlier_sigma = 20.0;

  double outlier_frac = 0.30;

  int seed = 40;
  vnl_random mz_random(seed);
  int inliers=0, outliers=0;

  //  generate and output data for left side
  int x;
  for ( x = x0; x<=xd; ++ x ) {
    double y = a0 + a1*x;
    if ( mz_random.drand32() < outlier_frac ) {
      y += 20 + outlier_sigma * mz_random.normal();
      outliers ++;
    }
    else {
      y += sigma * mz_random.normal();
      inliers ++ ;
    }
    vcl_cout << double(x) << "  " << y << vcl_endl;
  }

  for ( ; x<=x1; ++ x) {
    double y = b0 + b1*x;
    if ( mz_random.drand32() < outlier_frac ) {
      y += outlier_sigma * mz_random.normal();
      outliers ++;
    }
    else {
      y += sigma * mz_random.normal();
      inliers ++ ;
    }
    vcl_cout << double(x) << "  " << y << vcl_endl;
  }
  vcl_cerr << "outlier frac = " << outliers / double(inliers+outliers) << vcl_endl;

  return 0;
}
