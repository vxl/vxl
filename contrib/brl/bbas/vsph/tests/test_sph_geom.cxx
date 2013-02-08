#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>

#include <vnl/vnl_math.h>


static void test_sph_geom()
{
  double er = 0;
  //test azimuth difference
  double az_a = 15.0, az_b = 180.0;
  double ang1=0, ang2=0;
  double diff0 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer0 =  vcl_fabs(ang1-97.5)+vcl_fabs(ang2+82.5);
  az_a = -15.0, az_b = -180.0;
  double diff1 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer1 =  vcl_fabs(ang1+97.5)+vcl_fabs(ang2-82.5);
  az_a = -90.0, az_b = 91.0;
  double diff2 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
   double haer2 =  vcl_fabs(ang1+179.5)+vcl_fabs(ang2-0.5);
  az_a = -179.0, az_b = 180.0;
  double diff3 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer3 =  vcl_fabs(ang1+179.5)+vcl_fabs(ang2-0.5);
  az_a = 179.0, az_b = -180.0;
  double diff4 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer4 =  vcl_fabs(ang1-179.5)+vcl_fabs(ang2+0.5);
  er = vcl_fabs(diff0-165.0) + vcl_fabs(diff1 + 165.0);
  er += vcl_fabs(diff2+179.0) + vcl_fabs(diff3+1.0);
  er += vcl_fabs(diff4-1.0);
  TEST_NEAR("angle difference ", er, 0.0, 0.01);
  er = haer0 + haer1 + haer2 + haer3 + haer4;
  TEST_NEAR("half angle ", er, 0.0, 0.01);

  // test box spanning 360 degrees in azimuth
  vsph_sph_point_2d p10(80.0, 0.0, false);
  vsph_sph_point_2d p11(110.0,10.0, false);
  vsph_sph_point_2d p12(80.0, 60.0, false);
  vsph_sph_point_2d p13(110.0, 110.0, false);
  vsph_sph_point_2d p14(80.0, 165.0, false);
  vsph_sph_point_2d p14a(80.0, 166.0, false);
  vsph_sph_point_2d p15(110.0, -175.0, false);
  vsph_sph_point_2d p15a(110.0, -180.0, false);
  vsph_sph_point_2d p16(80.0, -130.0, false);
  vsph_sph_point_2d p17(110.0, -100.0, false);
  vsph_sph_point_2d p18(80.0,  45.0, false);
  vsph_sph_point_2d p19(80.0, -10.0, false);
  vsph_sph_point_2d p20(80.0, 179.0, false);
  vsph_sph_box_2d bb_long(p10, p14, p15);
  bb_long.print(vcl_cout, false);
  bool c16 = bb_long.contains(p16);//true
  bool c11 = bb_long.contains(p11);//false
  bb_long.add(p11);
  bool c11add = bb_long.contains(p11);//true
  bool c12 = bb_long.contains(p12);//false
  bool c13 = bb_long.contains(p13);//false
  bool c14a = bb_long.contains(p14a);//true
  bool c15a = bb_long.contains(p15a);//true
  bool test1 = c16&&!c11&&c11add&&!c12&&!c13&&c14a&&c15a;
  TEST("Long interval contains c", test1, true);
  vsph_sph_box_2d bb_short(p10, p12, p11);
  bool c12s = bb_short.contains(p12);//true
  bool c15s = bb_short.contains(p15);//false
  bool c18s = bb_short.contains(p18);//true
  bool test2 = c12s&&!c15s&&c18s;
  TEST("Short interval contains c", test2, true);

  vsph_sph_box_2d bb_ext(p10, p12, p11);
  bb_ext.add(p14); 
  bb_ext.add(p15a);
  bool ext_15 = bb_ext.contains(p15);
  bool ext_20 = bb_ext.contains(p20);
  bb_ext.add(p15);
  bool ext_17 = bb_ext.contains(p17);
  // test incremental updates 
  vsph_sph_box_2d bb_inc(false);
  bb_inc.add(p10); bb_inc.add(p11); bb_inc.add(p12);
  bool inc18 = bb_inc.contains(p18);//true
  bool incp15a = bb_inc.contains(p15a);//false
  vsph_sph_box_2d bb_inc_cut(false);
  bb_inc_cut.add(p14a);bb_inc_cut.add(p15);bb_inc_cut.add(p15a);
  bool inc_cut_14 = bb_inc_cut.contains(p14);//false
  bool inc_cut_20 = bb_inc_cut.contains(p20);//true
  bool test_inc = inc18&&!incp15a&&!inc_cut_14&&inc_cut_20;
  TEST("incremental update", test_inc, true);
  // test constructors
  // points in degrees
  vsph_sph_point_2d p0(0.0, 0.0, false), p1(90.0, 90.0, false), pc(45.0, 45.0, false);
  // box in radians
  vsph_sph_box_2d bb(p0, p1, pc);
  double mnth = bb.min_theta(false), mxth = bb.max_theta(false);
  double mnph = bb.min_phi(false), mxph = bb.max_phi(false);
  er = mnph + mnth + vcl_fabs(mxth-90.0)+ vcl_fabs(mxph-90.0);
  er += vcl_abs(bb.min_theta()+ bb.min_phi());
  TEST_NEAR("constructors", er, 0.0, 0.01);
  // test box area
  double area = bb.area();
  TEST_NEAR("Sperical Area", area, vnl_math::pi_over_2, 0.01);

  // test box contains
  vsph_sph_box_2d bin(p10, p12, p11), bout(p19, p13, p11);
  bool box_inside = bout.contains(bin);
  TEST("box contains box", box_inside, true);
  // test boxes spanning cut
  vsph_sph_point_2d p2(70.0, 180.0, false), p3(90.0, -150.0, false),p3c(90.0, -170.0, false);
  vsph_sph_point_2d p4(80.0, 170.0, false), p5(100.0, -170.0, false), p5c(100.0, 180.0, false);
  vsph_sph_box_2d bba(p2, p3, p3c), bbb(p4,p5,p5c);
  double min_ph_a = bba.min_phi(false), max_ph_a = bba.max_phi(false);
  double min_ph_b = bbb.min_phi(false), max_ph_b = bbb.max_phi(false);
  bool min_altb = vsph_utils::a_lt_b(min_ph_a, min_ph_b, false);
  bool max_blta = vsph_utils::a_lt_b(max_ph_b, max_ph_a, false);
  TEST("comparison operators", (!min_altb)&&max_blta, true);

  double area_a = bba.area();
  double area_b = bbb.area();
  er = vcl_fabs(area_a-0.179081) + vcl_fabs(area_b-0.121229);
  TEST_NEAR("areas spanning +-180", er, 0.0, 0.001);
  //test intersection of boxes spanning cut
  vsph_sph_box_2d bint = intersection(bba, bbb);
  double min_ph_int = bint.min_phi(false), max_ph_int = bint.max_phi(false);
  double min_th_int = bint.min_theta(false), max_th_int = bint.max_theta(false);

  bool good = vsph_utils::a_eq_b(min_ph_int, p2.phi_);
  good = good && vsph_utils::a_eq_b(max_ph_int, p5.phi_);
  good = good && vsph_utils::a_eq_b(min_th_int, p4.theta_);
  good = good && vsph_utils::a_eq_b(max_th_int, p3.theta_);
  TEST("intersection", good, true);
  

}

TESTMAIN(test_sph_geom);
