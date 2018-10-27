//:
// \file
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_spline_region_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#define TEST_SPLINE_REGION 0
//: Test changes
static void test_spline_region()
{
#if TEST_SPLINE_REGION
  std::string base_dir = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/mouth/";
  vgl_point_3d<double> p0(-27.048786163330,-58.756767272949,61.351360321045);
  vgl_point_3d<double> p1(-22.989215850830,-61.527729034424,69.345726013184);
  vgl_point_3d<double> p2(-16.756101608276,-63.819145202637,72.460670471191);
  vgl_point_3d<double> p3(-10.471982955933,-66.513549804688,75.632919311523);
  vgl_point_3d<double> p4(-4.012704849243,-65.970741271973,77.426780700684);
  vgl_point_3d<double> p5(1.456362962723,-65.779235839844,77.438545227051);
  vgl_point_3d<double> p6(9.006361961365,-64.448524475098,76.230621337891);
  vgl_point_3d<double> p7(14.902037620544,-64.017341613770,73.427680969238);
  vgl_point_3d<double> p8(22.068965911865,-61.970176696777,69.302963256836);
  vgl_point_3d<double> p9(25.956886291504,-59.957836151123,62.066261291504);
  std::vector<vgl_point_3d<double> > knots;
  knots.push_back(p0); knots.push_back(p1); knots.push_back(p2); knots.push_back(p3);
  knots.push_back(p4); knots.push_back(p5); knots.push_back(p6); knots.push_back(p7);
  knots.push_back(p8); knots.push_back(p9);
  bvgl_spline_region_3d<double> spl_reg(knots, 0.5);
  vgl_point_3d<double> p_pos(0.0, -40.0, 65.0);
  spl_reg.set_point_positive(p_pos);
  vgl_point_3d<double> p3d(0.0, -65.8559, 77.0);
  bool inside = spl_reg.in(p3d);
  double sd;
  vgl_point_3d<double> psd(1.08,-60.0 , 68.39);
  bool good = spl_reg.signed_distance(psd, sd);
  if(good)
    std::cout << "signed distance " << sd << '\n';
  vgl_point_3d<double> cent = spl_reg.centroid();
  std::cout << "centroid " << cent << '\n';
  vgl_pointset_3d<double> ptset = spl_reg.random_pointset(1000);
  std::string pt_path = base_dir + "lower_mouth_plane.txt";
  std::ofstream ostr(pt_path.c_str());
  if(ostr){
    ostr << ptset;
    ostr.close();
  }
  // test area
  vgl_point_3d<double> a0(0.0, 0.0, 0.0);
  vgl_point_3d<double> a1(1.0, 0.0, 0.0);
  vgl_point_3d<double> a2(1.0, 1.0, 0.0);
  vgl_point_3d<double> a3(0.0, 1.0, 0.0);
  std::vector<vgl_point_3d<double> > aknots;
  aknots.push_back(a0);   aknots.push_back(a1);
  aknots.push_back(a2);   aknots.push_back(a3);
  bvgl_spline_region_3d<double> aspl_reg(aknots, 0.5);
  double area = aspl_reg.area();
  std::cout << area << '\n';
  // test 2d constructor
  vgl_vector_3d<double> normal(-0.73911, 0.100747, -0.666008);
  vgl_point_3d<double> origin(23.56,5.32,94.1);
  std::vector<vgl_point_2d<double > > knots2d;
  vgl_point_2d<double> p00(4.806863675505915, 6.582528807225162);
  vgl_point_2d<double> p01(11.01628093103361, 10.276435637911343);
  vgl_point_2d<double> p02(32.74932515259732, 15.152382902522577);
  vgl_point_2d<double> p03(57.214391510270254,17.81199050140143);
  vgl_point_2d<double> p04(64.66569966821164, 14.118083670715249);
  vgl_point_2d<double> p05(68.76391915507942,  6.878033374857261);
  vgl_point_2d<double> p06(70.99931160246183, -5.533535243434039);
  vgl_point_2d<double> p07(72.48957323405011, -16.61523357209593);
  vgl_point_2d<double> p08(72.98638920548085,-22.673154780442236);
  vgl_point_2d<double> p09(71.86869298178964, -33.016147098515525);
  vgl_point_2d<double> p10(71.24762644682524, -39.81277431745044);
  vgl_point_2d<double> p11(68.26710318364867,-45.27981937091249);
  vgl_point_2d<double> p12(62.43030722082923, -53.554080244991184);
  vgl_point_2d<double> p13(56.34501013094244, -58.13458943216029);
  vgl_point_2d<double> p14(50.756529012486396, -61.68073289733209);
  vgl_point_2d<double> p15(44.91973304966695, -64.78360842935743);
  vgl_point_2d<double> p16(37.468424891725554, -66.70451001764768);
  vgl_point_2d<double> p17(29.768801889420764,-66.55668016194333);
  vgl_point_2d<double> p18(24.801201023225186, -66.26124208450119);
  vgl_point_2d<double> p19(19.585285312666212, -65.2268763625039);
  vgl_point_2d<double> p20(14.245119038573563,-60.35092909789266);
  vgl_point_2d<double> p21(11.38884633893068, -54.29300788954636);
  vgl_point_2d<double> p22(7.538997581237495, -40.25604225059692);
  vgl_point_2d<double> p23(3.8133435022667994,-24.594056368732495);
  vgl_point_2d<double> p24(0.8328202390902426, -10.70492058548739);
  vgl_point_2d<double> p25(-0.2848759846009665, -3.760463510848136);
  vgl_point_2d<double> p26(-0.03648662715599116,-0.06649018997197387);
  vgl_point_2d<double> p27(2.8198419572980784,  4.218425775978408);
  vgl_point_2d<double> p28(32.87338943342705,  15.152382902522577);
  vgl_point_2d<double> p29(1.081209596535218, -11.00035866292951);
  vgl_point_2d<double> p30(0.9570149178127298, -10.70492058548739);
  vgl_point_2d<double> p31(11.140475609756098,  10.128672272396967);
  vgl_point_2d<double> p32(19.585285312666212, -65.37470621820825);
  vgl_point_2d<double> p33(56.34501013094244, -58.13458943216029);
  knots2d.push_back(p00);   knots2d.push_back(p01);   knots2d.push_back(p02);
  knots2d.push_back(p03);   knots2d.push_back(p04);   knots2d.push_back(p05);
  knots2d.push_back(p06);   knots2d.push_back(p07);   knots2d.push_back(p08);
  knots2d.push_back(p09);   knots2d.push_back(p10);   knots2d.push_back(p11);
  knots2d.push_back(p12);   knots2d.push_back(p13);   knots2d.push_back(p14);
  knots2d.push_back(p15);   knots2d.push_back(p16);   knots2d.push_back(p17);
  knots2d.push_back(p18);   knots2d.push_back(p19);   knots2d.push_back(p20);
  knots2d.push_back(p21);   knots2d.push_back(p22);   knots2d.push_back(p23);
  knots2d.push_back(p24);   knots2d.push_back(p25);   knots2d.push_back(p26);
  knots2d.push_back(p27);   knots2d.push_back(p28);   knots2d.push_back(p29);
  knots2d.push_back(p30);   knots2d.push_back(p31);   knots2d.push_back(p32);
  knots2d.push_back(p33);
  bvgl_spline_region_3d<double> kf2d(knots2d, normal, origin, 0.5);
  for(double t = 0.0; t<=kf2d.max_t(); t+=0.5)
    std::cout << t << ' ' << kf2d(t) <<'\n';
  bool isin = kf2d.in(origin);
  vgl_vector_3d<double> L1(0.55,0.68,-0.42);
  double lambda = 0.85, gamma = 0.2;
  double su = lambda, sv = std::pow(lambda,-gamma), sw = 1.0/(su*sv);
  vgl_vector_3d<double> dv = (sw-1.0)*normal;
  // test inverse vector field

  bvgl_spline_region_3d<double> def_reg = kf2d.scale(su, sv, dv, L1);

   vgl_point_3d<double> p_targ = (def_reg.knots())[0]+dv;
   kf2d.set_principal_eigenvector(L1);
   kf2d.set_deformation_eigenvalues(su, sv);
   kf2d.set_offset_vector(dv);
  vgl_vector_3d<double> inv;
  good = kf2d.inverse_vector_field(p_targ, inv);
  vgl_point_3d<double> p_source = p_targ + inv;
  vgl_point_3d<double> p3d_source_act = (kf2d.knots())[0];
  double diff = (p_source - p3d_source_act).length();
#endif
}

TESTMAIN( test_spline_region );
