#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/algo/vpgl_fm_compute_8_point.h>
#include <vpgl/algo/vpgl_fm_compute_2_point.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>

static void test_fm_compute()
{
  // Test the 8 point algorithm from perfect correspondences.

  double random_list1r[12] = { 1, 15, 9, -1, 2, -6, -9, 7, -5, 6, 10, 0 };
  double random_list1l[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vpgl_proj_camera<double> C1r( random_list1r );
  vpgl_proj_camera<double> C1l( random_list1l );
  vpgl_fundamental_matrix<double> fm1( C1r, C1l );
  std::cerr << "Break 1:\n" << fm1.get_matrix(); //DEBUG
  std::vector< vgl_homg_point_3d<double> > p1w;
  p1w.emplace_back( 2, -1, 5 );
  p1w.emplace_back( 1, 10, 0 );
  p1w.emplace_back( -5, -7, 1 );
  p1w.emplace_back( 0, 8, 10 );
  p1w.emplace_back( 1, 2, 3 );
  p1w.emplace_back( -4, -10, 0 );
  p1w.emplace_back( 6, 8, -5 );
  p1w.emplace_back( -2, 0, -1 );

  std::vector< vgl_homg_point_2d<double> > p1r, p1l;
  for (const auto & i : p1w) {
    p1r.push_back( C1r.project( i ) );
    p1l.push_back( C1l.project( i ) );
  }

  vpgl_fm_compute_8_point fmc;
  vpgl_fundamental_matrix<double> fm1est;
  fmc.compute( p1r, p1l, fm1est );
  std::cerr << "Break 2:\n" << fm1est.get_matrix(); //DEBUG

  vnl_double_3x3 fm1_vnl = fm1.get_matrix();
  fm1_vnl/=fm1_vnl(0,0);
  vnl_double_3x3 fm1est_vnl = fm1est.get_matrix();
  fm1est_vnl/=fm1est_vnl(0,0);

  std::cerr << "\nTrue fundamental matrix:\n" << fm1_vnl << '\n'
           << "\nEstimated fundamental matrix:\n" << fm1est_vnl << '\n';
           //<< "\nMVL estimated fundamental_matrix:\n" << fm1est_mvl_vnl << '\n';
  TEST_NEAR( "fm compute 8 point from perfect correspondences with outliers",
             (fm1_vnl-fm1est_vnl).frobenius_norm(), 0, 1 );

  //Part 2a: Test the 2 point algorithm
  double clm[] = { 1.0, 0.0, 0.0, 0,
                   0.0, 1.0, 0.0, 0,
                   0.0, 1.0, 1.0, 0 };
  double crm[] = { 1.0, 0.0, 0.0, 2,
                   0.0, 1.0, 0.0, 4,
                   0.0, 1.0, 1.0, 6 };

  vpgl_proj_camera<double> Ctl = vnl_matrix_fixed<double,3,4>(clm),
                           Ctr = vnl_matrix_fixed<double,3,4>(crm);
  vpgl_fundamental_matrix<double> fm3p( Ctr, Ctl );
  vnl_double_3x3 mideal = fm3p.get_matrix();
  std::cerr << "Two Point F Matrix Ideal:\n" << mideal; //DEBUG
  p1r.clear(); p1l.clear();
  //for ( unsigned i = 0; i < p1w.size(); i++ ) {
  for ( unsigned i = 0; i < 2; i++ ) {
    p1r.push_back( Ctl.project( p1w[i] ) );
    p1l.push_back( Ctr.project( p1w[i] ) );
  }
  vpgl_fm_compute_2_point fc2(true);
  vpgl_fundamental_matrix<double> f3lest;
  fc2.compute( p1r, p1l, f3lest );
  vnl_double_3x3 m3lin = f3lest.get_matrix();
  double sc = mideal[0][1]/m3lin[0][1];
  m3lin*=sc;
  std::cerr << "Two Point F Matrix:\n" << m3lin; //DEBUG
  TEST_NEAR( "fm two point linear from perfect correspondences",
             (mideal-m3lin).frobenius_norm(), 0, 1 );
}

TESTMAIN(test_fm_compute);
