#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_intersection.h>

static void test_sph_geom()
{
  double er = 0;
  //test azimuth difference
  double az_a = 15.0, az_b = 180.0;
  double ang1=0, ang2=0;
  double diff0 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer0 =  std::fabs(ang1-97.5)+std::fabs(ang2+82.5);
  az_a = -15.0, az_b = -180.0;
  double diff1 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer1 =  std::fabs(ang1+97.5)+std::fabs(ang2-82.5);
  az_a = -90.0, az_b = 91.0;
  double diff2 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer2 =  std::fabs(ang2+179.5)+std::fabs(ang1-0.5);
  az_a = -179.0, az_b = 180.0;
  double diff3 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer3 =  std::fabs(ang2+179.5)+std::fabs(ang1-0.5);
  az_a = 179.0, az_b = -180.0;
  double diff4 = vsph_utils::azimuth_diff(az_a, az_b, false);
  vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  double haer4 =  std::fabs(ang2-179.5)+std::fabs(ang1+0.5);
  er = std::fabs(diff0-165.0) + std::fabs(diff1 + 165.0);
  er += std::fabs(diff2+179.0) + std::fabs(diff3+1.0);
  er += std::fabs(diff4-1.0);
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
  bb_long.print(std::cout, false);
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
  TEST("bb_ext.contains(p15)", bb_ext.contains(p15), false);
  TEST("bb_ext.contains(p20)", bb_ext.contains(p20), true);
  bb_ext.add(p15);
  TEST("bb_ext.contains(p15)", bb_ext.contains(p15), true);
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
  //test min, max points
  vsph_sph_point_2d pl_min = bb_long.min_point(false), pl_max = bb_long.max_point(false);
  er = std::fabs(pl_min.phi_-165.0)+std::fabs(pl_max.phi_-10.0);
  vsph_sph_point_2d pl_inc_min = bb_inc_cut.min_point(false), pl_inc_max = bb_inc_cut.max_point(false);
  er += std::fabs(pl_inc_min.phi_-166.0)+std::fabs(pl_inc_max.phi_+175.0);
  TEST_NEAR("min and max points", er, 0.0, 0.01);
  // test constructors
  // points in degrees
  vsph_sph_point_2d p0(0.0, 0.0, false), p1(90.0, 90.0, false), pc(45.0, 45.0, false);
  // box in radians
  vsph_sph_box_2d bb(p0, p1, pc);
  double mnth = bb.min_theta(false), mxth = bb.max_theta(false);
  double mnph = bb.min_phi(false), mxph = bb.max_phi(false);
  er = mnph + mnth + std::fabs(mxth-90.0)+ std::fabs(mxph-90.0);
  er += std::abs(bb.min_theta()+ bb.min_phi());
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
  er = std::fabs(area_a-0.179081) + std::fabs(area_b-0.121229);
  TEST_NEAR("areas spanning +-180", er, 0.0, 0.001);
  //test intersection of boxes spanning cut
  std::vector<vsph_sph_box_2d> boxes;
  bool good = intersection(bba, bbb, boxes);
  vsph_sph_box_2d bint;
  if (good)
  bint = boxes[0];
  double min_ph_int = bint.min_phi(false), max_ph_int = bint.max_phi(false);
  double min_th_int = bint.min_theta(false), max_th_int = bint.max_theta(false);

  good = good && vsph_utils::a_eq_b(min_ph_int, p2.phi_, false)
              && vsph_utils::a_eq_b(max_ph_int, p5.phi_, false)
              && vsph_utils::a_eq_b(min_th_int, p4.theta_, false)
              && vsph_utils::a_eq_b(max_th_int, p3.theta_,false);
  TEST("intersection", good, true);
  // test boxes forming a cross (no endpoints inside each box
  vsph_sph_point_2d p6 ( 50.0,  -10.0, false);
  vsph_sph_point_2d p7 (130.0,   10.0, false);
  vsph_sph_point_2d p8 ( 90.0,    0.0, false);
  vsph_sph_point_2d p9 ( 75.0,  -60.0, false);
  vsph_sph_point_2d p100(105.0,   60.0, false);

  vsph_sph_box_2d bbc1(p6, p7, p8), bbc2(p9,p100,p8);
  boxes.clear();
  good = intersection(bbc1, bbc2, boxes);

  vsph_sph_box_2d bint_cross;
  if (good) bint_cross = boxes[0];
  boxes.clear();
  good = good && intersection(bbc2, bbc1, boxes);
  vsph_sph_box_2d bint_cross_rev;
  if (good) bint_cross_rev = boxes[0];
  good = vsph_utils::a_eq_b(bint_cross.min_phi(false), p6.phi_, false)
      && vsph_utils::a_eq_b(bint_cross.max_phi(false), p7.phi_, false)
      && vsph_utils::a_eq_b(bint_cross_rev.max_phi(false),bint_cross.max_phi(false) , false)
      && bint_cross.min_theta(false) == p9.theta_
      && bint_cross.max_theta(false) == p100.theta_;
  TEST("Crossing box arrangement", good, true);
  // one box essentially covers the entire +- 180 phi circle
  vsph_sph_box_2d box_s1, box_s2, box_s12;
  double b1_thmin = 1.4836689630573823, b1_thmax = 1.6579236905324111;
  double b1_phia = -vnl_math::pi_over_2, b1_phib =vnl_math::pi_over_2;
  double b1_phic = 0.51656139130052758;
  box_s1.set(b1_thmin, b1_thmax, b1_phia, b1_phib, b1_phic);
  double b2_thmin = 1.3088871075562318, b2_thmax = vnl_math::pi*7/12;
  double b2_phia =-0.31728356503269012, b2_phib = -0.31911878808324995;
  double b2_phic = 0.54737987781481912;
  box_s2.set(b2_thmin, b2_thmax, b2_phia, b2_phib, b2_phic);
  boxes.clear();

  // two boxes are produced
  good = intersection(box_s1, box_s2, boxes)
      && box_s1.contains(boxes[0])&& box_s1.contains(boxes[1]);
  TEST("each box contains the other's bounds", good, true);

  // transform a box
  vsph_sph_box_2d tb1 = box_s1.transform(0.5, 0.25, 1.2, true);
  double tth_min = 1.96624, tth_max = 2.17535;
  double tph_min =-1.63496, tph_max = 2.13496, tphc = 0.25;
  er = std::fabs(tb1.min_theta()-tth_min) + std::fabs(tb1.max_theta()-tth_max);
  double tb_a_ph = tb1.min_phi(), tb_b_phi = tb1.max_phi(), tb_c_phi = tb1.c_phi();
  er += std::fabs(tph_min-tb_a_ph) + std::fabs(tph_max-tb_b_phi) +std::fabs(tphc-tb_c_phi);
  TEST_NEAR("transform box no phi cut", er, 0.0, 0.001);

  vsph_sph_box_2d tb2 = bba.transform(0.5, 0.25, 1.2, true);
  double tb2_a_ph = tb2.a_phi(false), tb2_b_phi = tb2.b_phi(false), tb2_c_phi = tb2.c_phi(false);
  double tb2_ph_min = -168.676, tb2_ph_max = -132.676, tb2_c = -150.676;
  er = std::fabs(tb2_ph_min-tb2_a_ph) + std::fabs(tb2_ph_max-tb2_b_phi) +
       std::fabs(tb2_c-tb2_c_phi);
  TEST_NEAR("transform box contains +-180 cut", er, 0.0, 0.001);

  vsph_sph_box_2d tb1_about = box_s1.transform(0.5, 0.25, 1.6,1.57,2.2, true);
  double tth_min_about = 1.93187, tth_max_about = 2.21068;
  double tph_min_about = 0.18663706143591696,
         tph_max_about = 1.4432741228718344,
         tphc_about = -1.0700000000000003;
  er = std::fabs(tb1_about.min_theta()-tth_min_about) + std::fabs(tb1_about.max_theta()-tth_max_about);
  double tb_a_ph_about = tb1_about.a_phi(), tb_b_phi_about = tb1_about.b_phi(), tb_c_phi_about = tb1_about.c_phi();
  er += std::fabs(tph_min_about-tb_a_ph_about) + std::fabs(tph_max_about-tb_b_phi_about) +std::fabs(tphc_about-tb_c_phi_about);
  TEST_NEAR("transform box about a point no phi cut", er, 0.0, 0.001);

  vsph_sph_box_2d tb3 = bba.transform(0.5, 0.25, 1.2, true);
  double tb3_a_ph = tb3.a_phi(false), tb3_b_phi = tb3.b_phi(false), tb3_c_phi = tb3.c_phi(false);
  double tb3_ph_min = -168.676, tb3_ph_max = -132.676, tb3_c = -150.676;
  er = std::fabs(tb3_ph_min-tb3_a_ph) + std::fabs(tb3_ph_max-tb3_b_phi) +
    std::fabs(tb3_c-tb3_c_phi);

  TEST_NEAR("transform box contains +-180 cut", er, 0.0, 0.001);
  double grok_a_phi = -2.7617465101715433;
  double grok_b_phi = -2.4723473031673180;
  double grok_c_phi = -2.5151716844356731;
  double grok_min_th = 1.6235700411813854;
  double grok_max_th = 1.8268944131886669;
  vsph_sph_box_2d grok;
  grok.set(grok_min_th, grok_max_th, grok_a_phi, grok_b_phi, grok_c_phi, true);
  vsph_sph_box_2d tb4 = grok.transform(0.75, -1.0, 1.2, true);
  double t_grok_a = 2.4925, t_grok_b = 2.83984;
  er = std::fabs(t_grok_a-tb4.a_phi()) + std::fabs(t_grok_b-tb4.b_phi());
  TEST_NEAR("negative trans with roll-over", er, 0.0, 0.001);

  std::vector<vsph_sph_box_2d> sub_boxes;
  grok.sub_divide(sub_boxes);
  unsigned nb = sub_boxes.size();
  TEST("number of boxes", nb, 4);
  if (nb!=4)
    return;

  double a00 = sub_boxes[0].area(), sum = a00;
  std::cout << "a00 " << a00 << '\n';
  er = 0.0;
  for (unsigned i = 1; i<nb; ++i) {
    double a = sub_boxes[i].area();
    std::cout<< "a = " << a << '\n';
    er += std::fabs(a00-a);
    sum += a;
  }
  std::cout << "grok area  " << grok.area() << " sum " << sum << '\n';
  er += std::fabs(sum - grok.area());
  TEST_NEAR("equal angle subdivision of a box", er, 0.0, 0.001);
  double grok_a = intersection_area(grok, grok);
  TEST_NEAR("box self-intersection area  ", grok_a, grok.area(), 0.001);

  double abba = bint.area();
  double abba_area = intersection_area(bba,bbb);
  er = std::fabs(abba-abba_area);
  TEST_NEAR("intersection area", er, 0.0, 0.001);
  double abba_area_utils = vsph_utils::sph_inter_area(bba, bbb);
  er = std::fabs(abba-abba_area_utils);
  TEST_NEAR("intersection area utils", er, 0.0, 0.001);
  vsph_sph_box_2d prod_a, prod_b;
  prod_a.set(88.9573,  99.6052, 165.913, -179.858,173.027, false);
  prod_b.set(90.7031,  90.7073, 171.554, 180,172.258, false);
  double prod_area = intersection_area(prod_a,prod_b);
  std::cout << "Prod area: " << prod_area << std::endl;

  std::vector<vsph_sph_box_2d> bxs;
  nb = 100000000;
  vul_timer t;
#if 0
  for (unsigned i = 0; i<nb; ++i) {
    double a0 = intersection_area(bbc1, bbc2); // vsph_utils::sph_inter_area(bbc1, bbc2);
    double a1 = intersection_area(bba, bbb); // vsph_utils::sph_inter_area(bba, bbb);
    bba.in_interval(180.0, false);
    grok.set(grok_min_th, grok_max_th, grok_a_phi, grok_b_phi, grok_c_phi, true);
    diff4 = vsph_utils::azimuth_diff(az_a, az_b, false);
    vsph_utils::half_angle(az_a, az_b, ang1, ang2, false);
  }
#endif
  double td_vsph = static_cast<double>(t.real())/(2.0*nb*1000.0);
  std::cout << "vsph box intersection time = " << td_vsph << " secs\n";
  vgl_point_2d<double> vp0(0.0, 0.0), vp1(1.0, 1.0);
  vgl_point_2d<double> vp2(0.25, 0.0), vp3(0.75, 1.0);
  vgl_box_2d<double> b1, b2;
  b1.add(vp0);  b1.add(vp1);   b2.add(vp2);  b2.add(vp3);
  t.mark();
  for (unsigned i = 0; i<nb; ++i) {
    vgl_box_2d<double> tin = vgl_intersection(b1, b2);
    vgl_box_2d<double> tYn = vgl_intersection(b1, b2);
  }
  double td_vgl = static_cast<double>(t.real())/(2.0*nb*1000.0);
  std::cout << "vgl box intersection time = " << td_vgl << " secs\n"
           << "ratio = " << td_vsph/td_vgl << '\n';
}

TESTMAIN(test_sph_geom);
