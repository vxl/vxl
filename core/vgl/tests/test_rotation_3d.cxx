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
    vcl_cout << "Matrix:\n" << R << vcl_endl;

  qr_other = vgl_rotation_3d<double>(H).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff2 = (qr_other - qr).magnitude();
  TEST_NEAR("3D Homography conversion", diff2, 0.0, epsilon);
  if (diff2 > epsilon)
    vcl_cout << "3D Homography:\n" << H << vcl_endl;

  qr_other = vgl_rotation_3d<double>(rr).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff3 = (qr_other - qr).magnitude();
  TEST_NEAR("Rodrigues conversion", diff3, 0.0, epsilon);
  if (diff3 > epsilon)
    vcl_cout << "Rodrigues: " << rr << vcl_endl;

  qr_other = vgl_rotation_3d<double>(er[0], er[1], er[2]).as_quaternion();
  if (qr_other.real() < 0.0) qr_other *= -1.0;
  double diff4 = (qr_other - qr).magnitude();
  TEST_NEAR("Euler conversion", diff4, 0.0, epsilon);
  if (diff4 > epsilon)
    vcl_cout << "Euler:  Rx=" << er[0]<< " Ry="<<er[1]<<" Rz="<<er[2] << vcl_endl;
}


// Test that the inverse rotation works as expected
static void test_inverse(const vgl_rotation_3d<double>& rot)
{
  vgl_rotation_3d<double> I = rot * rot.inverse();
  double diff = (I.as_quaternion()-vgl_rotation_3d<double>().as_quaternion()).magnitude();
  TEST_NEAR("Inverse rotation", diff, 0.0, epsilon);
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
  vcl_cout << "rotated hl = " << r_hl << vcl_endl;
  //FIXME  add test

  vgl_line_3d_2_points<double> l(pt, pt+v);
  vgl_line_3d_2_points<double> r_l = rot*l;
  vcl_cout << "rotated l = " << r_l << vcl_endl;
  //FIXME  add test

  vgl_line_segment_3d<double> s(pt, pt+v);
  vgl_line_segment_3d<double> r_s = rot*s;
  vcl_cout << "rotated s = " << r_s << vcl_endl;
  //FIXME  add test
}


void test_rotation_3d()
{
  vcl_cout << "*************************\n"
           << " Testing vgl_rotation_3d\n"
           << "*************************\n\n"
           << "1. Rotation about the x axis over 90 degrees.\n";

  vgl_rotation_3d<double> rot_x90(1.57079632679489661923, 0.0, 0.0);
  test_conversions(rot_x90);
  test_inverse(rot_x90);
  test_application(rot_x90);

  vnl_random rnd;
  vgl_rotation_3d<double> rot_rand(rnd.normal(), rnd.normal(), rnd.normal());
  vcl_cout << "\n2. Random rotation: " << rot_rand.as_quaternion() << vcl_endl;
  test_conversions(rot_rand);
  test_inverse(rot_rand);
  test_application(rot_rand);
}

TESTMAIN(test_rotation_3d);
