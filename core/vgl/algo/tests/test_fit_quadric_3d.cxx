// Some tests for vgl_fit_sphere_3d
// J.L. Mundy, July 7, 2017.
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_fit_quadric_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_pointset_3d.h>
#include <vgl/vgl_distance.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_generalized_eigensystem.h>
#include <vnl/algo/vnl_svd.h>
//
// creates points on the unit sphere
static void test_linear_fit_sphere_taubin()
{
  std::cout << "\n======== test_linear_fit_Taubin_sphere ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
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
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit Taubin perfect unit sphere", error, 0.0, 1e-6);
}
static void test_linear_fit_taubin_sphere_rand_error()
{
  std::cout << "\n======== test_linear_fit_Taubin_sphere_rand_error ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
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
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  // add random error (+-10%) to the points
  vnl_random ran;
  double e = 0.1;
  std::cout << "error " << e << std::endl;
  for(auto & pt : pts){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit sphere with error", error, 0.0, 0.1);
}
static void test_linear_fit_Taubin_parabolid()
{
  std::cout << "\n======== test_linear_fit_Allaire_paraboloid ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
  vnl_matrix<double> P(3,12);// to allow for rotation
  P[0][0]=1.0;    P[1][0]=1.0;     P[2][0]=2.0;
  P[0][1]=2.0;    P[1][1]=2.0;     P[2][1]=8.0;
  P[0][2]=3.0;    P[1][2]=3.0;     P[2][2]=18.0;
  P[0][3]=-1.0;   P[1][3]=-1.0;   P[2][3]=2.0;
  P[0][4]=-2.0;   P[1][4]=-2.0;   P[2][4]=8.0;
  P[0][5]=-3.0;   P[1][5]=-3.0;   P[2][5]=18.0;
  P[0][6]= 1.0;   P[1][6]=-1.0;   P[2][6]=2.0;
  P[0][7]= 2.0;   P[1][7]=-2.0;   P[2][7]=8.0;
  P[0][8]= 3.0;   P[1][8]=-3.0;   P[2][8]=18.0;
  P[0][9]= -1.0;  P[1][9]=1.0;   P[2][9]=2.0;
  P[0][10]= -2.0; P[1][10]=2.0; P[2][10]=8.0;
  P[0][11]= -3.0; P[1][11]=3.0; P[2][11]=18.0;

  double th = 1.047197551;
  //double th = 0.0;
  vgl_vector_3d<double> off(0.1, 0.2, 0.3);
  double c = cos(th), s = sin(th);
  vnl_matrix<double> R(3, 3), RP;
  R[0][0] = c;   R[0][1] = 0.0; R[0][2]=-s;
  R[1][0] = 0.0; R[1][1] = 1.0; R[1][2]=0.0;
  R[2][0] = s;   R[2][1] = 0.0; R[2][2]=c;
  RP = R * P;
  for(size_t i = 0; i< 11; ++i){
    vgl_point_3d<double> p(RP[0][i], RP[1][i], RP[2][i]);
    pts.push_back(p+off);
    std::cout << p+off << std::endl;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);

  vgl_quadric_3d<double> q = fit_parab.quadric_Taubin_fit();
  std::vector<std::vector<double> > Qo = q.coef_matrix();
  std::cout << "Qo" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << Qo[r][0] << ' '<< Qo[r][1] << ' '<< Qo[r][2]<< ' '<< Qo[r][3] << std::endl;
  std::vector<std::vector<double> > H;
   std::vector<std::vector<double> > Q = q.canonical_quadric(H);

   std::cout << "Q" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << Q[r][0] << ' '<< Q[r][1] << ' '<< Q[r][2]<< ' '<< Q[r][3] << std::endl;
  TEST_NEAR("linear fit parabolid", lin_error, 0.0, 0.25);
  // check inverse transform
  vnl_matrix<double> Qc(4, 4, 0.0), T(4,4,0.0);
  for(size_t r = 0; r<4 ; ++r)
    for(size_t c = 0; c<4 ; ++c){
      Qc[r][c] = Q[r][c];
      T[r][c] = H[r][c];
    }
  std::cout << "H" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << T[r][0] << ' '<< T[r][1] << ' '<< T[r][2]<< ' '<< T[r][3] << std::endl;
  vnl_svd<double> svd(T);
  vnl_matrix<double> Hi = svd.inverse();
  vnl_matrix<double> Qi =  Hi.transpose() * Qc * Hi;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << Qi[r][0] << ' '<< Qi[r][1] << ' '<< Qi[r][2]<< ' '<< Qi[r][3] << std::endl;
  double qdiff = 0.0;
  for(size_t r = 0; r<4; ++r)
   for(size_t c = 0; c<4; ++c)
           qdiff += fabs(Qi[r][c]-Qo[r][c]);
  qdiff /= 16.0;
  TEST_NEAR("transform canonical frame paraboloid", qdiff, 0.0, 1.0e-6);
}
static void test_linear_fit_Taubin_parabolid_rand_error()
{
  std::cout << "\n======== test_linear_fit_Allaire_paraboloid_rand_error ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
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
  pts.push_back(p0); pts.push_back(p1); pts.push_back(p2); pts.push_back(p3);
  pts.push_back(p4); pts.push_back(p5); pts.push_back(p6); pts.push_back(p7);
  pts.push_back(p8); pts.push_back(p9); pts.push_back(p10); pts.push_back(p11);
  // add random error to the points
  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for(auto & pt : pts){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit parabolid with noise", lin_error, 0.0, 0.25);
}
static void test_linear_fit_parabolic_cylinder_Taubin(){
  std::cout << "\n======== test_linear_fit_Allaire_parabolic_cylinder ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
  //ax^2 + z = 0
  vnl_matrix<double> P(3,12);// to allow for rotation
  P[0][0]=0.0;    P[1][0]=1.0;     P[2][0]=0.0;
  P[0][1]=0.5;    P[1][1]=2.0;     P[2][1]=0.25;
  P[0][2]=1.0;    P[1][2]=3.0;     P[2][2]=1.0;
  P[0][3]=1.5;   P[1][3]=4.0;   P[2][3]=2.25;
  P[0][4]=2.0;   P[1][4]=5.0;   P[2][4]=4.0;
  P[0][5]=2.5;   P[1][5]=-1.0;   P[2][5]=6.25;
  P[0][6]=-0.5;   P[1][6]=-2.0;   P[2][6]=0.25;
  P[0][7]=-1.0;   P[1][7]=-3.0;   P[2][7]=1.0;
  P[0][8]=-1.5;   P[1][8]=-4.0;   P[2][8]=2.25;
  P[0][9]=-2.0;  P[1][9]=-5.0;   P[2][9]=4.0;
  P[0][10]=-2.5; P[1][10]=-0.5; P[2][10]=6.25;
  P[0][11]=-3.0; P[1][11]=0.5; P[2][11]=9.0;

  double th = 1.047197551;
  //double th = 0.0;
  vgl_vector_3d<double> off(0.1, 0.2, 0.3);
  //vgl_vector_3d<double> off(0.0, 0.0, 0.0);
  double c = cos(th), s = sin(th);
  vnl_matrix<double> R(3, 3), RP;
  R[0][0] = c;   R[0][1] = 0.0; R[0][2]=-s;
  R[1][0] = 0.0; R[1][1] = 1.0; R[1][2]=0.0;
  R[2][0] = s;   R[2][1] = 0.0; R[2][2]=c;
  RP = R * P;
  for(size_t i = 0; i< 11; ++i){
    vgl_point_3d<double> p(RP[0][i], RP[1][i], RP[2][i]);
    pts.push_back(p+off);
    std::cout << p+off << std::endl;
  }
  vgl_fit_quadric_3d<double> fit_parab(pts);
  double lin_error = fit_parab.fit_linear_Taubin(&std::cout);
  vgl_quadric_3d<double> q = fit_parab.quadric_Taubin_fit();
  std::cout << "Fit error = " << lin_error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
  TEST_NEAR("linear fit parabolic cylinder", lin_error, 0.0, 0.25);

   std::vector<std::vector<double> > Qo = q.coef_matrix();
  std::cout << "Qo" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << Qo[r][0] << ' '<< Qo[r][1] << ' '<< Qo[r][2]<< ' '<< Qo[r][3] << std::endl;
  std::vector<std::vector<double> > H;
   std::vector<std::vector<double> > Q = q.canonical_quadric(H);

   std::cout << "Q" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << Q[r][0] << ' '<< Q[r][1] << ' '<< Q[r][2]<< ' '<< Q[r][3] << std::endl;

  // check inverse transform
  vnl_matrix<double> Qc(4, 4, 0.0), T(4,4,0.0);
  for(size_t r = 0; r<4 ; ++r)
    for(size_t c = 0; c<4 ; ++c){
      Qc[r][c] = Q[r][c];
      T[r][c] = H[r][c];
    }
  std::cout << "H" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << T[r][0] << ' '<< T[r][1] << ' '<< T[r][2]<< ' '<< T[r][3] << std::endl;
  vnl_svd<double> svd(T);
  vnl_matrix<double> Hi = svd.inverse();
  vnl_matrix<double> Qi =  Hi.transpose() * Qc * Hi;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << Qi[r][0] << ' '<< Qi[r][1] << ' '<< Qi[r][2]<< ' '<< Qi[r][3] << std::endl;
  double qdiff = 0.0;
  for(size_t r = 0; r<4; ++r)
   for(size_t c = 0; c<4; ++c)
           qdiff += fabs(Qi[r][c]-Qo[r][c]);
  qdiff /= 16.0;
  TEST_NEAR("transform canonical frame parabolic cylinder", qdiff, 0.0, 1.0e-6);
}

static void test_linear_fit_sphere_Taubin_actual_pts()
{
  std::cout << "\n======== test_linear_fit_Allaire_sphere_actual_pts ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
  vgl_point_3d<double> p0(-10.356499671936,68.164497375488,48.227199554443);
  vgl_point_3d<double> p1(-12.030300140381,68.351600646973,46.570499420166);
  vgl_point_3d<double> p2(-12.924599647522,70.128097534180,46.037700653076);
  vgl_point_3d<double> p3(-12.602700233459,72.161201477051,47.159500122070);
  vgl_point_3d<double> p4(-11.404299736023,75.049201965332,49.112400054932);
  vgl_point_3d<double> p5(-9.647660255432,71.332901000977,49.860500335693);
  vgl_point_3d<double> p6(-13.605299949646,72.556297302246,46.091999053955);
  vgl_point_3d<double> p7(-15.126000404358,71.693496704102,43.016601562500);
  vgl_point_3d<double> p8(-16.063600540161,70.053901672363,41.029098510742);
  vgl_point_3d<double> p9(-13.483099937439,68.981697082520,44.901100158691);
  vgl_point_3d<double> p10(-9.131649971008,69.801300048828,49.823600769043);
  vgl_point_3d<double> p11(-8.480529785156,67.906501770020,50.200801849365);
  vgl_point_3d<double> p12(4.875539779663,71.071899414063,50.690101623535);
  vgl_point_3d<double> p13(-3.617100000381,67.346603393555,51.243801116943);
  vgl_point_3d<double> p14(-4.381750106812,68.257698059082,51.418098449707);
  vgl_point_3d<double> p15(-5.963240146637,68.928100585938,51.265399932861);
  vgl_point_3d<double> p16(3.054660081863,68.043899536133,51.342098236084);
  vgl_point_3d<double> p17(-11.062999725342,72.394500732422,48.930099487305);
  vgl_point_3d<double> p18(-11.003100395203,70.369201660156,48.363498687744);
  vgl_point_3d<double> p19(5.466730117798,69.970596313477,50.311599731445);
  vgl_point_3d<double> p20(-11.655500411987,71.570999145508,48.103099822998);
  vgl_point_3d<double> p21(-10.344300270081,72.406997680664,49.586498260498);
  vgl_point_3d<double> p22(-11.970600128174,72.936203002930,48.114101409912);
  vgl_point_3d<double> p23(-13.019499778748,73.698402404785,47.480598449707);
  vgl_point_3d<double> p24(4.039299964905,69.958900451660,50.850700378418);
  pts.push_back(p0); pts.push_back(p1); pts.push_back(p2); pts.push_back(p3);
  pts.push_back(p4); pts.push_back(p5); pts.push_back(p6); pts.push_back(p7);
  pts.push_back(p8); pts.push_back(p9); pts.push_back(p10); pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13); pts.push_back(p14); pts.push_back(p15);
  pts.push_back(p16); pts.push_back(p17); pts.push_back(p18); pts.push_back(p19);
  pts.push_back(p20); pts.push_back(p21); pts.push_back(p22); pts.push_back(p23);
  pts.push_back(p24);
  vgl_fit_quadric_3d<double> fit_sph_act(pts);
  double lin_error = fit_sph_act.fit_linear_Taubin(&std::cout);
  TEST_NEAR("linear fit sphere sclara pts", lin_error, 0.0, 0.25);
}
static void test_linear_fit_sphere_Alliare(){
  std::cout << "\n======== test_linear_fit_Allaire_sphere ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
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
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire perfect unit sphere", error, 0.0, 1e-6);
}
static void test_linear_fit_Allaire_sphere_rand_error()
{
  std::cout << "\n======== test_linear_fit_Allaire_sphere_rand_error ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
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
  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);
  vnl_random ran;
  double e = 0.5;
  std::cout << "error " << e << std::endl;
  for(auto & pt : pts){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  TEST_NEAR("linear fit Allaire sphere with error", error, 0.0, 0.5);
}
static void test_linear_fit_Allaire_thin_ellipsoid_rand_error()
{
  std::cout << "\n======== test_linear_fit_Allaire_thin_ellipsoid_rand_error ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
  double sx = 10.0, sy = 2.0, sz = 1.0;
  vnl_matrix<double> P(3,14);
  P[0][0]=sx; P[1][0]=0.0; P[2][0]=0.0;
  P[0][1]=0.0; P[1][1]=sy; P[2][1]=0.0;
  P[0][2]=0.0; P[1][2]=0.0; P[2][2]=sz;
  P[0][3]=-sx; P[1][3]=0.0; P[2][3]=0.0;
  P[0][4]=0.0; P[1][4]=-sy; P[2][4]=0.0;
  P[0][5]=0.0; P[1][5]=0.0; P[2][5]=-sz;
  double r = 0.5773503;
  P[0][6]=sx*r; P[1][6]=sy*r; P[2][6]=sz*r;
  P[0][7]=-sx*r; P[1][7]=-sy*r; P[2][7]=-sz*r;
  P[0][8]=-sx*r; P[1][8]=-sy*r; P[2][8]=sz*r;
  P[0][9]=-sx*r; P[1][9]= sy*r; P[2][9]=-sz*r;
  P[0][10]=sx*r; P[1][10]= -sy*r; P[2][10]=-sz*r;
  P[0][11]=sx*r; P[1][11]= sy*r; P[2][11]=-sz*r;
  P[0][12]=sx*r; P[1][12]= -sy*r; P[2][12]=sz*r;
  P[0][13]=-sx*r; P[1][13]= sy*r; P[2][13]=sz*r;

  double th = 1.047197551;
  vgl_vector_3d<double> off(1.0, 2.0, 3.0);
  double c = cos(th), s = sin(th);
  vnl_matrix<double> R(3, 3), RP;
  R[0][0] = c;   R[0][1] = 0.0; R[0][2]=-s;
  R[1][0] = 0.0; R[1][1] = 1.0; R[1][2]=0.0;
  R[2][0] = s;   R[2][1] = 0.0; R[2][2]=c;
  RP = R * P;
  for(size_t i = 0; i< 13; ++i){
    vgl_point_3d<double> p(RP[0][i], RP[1][i], RP[2][i]);
    pts.push_back(p+off);
    std::cout << p+off << std::endl;
  }
  vnl_random ran;
  double e = 0.0;
  std::cout << "error " << e << std::endl;
  for(auto & pt : pts){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sph(pts);
  double error = fit_sph.fit_ellipsoid_linear_Allaire(&std::cout);
  vgl_quadric_3d<double>& q = fit_sph.quadric_Allaire_fit();
  std::cout << "Fit error = " << error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
  TEST_NEAR("linear fit Allaire thin ellipsoid with error", error, 0.0, 0.5);

  std::vector<double> diag;
  std::vector<std::vector<double> > H;

  q.canonical_central_quadric(diag, H);
  std::cout << "canonical q diag[ " << diag[0] << ' ' << diag[1] << ' ' <<  diag[2] << ' ' << diag[3] << " ]" << std::endl;
  std::cout << "H" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << H[r][0] << ' '<< H[r][1] << ' '<< H[r][2]<< ' '<< H[r][3] << std::endl;

  vnl_matrix<double> Hf(4,4), D(4,4, 0.0);
  for(size_t r = 0; r<4; ++r){
    for(size_t c=0; c<4; ++c){
      Hf[r][c] = H[r][c];
        }
        D[r][r] = diag[r];
  }
  vnl_svd<double> svd(Hf);
  vnl_matrix<double> Hr = svd.inverse();

  vnl_matrix<double> Qr = Hr.transpose() * D * Hr;

  std::vector<std::vector<double> > Qo = q.coef_matrix();
  double dif = 0.0;
  for(size_t r = 0; r<4; ++r)
    for(size_t c=0; c<4; ++c)
      dif += fabs(Qo[r][c] - Qr[r][c]);
  dif/=16;
  TEST_NEAR("Allaire thin ellipsoid reconstruct dif", dif, 0.0, 1e-06);
}
static void test_linear_fit_Allaire_saddle_shape()
{
  std::cout << "\n======== test_linear_fit_Allaire_saddle_shape ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
   vgl_point_3d<double> p0(1.0, 0.0, 0.0);
  vgl_point_3d<double> p1(0.0, 1.0, 0.0);
  vgl_point_3d<double> p2(-1.0, 0.0, 0.0);
  vgl_point_3d<double> p3(0.0, -1.0, 0.0);
  vgl_point_3d<double> p4(2, 0.0, 1.0);
  vgl_point_3d<double> p5(0.0, 2.0,1.0);
  vgl_point_3d<double> p6(-2.0, 0.0,1.0);
  vgl_point_3d<double> p7(0.0, -2.0,1.0);
  vgl_point_3d<double> p8(2, 0.0, -1.0);
  vgl_point_3d<double> p9(0.0, 2.0,-1.0);
  vgl_point_3d<double> p10(-2.0, 0.0,-1.0);
  vgl_point_3d<double> p11(0.0, -2.0,-1.0);


  pts.push_back(p0);  pts.push_back(p1);  pts.push_back(p2);
  pts.push_back(p3);  pts.push_back(p4);  pts.push_back(p5);
  pts.push_back(p6);  pts.push_back(p7);  pts.push_back(p8);
  pts.push_back(p9);  pts.push_back(p10);  pts.push_back(p11);


  vnl_random ran;
  double e = 0.0;
  std::cout << "error " << e << std::endl;
  for(auto & pt : pts){
    vgl_vector_3d<double> er(ran.drand32(-e, e), ran.drand32(-e, e), ran.drand32(-e, e));
    pt = pt + er;
  }
  vgl_fit_quadric_3d<double> fit_sad(pts);
  double error = fit_sad.fit_saddle_shaped_quadric_linear_Allaire(&std::cout);
  vgl_quadric_3d<double>& q = fit_sad.quadric_Allaire_fit();
  std::cout << "Fit error = " << error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
  std::vector<double> diag;
  std::vector<std::vector<double> > H;
  q.canonical_central_quadric(diag, H);
  std::cout << "canonical q diag[ " << diag[0] << ' ' << diag[1] << ' ' <<  diag[2] << ' ' << diag[3] << " ]" << std::endl;
   std::cout << "H" << std::endl;
  for(size_t r = 0; r<4 ; ++r)
    std::cout << H[r][0] << ' '<< H[r][1] << ' '<< H[r][2]<< ' '<< H[r][3] << std::endl;
  TEST_NEAR("linear fit Allaire saddle", error, 0.0, 0.5);
}
static void test_linear_fit_Allaire_ellipsoid_actual_pts()
{
  std::cout << "\n======== test_linear_fit_Allaire_ellipsoid_actual_pts ===== " <<std::endl;
  std::vector<vgl_point_3d<double> > pts;
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
  pts.push_back(p0); pts.push_back(p1);pts.push_back(p2);pts.push_back(p3);
  pts.push_back(p4); pts.push_back(p5);pts.push_back(p6);pts.push_back(p7);
  pts.push_back(p8); pts.push_back(p9);pts.push_back(p10);pts.push_back(p11);
  pts.push_back(p12); pts.push_back(p13);pts.push_back(p14);pts.push_back(p15);
  pts.push_back(p15); pts.push_back(p17);pts.push_back(p18);

  vgl_fit_quadric_3d<double> fit_ellp(pts);
  double error = fit_ellp.fit_ellipsoid_linear_Allaire(&std::cout);
  if(error != -1.0){
    vgl_quadric_3d<double>& q = fit_ellp.quadric_Allaire_fit();
    std::cout << "Fit error = " << error << " Quadric type " << q.type_by_number(q.type()) << std::endl;
    std::vector<double> diag;
    std::vector<std::vector<double> > H;
    q.canonical_central_quadric(diag, H);
    std::vector<std::vector<double > > Q = q.coef_matrix();
    vnl_matrix<double> Qn(4, 4), D(4,4,0.0);
    for(size_t r = 0; r<4; ++r)
      for(size_t c = 0; c<4; ++c)
        Qn[r][c] = Q[r][c];
    std::cout << "canonical q diag[ " << diag[0] << ' ' << diag[1] << ' ' <<  diag[2] << ' ' << diag[3] << " ]" << std::endl;
    for(size_t i = 0; i<4; ++i)
      D[i][i] = diag[i];
    vnl_matrix<double> Hf(4,4);
    for(size_t r = 0; r<4; ++r)
      for(size_t c=0; c<4; ++c)
        Hf[r][c] = H[r][c];
    vnl_svd<double> svd(Hf);
    vnl_matrix<double> Hr = svd.inverse();
    vnl_matrix<double> Qo = Hr.transpose() * D * Hr;
    std::cout << "Original dem quadric\n" << Qn << std::endl;
    std::cout << "Reconstructed dem quadric\n" << Qo << std::endl;
    double dif = 0.0;
    for(size_t r = 0; r<4; ++r)
      for(size_t c=0; c<4; ++c)
        dif += fabs(Qn[r][c] - Qo[r][c]);
    dif/=16;
    TEST_NEAR("reconstruct quadric in original frame", dif, 0.0, 1.0e-6);
  }else std::cout << "FAIL" << std::endl;
}
static void test_fit_quadric_3d()
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
}

TESTMAIN(test_fit_quadric_3d);
