// This is core/vpgl/algo/vpgl_calibration_matrix_compute.cxx
#include <iostream>
#include <vpgl/algo/vpgl_calibration_matrix_compute.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_det.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_linear.h>
#include <vgl/algo/vgl_h_matrix_2d_optimize_lmq.h>

bool
vpgl_calibration_matrix_compute::natural(const vgl_h_matrix_2d<double>& homography,
                                         const vgl_point_2d<double>& principal_point,
                                         vpgl_calibration_matrix<double>& K)
{
  // For zero skew and square aspect ratio, the IAC is of the following form:
  //
  //       1   [   1      0           -u0        ]
  // w = ----- [   0      1           -v0        ]
  //      f^2  [ -u0    -v0    u0^2 + v0^2 + f^2 ]
  //
  // For a homography, we have two constraint equations:
  //    h1^T * w * h2 = 0
  //    h1^T * w * h1 = h2^T * w * h2
  //
  // Although 'f' is the single unknown we want to calculate, it's easier
  // to solve for w[2][2], as that is a system of over determined linear
  // equations.  Once we have a value for w[2][2] we can compute 'f'.

  vnl_matrix_fixed<double,3,3 > H = homography.get_matrix();
  const double u0 = principal_point.x();
  const double v0 = principal_point.y();

  if ( vnl_det( H ) < 0 )
  {
    std::cerr << "compute::natural input homography is an inversion" << std::endl;
    return false;
  }

  // Set-up the system of equations Ax = b, where 'x' is 'w[2][2]'.
  double a = - H[2][0] * H[2][1]
           + H[2][1] * H[2][1] - H[2][0] * H[2][0];

  double b = H[0][0] * ( H[0][1] - u0 * H[2][1] )
           + H[1][0] * ( H[1][1] - v0 * H[2][1] )
           + H[2][0] * ( -u0 * H[0][1] - v0 * H[1][1] )
           + H[0][0] * ( H[0][0] - u0 * H[2][0] )
           + H[1][0] * ( H[1][0] - v0 * H[2][0] )
           + H[2][0] * ( - u0 * H[0][0] - v0 * H[1][0] )
           - H[0][1] * ( H[0][1] - u0 * H[2][1] )
           - H[1][1] * ( H[1][1] - v0 * H[2][1] )
           - H[2][1] * ( - u0 * H[0][1] - v0 * H[1][1] );

  // solve
  double x = b / a;

  // re-arrange w[2][2] to get f^2
  double f2 = x - u0 * u0 - v0 * v0;
  if ( f2 < 0 )
  {
    std::cout << "suspicious square focal length: " << f2 << std::endl;
    return false;
  }

  double f = std::sqrt( f2 );

  // Done
  K = vpgl_calibration_matrix<double>( f, principal_point );

  return true;
}

bool
vpgl_calibration_matrix_compute::natural(const std::vector< vgl_point_2d<double> >& image_pts,
                                         const std::vector< vgl_point_2d<double> >& ground_pts,
                                         const vgl_point_2d<double>& principal_point,
                                         vpgl_calibration_matrix<double>& K)
{
  if ( image_pts.size() != ground_pts.size() )
  {
    std::cerr << "number of image and ground points must be the same" << std::endl;
    return false;
  }

  if ( image_pts.size() < 4 )
  {
    std::cerr << "at least four point correspondences are required" << std::endl;
    return false;
  }


  // convert to homogeneous coords to estimate homography
  std::vector< vgl_homg_point_2d<double> > homg_image_pts;
  std::vector< vgl_homg_point_2d<double> > homg_ground_pts;
  for ( unsigned int k = 0; k < image_pts.size(); ++k )
  {
    homg_image_pts.emplace_back( image_pts[k] );
    homg_ground_pts.emplace_back( ground_pts[k] );
  }

  // estimate and optimize the ground to image homography
  vgl_h_matrix_2d<double> H;
  if ( vgl_h_matrix_2d_compute_linear().compute( homg_ground_pts, homg_image_pts, H ) == false )
  {
    std::cerr << "failed to compute homography" << std::endl;
    return false;
  }

  // cheirality
  if ( vnl_det(H.get_matrix()) < 0 )
  {
    H = vgl_h_matrix_2d<double>(-H.get_matrix());
  }

  // optimize
  vgl_h_matrix_2d_optimize_lmq(H).optimize(homg_ground_pts, homg_image_pts, H);

  return vpgl_calibration_matrix_compute::natural(H, principal_point, K);
}
