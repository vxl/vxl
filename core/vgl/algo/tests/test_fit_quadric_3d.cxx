// Some tests for vgl_fit_sphere_3d
// J.L. Mundy, July 7, 2017.
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "testlib/testlib_test.h"
#include <vgl/algo/vgl_fit_quadric_3d.h>
#include <vgl/algo/vgl_fit_xy_paraboloid_3d.h>
#include "vgl/vgl_point_3d.h"
#include "vgl/vgl_pointset_3d.h"
#include "vgl/vgl_distance.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vnl/vnl_random.h"
#include "vnl/vnl_matrix.h"
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/algo/vnl_svd.h>
//
// creates points on the unit sphere
static void
test_linear_fit_sphere_taubin()
{
  std::cout << "\n======== test_linear_fit_Taubin_sphere ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(0.0, 0.0, 1.0);
  vgl_point_3d<double> p3(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p4(0.0, -1.0, 0.0);
  vgl_point_3d<double> p5(0.0, 0.0, -1.0);
  double r = 0.5773503;
  vgl_point_3d<double> p6(r, r, r);
  vgl_point_3d<double> p7(-r, -r, -r);
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit Taubin perfect unit sphere", error, 0.0, 1e-6);
}
static void
test_linear_fit_taubin_sphere_rand_error()
{
  std::cout << "\n======== test_linear_fit_Taubin_sphere_rand_error ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(0.0, 0.0, 1.0);
  vgl_point_3d<double> p3(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p4(0.0, -1.0, 0.0);
  vgl_point_3d<double> p5(0.0, 0.0, -1.0);
  double r = 0.5773503;
  vgl_point_3d<double> p6(r, r, r);
  vgl_point_3d<double> p7(-r, -r, -r);
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  // add random error (+-10%) to the points
  vnl_random ran;
  double e = 0.1;
  std::cout << "error " << e << std::endl;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit sphere with error", error, 0.0, 0.1);
}
static void
test_linear_fit_Taubin_parabolid()
{
  std::cout << "\n======== test_linear_fit_Allaire_paraboloid ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vnl_matrix<double> P(3, 12); // to allow for rotation
  P[0][0] = 1.0;
  P[1][0] = 1.0;
  P[2][0] = 2.0;
  P[0][1] = 2.0;
  P[1][1] = 2.0;
  P[2][1] = 8.0;
  P[0][2] = 3.0;
  P[1][2] = 3.0;
  P[2][2] = 18.0;
  P[0][3] = -1.0;
  P[1][3] = -1.0;
  P[2][3] = 2.0;
  P[0][4] = -2.0;
  P[1][4] = -2.0;
  P[2][4] = 8.0;
  P[0][5] = -3.0;
  P[1][5] = -3.0;
  P[2][5] = 18.0;
  P[0][6] = 1.0;
  P[1][6] = -1.0;
  P[2][6] = 2.0;
  P[0][7] = 2.0;
  P[1][7] = -2.0;
  P[2][7] = 8.0;
  P[0][8] = 3.0;
  P[1][8] = -3.0;
  P[2][8] = 18.0;
  P[0][9] = -1.0;
  P[1][9] = 1.0;
  P[2][9] = 2.0;
  P[0][10] = -2.0;
  P[1][10] = 2.0;
  P[2][10] = 8.0;
  P[0][11] = -3.0;
  P[1][11] = 3.0;
  P[2][11] = 18.0;

  double th = 1.047197551;
  // double th = 0.0;
  vgl_vector_3d<double> off(0.1, 0.2, 0.3);
  double c = cos(th), s = sin(th);
  vnl_matrix<double> R(3, 3), RP;
  R[0][0] = c;
  R[0][1] = 0.0;
  R[0][2] = -s;
  R[1][0] = 0.0;
  R[1][1] = 1.0;
  R[1][2] = 0.0;
  R[2][0] = s;
  R[2][1] = 0.0;
  R[2][2] = c;
  RP = R * P;
  for (size_t i = 0; i < 11; ++i)
  {
    vgl_point_3d<double> p(RP[0][i], RP[1][i], RP[2][i]);
    pts.push_back(p + off);
    std::cout << p + off << std::endl;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);

  vgl_quadric_3d<double> q = fit_parab.quadric_Taubin_fit();
  std::vector<std::vector<double>> Qo = q.coef_matrix();
  std::cout << "Qo" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << Qo[r][0] << ' ' << Qo[r][1] << ' ' << Qo[r][2] << ' ' << Qo[r][3] << std::endl;
  std::vector<std::vector<double>> H;
  std::vector<std::vector<double>> Q = q.canonical_quadric(H);

  std::cout << "Q" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << Q[r][0] << ' ' << Q[r][1] << ' ' << Q[r][2] << ' ' << Q[r][3] << std::endl;
  TEST_NEAR("linear fit parabolid", lin_error, 0.0, 0.25);
  // check inverse transform
  vnl_matrix<double> Qc(4, 4, 0.0), T(4, 4, 0.0);
  for (size_t r = 0; r < 4; ++r)
    for (size_t c = 0; c < 4; ++c)
    {
      Qc[r][c] = Q[r][c];
      T[r][c] = H[r][c];
    }
  std::cout << "H" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << T[r][0] << ' ' << T[r][1] << ' ' << T[r][2] << ' ' << T[r][3] << std::endl;
  vnl_svd<double> svd(T);
  vnl_matrix<double> Hi = svd.inverse();
  vnl_matrix<double> Qi = Hi.transpose() * Qc * Hi;
  for (size_t r = 0; r < 4; ++r)
    std::cout << Qi[r][0] << ' ' << Qi[r][1] << ' ' << Qi[r][2] << ' ' << Qi[r][3] << std::endl;
  double qdiff = 0.0;
  for (size_t r = 0; r < 4; ++r)
    for (size_t c = 0; c < 4; ++c)
      qdiff += fabs(Qi[r][c] - Qo[r][c]);
  qdiff /= 16.0;
  TEST_NEAR("transform canonical frame paraboloid", qdiff, 0.0, 1.0e-6);
}
static void
test_linear_fit_Taubin_parabolid_rand_error()
{
  std::cout << "\n======== test_linear_fit_Allaire_paraboloid_rand_error ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(1.0, 1.0, 2.0);
  vgl_point_3d<double> p1(2.0, 2.0, 8.0);
  vgl_point_3d<double> p2(3.0, 3.0, 18.0);
  vgl_point_3d<double> p3(-1.0, -1.0, 2.0);
  vgl_point_3d<double> p4(-2.0, -2.0, 8.0);
  vgl_point_3d<double> p5(-3.0, -3.0, 18.0);
  vgl_point_3d<double> p6(1.0, -1.0, 2.0);
  vgl_point_3d<double> p7(2.0, -2.0, 8.0);
  vgl_point_3d<double> p8(3.0, -3.0, 18.0);
  vgl_point_3d<double> p9(-1.0, 1.0, 2.0);
  vgl_point_3d<double> p10(-2.0, 2.0, 8.0);
  vgl_point_3d<double> p11(-3.0, 3.0, 18.0);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  // add random error to the points
  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit parabolid with noise", lin_error, 0.0, 0.25);
}
static void
test_linear_fit_parabolic_cylinder_Taubin()
{
  std::cout << "\n======== test_linear_fit_Allaire_parabolic_cylinder ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  // ax^2 + z = 0
  vnl_matrix<double> P(3, 12); // to allow for rotation
  P[0][0] = 0.0;
  P[1][0] = 1.0;
  P[2][0] = 0.0;
  P[0][1] = 0.5;
  P[1][1] = 2.0;
  P[2][1] = 0.25;
  P[0][2] = 1.0;
  P[1][2] = 3.0;
  P[2][2] = 1.0;
  P[0][3] = 1.5;
  P[1][3] = 4.0;
  P[2][3] = 2.25;
  P[0][4] = 2.0;
  P[1][4] = 5.0;
  P[2][4] = 4.0;
  P[0][5] = 2.5;
  P[1][5] = -1.0;
  P[2][5] = 6.25;
  P[0][6] = -0.5;
  P[1][6] = -2.0;
  P[2][6] = 0.25;
  P[0][7] = -1.0;
  P[1][7] = -3.0;
  P[2][7] = 1.0;
  P[0][8] = -1.5;
  P[1][8] = -4.0;
  P[2][8] = 2.25;
  P[0][9] = -2.0;
  P[1][9] = -5.0;
  P[2][9] = 4.0;
  P[0][10] = -2.5;
  P[1][10] = -0.5;
  P[2][10] = 6.25;
  P[0][11] = -3.0;
  P[1][11] = 0.5;
  P[2][11] = 9.0;

  double th = 1.047197551;
  // double th = 0.0;
  vgl_vector_3d<double> off(0.1, 0.2, 0.3);
  // vgl_vector_3d<double> off(0.0, 0.0, 0.0);
  double c = cos(th), s = sin(th);
  vnl_matrix<double> R(3, 3), RP;
  R[0][0] = c;
  R[0][1] = 0.0;
  R[0][2] = -s;
  R[1][0] = 0.0;
  R[1][1] = 1.0;
  R[1][2] = 0.0;
  R[2][0] = s;
  R[2][1] = 0.0;
  R[2][2] = c;
  RP = R * P;
  for (size_t i = 0; i < 11; ++i)
  {
    vgl_point_3d<double> p(RP[0][i], RP[1][i], RP[2][i]);
    pts.push_back(p + off);
    std::cout << p + off << std::endl;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);
  vgl_quadric_3d<double> q = fit_parab.quadric_Taubin_fit();
  std::cout << "Fit error = " << lin_error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
  TEST_NEAR("linear fit parabolic cylinder", lin_error, 0.0, 0.25);

  std::vector<std::vector<double>> Qo = q.coef_matrix();
  std::cout << "Qo" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << Qo[r][0] << ' ' << Qo[r][1] << ' ' << Qo[r][2] << ' ' << Qo[r][3] << std::endl;
  std::vector<std::vector<double>> H;
  std::vector<std::vector<double>> Q = q.canonical_quadric(H);

  std::cout << "Q" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << Q[r][0] << ' ' << Q[r][1] << ' ' << Q[r][2] << ' ' << Q[r][3] << std::endl;

  // check inverse transform
  vnl_matrix<double> Qc(4, 4, 0.0), T(4, 4, 0.0);
  for (size_t r = 0; r < 4; ++r)
    for (size_t c = 0; c < 4; ++c)
    {
      Qc[r][c] = Q[r][c];
      T[r][c] = H[r][c];
    }
  std::cout << "H" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << T[r][0] << ' ' << T[r][1] << ' ' << T[r][2] << ' ' << T[r][3] << std::endl;
  vnl_svd<double> svd(T);
  vnl_matrix<double> Hi = svd.inverse();
  vnl_matrix<double> Qi = Hi.transpose() * Qc * Hi;
  for (size_t r = 0; r < 4; ++r)
    std::cout << Qi[r][0] << ' ' << Qi[r][1] << ' ' << Qi[r][2] << ' ' << Qi[r][3] << std::endl;
  double qdiff = 0.0;
  for (size_t r = 0; r < 4; ++r)
    for (size_t c = 0; c < 4; ++c)
      qdiff += fabs(Qi[r][c] - Qo[r][c]);
  qdiff /= 16.0;
  TEST_NEAR("transform canonical frame parabolic cylinder", qdiff, 0.0, 1.0e-6);
}

static void
test_linear_fit_sphere_Taubin_actual_pts()
{
  std::cout << "\n======== test_linear_fit_Allaire_sphere_actual_pts ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(-10.356499671936, 68.164497375488, 48.227199554443);
  vgl_point_3d<double> p1(-12.030300140381, 68.351600646973, 46.570499420166);
  vgl_point_3d<double> p2(-12.924599647522, 70.128097534180, 46.037700653076);
  vgl_point_3d<double> p3(-12.602700233459, 72.161201477051, 47.159500122070);
  vgl_point_3d<double> p4(-11.404299736023, 75.049201965332, 49.112400054932);
  vgl_point_3d<double> p5(-9.647660255432, 71.332901000977, 49.860500335693);
  vgl_point_3d<double> p6(-13.605299949646, 72.556297302246, 46.091999053955);
  vgl_point_3d<double> p7(-15.126000404358, 71.693496704102, 43.016601562500);
  vgl_point_3d<double> p8(-16.063600540161, 70.053901672363, 41.029098510742);
  vgl_point_3d<double> p9(-13.483099937439, 68.981697082520, 44.901100158691);
  vgl_point_3d<double> p10(-9.131649971008, 69.801300048828, 49.823600769043);
  vgl_point_3d<double> p11(-8.480529785156, 67.906501770020, 50.200801849365);
  vgl_point_3d<double> p12(4.875539779663, 71.071899414063, 50.690101623535);
  vgl_point_3d<double> p13(-3.617100000381, 67.346603393555, 51.243801116943);
  vgl_point_3d<double> p14(-4.381750106812, 68.257698059082, 51.418098449707);
  vgl_point_3d<double> p15(-5.963240146637, 68.928100585938, 51.265399932861);
  vgl_point_3d<double> p16(3.054660081863, 68.043899536133, 51.342098236084);
  vgl_point_3d<double> p17(-11.062999725342, 72.394500732422, 48.930099487305);
  vgl_point_3d<double> p18(-11.003100395203, 70.369201660156, 48.363498687744);
  vgl_point_3d<double> p19(5.466730117798, 69.970596313477, 50.311599731445);
  vgl_point_3d<double> p20(-11.655500411987, 71.570999145508, 48.103099822998);
  vgl_point_3d<double> p21(-10.344300270081, 72.406997680664, 49.586498260498);
  vgl_point_3d<double> p22(-11.970600128174, 72.936203002930, 48.114101409912);
  vgl_point_3d<double> p23(-13.019499778748, 73.698402404785, 47.480598449707);
  vgl_point_3d<double> p24(4.039299964905, 69.958900451660, 50.850700378418);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  pts.push_back(p14);
  pts.push_back(p15);
  pts.push_back(p16);
  pts.push_back(p17);
  pts.push_back(p18);
  pts.push_back(p19);
  pts.push_back(p20);
  pts.push_back(p21);
  pts.push_back(p22);
  pts.push_back(p23);
  pts.push_back(p24);
  vgl_fit_quadric_3d<double> fit_sph_act(pts);
  double lin_error = fit_sph_act.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit sphere sclara pts", lin_error, 0.0, 0.25);
}
static void
test_linear_fit_sphere_Alliare()
{
  std::cout << "\n======== test_linear_fit_Allaire_sphere ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(0.0, 0.0, 1.0);
  vgl_point_3d<double> p3(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p4(0.0, -1.0, 0.0);
  vgl_point_3d<double> p5(0.0, 0.0, -1.0);
  double r = 0.5773503;
  vgl_point_3d<double> p6(r, r, r);
  vgl_point_3d<double> p7(-r, -r, -r);
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire perfect unit sphere", error, 0.0, 1e-6);
}
static void
test_linear_fit_Allaire_sphere_rand_error()
{
  std::cout << "\n======== test_linear_fit_Allaire_sphere_rand_error ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(0.0, 0.0, 1.0);
  vgl_point_3d<double> p3(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p4(0.0, -1.0, 0.0);
  vgl_point_3d<double> p5(0.0, 0.0, -1.0);
  double r = 0.5773503;
  vgl_point_3d<double> p6(r, r, r);
  vgl_point_3d<double> p7(-r, -r, -r);
  vgl_point_3d<double> p8(-r, -r, r);
  vgl_point_3d<double> p9(-r, r, -r);
  vgl_point_3d<double> p10(r, -r, -r);
  vgl_point_3d<double> p11(r, r, -r);
  vgl_point_3d<double> p12(r, -r, r);
  vgl_point_3d<double> p13(-r, r, r);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire sphere with error", error, 0.0, 0.5);
}
static void
test_linear_fit_Allaire_thin_ellipsoid_rand_error()
{
  std::cout << "\n======== test_linear_fit_Allaire_thin_ellipsoid_rand_error ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  double sx = 10.0, sy = 2.0, sz = 1.0;
  vnl_matrix<double> P(3, 14);
  P[0][0] = sx;
  P[1][0] = 0.0;
  P[2][0] = 0.0;
  P[0][1] = 0.0;
  P[1][1] = sy;
  P[2][1] = 0.0;
  P[0][2] = 0.0;
  P[1][2] = 0.0;
  P[2][2] = sz;
  P[0][3] = -sx;
  P[1][3] = 0.0;
  P[2][3] = 0.0;
  P[0][4] = 0.0;
  P[1][4] = -sy;
  P[2][4] = 0.0;
  P[0][5] = 0.0;
  P[1][5] = 0.0;
  P[2][5] = -sz;
  double r = 0.5773503;
  P[0][6] = sx * r;
  P[1][6] = sy * r;
  P[2][6] = sz * r;
  P[0][7] = -sx * r;
  P[1][7] = -sy * r;
  P[2][7] = -sz * r;
  P[0][8] = -sx * r;
  P[1][8] = -sy * r;
  P[2][8] = sz * r;
  P[0][9] = -sx * r;
  P[1][9] = sy * r;
  P[2][9] = -sz * r;
  P[0][10] = sx * r;
  P[1][10] = -sy * r;
  P[2][10] = -sz * r;
  P[0][11] = sx * r;
  P[1][11] = sy * r;
  P[2][11] = -sz * r;
  P[0][12] = sx * r;
  P[1][12] = -sy * r;
  P[2][12] = sz * r;
  P[0][13] = -sx * r;
  P[1][13] = sy * r;
  P[2][13] = sz * r;

  double th = 1.047197551;
  vgl_vector_3d<double> off(1.0, 2.0, 3.0);
  double c = cos(th), s = sin(th);
  vnl_matrix<double> R(3, 3), RP;
  R[0][0] = c;
  R[0][1] = 0.0;
  R[0][2] = -s;
  R[1][0] = 0.0;
  R[1][1] = 1.0;
  R[1][2] = 0.0;
  R[2][0] = s;
  R[2][1] = 0.0;
  R[2][2] = c;
  RP = R * P;
  for (size_t i = 0; i < 13; ++i)
  {
    vgl_point_3d<double> p(RP[0][i], RP[1][i], RP[2][i]);
    pts.push_back(p + off);
    std::cout << p + off << std::endl;
  }
  vnl_random ran;
  double e = 0.0;
  std::cout << "error " << e << std::endl;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  vgl_quadric_3d<double> & q = fit_sph.quadric_Allaire_fit();
  std::cout << "Fit error = " << error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
  TEST_NEAR("linear fit Allaire thin ellipsoid with error", error, 0.0, 0.5);

  std::vector<double> diag;
  std::vector<std::vector<double>> H;

  q.canonical_central_quadric(diag, H);
  std::cout << "canonical q diag[ " << diag[0] << ' ' << diag[1] << ' ' << diag[2] << ' ' << diag[3] << " ]"
            << std::endl;
  std::cout << "H" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << H[r][0] << ' ' << H[r][1] << ' ' << H[r][2] << ' ' << H[r][3] << std::endl;

  vnl_matrix<double> Hf(4, 4), D(4, 4, 0.0);
  for (size_t r = 0; r < 4; ++r)
  {
    for (size_t c = 0; c < 4; ++c)
    {
      Hf[r][c] = H[r][c];
    }
    D[r][r] = diag[r];
  }
  vnl_svd<double> svd(Hf);
  vnl_matrix<double> Hr = svd.inverse();

  vnl_matrix<double> Qr = Hr.transpose() * D * Hr;

  std::vector<std::vector<double>> Qo = q.coef_matrix();
  double dif = 0.0;
  for (size_t r = 0; r < 4; ++r)
    for (size_t c = 0; c < 4; ++c)
      dif += fabs(Qo[r][c] - Qr[r][c]);
  dif /= 16;
  TEST_NEAR("Allaire thin ellipsoid reconstruct dif", dif, 0.0, 1e-06);
}
static void
test_linear_fit_Allaire_saddle_shape()
{
  std::cout << "\n======== test_linear_fit_Allaire_saddle_shape ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p3(0.0, -1.0, 0.0);
  vgl_point_3d<double> p4(2, 0.0, 1.0);
  vgl_point_3d<double> p5(0.0, 2.0, 1.0);
  vgl_point_3d<double> p6(-2.0, 0.0, 1.0);
  vgl_point_3d<double> p7(0.0, -2.0, 1.0);
  vgl_point_3d<double> p8(2, 0.0, -1.0);
  vgl_point_3d<double> p9(0.0, 2.0, -1.0);
  vgl_point_3d<double> p10(-2.0, 0.0, -1.0);
  vgl_point_3d<double> p11(0.0, -2.0, -1.0);


  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);


  vnl_random ran;
  double e = 0.0;
  std::cout << "error " << e << std::endl;
  for (auto & pt : pts)
  {
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sad(pts);
  double error = fit_sad.fit_saddle_shaped_quadric_linear_Allaire(&std::cout);
  vgl_quadric_3d<double> & q = fit_sad.quadric_Allaire_fit();
  std::cout << "Fit error = " << error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
  std::vector<double> diag;
  std::vector<std::vector<double>> H;
  q.canonical_central_quadric(diag, H);
  std::cout << "canonical q diag[ " << diag[0] << ' ' << diag[1] << ' ' << diag[2] << ' ' << diag[3] << " ]"
            << std::endl;
  std::cout << "H" << std::endl;
  for (size_t r = 0; r < 4; ++r)
    std::cout << H[r][0] << ' ' << H[r][1] << ' ' << H[r][2] << ' ' << H[r][3] << std::endl;
  TEST_NEAR("linear fit Allaire saddle", error, 0.0, 0.5);
}
static void
test_linear_fit_Allaire_ellipsoid_actual_pts()
{
  std::cout << "\n======== test_linear_fit_Allaire_ellipsoid_actual_pts ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  vgl_point_3d<double> p0(11.25000000, 222.75000000, 27.50236130);
  vgl_point_3d<double> p1(11.55000019, 222.75000000, 27.50236130);
  vgl_point_3d<double> p2(19.35000038, 220.05000305, 31.17736053);
  vgl_point_3d<double> p3(19.64999962, 220.05000305, 31.40236092);
  vgl_point_3d<double> p4(18.45000076, 217.64999390, 29.75236130);
  vgl_point_3d<double> p5(24.75000000, 214.94999695, 25.25235939);
  vgl_point_3d<double> p6(23.85000038, 212.55000305, 24.05236053);
  vgl_point_3d<double> p7(24.14999962, 212.55000305, 25.10235977);
  vgl_point_3d<double> p8(16.95000076, 211.35000610, 24.57736015);
  vgl_point_3d<double> p9(17.25000000, 211.35000610, 25.55236053);
  vgl_point_3d<double> p10(19.04999924, 211.35000610, 23.30236053);
  vgl_point_3d<double> p11(19.35000038, 211.35000610, 22.10235977);
  vgl_point_3d<double> p12(21.14999962, 211.35000610, 21.65236092);
  vgl_point_3d<double> p13(21.45000076, 211.35000610, 21.65236092);
  vgl_point_3d<double> p14(23.25000000, 211.35000610, 23.45236015);
  vgl_point_3d<double> p15(23.54999924, 211.35000610, 24.12736130);
  vgl_point_3d<double> p16(25.35000038, 211.35000610, 27.05236053);
  vgl_point_3d<double> p17(25.64999962, 211.35000610, 27.05236053);
  vgl_point_3d<double> p18(27.45000076, 211.35000610, 27.05236053);
  pts.push_back(p0);
  pts.push_back(p1);
  pts.push_back(p2);
  pts.push_back(p3);
  pts.push_back(p4);
  pts.push_back(p5);
  pts.push_back(p6);
  pts.push_back(p7);
  pts.push_back(p8);
  pts.push_back(p9);
  pts.push_back(p10);
  pts.push_back(p11);
  pts.push_back(p12);
  pts.push_back(p13);
  pts.push_back(p14);
  pts.push_back(p15);
  pts.push_back(p15);
  pts.push_back(p17);
  pts.push_back(p18);

  vgl_fit_quadric_3d<double> fit_ellp(pts);
  double error = fit_ellp.fit_ellipsoid_linear_Allaire(&std::cout);
  if (error != -1.0)
  {
    vgl_quadric_3d<double> & q = fit_ellp.quadric_Allaire_fit();
    std::cout << "Fit error = " << error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
    std::vector<double> diag;
    std::vector<std::vector<double>> H;
    q.canonical_central_quadric(diag, H);
    std::vector<std::vector<double>> Q = q.coef_matrix();
    vnl_matrix<double> Qn(4, 4), D(4, 4, 0.0);
    for (size_t r = 0; r < 4; ++r)
      for (size_t c = 0; c < 4; ++c)
        Qn[r][c] = Q[r][c];
    std::cout << "canonical q diag[ " << diag[0] << ' ' << diag[1] << ' ' << diag[2] << ' ' << diag[3] << " ]"
              << std::endl;
    for (size_t i = 0; i < 4; ++i)
      D[i][i] = diag[i];
    vnl_matrix<double> Hf(4, 4);
    for (size_t r = 0; r < 4; ++r)
      for (size_t c = 0; c < 4; ++c)
        Hf[r][c] = H[r][c];
    vnl_svd<double> svd(Hf);
    vnl_matrix<double> Hr = svd.inverse();
    vnl_matrix<double> Qo = Hr.transpose() * D * Hr;
    std::cout << "Original dem quadric\n" << Qn << std::endl;
    std::cout << "Reconstructed dem quadric\n" << Qo << std::endl;
    double dif = 0.0;
    for (size_t r = 0; r < 4; ++r)
      for (size_t c = 0; c < 4; ++c)
        dif += fabs(Qn[r][c] - Qo[r][c]);
    dif /= 16;
    TEST_NEAR("reconstruct quadric in original frame", dif, 0.0, 1.0e-6);
  }
  else
    std::cout << "FAIL" << std::endl;
}
static void
test_linear_fit_xy_paraboloid()
{
  std::cout << "\n======== test_linear_fit_xy_paraboloid ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  pts.emplace_back(-3.77509, -0.433246, 12.0539);
  pts.emplace_back(-0.0566412, -0.26103, -0.163405);
  pts.emplace_back(-0.830695, 1.10539, 2.4903);
  pts.emplace_back(-3.13865, 3.56061, 24.4535);
  pts.emplace_back(0.571889, -4.21001, 29.7296);
  pts.emplace_back(-1.88323, 0.880896, 2.43725);
  pts.emplace_back(-1.35966, 3.77834, 27.6817);
  pts.emplace_back(2.98523, 1.48247, 22.2002);
  pts.emplace_back(0.13771, -2.70051, 11.6698);
  pts.emplace_back(4.07775, -2.37528, 19.9287);
  pts.emplace_back(-3.36276, 2.38114, 13.659);
  pts.emplace_back(-3.60372, -1.16172, 15.1071);
  pts.emplace_back(4.81472, -0.641888, 25.0879);
  pts.emplace_back(4.09531, -1.95884, 18.5601);
  pts.emplace_back(0.626256, -4.52967, 34.6878);
  pts.emplace_back(-3.26756, 0.484878, 6.78013);
  pts.emplace_back(-0.910881, -3.24852, 20.7351);
  pts.emplace_back(0.403394, -4.45446, 33.9992);
  pts.emplace_back(1.01449, 4.31895, 48.0507);
  pts.emplace_back(0.0563962, 0.514825, 1.13353);
  vgl_fit_xy_paraboloid_3d<double> fxy(pts);
  double zerr = fxy.fit_linear(&std::cout);
  std::string type = fxy.quadric_type();
  std::cout << fxy.quadric_type() << std::endl;
  TEST("quadric type - linearly fit paraboloid - exact points", fxy.quadric_type(), "elliptic_paraboloid");
  TEST_NEAR(" z error - linearly fit paraboloid - exact points", zerr, 0.0, 1.0e-4);
  vgl_point_2d<double> etr = fxy.extremum_point();
  vgl_point_2d<double> actual_extr(-0.42857, -0.14285);
  double extr_error = (etr - actual_extr).length();
  TEST_NEAR("compute paraboloid extremum - exact points", extr_error, 0.0, 1.0e-4);
}
static void
test_linear_paraboloid_actual_cost_surface()
{
  std::cout << "\n======== test_linear_fit_xy_paraboloid - actual cost surface ===== " << std::endl;
  std::vector<vgl_point_3d<double>> pts;
  pts.emplace_back(-0.5, -0.5, 0.292945);
  pts.emplace_back(-0.4, -0.5, 0.291593);
  pts.emplace_back(-0.3, -0.5, 0.290645);
  pts.emplace_back(-0.2, -0.5, 0.290033);
  pts.emplace_back(-0.1, -0.5, 0.288681);
  pts.emplace_back(0.0, -0.5, 0.287929);
  pts.emplace_back(0.1, -0.5, 0.286838);
  pts.emplace_back(0.2, -0.5, 0.286228);
  pts.emplace_back(0.3, -0.5, 0.286002);
  pts.emplace_back(0.4, -0.5, 0.285392);
  pts.emplace_back(0.5, -0.5, 0.285062);
  pts.emplace_back(0.6, -0.5, 0.284574);
  pts.emplace_back(0.7, -0.5, 0.284491);
  pts.emplace_back(0.8, -0.5, 0.28472);
  pts.emplace_back(0.9, -0.5, 0.28445);
  pts.emplace_back(1, -0.5, 0.284758);
  pts.emplace_back(1.1, -0.5, 0.2848);
  pts.emplace_back(1.2, -0.5, 0.285052);
  pts.emplace_back(1.3, -0.5, 0.285817);
  pts.emplace_back(1.4, -0.5, 0.286438);
  pts.emplace_back(-0.5, -0.4, 0.291896);
  pts.emplace_back(-0.4, -0.4, 0.290786);
  pts.emplace_back(-0.3, -0.4, 0.289734);
  pts.emplace_back(-0.2, -0.4, 0.288658);
  pts.emplace_back(-0.1, -0.4, 0.287803);
  pts.emplace_back(0.0, -0.4, 0.286678);
  pts.emplace_back(0.1, -0.4, 0.286131);
  pts.emplace_back(0.2, -0.4, 0.285353);
  pts.emplace_back(0.3, -0.4, 0.284841);
  pts.emplace_back(0.4, -0.4, 0.284439);
  pts.emplace_back(0.5, -0.4, 0.283863);
  pts.emplace_back(0.6, -0.4, 0.283618);
  pts.emplace_back(0.7, -0.4, 0.283512);
  pts.emplace_back(0.8, -0.4, 0.283574);
  pts.emplace_back(0.9, -0.4, 0.283798);
  pts.emplace_back(1, -0.4, 0.283824);
  pts.emplace_back(1.1, -0.4, 0.284018);
  pts.emplace_back(1.2, -0.4, 0.284341);
  pts.emplace_back(1.3, -0.4, 0.284901);
  pts.emplace_back(1.4, -0.4, 0.285753);
  pts.emplace_back(-0.5, -0.3, 0.291097);
  pts.emplace_back(-0.4, -0.3, 0.289835);
  pts.emplace_back(-0.3, -0.3, 0.288775);
  pts.emplace_back(-0.2, -0.3, 0.287713);
  pts.emplace_back(-0.1, -0.3, 0.286809);
  pts.emplace_back(0.0, -0.3, 0.285914);
  pts.emplace_back(0.1, -0.3, 0.285087);
  pts.emplace_back(0.2, -0.3, 0.28477);
  pts.emplace_back(0.3, -0.3, 0.284185);
  pts.emplace_back(0.4, -0.3, 0.283735);
  pts.emplace_back(0.5, -0.3, 0.283226);
  pts.emplace_back(0.6, -0.3, 0.282904);
  pts.emplace_back(0.7, -0.3, 0.282882);
  pts.emplace_back(0.8, -0.3, 0.2829);
  pts.emplace_back(0.9, -0.3, 0.282873);
  pts.emplace_back(1, -0.3, 0.283007);
  pts.emplace_back(1.1, -0.3, 0.283184);
  pts.emplace_back(1.2, -0.3, 0.283769);
  pts.emplace_back(1.3, -0.3, 0.284392);
  pts.emplace_back(1.4, -0.3, 0.285051);
  pts.emplace_back(-0.5, -0.2, 0.290024);
  pts.emplace_back(-0.4, -0.2, 0.288805);
  pts.emplace_back(-0.3, -0.2, 0.287729);
  pts.emplace_back(-0.2, -0.2, 0.286842);
  pts.emplace_back(-0.1, -0.2, 0.285912);
  pts.emplace_back(0.0, -0.2, 0.28503);
  pts.emplace_back(0.1, -0.2, 0.284147);
  pts.emplace_back(0.2, -0.2, 0.283785);
  pts.emplace_back(0.3, -0.2, 0.283408);
  pts.emplace_back(0.4, -0.2, 0.282794);
  pts.emplace_back(0.5, -0.2, 0.282288);
  pts.emplace_back(0.6, -0.2, 0.282006);
  pts.emplace_back(0.7, -0.2, 0.281878);
  pts.emplace_back(0.8, -0.2, 0.282093);
  pts.emplace_back(0.9, -0.2, 0.282208);
  pts.emplace_back(1, -0.2, 0.28234);
  pts.emplace_back(1.1, -0.2, 0.282571);
  pts.emplace_back(1.2, -0.2, 0.28301);
  pts.emplace_back(1.3, -0.2, 0.283886);
  pts.emplace_back(1.4, -0.2, 0.284514);
  pts.emplace_back(-0.5, -0.1, 0.289103);
  pts.emplace_back(-0.4, -0.1, 0.288101);
  pts.emplace_back(-0.3, -0.1, 0.286765);
  pts.emplace_back(-0.2, -0.1, 0.286054);
  pts.emplace_back(-0.1, -0.1, 0.285041);
  pts.emplace_back(0.0, -0.1, 0.284275);
  pts.emplace_back(0.1, -0.1, 0.283566);
  pts.emplace_back(0.2, -0.1, 0.282851);
  pts.emplace_back(0.3, -0.1, 0.282406);
  pts.emplace_back(0.4, -0.1, 0.28207);
  pts.emplace_back(0.5, -0.1, 0.281568);
  pts.emplace_back(0.6, -0.1, 0.281322);
  pts.emplace_back(0.7, -0.1, 0.281231);
  pts.emplace_back(0.8, -0.1, 0.281267);
  pts.emplace_back(0.9, -0.1, 0.28149);
  pts.emplace_back(1, -0.1, 0.281595);
  pts.emplace_back(1.1, -0.1, 0.282096);
  pts.emplace_back(1.2, -0.1, 0.282608);
  pts.emplace_back(1.3, -0.1, 0.283261);
  pts.emplace_back(1.4, -0.1, 0.283892);
  pts.emplace_back(-0.5, 0.0, 0.288178);
  pts.emplace_back(-0.4, 0.0, 0.287051);
  pts.emplace_back(-0.3, 0.0, 0.285765);
  pts.emplace_back(-0.2, 0.0, 0.285333);
  pts.emplace_back(-0.1, 0.0, 0.284063);
  pts.emplace_back(0.0, 0.0, 0.28341);
  pts.emplace_back(0.1, 0.0, 0.282461);
  pts.emplace_back(0.2, 0.0, 0.282134);
  pts.emplace_back(0.3, 0.0, 0.281903);
  pts.emplace_back(0.4, 0.0, 0.281268);
  pts.emplace_back(0.5, 0.0, 0.280933);
  pts.emplace_back(0.6, 0.0, 0.280443);
  pts.emplace_back(0.7, 0.0, 0.280554);
  pts.emplace_back(0.8, 0.0, 0.280806);
  pts.emplace_back(0.9, 0.0, 0.280744);
  pts.emplace_back(1, 0.0, 0.281191);
  pts.emplace_back(1.1, 0.0, 0.281503);
  pts.emplace_back(1.2, 0.0, 0.281944);
  pts.emplace_back(1.3, 0.0, 0.282934);
  pts.emplace_back(1.4, 0.0, 0.283435);
  pts.emplace_back(-0.5, 0.1, 0.287581);
  pts.emplace_back(-0.4, 0.1, 0.286549);
  pts.emplace_back(-0.3, 0.1, 0.28551);
  pts.emplace_back(-0.2, 0.1, 0.284606);
  pts.emplace_back(-0.1, 0.1, 0.283677);
  pts.emplace_back(0.0, 0.1, 0.282876);
  pts.emplace_back(0.1, 0.1, 0.28224);
  pts.emplace_back(0.2, 0.1, 0.281825);
  pts.emplace_back(0.3, 0.1, 0.281511);
  pts.emplace_back(0.4, 0.1, 0.280922);
  pts.emplace_back(0.5, 0.1, 0.280532);
  pts.emplace_back(0.6, 0.1, 0.280291);
  pts.emplace_back(0.7, 0.1, 0.280356);
  pts.emplace_back(0.8, 0.1, 0.280418);
  pts.emplace_back(0.9, 0.1, 0.280568);
  pts.emplace_back(1, 0.1, 0.280956);
  pts.emplace_back(1.1, 0.1, 0.281411);
  pts.emplace_back(1.2, 0.1, 0.281878);
  pts.emplace_back(1.3, 0.1, 0.282531);
  pts.emplace_back(1.4, 0.1, 0.283313);
  pts.emplace_back(-0.5, 0.2, 0.287576);
  pts.emplace_back(-0.4, 0.2, 0.286246);
  pts.emplace_back(-0.3, 0.2, 0.285224);
  pts.emplace_back(-0.2, 0.2, 0.284359);
  pts.emplace_back(-0.1, 0.2, 0.283449);
  pts.emplace_back(0.0, 0.2, 0.28263);
  pts.emplace_back(0.1, 0.2, 0.281997);
  pts.emplace_back(0.2, 0.2, 0.281661);
  pts.emplace_back(0.3, 0.2, 0.281263);
  pts.emplace_back(0.4, 0.2, 0.280764);
  pts.emplace_back(0.5, 0.2, 0.280353);
  pts.emplace_back(0.6, 0.2, 0.280094);
  pts.emplace_back(0.7, 0.2, 0.280436);
  pts.emplace_back(0.8, 0.2, 0.280504);
  pts.emplace_back(0.9, 0.2, 0.280531);
  pts.emplace_back(1, 0.2, 0.280934);
  pts.emplace_back(1.1, 0.2, 0.281246);
  pts.emplace_back(1.2, 0.2, 0.282096);
  pts.emplace_back(1.3, 0.2, 0.282566);
  pts.emplace_back(1.4, 0.2, 0.283251);
  pts.emplace_back(-0.5, 0.3, 0.287066);
  pts.emplace_back(-0.4, 0.3, 0.285756);
  pts.emplace_back(-0.3, 0.3, 0.28477);
  pts.emplace_back(-0.2, 0.3, 0.283944);
  pts.emplace_back(-0.1, 0.3, 0.283053);
  pts.emplace_back(0.0, 0.3, 0.282394);
  pts.emplace_back(0.1, 0.3, 0.281603);
  pts.emplace_back(0.2, 0.3, 0.281087);
  pts.emplace_back(0.3, 0.3, 0.280849);
  pts.emplace_back(0.4, 0.3, 0.28056);
  pts.emplace_back(0.5, 0.3, 0.280175);
  pts.emplace_back(0.6, 0.3, 0.279959);
  pts.emplace_back(0.7, 0.3, 0.280089);
  pts.emplace_back(0.8, 0.3, 0.280237);
  pts.emplace_back(0.9, 0.3, 0.280511);
  pts.emplace_back(1, 0.3, 0.280809);
  pts.emplace_back(1.1, 0.3, 0.281169);
  pts.emplace_back(1.2, 0.3, 0.281781);
  pts.emplace_back(1.3, 0.3, 0.282418);
  pts.emplace_back(1.4, 0.3, 0.283335);
  pts.emplace_back(-0.5, 0.4, 0.286648);
  pts.emplace_back(-0.4, 0.4, 0.285593);
  pts.emplace_back(-0.3, 0.4, 0.284377);
  pts.emplace_back(-0.2, 0.4, 0.283637);
  pts.emplace_back(-0.1, 0.4, 0.28299);
  pts.emplace_back(0.0, 0.4, 0.282105);
  pts.emplace_back(0.1, 0.4, 0.281512);
  pts.emplace_back(0.2, 0.4, 0.280864);
  pts.emplace_back(0.3, 0.4, 0.280458);
  pts.emplace_back(0.4, 0.4, 0.280426);
  pts.emplace_back(0.5, 0.4, 0.280057);
  pts.emplace_back(0.6, 0.4, 0.280084);
  pts.emplace_back(0.7, 0.4, 0.279927);
  pts.emplace_back(0.8, 0.4, 0.280283);
  pts.emplace_back(0.9, 0.4, 0.28047);
  pts.emplace_back(1, 0.4, 0.280901);
  pts.emplace_back(1.1, 0.4, 0.281407);
  pts.emplace_back(1.2, 0.4, 0.281853);
  pts.emplace_back(1.3, 0.4, 0.282744);
  pts.emplace_back(1.4, 0.4, 0.28335);
  pts.emplace_back(-0.5, 0.5, 0.286295);
  pts.emplace_back(-0.4, 0.5, 0.285269);
  pts.emplace_back(-0.3, 0.5, 0.284209);
  pts.emplace_back(-0.2, 0.5, 0.283568);
  pts.emplace_back(-0.1, 0.5, 0.282478);
  pts.emplace_back(0.0, 0.5, 0.281741);
  pts.emplace_back(0.1, 0.5, 0.281168);
  pts.emplace_back(0.2, 0.5, 0.280716);
  pts.emplace_back(0.3, 0.5, 0.280745);
  pts.emplace_back(0.4, 0.5, 0.280095);
  pts.emplace_back(0.5, 0.5, 0.279952);
  pts.emplace_back(0.6, 0.5, 0.279727);
  pts.emplace_back(0.7, 0.5, 0.279979);
  pts.emplace_back(0.8, 0.5, 0.280331);
  pts.emplace_back(0.9, 0.5, 0.280499);
  pts.emplace_back(1, 0.5, 0.281021);
  pts.emplace_back(1.1, 0.5, 0.281563);
  pts.emplace_back(1.2, 0.5, 0.281921);
  pts.emplace_back(1.3, 0.5, 0.282863);
  pts.emplace_back(1.4, 0.5, 0.283477);
  pts.emplace_back(-0.5, 0.6, 0.286423);
  pts.emplace_back(-0.4, 0.6, 0.285262);
  pts.emplace_back(-0.3, 0.6, 0.28443);
  pts.emplace_back(-0.2, 0.6, 0.28355);
  pts.emplace_back(-0.1, 0.6, 0.282803);
  pts.emplace_back(0.0, 0.6, 0.282149);
  pts.emplace_back(0.1, 0.6, 0.281254);
  pts.emplace_back(0.2, 0.6, 0.280967);
  pts.emplace_back(0.3, 0.6, 0.28077);
  pts.emplace_back(0.4, 0.6, 0.280213);
  pts.emplace_back(0.5, 0.6, 0.280201);
  pts.emplace_back(0.6, 0.6, 0.280063);
  pts.emplace_back(0.7, 0.6, 0.280252);
  pts.emplace_back(0.8, 0.6, 0.280555);
  pts.emplace_back(0.9, 0.6, 0.280768);
  pts.emplace_back(1, 0.6, 0.281267);
  pts.emplace_back(1.1, 0.6, 0.281888);
  pts.emplace_back(1.2, 0.6, 0.282529);
  pts.emplace_back(1.3, 0.6, 0.283301);
  pts.emplace_back(1.4, 0.6, 0.283955);
  pts.emplace_back(-0.5, 0.7, 0.286759);
  pts.emplace_back(-0.4, 0.7, 0.285661);
  pts.emplace_back(-0.3, 0.7, 0.284859);
  pts.emplace_back(-0.2, 0.7, 0.283953);
  pts.emplace_back(-0.1, 0.7, 0.283162);
  pts.emplace_back(0.0, 0.7, 0.28225);
  pts.emplace_back(0.1, 0.7, 0.28185);
  pts.emplace_back(0.2, 0.7, 0.281422);
  pts.emplace_back(0.3, 0.7, 0.281056);
  pts.emplace_back(0.4, 0.7, 0.280791);
  pts.emplace_back(0.5, 0.7, 0.28061);
  pts.emplace_back(0.6, 0.7, 0.280673);
  pts.emplace_back(0.7, 0.7, 0.280871);
  pts.emplace_back(0.8, 0.7, 0.281144);
  pts.emplace_back(0.9, 0.7, 0.281433);
  pts.emplace_back(1, 0.7, 0.282064);
  pts.emplace_back(1.1, 0.7, 0.282663);
  pts.emplace_back(1.2, 0.7, 0.283468);
  pts.emplace_back(1.3, 0.7, 0.284094);
  pts.emplace_back(1.4, 0.7, 0.284706);
  pts.emplace_back(-0.5, 0.8, 0.287119);
  pts.emplace_back(-0.4, 0.8, 0.286006);
  pts.emplace_back(-0.3, 0.8, 0.285063);
  pts.emplace_back(-0.2, 0.8, 0.284372);
  pts.emplace_back(-0.1, 0.8, 0.283659);
  pts.emplace_back(0.0, 0.8, 0.282923);
  pts.emplace_back(0.1, 0.8, 0.282194);
  pts.emplace_back(0.2, 0.8, 0.281719);
  pts.emplace_back(0.3, 0.8, 0.281597);
  pts.emplace_back(0.4, 0.8, 0.281302);
  pts.emplace_back(0.5, 0.8, 0.281324);
  pts.emplace_back(0.6, 0.8, 0.281181);
  pts.emplace_back(0.7, 0.8, 0.281291);
  pts.emplace_back(0.8, 0.8, 0.281785);
  pts.emplace_back(0.9, 0.8, 0.282108);
  pts.emplace_back(1, 0.8, 0.282462);
  pts.emplace_back(1.1, 0.8, 0.283256);
  pts.emplace_back(1.2, 0.8, 0.283913);
  pts.emplace_back(1.3, 0.8, 0.284673);
  pts.emplace_back(1.4, 0.8, 0.285532);
  pts.emplace_back(-0.5, 0.9, 0.287555);
  pts.emplace_back(-0.4, 0.9, 0.286686);
  pts.emplace_back(-0.3, 0.9, 0.285509);
  pts.emplace_back(-0.2, 0.9, 0.284652);
  pts.emplace_back(-0.1, 0.9, 0.284068);
  pts.emplace_back(0.0, 0.9, 0.283225);
  pts.emplace_back(0.1, 0.9, 0.282715);
  pts.emplace_back(0.2, 0.9, 0.282156);
  pts.emplace_back(0.3, 0.9, 0.281997);
  pts.emplace_back(0.4, 0.9, 0.281985);
  pts.emplace_back(0.5, 0.9, 0.281766);
  pts.emplace_back(0.6, 0.9, 0.28191);
  pts.emplace_back(0.7, 0.9, 0.281907);
  pts.emplace_back(0.8, 0.9, 0.282343);
  pts.emplace_back(0.9, 0.9, 0.282738);
  pts.emplace_back(1, 0.9, 0.283173);
  pts.emplace_back(1.1, 0.9, 0.283829);
  pts.emplace_back(1.2, 0.9, 0.284438);
  pts.emplace_back(1.3, 0.9, 0.285333);
  pts.emplace_back(1.4, 0.9, 0.28608);
  pts.emplace_back(-0.5, 1, 0.28808);
  pts.emplace_back(-0.4, 1, 0.286996);
  pts.emplace_back(-0.3, 1, 0.286172);
  pts.emplace_back(-0.2, 1, 0.285551);
  pts.emplace_back(-0.1, 1, 0.284339);
  pts.emplace_back(0.0, 1, 0.283857);
  pts.emplace_back(0.1, 1, 0.283146);
  pts.emplace_back(0.2, 1, 0.282828);
  pts.emplace_back(0.3, 1, 0.282928);
  pts.emplace_back(0.4, 1, 0.282448);
  pts.emplace_back(0.5, 1, 0.282498);
  pts.emplace_back(0.6, 1, 0.28245);
  pts.emplace_back(0.7, 1, 0.282712);
  pts.emplace_back(0.8, 1, 0.28328);
  pts.emplace_back(0.9, 1, 0.283363);
  pts.emplace_back(1, 1, 0.283829);
  pts.emplace_back(1.1, 1, 0.284362);
  pts.emplace_back(1.2, 1, 0.285165);
  pts.emplace_back(1.3, 1, 0.286107);
  pts.emplace_back(1.4, 1, 0.286721);
  pts.emplace_back(-0.5, 1.1, 0.288913);
  pts.emplace_back(-0.4, 1.1, 0.288147);
  pts.emplace_back(-0.3, 1.1, 0.287084);
  pts.emplace_back(-0.2, 1.1, 0.286301);
  pts.emplace_back(-0.1, 1.1, 0.285359);
  pts.emplace_back(0.0, 1.1, 0.284524);
  pts.emplace_back(0.1, 1.1, 0.283987);
  pts.emplace_back(0.2, 1.1, 0.283926);
  pts.emplace_back(0.3, 1.1, 0.283705);
  pts.emplace_back(0.4, 1.1, 0.283569);
  pts.emplace_back(0.5, 1.1, 0.283598);
  pts.emplace_back(0.6, 1.1, 0.283447);
  pts.emplace_back(0.7, 1.1, 0.283829);
  pts.emplace_back(0.8, 1.1, 0.284257);
  pts.emplace_back(0.9, 1.1, 0.284614);
  pts.emplace_back(1, 1.1, 0.285);
  pts.emplace_back(1.1, 1.1, 0.285558);
  pts.emplace_back(1.2, 1.1, 0.286187);
  pts.emplace_back(1.3, 1.1, 0.2871);
  pts.emplace_back(1.4, 1.1, 0.287945);
  pts.emplace_back(-0.5, 1.2, 0.289935);
  pts.emplace_back(-0.4, 1.2, 0.289057);
  pts.emplace_back(-0.3, 1.2, 0.288171);
  pts.emplace_back(-0.2, 1.2, 0.287253);
  pts.emplace_back(-0.1, 1.2, 0.286476);
  pts.emplace_back(0.0, 1.2, 0.285807);
  pts.emplace_back(0.1, 1.2, 0.285517);
  pts.emplace_back(0.2, 1.2, 0.285359);
  pts.emplace_back(0.3, 1.2, 0.284987);
  pts.emplace_back(0.4, 1.2, 0.285074);
  pts.emplace_back(0.5, 1.2, 0.285044);
  pts.emplace_back(0.6, 1.2, 0.284985);
  pts.emplace_back(0.7, 1.2, 0.285303);
  pts.emplace_back(0.8, 1.2, 0.285491);
  pts.emplace_back(0.9, 1.2, 0.28594);
  pts.emplace_back(1, 1.2, 0.286358);
  pts.emplace_back(1.1, 1.2, 0.286956);
  pts.emplace_back(1.2, 1.2, 0.287596);
  pts.emplace_back(1.3, 1.2, 0.288482);
  pts.emplace_back(1.4, 1.2, 0.289202);
  pts.emplace_back(-0.5, 1.3, 0.290931);
  pts.emplace_back(-0.4, 1.3, 0.289712);
  pts.emplace_back(-0.3, 1.3, 0.289169);
  pts.emplace_back(-0.2, 1.3, 0.288358);
  pts.emplace_back(-0.1, 1.3, 0.287687);
  pts.emplace_back(0.0, 1.3, 0.28714);
  pts.emplace_back(0.1, 1.3, 0.286824);
  pts.emplace_back(0.2, 1.3, 0.286648);
  pts.emplace_back(0.3, 1.3, 0.286387);
  pts.emplace_back(0.4, 1.3, 0.286364);
  pts.emplace_back(0.5, 1.3, 0.286364);
  pts.emplace_back(0.6, 1.3, 0.286321);
  pts.emplace_back(0.7, 1.3, 0.286497);
  pts.emplace_back(0.8, 1.3, 0.286903);
  pts.emplace_back(0.9, 1.3, 0.287293);
  pts.emplace_back(1, 1.3, 0.287669);
  pts.emplace_back(1.1, 1.3, 0.28827);
  pts.emplace_back(1.2, 1.3, 0.288908);
  pts.emplace_back(1.3, 1.3, 0.289395);
  pts.emplace_back(1.4, 1.3, 0.290513);
  pts.emplace_back(-0.5, 1.4, 0.291937);
  pts.emplace_back(-0.4, 1.4, 0.290964);
  pts.emplace_back(-0.3, 1.4, 0.29022);
  pts.emplace_back(-0.2, 1.4, 0.289509);
  pts.emplace_back(-0.1, 1.4, 0.288959);
  pts.emplace_back(0.0, 1.4, 0.288364);
  pts.emplace_back(0.1, 1.4, 0.288155);
  pts.emplace_back(0.2, 1.4, 0.2878);
  pts.emplace_back(0.3, 1.4, 0.287783);
  pts.emplace_back(0.4, 1.4, 0.28762);
  pts.emplace_back(0.5, 1.4, 0.287788);
  pts.emplace_back(0.6, 1.4, 0.287655);
  pts.emplace_back(0.7, 1.4, 0.287767);
  pts.emplace_back(0.8, 1.4, 0.288211);
  pts.emplace_back(0.9, 1.4, 0.288732);
  pts.emplace_back(1, 1.4, 0.289026);
  pts.emplace_back(1.1, 1.4, 0.289573);
  pts.emplace_back(1.2, 1.4, 0.29023);
  pts.emplace_back(1.3, 1.4, 0.29115);
  pts.emplace_back(1.4, 1.4, 0.291991);
  vgl_fit_xy_paraboloid_3d<double> fxy(pts);
  double zerr = fxy.fit_linear(&std::cout);
  std::cout << fxy.quadric_type() << std::endl;
  TEST("quadric type - linearly fit paraboloid - actual cost surface", fxy.quadric_type(), "elliptic_paraboloid");
  TEST_NEAR("z error - linearly fit paraboloid - actual cost surface", zerr, 0.0, 1.0e-3);
  vgl_point_2d<double> etr = fxy.extremum_point();
  vgl_point_2d<double> actual_extr(0.63, 0.345);
  double extr_error = (etr - actual_extr).length();
  TEST_NEAR("compute paraboloid extremum - actual cost surface", extr_error, 0.0, 1.0e-3);
}
static void
test_fit_quadric_3d()
{
  test_linear_fit_sphere_taubin();
  test_linear_fit_taubin_sphere_rand_error();
  test_linear_fit_Taubin_parabolid();
  test_linear_fit_Taubin_parabolid_rand_error();
  test_linear_fit_parabolic_cylinder_Taubin();
  test_linear_fit_sphere_Taubin_actual_pts();
  test_linear_fit_sphere_Alliare();
  test_linear_fit_Allaire_sphere_rand_error();
  test_linear_fit_Allaire_saddle_shape();
  test_linear_fit_Allaire_thin_ellipsoid_rand_error();
  test_linear_fit_Allaire_ellipsoid_actual_pts();
  test_linear_fit_xy_paraboloid();
  test_linear_paraboloid_actual_cost_surface();
}

TESTMAIN(test_fit_quadric_3d);
