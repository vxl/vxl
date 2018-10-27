//:
// \file
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <testlib/testlib_test.h>
#include <bvgl/bvgl_spline_region_3d.h>
#include <bvgl/bvgl_scaled_shape_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#define TEST_SCALED_SHAPE 0
//: Test changes
static void test_scaled_shape()
{
#if TEST_SCALED_SHAPE
  std::string base_dir = "d:/VisionSystems/Janus/RelevantPapers/FacialMusclesExpression/fat_pocket/";
  std::string display_random_path = base_dir + "random_pocket_points.txt";
  std::string knot_path = base_dir + "fat_pocket_knots_2d.txt";
  //vgl_vector_3d<double> normal(-0.73911, 0.100747, -0.666008);//from pts
  vgl_vector_3d<double> normal(-0.73911, 0.2, -0.666008);//from pts but 2 x y comp
  //vgl_point_3d<double> origin(23.56,5.32,94.1);// manual
//vgl_point_3d<double> origin(16.1689, 0.0, 87.4399); // move 10 along normal zero out y
 vgl_point_3d<double> origin(26.14, -7.45, 72.1); // from skull
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

  knots2d.push_back(p00);   knots2d.push_back(p01);   knots2d.push_back(p02);
  knots2d.push_back(p03);   knots2d.push_back(p04);   knots2d.push_back(p05);
  knots2d.push_back(p06);   knots2d.push_back(p07);   knots2d.push_back(p08);
  knots2d.push_back(p09);   knots2d.push_back(p10);   knots2d.push_back(p11);
  knots2d.push_back(p12);   knots2d.push_back(p13);   knots2d.push_back(p14);
  knots2d.push_back(p15);   knots2d.push_back(p16);   knots2d.push_back(p17);
  knots2d.push_back(p18);   knots2d.push_back(p19);   knots2d.push_back(p20);
  knots2d.push_back(p21);   knots2d.push_back(p22);   knots2d.push_back(p23);
  knots2d.push_back(p24);   knots2d.push_back(p25);   knots2d.push_back(p26);
  knots2d.push_back(p27);
  vgl_vector_3d<double> L1(0.55,0.68,-0.42);
  bvgl_spline_region_3d<double> kf2d(knots2d, normal, origin, 0.5);
  bvgl_spline_region_3d<double> temp = kf2d.scale(1.0, 0.6,vgl_vector_3d<double>(0.0, 0.0, 0.0),L1);
  std::vector<vgl_point_2d<double> > sknots2d = temp.knots_2d();
  std::ofstream kistr(knot_path.c_str());
  for(std::vector<vgl_point_2d<double> >::iterator kit = sknots2d.begin();
      kit != sknots2d.end(); ++kit)
    kistr << kit->x() << ',' << kit->y() << '\n';
  kistr.close();
  bvgl_spline_region_3d<double> skf2d(sknots2d, normal, origin, 0.5);
  double max_norm_distance = 25.0, scale_at_midpt = 0.85, scale_at_max = 0.1;
  bvgl_scaled_shape_3d<double> ss3d(skf2d, max_norm_distance, scale_at_midpt, scale_at_max,0.5);
  unsigned indx = 0;
  vgl_point_3d<double> pt(50.23, -14.95, 83.64);
  bool inpt = ss3d.in(pt);
  vgl_point_3d<double> tpt(56.0, -28.0, 85.0);
  vgl_point_3d<double> cp = ss3d.closest_point(tpt);
  double volume = ss3d.volume();
  std::cout << "Volume before uniform scaling " << volume << " mm^3\n";
  bvgl_scaled_shape_3d<double> ss3d_scaled = ss3d.scale(1.1);
   volume = ss3d_scaled.volume();
  std::cout << "Volume after scaling " << volume << " mm^3  max norm distance " << ss3d_scaled.max_norm_distance() << '\n';
  ss3d.set_aniso_scale(1.1, 1.1, 1.1);
  ss3d.apply_parameters_to_cross_sections();
  const std::vector<bvgl_spline_region_3d<double> >& csects_scal = ss3d_scaled.cross_sections();
  vgl_point_3d<double> p_targ_scl = (csects_scal[csects_scal.size()-1].knots())[0];
  vgl_vector_3d<double> inv_scl;
  bool good_scl = ss3d.inverse_vector_field(p_targ_scl, inv_scl);
  const std::vector<bvgl_spline_region_3d<double> >& csects = ss3d.cross_sections();
  vgl_point_3d<double> p_source_act = (csects[csects.size()-1].knots())[0];
  vgl_point_3d<double> p_source_scl = p_targ_scl + inv_scl;
  double dif_scl = (p_source_scl-p_source_act).length();
  TEST_NEAR("Compare scaling to inverse VF", dif_scl, 0.0, 0.001);

  std::cout << "Volume before deformation " << volume << " mm^3\n";
  double lambda = 0.85, gamma = 0.2;
  //double lambda = 1.0, gamma = 0.2;
  bvgl_scaled_shape_3d<double> ss3d_deformed = ss3d.deform(lambda, gamma,L1);
  volume = ss3d_deformed.volume();
  std::cout << "Volume after deformation " << volume << " mm^3  max norm distance " << ss3d_deformed.max_norm_distance() << '\n';;
  // set deformation parameters
  ss3d.set_lambda(lambda);
  ss3d.set_gamma(gamma);
  ss3d.set_principal_eigenvector(L1);
  ss3d.apply_parameters_to_cross_sections();
  // get a point in the deformed volume
  const std::vector<bvgl_spline_region_3d<double> >& csects_def = ss3d_deformed.cross_sections();
  vgl_point_3d<double> p_targ = (csects_def[csects_def.size()-1].knots())[0];
  vgl_vector_3d<double> inv;
  bool good = ss3d.inverse_vector_field(p_targ, inv);
  vgl_point_3d<double> p_source = p_targ + inv;
  double dif = (p_source-p_source_act).length();
  TEST_NEAR("Compare deformation to inverse VF", dif, 0.0, 0.001);
#if 0
  vgl_pointset_3d<double> ptset = ss3d_deformed.random_pointset(100000);
  std::ofstream ostr(display_random_path.c_str());
  if(ostr){
          ostr << ptset;
          ostr.close();
  }else
   std::cout << "couldn't open " << display_random_path << '\n';
#endif
#endif
}

TESTMAIN( test_scaled_shape );
