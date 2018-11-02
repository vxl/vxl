// Some tests for vgl_affine_coordinates
// J.L. Mundy Jan 31, 2016

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <testlib/testlib_test.h>
#include <vgl/vgl_affine_coordinates.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_pointset_3d.h>
static double rnd(double r){
  double b = (2.0*r*double(rand())/RAND_MAX) - r;
  return b;
}
static vgl_vector_2d<double>  rand_vector(double r){
  return {rnd(r), rnd(r)};
}
static vgl_point_3d<double> trans_3d(std::vector<std::vector<double> > const&  T,
                                     std::vector<double> const& t, vgl_point_3d<double> const& P){
  double px = T[0][0]*P.x() +  T[0][1]*P.y() + T[0][2]*P.z();
  double py = T[1][0]*P.x() +  T[1][1]*P.y() + T[1][2]*P.z();
  double pz = T[2][0]*P.x() +  T[2][1]*P.y() + T[2][2]*P.z();
  px += t[0]; py+=t[1]; pz += t[2];
  return {px, py, pz};
}
// C is a 2x3 affine camera matrix without the translation column, t is the 2x1 translation vector
static vgl_point_2d<double> proj_3d(std::vector<std::vector<double> > const&  C, std::vector<double> const& t, vgl_point_3d<double> const& P){
  double px = C[0][0]*P.x() + C[0][1]*P.y() +  C[0][2]*P.z() + t[0];
  double py = C[1][0]*P.x() + C[1][1]*P.y() +  C[1][2]*P.z() + t[1];
  return {px, py};
}
static void test_all()
{
  // basis
  vgl_point_2d<double> p0(1.0, 2.0), p1(3.0, 2.0), p2(1.0, 4.0);
  vgl_point_2d<double> p(4.0, 3.0);
  double a00 = 0.5, a11 = 0.5, a10 = -0.3, a01 = 0.3, tx = 10.0, ty = 5.0;
  double q0x = a00*p0.x() + a01*p0.y() + tx;
  double q0y = a10*p0.x() + a11*p0.y() + ty;
  vgl_point_2d<double> q0(q0x, q0y);

  double q1x = a00*p1.x() + a01*p1.y() + tx;
  double q1y = a10*p1.x() + a11*p1.y() + ty;
  vgl_point_2d<double> q1(q1x, q1y);

  double q2x = a00*p2.x() + a01*p2.y() + tx;
  double q2y = a10*p2.x() + a11*p2.y() + ty;
  vgl_point_2d<double> q2(q2x, q2y);

  double qx = a00*p.x() + a01*p.y() + tx;
  double qy = a10*p.x() + a11*p.y() + ty;
  vgl_point_2d<double> q(qx, qy);
  std::vector<vgl_point_2d<double> > p_pts, affine_p_pts;
  p_pts.push_back(p0);   p_pts.push_back(p1);
  p_pts.push_back(p2);   p_pts.push_back(p);

  std::vector<vgl_point_2d<double> > q_pts, affine_q_pts;
  q_pts.push_back(q0);   q_pts.push_back(q1);
  q_pts.push_back(q2);   q_pts.push_back(q);

  vgl_affine_coordinates_2d(p_pts, affine_p_pts);
  vgl_affine_coordinates_2d(q_pts, affine_q_pts);
  bool good = true;
  for(unsigned i = 0; i<affine_p_pts.size(); ++i){
    std::cout << affine_p_pts[i] << ' ' << affine_q_pts[i] << std::endl;
    double d = (affine_p_pts[i]-affine_q_pts[i]).length();
    good = good && d<1.0e-9;
  }
  TEST("invariance of affine 2d coords", good, true);
  std::vector<vgl_point_3d<double> > Ppts, Qpts;
  vgl_point_3d<double> P0(1.0, 2.0, 3.0), P1(3.0, 2.5, 4.0), P2(1.5, 4.0, -1.0), P3(0.0,10.0,7.0);
  vgl_point_3d<double> P(4.0, 3.0, 10.0), Pm(-4.0, 3.0, 10.0);
  Ppts.push_back(P0);   Ppts.push_back(P1);   Ppts.push_back(P2);
  Ppts.push_back(P3);   Ppts.push_back(P); Ppts.push_back(Pm);
  std::vector<std::vector<double> > T(3);
  T[0]=std::vector<double>(3);T[1]=std::vector<double>(3);T[2]=std::vector<double>(3);
  T[0][0] = 0.5;   T[1][1] = 1.5; T[2][2] = 1.0;
  T[0][1] = -0.7;   T[0][2] = -0.5;
  T[1][0] = 0.7;    T[1][2] = -0.3;
  T[2][0] = 0.5;    T[2][1] =  0.3;
  std::vector<double> t(3);
  t[0]=10.0;   t[1]=20.0;   t[2]=100.0;
  vgl_point_3d<double> Q0 = trans_3d(T,t,P0), Q1 = trans_3d(T,t,P1), Q2 = trans_3d(T,t,P2);
  vgl_point_3d<double> Q3 = trans_3d(T,t,P3), Q = trans_3d(T,t,P),  Qm = trans_3d(T,t,Pm);
  Qpts.push_back(Q0);   Qpts.push_back(Q1);   Qpts.push_back(Q2);
  Qpts.push_back(Q3);   Qpts.push_back(Q); Qpts.push_back(Qm);
  std::vector<vgl_point_3d<double> > affine_P_coords, affine_Q_coords;
  vgl_affine_coordinates_3d(Ppts, affine_P_coords);
  vgl_affine_coordinates_3d(Qpts, affine_Q_coords);
  good = true;
  for(unsigned i = 0; i<Ppts.size(); ++i){
    std::cout << affine_P_coords[i] << ' ' << affine_Q_coords[i] << std::endl;
    double d = (affine_P_coords[i]-affine_Q_coords[i]).length();
    good = good && d<1.0e-9;
  }
  TEST("invariance of affine 3d coords", good, true);
  // project Pi into view1
  std::vector<vgl_point_2d<double> > ppts1, ppts2;
  std::vector<std::vector<double> > C1(2);
  C1[0]=std::vector<double>(3);   C1[1]=std::vector<double>(3);
 std::vector<double> tc(2, 0.0);
  C1[0][0] = 0.5; C1[0][1] = 0.0; C1[0][2] = 2.0;
  C1[1][0] = 0.0; C1[1][1] = 2.0; C1[1][2] = 3.0;
  std::vector<std::vector<double> > C2 = C1;
  C2[0][0]=1.5;    C2[0][1]=0.707; C2[0][2] = 4.0;
  C2[1][0]=-0.707; C2[1][1]=0.5;   C2[1][2] = 5.0;
  for(const auto & Ppt : Ppts){
    vgl_point_2d<double> p1 = proj_3d(C1, tc, Ppt);
    p1.set(p1.x(), p1.y());
    ppts1.push_back(p1);
    vgl_point_2d<double> p2 = proj_3d(C2, tc, Ppt);
    ppts2.push_back(p2);
  }
  std::vector<vgl_point_3d<double> > recon_affine_pts;
  vgl_affine_coordinates_3d(ppts1, ppts2, recon_affine_pts);
  good = true;
  for(unsigned i = 0; i<ppts1.size(); ++i){
    std::cout << affine_P_coords[i] << ' ' << recon_affine_pts[i] << std::endl;
    double d = (affine_P_coords[i]-recon_affine_pts[i]).length();
    good = good && d<1.0e-9;
  }
  TEST("affine 3-d coords from 2-d views", good, true);
  //image test JF - below are image coordinates on three face images of the same person
  std::vector<vgl_point_2d<double> >  JF0, JF1, JF2;
  vgl_point_2d<double> p00(402.056, 429.889), p01(421.611, 373), p02(383.389, 362.778),
    p03(427.389, 304.556), p04(490.056, 301.889);
  JF0.push_back(p00);   JF0.push_back(p01);   JF0.push_back(p02);   JF0.push_back(p03);   JF0.push_back(p04);
  vgl_point_2d<double> p10(235.444, 343.889), p11(253.222, 309.222), p12(236.333, 291),
    p13(259.889, 253.222), p14(303, 251.889);
  JF1.push_back(p10);   JF1.push_back(p11);   JF1.push_back(p12);   JF1.push_back(p13);   JF1.push_back(p14);
  vgl_point_2d<double> p20(  293.667, 301), p21(307, 270.778), p22(282.556, 257.444),
    p23(319.444, 219.667), p24(351.444, 220.556);
  JF2.push_back(p20);   JF2.push_back(p21);   JF2.push_back(p22);   JF2.push_back(p23);   JF2.push_back(p24);
#if 0
  for(unsigned i = 0; i<5; ++i){
    //roughly normalize the coordinates
    vgl_point_2d<double>& p0 = JF0[i];
    p0.set((p0.x()-275.0)/200.0, (p0.y()-275.0)/200.0);
    vgl_point_2d<double>& p1 = JF1[i];
    p1.set((p1.x()-275.0)/200.0, (p1.y()-275.0)/200.0);
    vgl_point_2d<double>& p2 = JF2[i];
    p2.set((p1.x()-275.0)/200.0, (p2.y()-275.0)/200.0);
  }
#endif
  std::vector<vgl_point_3d<double> > affine_01, affine_02;
  vgl_affine_coordinates_3d(JF0, JF1, affine_01);
  vgl_affine_coordinates_3d(JF0, JF2, affine_02);
  for(unsigned i = 0; i<affine_01.size(); ++i){
    std::cout << affine_01[i] << ' ' << affine_02[i] << std::endl;
  }
  std::cout << "Christina sensitivity test" << std::endl;
  //sensitivity test  3-d points from a face scan for a realistic test case

  vgl_point_3d<double> c0(-33.633899688721,23.875200271606,-31.615800857544);//right lobe
  vgl_point_3d<double> c1(21.458400726318,27.088699340820,73.859703063965);//alar tip
  vgl_point_3d<double> c2(10.649299621582,67.683601379395,50.533798217773);//right medial canthus
  vgl_point_3d<double> c3(21.124300003052,37.831298828125,84.872703552246);//nose apex
  vgl_point_3d<double> c4(-18.988700866699,71.193702697754,39.596099853516);//right lateral canthus
  vgl_point_3d<double> c5(67.848396301270,69.642601013184,49.217899322510);//left lateral canthus
  std::vector<vgl_point_3d<double> > OrigCpts, Cpts;
  OrigCpts.push_back(c0);   OrigCpts.push_back(c1);   OrigCpts.push_back(c2);
  OrigCpts.push_back(c3);   OrigCpts.push_back(c4);   OrigCpts.push_back(c5);
  Cpts.push_back(c3);   Cpts.push_back(c1); Cpts.push_back(c2);
  Cpts.push_back(c0); Cpts.push_back(c4); Cpts.push_back(c5);
  std::vector<vgl_point_3d<double> > affine_C_coords;
  vgl_affine_coordinates_3d(Cpts, affine_C_coords);
  for(unsigned i = 0; i<Cpts.size(); ++i){
    std::cout << affine_C_coords[i] << std::endl;
  }
  std::vector<vgl_point_2d<double> > cpts1, cpts2;
  std::vector<std::vector<double> > CC1(2);
  CC1[0]=std::vector<double>(3);   CC1[1]=std::vector<double>(3);
  std::vector<double> tcc(2,0.0);
  //set up camera looking along +X
  CC1[0][0] = 0.0; CC1[0][1] = 0.0; CC1[0][2] = 1.0;
  CC1[1][0] = 0.0; CC1[1][1] = -1.0; CC1[1][2] = 0.0;

  std::vector<std::vector<double> > CC2 = CC1;

  //rotate 15deg about Y
  CC1[0][0] = 0.9659258;//sqrt(2.0)/2.0;
  CC2[0][2] = 0.2588190;//CC1[0][0];
  for(const auto & Cpt : Cpts){
    vgl_point_2d<double> p1 = proj_3d(CC1, tcc, Cpt);
    cpts1.push_back(p1);
    vgl_point_2d<double> p2 = proj_3d(CC2, tcc, Cpt);
    cpts2.push_back(p2);
  }

  std::vector<vgl_point_3d<double> > c_affine_pts;
  vgl_affine_coordinates_3d(cpts1, cpts2, c_affine_pts);
  good = true;
  for(unsigned i = 0; i<cpts1.size(); ++i){
    std::cout << affine_C_coords[i] << ' ' << c_affine_pts[i] << std::endl;
    double d = (affine_C_coords[i]-c_affine_pts[i]).length();
    good = good && d<1.0e-9;
  }
  TEST("Affine coordinates for face pts", good, true);
  //Determine coordinate sensitivity
  vgl_box_2d<double> box1, box2;
  for(unsigned i = 0; i<cpts1.size(); ++i){
    vgl_point_2d<double> p1 = cpts1[i];
    vgl_point_2d<double> p2 = cpts2[i];
    box1.add(p1); box2.add(p2);
  }
  vgl_point_2d<double> min_p1 = box1.min_point();
  vgl_point_2d<double> max_p1 = box1.max_point();
  double diag1 = (max_p1-min_p1).length();
  vgl_point_2d<double> min_p2 = box2.min_point();
  vgl_point_2d<double> max_p2 = box2.max_point();
  double diag2 = (max_p2-min_p2).length();
  double diam = diag1;
  if(diam<diag2)
    diam = diag2;
  unsigned nr = 1000;
  double norm_radius = 0.02;
  double r = norm_radius*diam;
  std::cout << "diameter " << diam << " Norm radius " << norm_radius << " Abs radius " << r << std::endl;
  vgl_pointset_3d<double> pset4, pset5;
  vgl_point_3d<double> z(0.0, 0.0, 0.0);
  double L4 = (affine_C_coords[4]-z).length();
  double L5 = (affine_C_coords[5]-z).length();
  double max_diff = 0.0;
  for(unsigned k = 0; k<nr; ++k){
    std::vector<vgl_point_2d<double> > rpts1, rpts2;
    for(unsigned i = 0; i<cpts1.size(); ++i){
      vgl_point_2d<double> p1 = cpts1[i];
      vgl_point_2d<double> p2 = cpts2[i];
      p1 = p1 +rand_vector(r);
      p2 = p2 +rand_vector(r);
      rpts1.push_back(p1); rpts2.push_back(p2);
    }
    std::vector<vgl_point_3d<double> > aff_pts_3d;
    vgl_affine_coordinates_3d(rpts1, rpts2, aff_pts_3d);
    pset4.add_point(aff_pts_3d[4]);
    pset5.add_point(aff_pts_3d[5]);
    double d1 = (aff_pts_3d[4] - affine_C_coords[4]).length();
    double d2 = (aff_pts_3d[5] - affine_C_coords[5]).length();
    d1/= L4; d2/=L5;
    if(d1>max_diff)
      max_diff = d1;
    if(d2>max_diff)
      max_diff = d2;
  }
  std::cout << "max error " << max_diff << std::endl;
  // write pset to file
  std::string pfile4 = "D:/VisionSystems/Janus/Invariants/pt4_spread.txt";
  std::string pfile5 = "D:/VisionSystems/Janus/Invariants/pt5_spread.txt";
  std::ofstream ostr4(pfile4.c_str());
  ostr4 << pset4;
  ostr4.close();
  std::ofstream ostr5(pfile5.c_str());
  ostr5 << pset5;
  ostr5.close();
  //  linden face test
  std::cout << "linden test" << std::endl;
  vgl_point_3d<double> l0(-100.140998840332,-63.093498229980,-97.007896423340);//right lobe
  vgl_point_3d<double> l1(-35.828701019287,-45.380100250244,-8.269379615784);//alar tip
  vgl_point_3d<double> l2(-51.182998657227,-2.846539974213,-30.991199493408);//right medial canthus
  vgl_point_3d<double> l3(-32.956798553467,-32.738601684570,5.117280006409);//nose apex
  vgl_point_3d<double> l4(-82.580596923828,0.867244005203,-42.126300811768);//right lateral canthus
  vgl_point_3d<double> l5(15.527600288391,-4.037390232086,-39.543399810791);//left lateral canthus
  std::vector<vgl_point_3d<double> > Lpts;
  Lpts.push_back(l0);   Lpts.push_back(l1);   Lpts.push_back(l2);
  Lpts.push_back(l3);   Lpts.push_back(l4);  Lpts.push_back(l5);
  std::vector<vgl_point_3d<double> > affine_L_coords;
  vgl_affine_coordinates_3d(Lpts, affine_L_coords);
  for(unsigned i = 0; i<Lpts.size(); ++i){
    std::cout << affine_L_coords[i] << std::endl;
  }
}
void test_affine_coordinates()
{
  std::cout << "*****************************\n"
           << " Testing vgl_affine_coordinates\n"
           << "*****************************\n\n";

  test_all();
}


TESTMAIN(test_affine_coordinates);
