#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>

#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>

static void test_camera_compute()
{
  // PART 1: Test the affine camera computation

  vnl_vector_fixed<double,4> r1( 1, 2, 3, 4 );
  vnl_vector_fixed<double,4> r2( -1, 4, -2, 0 );
  vpgl_affine_camera<double> C1( r1, r2 );
  vcl_vector< vgl_point_3d<double> > world_pts;
  world_pts.push_back( vgl_point_3d<double>( 1, 0, -1 ) );
  world_pts.push_back( vgl_point_3d<double>( 6, 1, 2 ) );
  world_pts.push_back( vgl_point_3d<double>( -1, -3, -2 ) );
  world_pts.push_back( vgl_point_3d<double>( 0, 0, 2 ) );
  world_pts.push_back( vgl_point_3d<double>( 2, -1, -5 ) );
  world_pts.push_back( vgl_point_3d<double>( 8, 1, -2 ) );
  world_pts.push_back( vgl_point_3d<double>( -4, -4, 5 ) );
  world_pts.push_back( vgl_point_3d<double>( -1, 3, 4 ) );
  world_pts.push_back( vgl_point_3d<double>( 1, 2, -7 ) );
  vcl_vector< vgl_point_2d<double> > image_pts;
  for ( unsigned int i = 0; i < world_pts.size(); ++i )
    image_pts.push_back( C1.project( vgl_homg_point_3d<double>(world_pts[i]) ) );

  vpgl_affine_camera_compute acc;
  vpgl_affine_camera<double> C1e;
  acc.compute( image_pts, world_pts, C1e );

  vcl_cerr << "\nTrue camera matrix:\n" << C1.get_matrix() << '\n'
           << "\nEstimated camera matrix:\n" << C1e.get_matrix() << '\n';
  TEST_NEAR( "vpgl_affine_camera_compute:",
    ( C1.get_matrix()-C1e.get_matrix() ).frobenius_norm(), 0, 1 );
}

TESTMAIN(test_camera_compute);
