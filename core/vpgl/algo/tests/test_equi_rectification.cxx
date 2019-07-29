#include <limits>
#include <iostream>
#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_equi_rectification.h>
#include <vpgl/algo/vpgl_fm_compute_8_point.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_fundamental_matrix.h>
#include <vpgl/vpgl_affine_fundamental_matrix.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_equi_rectification()
{
  //
  // **************  the affine case **************
  //
  vnl_vector_fixed<double,4> row00, row01;
  row00[0] = 2.13641;   row00[1] = 0.000728937; row00[2] = -0.103897; row00[3] = 253.827;
  row01[0] = -0.0107484;   row01[1] = -2.13683; row01[2] = -0.229387; row01[3] = 648.49;
  vpgl_affine_camera<double> acam0(row00, row01);
  vnl_vector_fixed<double, 4> row10, row11;
  row10[0] = 2.8301;   row10[1] = -0.00578537; row10[2] = 0.965491; row10[3] = 370.849;
  row11[0] = 0.250657;   row11[1] = -2.88516; row11[2] = -0.754392; row11[3] = 1061.18;
  vpgl_affine_camera<double> acam1(row10, row11);
  std::vector<vnl_vector_fixed<double, 3> > pts0;
  pts0.emplace_back(517.08519082044199, 405.49679591436848, 1.0);
  pts0.emplace_back(368.62656130374893, 533.20968068909804, 1.0);
  pts0.emplace_back(392.29848889951120, 423.99376519663429, 1.0);
  pts0.emplace_back(415.10361646971978, 515.81585132594773, 1.0);
  pts0.emplace_back(307.54159114243384, 576.47594817744562, 1.0);
  pts0.emplace_back(354.56356426606294, 455.17288861683886, 1.0);
  pts0.emplace_back(461.26439037279738, 470.63791935664727, 1.0);
  pts0.emplace_back(468.61976984314731, 472.78601453482474, 1.0);
  pts0.emplace_back(291.13628062306191, 534.29929831742788, 1.0);
  pts0.emplace_back(472.90116979986323, 509.41945691988161, 1.0);
  pts0.emplace_back(505.67433037185447, 412.38062091046197, 1.0);
  pts0.emplace_back(418.21079400745646, 339.73123667808431, 1.0);
  pts0.emplace_back(395.10755292805965, 541.36311485813326, 1.0);
  pts0.emplace_back(431.57972126743914, 413.56864677893407, 1.0);
  pts0.emplace_back(409.02926512641517, 523.17245556597663, 1.0);
  pts0.emplace_back(499.77563737866802, 369.29886348233589, 1.0);
  pts0.emplace_back(374.57684472738765, 462.98842569351308, 1.0);
  pts0.emplace_back(394.33365039580144, 535.31987299994341, 1.0);
  pts0.emplace_back(325.92706836270702, 290.57854159710979, 1.0);
  pts0.emplace_back(294.89461576267445, 551.15374537669527, 1.0);

  std::vector<vnl_vector_fixed<double , 3> > pts1;
  pts1.emplace_back(804.37166610597023, 732.29242173264697, 1.0);
  pts1.emplace_back(608.10981184076206, 886.30988997648990, 1.0);
  pts1.emplace_back(639.12331300363223, 741.77946271747237, 1.0);
  pts1.emplace_back(669.62370166924109, 868.59261223726821, 1.0);
  pts1.emplace_back(527.32634094356399, 937.14824986983365, 1.0);
  pts1.emplace_back(589.23380555663482, 779.19537477125436, 1.0);
  pts1.emplace_back(730.63082919432691, 813.32064833166964, 1.0);
  pts1.emplace_back(740.38139587443618, 817.13405514819260, 1.0);
  pts1.emplace_back(505.46073221505446, 878.16309403135233, 1.0);
  pts1.emplace_back(746.16876781993733, 867.12969312267182, 1.0);
  pts1.emplace_back(789.27727876815152, 740.17098322795971, 1.0);
  pts1.emplace_back(673.18351202450344, 631.22037119164338, 1.0);
  pts1.emplace_back(643.21533310416612, 900.60585407866961, 1.0);
  pts1.emplace_back(691.12680646128774, 732.57853165751123, 1.0);
  pts1.emplace_back(661.60016063111368, 877.77190747452846, 1.0);
  pts1.emplace_back(781.32708227848002, 681.26778313753960, 1.0);
  pts1.emplace_back(615.77038654683304, 792.23232532727650, 1.0);
  pts1.emplace_back(642.17104021771706, 892.34992238263681, 1.0);
  pts1.emplace_back(550.77860354735083, 553.39783796559527, 1.0);
  pts1.emplace_back(510.49270359363777, 901.38726023140657, 1.0);

  vpgl_affine_fundamental_matrix<double>  aF(acam0, acam1);
  vnl_matrix_fixed<double, 3, 3> H0,  H1;
  bool good = vpgl_equi_rectification::rectify_pair(aF, pts0, pts1, H0, H1);
  TEST("Run on a real example", good, true);

  //check colinear epipolar lines
  bool equal_v = true;

  size_t n = pts0.size();
  for (size_t k = 0; k < n; ++k) {
    vnl_vector_fixed<double, 3> p0 = pts0[k], hp0, p1 = pts1[k], hp1;
    hp0 = H0*p0;  hp1 = H1*p1;

    double v0 = hp0[1]/hp0[2], v1 = hp1[1]/hp1[2];
    double dv = fabs(v1-v0);
    if(dv>0.001)
      equal_v = false;
  }

  TEST("real_example:affine_rectification has collinear epipolar lines", equal_v, true);

  //  check with synthetic data for u transform
  double su = -0.5, sigma_u = 0.1, tu = 10;
  double u0_avg = 0.0, u1_avg = 0.0, v1_avg = 0.0;
  std::vector<vnl_vector_fixed<double, 3> > syn_pts0, syn_pts1=pts1;
  for (size_t i = 0; i < n; ++i) {
    const vnl_vector_fixed<double, 3>& sp1 = syn_pts1[i];
    u1_avg += sp1[0];   v1_avg += sp1[1];
    vnl_vector_fixed<double, 3> syn_p0;
    syn_p0[0] = su * sp1[0] + sigma_u * sp1[1] + tu;
    syn_p0[1] = sp1[1]; syn_p0[2] = 1.0;
    u0_avg += syn_p0[0];
    syn_pts0.push_back(syn_p0);
  }
  u0_avg/=n; u1_avg/=n; v1_avg/=n;
  vnl_matrix_fixed<double, 3,3> fm(0.0);
  fm[1][2] = fm[2][1] = -1.0;
  vpgl_affine_fundamental_matrix<double>  syn_aF(fm);
  vnl_matrix_fixed<double, 3, 3> syn_H0, syn_H1, syn_test_H0, syn_test_H1;
  good = vpgl_equi_rectification::rectify_pair(syn_aF, syn_pts0, syn_pts1, syn_H0, syn_H1);

  syn_test_H0.set_identity();
  syn_test_H0[0][0] = -1.414213562; syn_test_H0[0][1] = 1.414213562*0.058578644;
  syn_test_H0[0][2] =5.857864376*1.414213562; syn_test_H0[1][1] = -1;

  syn_test_H1.set_identity();
  syn_test_H1[0][0] = 0.707106781; syn_test_H1[0][1] = -0.058578644; syn_test_H1[0][2] = -5.857864376;
  syn_test_H1[1][1] = -1;
  double syn_er0 = 0.0, syn_er1 = 0.0;
  for (size_t r = 0; r<2; ++r)
    for (size_t c = 0; c < 2; ++c) {
      syn_er0 += fabs(syn_H0[r][c] - syn_test_H0[r][c]);
      syn_er1 += fabs(syn_H1[r][c] - syn_test_H1[r][c]);
    }
  syn_er0 /= 6; syn_er1 /= 6;
  TEST_NEAR("affine_rectification collinear epipolar lines", syn_er0+syn_er1, 0.0, 1e-8);

  //
  // **************  the projective case **************
  //
  double random_list0[12] = { 1, 15, 9, -1, 2, -6, -9, 7, -5, 6, 10, 0 };
  double random_list1[12] = { 10.6, 1.009, .676, .5, -13, -10, 8, 5, 88, -2, -100, 11 };
  vpgl_proj_camera<double> C0( random_list0 );
  vpgl_proj_camera<double> C1( random_list1 );
  vpgl_fundamental_matrix<double> F( C0, C1 );
  vnl_matrix_fixed<double, 3, 3> mF = F.get_matrix();

  std::vector< vgl_homg_point_3d<double> > p3d;
  p3d.emplace_back( 2, -1, 5 );
  p3d.emplace_back( 1, 10, 0 );
  p3d.emplace_back( -5, -7, 1 );
  p3d.emplace_back( 0, 8, 10 );
  p3d.emplace_back( 1, 2, 3 );
  p3d.emplace_back( -4, -10, 0 );
  p3d.emplace_back( 6, 8, -5 );
  p3d.emplace_back( -2, 0, -1.5 );

  std::vector< vgl_homg_point_2d<double> > pts2d0, pts2d1;
  for (const auto & i : p3d) {
    pts2d0.push_back( C0.project( i ) );
    pts2d1.push_back( C1.project( i ) );
  }
  std::vector<vnl_vector_fixed<double, 3> > ppts0, ppts1;
  for(size_t i = 0; i< pts2d0.size(); ++i){
    ppts0.emplace_back(pts2d0[i].x(), pts2d0[i].y(), pts2d0[i].w());
    ppts1.emplace_back(pts2d1[i].x(), pts2d1[i].y(), pts2d1[i].w());
  }
  vnl_matrix_fixed<double, 3, 3> pH0,  pH1;
  good = vpgl_equi_rectification::rectify_pair(F, ppts0, ppts1, pH0, pH1);
  TEST("equi rectification projective", good, true);
  equal_v = true;
  for (size_t k = 0; k < pts2d0.size() && equal_v; ++k) {
    vnl_vector_fixed<double, 3> p0 = pts0[k], hp0, p1 = pts1[k], hp1;
    hp0 = pH0 * p0;  hp1 = pH1 * p1;
    double v0 = hp0[1] / hp0[2], v1 = hp1[1] / hp1[2];
    double dv = fabs(v1 - v0);
    if (dv > 1.1)
      equal_v = false;
  }
  TEST("collinear epipolar lines projective", equal_v, true);

}

TESTMAIN(test_equi_rectification);
