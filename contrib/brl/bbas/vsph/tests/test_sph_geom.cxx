#include <testlib/testlib_test.h>

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>

#include <vnl/vnl_math.h>


static void test_sph_geom()
{
  double er = 0;
  // test constructors
  // points in degrees
  vsph_sph_point_2d p0(0.0, 0.0, false), p1(90.0, 90.0, false);
  // box in radians
  vsph_sph_box_2d bb;
  bb.add(p0); 
  bb.add(p1);
  double mnth = bb.min_theta(false), mxth = bb.max_theta(false);
  double mnph = bb.min_phi(false), mxph = bb.max_phi(false);
  er = mnph + mnth + vcl_fabs(mxth-90.0)+ vcl_fabs(mxph-90.0);
  er += vcl_abs(bb.min_theta()+ bb.min_phi());
  TEST_NEAR("constructors", er, 0.0, 0.01);
  //test azimuth difference
  double az_a = 15.0, az_b = 180.0;
  double diff0 = vsph_utils::azimuth_diff(az_a, az_b, false);
  az_a = -15.0, az_b = -180.0;
  double diff1 = vsph_utils::azimuth_diff(az_a, az_b, false);
  az_a = -90.0, az_b = 91.0;
  double diff2 = vsph_utils::azimuth_diff(az_a, az_b, false);
  az_a = -179.0, az_b = 180.0;
  double diff3 = vsph_utils::azimuth_diff(az_a, az_b, false);
  az_a = 179.0, az_b = -180.0;
  double diff4 = vsph_utils::azimuth_diff(az_a, az_b, false);
  er = vcl_fabs(diff0-165.0) + vcl_fabs(diff1 + 165.0);
  er += vcl_fabs(diff2+179.0) + vcl_fabs(diff3+1.0);
  er += vcl_fabs(diff4-1.0);
  TEST_NEAR("angle difference ", er, 0.0, 0.01);
  // test box area
  double area = bb.area();
  TEST_NEAR("Sperical Area", area, vnl_math::pi_over_2, 0.01);
  // test boxes spanning cut
  vsph_sph_point_2d p2(70.0, 180.0, false), p3(90.0, -150.0, false);
  vsph_sph_point_2d p4(80.0, 170.0, false), p5(100.0, -170.0, false);
  vsph_sph_box_2d bba, bbb;
  bba.add(p2); bba.add(p3);
  bbb.add(p4); bbb.add(p5);
  double min_ph_a = bba.min_phi(false), max_ph_a = bba.max_phi(false);
  double min_th_a = bba.min_theta(false), max_theta_a = bba.max_theta(false);
  double min_ph_b = bbb.min_phi(false), max_ph_b = bbb.max_phi(false);
  double min_th_b = bbb.min_theta(false), max_theta_b = bbb.max_theta(false);
  bool min_altb = vsph_utils::a_lt_b(min_ph_a, min_ph_b, false);
  bool max_blta = vsph_utils::a_lt_b(max_ph_b, max_ph_a, false);
  TEST("comparison operators", (!min_altb)&&max_blta, true);

  double area_a = bba.area();
  double area_b = bbb.area();
  er = vcl_fabs(area_a-0.179081) + vcl_fabs(area_b-0.121229);
  TEST_NEAR("areas spanning +-180", er, 0.0, 0.001);
  //test intersection of boxes spanning cut
  vsph_sph_box_2d bint = vsph_utils::intersection(bba, bbb);
  double min_ph_int = bint.min_phi(false), max_ph_int = bint.max_phi(false);
  double min_th_int = bint.min_theta(false), max_th_int = bint.max_theta(false);
  bool good = vsph_utils::a_eq_b(min_ph_int, p2.phi_);
  good = good && vsph_utils::a_eq_b(max_ph_int, p5.phi_);
  good = good && vsph_utils::a_eq_b(min_th_int, p4.theta_);
  good = good && vsph_utils::a_eq_b(max_th_int, p3.theta_);
  TEST("intersection", good, true);

TESTMAIN(test_sph_geom);
