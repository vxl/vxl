// This is rpl/rrel/examples/homography2d_gen.cxx

//:
// \file
// \brief Simple program to generate homography2d data plus outliers.
//

#include <vcl_iostream.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_random.h>

int
main()
{
  int x0 = 0;
  int x1 = 10;
  int y0 = 0;
  int y1 = 10;
  int d = 50;

  double sigma = 1;
  double outlier_sigma = 10.0;
  double outlier_frac = 0.2;

  vnl_matrix<double> H(3,3);
  H(0, 0) = 1;
  H(0, 1) = 0;
  H(0, 2) = -4;
  H(1, 0) = 0;
  H(1, 1) = 1;
  H(1, 2) = 2;
  H(2, 0) = 0;
  H(2, 1) = 0;
  H(2, 2) = 1;

  vnl_matrix<double> A(3, 1);
  vnl_matrix<double> B(3, 1);

  int seed = 40;
  vnl_random mz_random(seed);
  int inliers=0, outliers=0;

  //  generate and output data for left side
  int x, y;
  for ( x = x0; x<=x1; ++x ) {
    for ( y = y0; y<=y1; ++y ) {
      A(0, 0) = x * d;
      A(1, 0) = y * d;
      A(2, 0) = 1;
      B = H * A;
      if ( mz_random.drand32() < outlier_frac ) {
        A(0, 0) += outlier_sigma * mz_random.normal();
        A(1, 0) += outlier_sigma * mz_random.normal();
        B(0, 0) += B(2,0) * outlier_sigma * mz_random.normal();
        B(1, 0) += B(2,0) * outlier_sigma * mz_random.normal();
        outliers ++;
      }
      else {
        A(0, 0) += sigma * mz_random.normal();
        A(1, 0) += sigma * mz_random.normal();
        B(0, 0) += B(2,0) * sigma * mz_random.normal();
        B(1, 0) += B(2,0) * sigma * mz_random.normal();
        inliers ++ ;
      }
      vcl_cout << A(0, 0) << " " << A(1, 0) << " " << A(2, 0) << " "
               << B(0, 0) << " " << B(1, 0) << " " << B(2, 0) << vcl_endl;
    }
  }

  vcl_cerr << "outlier frac = " << outliers / double(inliers+outliers) << vcl_endl;

  return 0;
}

