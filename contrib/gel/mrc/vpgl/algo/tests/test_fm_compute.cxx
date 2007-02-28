#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vpgl/algo/vpgl_fm_compute_8_point.h>
#include <vpgl/algo/vpgl_fm_compute_2_point.h>
#include <vpgl/algo/vpgl_fm_compute_ransac.h>
#include <vpgl/vpgl_reg_fundamental_matrix.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vpgl/algo/vpgl_fm_compute_affine_ransac.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_rank.h>
#include <vnl/vnl_double_3x3.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

// Following are for comparison
#include <mvl/FMatrix.h>
#include <mvl/FMatrixCompute7Point.h>

static void test_fm_compute()
{
  // PART 1: Test the affine fundamental matrix computation.
  vpgl_affine_fundamental_matrix<double> fm_aff1;
  fm_aff1.set_from_params(-3,2,5,-1,2);
  vpgl_proj_camera<double> rc_aff;
  vpgl_proj_camera<double> lc_aff = 
    fm_aff1.extract_left_camera( vnl_vector_fixed<double,3>(1,2,3), 1 );
  
  vpgl_fm_compute_affine_ransac_params params_aff;
  vpgl_fm_compute_affine_ransac R( &params_aff );
  vcl_vector< vgl_point_2d<double> > lp_aff, rp_aff;
  for( int i = 0; i < 10; i++ ){
    vgl_homg_point_3d<double> world_point;
    if( i == 0 ) world_point.set(1,4,-2);
    if( i == 1 ) world_point.set(-2,7,1);
    if( i == 2 ) world_point.set(3,0,3);
    if( i == 3 ) world_point.set(2,2,-4);
    if( i == 4 ) world_point.set(6,-2,1);
    if( i == 5 ) world_point.set(4,2,-4);
    if( i == 6 ) world_point.set(-5,-5,-1);
    if( i == 7 ) world_point.set(2,-3,-1);
    if( i == 8 ) world_point.set(-1,0,-2);
    if( i == 9 ) world_point.set(5,6,-4);
    lp_aff.push_back( vgl_point_2d<double>( lc_aff.project( world_point ) ) );
    rp_aff.push_back( vgl_point_2d<double>( rc_aff.project( world_point ) ) );
  }
  lp_aff.push_back( vgl_point_2d<double>(4,5) );
  rp_aff.push_back( vgl_point_2d<double>(1,2) );
  lp_aff.push_back( vgl_point_2d<double>(2,8) );
  rp_aff.push_back( vgl_point_2d<double>(2,0) );
  lp_aff.push_back( vgl_point_2d<double>(-2,4) );
  rp_aff.push_back( vgl_point_2d<double>(-5,-3) );
  lp_aff.push_back( vgl_point_2d<double>(4,-5) );
  rp_aff.push_back( vgl_point_2d<double>(-1,2) );

  vpgl_affine_fundamental_matrix<double> fm_aff_est1;
  R.compute( rp_aff, lp_aff, fm_aff_est1 );
  vcl_cerr << "True registered fundamental matrix:\n" << fm_aff1.get_matrix();
  vcl_cerr << "Estimated registered fundamental matrix:\n" << fm_aff_est1.get_matrix();

  // PART 2: Test the 8 point algorithm.
  // Test the 8 point algorithm from perfect correspondences.

  double random_list1r[12] = { 1, 15, 9, -1, 2, -6, -9, 7, -5, 6, 10, 0 };
  double random_list1l[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vpgl_proj_camera<double> C1r( random_list1r );
  vpgl_proj_camera<double> C1l( random_list1l );
  vpgl_fundamental_matrix<double> fm1( C1r, C1l );
  vcl_cerr << "Break 1:\n" << fm1.get_matrix(); //DEBUG
  vcl_vector< vgl_homg_point_3d<double> > p1w;
  p1w.push_back( vgl_homg_point_3d<double>( 2, -1, 5 ) );
  p1w.push_back( vgl_homg_point_3d<double>( 1, 10, 0 ) );
  p1w.push_back( vgl_homg_point_3d<double>( -5, -7, 1 ) );
  p1w.push_back( vgl_homg_point_3d<double>( 0, 8, 10 ) );
  p1w.push_back( vgl_homg_point_3d<double>( 1, 2, 3 ) );
  p1w.push_back( vgl_homg_point_3d<double>( -4, -10, 0 ) );
  p1w.push_back( vgl_homg_point_3d<double>( 6, 8, -5 ) );
  p1w.push_back( vgl_homg_point_3d<double>( -2, 0, -1 ) );

  vcl_vector< vgl_homg_point_2d<double> > p1r, p1l;
  for( unsigned i = 0; i < p1w.size(); i++ ){
    p1r.push_back( C1r.project( p1w[i] ) );
    p1l.push_back( C1l.project( p1w[i] ) );
  }

  vpgl_fm_compute_8_point fmc;
  vpgl_fundamental_matrix<double> fm1est;
  fmc.compute( p1r, p1l, fm1est );
  vcl_cerr << "Break 2:\n" << fm1est.get_matrix(); //DEBUG

  vnl_double_3x3 fm1_vnl = fm1.get_matrix();
  fm1_vnl/=fm1_vnl(0,0);
  vnl_double_3x3 fm1est_vnl = fm1est.get_matrix();
  fm1est_vnl/=fm1est_vnl(0,0);

  vcl_vector< FMatrix* > fm1est_mvl;
  FMatrixCompute7Point fmc_mvl;
  fmc_mvl.compute( p1r, p1l, fm1est_mvl );
  vnl_matrix<double> fm1est_mvl_vnl(3,3);
  vcl_cerr << "Break 3:\n" << fm1est_mvl_vnl; //DEBUG
  fm1est_mvl[1]->get( &fm1est_mvl_vnl );
  fm1est_mvl_vnl/=fm1est_mvl_vnl(0,0);

  vcl_cerr << "\nTrue fundamental matrix:\n" << fm1_vnl << '\n';
  vcl_cerr << "\nEstimated fundamental matrix:\n" << fm1est_vnl << '\n';
  vcl_cerr << "\nMVL estimated fundamental_matrix:\n" << fm1est_mvl_vnl << '\n';
  TEST_NEAR( "fm compute 8 point from perfect correspondences with outliers", 
             (fm1_vnl-fm1est_vnl).frobenius_norm(), 0, 1 );

  //Part 2a: Test the 2 point algorithm
  vnl_matrix_fixed<double,3,4> crm, clm;
  clm[0][0]=1.0; clm[0][1]=0.0; clm[0][2]=0.0; clm[0][3]=0;
  clm[1][0]=0.0; clm[1][1]=1.0; clm[1][2]=0.0; clm[1][3]=0;
  clm[2][0]=0.0; clm[2][1]=1.0; clm[2][2]=1.0; clm[2][3]=0;
  crm[0][0]=1.0; crm[0][1]=0.0; crm[0][2]=0.0; crm[0][3]=2;
  crm[1][0]=0.0; crm[1][1]=1.0; crm[1][2]=0.0; crm[1][3]=4;
  crm[2][0]=0.0; crm[2][1]=1.0; crm[2][2]=1.0; crm[2][3]=6;
  vpgl_proj_camera<double> Ctl(clm), Ctr(crm);  
  vpgl_fundamental_matrix<double> fm3p( Ctr, Ctl );
  vnl_double_3x3 mideal = fm3p.get_matrix();
  vcl_cerr << "Two Point F Matrix Ideal:\n" << mideal; //DEBUG
  p1r.clear(); p1l.clear();
  //for( unsigned i = 0; i < p1w.size(); i++ ){
  for( unsigned i = 0; i < 2; i++ ){
    p1r.push_back( Ctl.project( p1w[i] ) );
    p1l.push_back( Ctr.project( p1w[i] ) );
  }
  vpgl_fm_compute_2_point fc2(true);
  vpgl_fundamental_matrix<double> f3lest;
  fc2.compute( p1r, p1l, f3lest );
  vnl_double_3x3 m3lin = f3lest.get_matrix();
  double sc = mideal[0][1]/m3lin[0][1];
  m3lin*=sc;
  vcl_cerr << "Two Point F Matrix:\n" << m3lin; //DEBUG
  TEST_NEAR( "fm two point linear fromm perfect correspondences", 
             (mideal-m3lin).frobenius_norm(), 0, 1 );
  // PART 3: Test the ransac algorithm with perfect correspondences.
  double random_list2r[12] = { -4, 15, 19, -12, 2, -26, -9, 17, -.5, -26, 11, 7 };
  double random_list2l[12] = { -10, 8, .676, .15, -13, -2, 8, 22, 34, -11, 4, 24 };
  vpgl_proj_camera<double> C2r( random_list2r );
  vpgl_proj_camera<double> C2l( random_list2l );
  vpgl_fundamental_matrix<double> fm2( C2r, C2l );

  vcl_vector< vgl_homg_point_3d<double> > p2w;
  p2w.push_back( vgl_homg_point_3d<double>( 2, -1, 5 ) );
  p2w.push_back( vgl_homg_point_3d<double>( 1, 10, 0 ) );
  p2w.push_back( vgl_homg_point_3d<double>( -5, -7, 1 ) );
  p2w.push_back( vgl_homg_point_3d<double>( 0, 8, 10 ) );
  p2w.push_back( vgl_homg_point_3d<double>( 1, 2, 3 ) );
  p2w.push_back( vgl_homg_point_3d<double>( -4, -10, 0 ) );
  p2w.push_back( vgl_homg_point_3d<double>( -2, 3, -1 ) );
  p2w.push_back( vgl_homg_point_3d<double>( -1, -1, 3 ) );
  p2w.push_back( vgl_homg_point_3d<double>( 4, -8, 2 ) );
  p2w.push_back( vgl_homg_point_3d<double>( 3, 6, 2 ) );
  p2w.push_back( vgl_homg_point_3d<double>( -2, 8, 6 ) );
  p2w.push_back( vgl_homg_point_3d<double>( 6, 8, -5 ) );

  vcl_vector< vgl_point_2d<double> > p2r, p2l;
  for( unsigned i = 0; i < p2w.size(); i++ ){
    p2r.push_back( vgl_point_2d<double>( C2r.project( p2w[i] ) ) );
    p2l.push_back( vgl_point_2d<double>( C2l.project( p2w[i] ) ) );
  }

  //Outliers
  p2r.push_back( vgl_point_2d<double>(5,2) );
  p2l.push_back( vgl_point_2d<double>(-1,4) );
  p2r.push_back( vgl_point_2d<double>(-2,-2) );
  p2l.push_back( vgl_point_2d<double>(1,4) );
  p2r.push_back( vgl_point_2d<double>(1,1) );
  p2l.push_back( vgl_point_2d<double>(2,-5) );
  p2r.push_back( vgl_point_2d<double>(8,-3) );
  p2l.push_back( vgl_point_2d<double>(-1,3) );
  p2r.push_back( vgl_point_2d<double>(-1,6) );
  p2l.push_back( vgl_point_2d<double>(-2,8) );
  p2r.push_back( vgl_point_2d<double>(8,-1) );
  p2l.push_back( vgl_point_2d<double>(-3,2) );
  p2r.push_back( vgl_point_2d<double>(5,1) );
  p2l.push_back( vgl_point_2d<double>(-2,2) );
  p2r.push_back( vgl_point_2d<double>(-1,3) );
  p2l.push_back( vgl_point_2d<double>(7,-4) );

  vpgl_fm_compute_ransac fmc2;
  vpgl_fundamental_matrix<double> fm2est;
  fmc2.compute( p2r, p2l, fm2est );

  vnl_double_3x3 fm2_vnl = fm2.get_matrix();
  fm2_vnl/=fm2_vnl(0,0);
  vnl_double_3x3 fm2est_vnl = fm2est.get_matrix();
  fm2est_vnl/=fm2est_vnl(0,0);

  vcl_cerr << "\nTrue fundamental matrix:\n" << fm2_vnl << '\n';
  vcl_cerr << "\nEstimated fundamental matrix:\n" << fm2est_vnl << '\n';
  TEST_NEAR( "fm compute ransac from perfect correspondences", 
             (fm2_vnl-fm2est_vnl).frobenius_norm(), 0, 1 );

}
TESTMAIN(test_fm_compute);
