#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/algo/vpgl_fm_compute_8_point.h>
#include <vpgl/algo/vpgl_fm_compute_2_point.h>
#include <bpgl/algo/bpgl_fm_compute_ransac.h>
#include <bpgl/bpgl_reg_fundamental_matrix.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <bpgl/algo/bpgl_fm_compute_affine_ransac.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>

static void test_fm_compute()
{
  // PART 1: Test the affine fundamental matrix computation.
  vpgl_affine_fundamental_matrix<double> fm_aff1;
  fm_aff1.set_from_params(-3,2,5,-1,2);
  vpgl_proj_camera<double> rc_aff;
  vpgl_proj_camera<double> lc_aff =
    fm_aff1.extract_left_camera( vnl_vector_fixed<double,3>(1,2,3), 1 );

  bpgl_fm_compute_affine_ransac_params params_aff;
  bpgl_fm_compute_affine_ransac R( &params_aff );
  std::vector< vgl_point_2d<double> > lp_aff, rp_aff;
  for ( int i = 0; i < 10; i++ )
  {
    vgl_homg_point_3d<double> world_point;
    if ( i == 0 ) world_point.set(1,4,-2);
    if ( i == 1 ) world_point.set(-2,7,1);
    if ( i == 2 ) world_point.set(3,0,3);
    if ( i == 3 ) world_point.set(2,2,-4);
    if ( i == 4 ) world_point.set(6,-2,1);
    if ( i == 5 ) world_point.set(4,2,-4);
    if ( i == 6 ) world_point.set(-5,-5,-1);
    if ( i == 7 ) world_point.set(2,-3,-1);
    if ( i == 8 ) world_point.set(-1,0,-2);
    if ( i == 9 ) world_point.set(5,6,-4);
    lp_aff.emplace_back( lc_aff.project( world_point ) );
    rp_aff.emplace_back( rc_aff.project( world_point ) );
  }
  lp_aff.emplace_back(4,5 );
  rp_aff.emplace_back(1,2 );
  lp_aff.emplace_back(2,8 );
  rp_aff.emplace_back(2,0 );
  lp_aff.emplace_back(-2,4 );
  rp_aff.emplace_back(-5,-3 );
  lp_aff.emplace_back(4,-5 );
  rp_aff.emplace_back(-1,2 );

  vpgl_affine_fundamental_matrix<double> fm_aff_est1;
  R.compute( rp_aff, lp_aff, fm_aff_est1 );
  std::cerr << "True registered fundamental matrix:\n" << fm_aff1.get_matrix()
           << "Estimated registered fundamental matrix:\n" << fm_aff_est1.get_matrix();


  // PART 3: Test the ransac algorithm with perfect correspondences.
  double random_list2r[12] = { -4, 15, 19, -12, 2, -26, -9, 17, -.5, -26, 11, 7 };
  double random_list2l[12] = { -10, 8, .676, .15, -13, -2, 8, 22, 34, -11, 4, 24 };
  vpgl_proj_camera<double> C2r( random_list2r );
  vpgl_proj_camera<double> C2l( random_list2l );
  vpgl_fundamental_matrix<double> fm2( C2r, C2l );

  std::vector< vgl_homg_point_3d<double> > p2w;
  p2w.emplace_back( 2, -1, 5 );
  p2w.emplace_back( 1, 10, 0 );
  p2w.emplace_back( -5, -7, 1 );
  p2w.emplace_back( 0, 8, 10 );
  p2w.emplace_back( 1, 2, 3 );
  p2w.emplace_back( -4, -10, 0 );
  p2w.emplace_back( -2, 3, -1 );
  p2w.emplace_back( -1, -1, 3 );
  p2w.emplace_back( 4, -8, 2 );
  p2w.emplace_back( 3, 6, 2 );
  p2w.emplace_back( -2, 8, 6 );
  p2w.emplace_back( 6, 8, -5 );

  std::vector< vgl_point_2d<double> > p2r, p2l;
  for (const auto & i : p2w) {
    p2r.emplace_back( C2r.project( i ) );
    p2l.emplace_back( C2l.project( i ) );
  }

  //Outliers
  p2r.emplace_back(5,2 );
  p2l.emplace_back(-1,4 );
  p2r.emplace_back(-2,-2 );
  p2l.emplace_back(1,4 );
  p2r.emplace_back(1,1 );
  p2l.emplace_back(2,-5 );
  p2r.emplace_back(8,-3 );
  p2l.emplace_back(-1,3 );
  p2r.emplace_back(-1,6 );
  p2l.emplace_back(-2,8 );
  p2r.emplace_back(8,-1 );
  p2l.emplace_back(-3,2 );
  p2r.emplace_back(5,1 );
  p2l.emplace_back(-2,2 );
  p2r.emplace_back(-1,3 );
  p2l.emplace_back(7,-4 );

  bpgl_fm_compute_ransac fmc2;
  vpgl_fundamental_matrix<double> fm2est;
  fmc2.compute( p2r, p2l, fm2est );

  vnl_double_3x3 fm2_vnl = fm2.get_matrix();
  fm2_vnl/=fm2_vnl(0,0);
  vnl_double_3x3 fm2est_vnl = fm2est.get_matrix();
  fm2est_vnl/=fm2est_vnl(0,0);

  std::cerr << "\nTrue fundamental matrix:\n" << fm2_vnl << '\n'
           << "\nEstimated fundamental matrix:\n" << fm2est_vnl << '\n';
  TEST_NEAR( "fm compute ransac from perfect correspondences",
             (fm2_vnl-fm2est_vnl).frobenius_norm(), 0, 2.5 );
}

TESTMAIN(test_fm_compute);
