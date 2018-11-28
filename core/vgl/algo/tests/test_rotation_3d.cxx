// Test vgl_rotate_3d

#include <testlib/testlib_test.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_line_3d_2_points.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_line_segment_3d.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_rational.h>
#include <vnl/vnl_rational_traits.h>
#include <vnl/vnl_math.h> // for sqrt2 and pi/2

static const double epsilon = 1e-11;

// Test conversions between various rotation representations
static void test_conversions(const vgl_rotation_3d<double>& rot)
{
  vnl_double_3x3 R = rot.as_matrix();
  vgl_h_matrix_3d<double> H = rot.as_h_matrix_3d();
  vnl_double_3 rr = rot.as_rodrigues();
  vnl_double_3 er = rot.as_euler_angles();
  vnl_quaternion<double> qr = rot.as_quaternion();
  if (qr.real() < 0.0) qr *= -1.0;

  vnl_quaternion<double> qr_other = vgl_rotation_3d<double>(R).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff1 = (qr_other - qr).magnitude();
  TEST_NEAR("Matrix conversion", diff1, 0.0, epsilon);
  if (diff1 > epsilon)
    std::cout << "Matrix:\n" << R << std::endl;

  qr_other = vgl_rotation_3d<double>(H).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff2 = (qr_other - qr).magnitude();
  TEST_NEAR("3D Homography conversion", diff2, 0.0, epsilon);
  if (diff2 > epsilon)
    std::cout << "3D Homography:\n" << H << std::endl;

  qr_other = vgl_rotation_3d<double>(rr).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff3 = (qr_other - qr).magnitude();
  TEST_NEAR("Rodrigues conversion", diff3, 0.0, epsilon);
  if (diff3 > epsilon)
    std::cout << "Rodrigues: " << rr << std::endl;

  qr_other = vgl_rotation_3d<double>(er[0], er[1], er[2]).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff4 = (qr_other - qr).magnitude();
  TEST_NEAR("Euler conversion", diff4, 0.0, epsilon);
  if (diff4 > epsilon)
    std::cout << "Euler:  Rx=" << er[0]<< " Ry="<<er[1]<<" Rz="<<er[2] << std::endl;
  //Test the case of flipping the orientation of a vector (rotation by pi)
  vgl_vector_3d<double> a(1.0, 1.0, 1.0), aflip(-1.0, -1.0, -1.0);
  vgl_rotation_3d<double> flip(a, aflip);
  vgl_vector_3d<double> v = flip*a;
  vgl_vector_3d<double> null = v+a;
  double err = std::fabs(null.x())+std::fabs(null.y())+std::fabs(null.z());
  TEST_NEAR("Flip vector",err , 0.0, epsilon);
}


// Test that the inverse rotation works as expected
static void test_inverse(const vgl_rotation_3d<double>& rot)
{
  vgl_rotation_3d<double> I = rot * rot.inverse();
  double diff = (I.as_quaternion()-vgl_rotation_3d<double>().as_quaternion()).magnitude();
  TEST_NEAR("Inverse rotation", diff, 0.0, epsilon);
}

// Test that the transpose or conjugate rotation works as expected
static void test_transpose(const vgl_rotation_3d<double>& rot)
{
  vgl_rotation_3d<double> I = rot * rot.transpose();
  double diff = (I.as_quaternion()-vgl_rotation_3d<double>().as_quaternion()).magnitude();
  TEST_NEAR("transpose or conjugate rotation", diff, 0.0, epsilon);
}

// Test application of rotation to other vgl objects
static void test_application(const vgl_rotation_3d<double>& rot)
{
  vnl_double_3x3 R = rot.as_matrix();

  vgl_homg_point_3d<double> hpt(100,50,200,2);
  vnl_double_3 p(hpt.x()/hpt.w(),hpt.y()/hpt.w(),hpt.z()/hpt.w());

  vgl_homg_plane_3d<double> hpl(0,1,0,100);

  vgl_homg_point_3d<double> r_hpt = rot*hpt;
  vnl_double_3 r_p = R*p;

  vgl_homg_plane_3d<double> r_hpl = rot*hpl;

  TEST_NEAR("Rotated point-plane dist (homg)", vgl_distance(hpt,hpl),
            vgl_distance(r_hpt, r_hpl), epsilon);

  double diff = (r_p - vnl_double_3(r_hpt.x()/r_hpt.w(),
                                    r_hpt.y()/r_hpt.w(),
                                    r_hpt.z()/r_hpt.w())).magnitude();
  TEST_NEAR("Rotated point", diff, 0.0, epsilon);

  vgl_point_3d<double> pt(-30, 0, 75);
  vgl_point_3d<double> r_pt = rot*pt;

  vgl_plane_3d<double> pl(0,0,1,50);
  vgl_plane_3d<double> r_pl = rot*pl;
  TEST_NEAR("Rotated point-plane dist", vgl_distance(pt,pl),
            vgl_distance(r_pt, r_pl), epsilon);

  vgl_vector_3d<double> v= vgl_point_3d<double>(hpt) - pt;
  vgl_vector_3d<double> r_v = rot*v;
  vgl_vector_3d<double> r_v2 = vgl_point_3d<double>(r_hpt) - r_pt;
  TEST_NEAR("Rotated vector", (r_v-r_v2).length(), 0.0, epsilon);

  vgl_homg_line_3d_2_points<double> hl(hpt, vgl_homg_point_3d<double>(v.x(),v.y(),v.z(),0));
  vgl_homg_line_3d_2_points<double> r_hl = rot*hl;
  std::cout << "rotated hl = " << r_hl << std::endl;
  //FIXME  add test

  vgl_line_3d_2_points<double> l(pt, pt+v);
  vgl_line_3d_2_points<double> r_l = rot*l;
  std::cout << "rotated l = " << r_l << std::endl;
  //FIXME  add test

  vgl_line_segment_3d<double> s(pt, pt+v);
  vgl_line_segment_3d<double> r_s = rot*s;
  std::cout << "rotated s = " << r_s << std::endl;
  //FIXME  add test
}


void test_rotation_3d()
{
  std::cout << "*************************\n"
           << " Testing vgl_rotation_3d\n"
           << "*************************\n";

  std::cout << "\n1. Rotation about the x axis over 90 degrees.\n";
  vgl_rotation_3d<double> rot_id;
  test_conversions(rot_id);
  test_inverse(rot_id);
  test_transpose(rot_id);
  test_application(rot_id);

  std::cout << "\n2. Rotation about the x axis over 90 degrees.\n";

  vgl_rotation_3d<double> rot_x90(vnl_math::pi_over_2, 0.0, 0.0);
  test_conversions(rot_x90);
  test_inverse(rot_x90);
  test_transpose(rot_x90);
  test_application(rot_x90);

  vnl_random rnd;
  vgl_rotation_3d<double> rot_rand(rnd.normal(), rnd.normal(), rnd.normal());
  std::cout << "\n3. Random rotation: " << rot_rand.as_quaternion() << std::endl;
  test_conversions(rot_rand);
  test_inverse(rot_rand);
  test_transpose(rot_rand);
  test_application(rot_rand);
  //test constructor from two vectors
  vnl_double_3 b(0.0, 0.0, 1.0),
               ap(vnl_math::sqrt1_2, 0.0, vnl_math::sqrt1_2), // vector of magnitude 1
               am(-1.0,  0.0, -1.0), // magnitude > 1, to test robustified constructor
               a1(0.5773502692, -0.5773502692, 0.5773502692), // magnitude 1
               a2(0,0,-1);  //  180 degree rotation, w/negative z component
  vgl_rotation_3d<double> r_abp(ap, b);
  vgl_rotation_3d<double> r_abm(am, b);
  vgl_rotation_3d<double> r_ab1(a1, b);
  vgl_rotation_3d<double> r_ab2(a2, b);
  vnl_double_3 ap_to_b = r_abp*ap, am_to_b = r_abm*am, a1_to_b = r_ab1*a1, a2_to_b = r_ab2*a2;
  double errorp = (b - ap_to_b).squared_magnitude();
  TEST_NEAR("constructor from 2 vectors: rotate 45d around Y axis", errorp, 0.0, epsilon);
  double errorm = (b*vnl_math::sqrt2 - am_to_b).squared_magnitude();
  TEST_NEAR("constructor from 2 vectors: rotate 225d around Y axis", errorm, 0.0, epsilon);
  double error1 = (b - a1_to_b).squared_magnitude();
  TEST_NEAR("constructor from 2 vectors: from arbitrary point", error1, 0.0, epsilon);
  double error2 = (b - a2_to_b).squared_magnitude();
  TEST_NEAR("constructor from 2 vectors: 180 degree rotation", error2, 0.0, epsilon);
  vgl_vector_3d<float> ag(1.0f, 1.0f, 0.0f), bg(0.0f, 0.0f, float(vnl_math::sqrt2));
  vgl_rotation_3d<float> r_abg(ag, bg);
  vgl_vector_3d<float> ag_to_bg = r_abg*ag;
  float errorf = (bg - ag_to_bg).sqr_length();
  TEST_NEAR("constructor from two vgl vectors", errorf, 0.0f, 1e-6f);
  vnl_vector_fixed<vnl_rational,3> ai(1L, 1L, 0L), bi(1L, -1L, 0L);
  vgl_rotation_3d<vnl_rational> r_abi(ai, bi);
#define sqr(x) ((x)*(x))
  error1 = sqr(double(r_abi.as_quaternion()[0]))
         + sqr(double(r_abi.as_quaternion()[1]))
         + sqr(double(r_abi.as_quaternion()[2]) + vnl_math::sqrt1_2)
         + sqr(double(r_abi.as_quaternion()[3]) - vnl_math::sqrt1_2);
  TEST_NEAR("rotation is 90 deg in XY plane", error1, 0.0, epsilon);
#if VXL_INT_64_IS_LONG || VXL_INT_64_IS_LONGLONG
  // temporary fix for a "bug" (actually just integer overflow) in vnl_rational:
  vnl_rational sqrthalf(453016774L,640662461L);
  r_abi = vnl_quaternion<vnl_rational>(0,0,-sqrthalf,sqrthalf);
#endif //  VXL_INT_64_IS_LONG
  vnl_vector_fixed<vnl_rational,3> ai_to_bi = r_abi*ai;
  vnl_rational errori = (bi - ai_to_bi).squared_magnitude();
  TEST_NEAR("constructor from two rational-coordinate vectors", errori, 0L, epsilon);
}

TESTMAIN(test_rotation_3d);
