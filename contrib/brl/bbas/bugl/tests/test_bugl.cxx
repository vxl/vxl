#include <bugl/bugl_gaussian_point_2d.h>
#include <bugl/bugl_gaussian_point_3d.h>
#include <bugl/bugl_normal_point_2d.h>
#include <bugl/bugl_normal_point_2d_sptr.h>
#include <bugl/bugl_normal_point_3d.h>
#include <bugl/bugl_normal_point_3d_sptr.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>
#include <testlib/testlib_test.h>

void test_bugl()
{
  // test constructor
  vnl_double_2x2 s2(0.0); s2[0][0] = 1; s2[1][1] = 3;
  bugl_gaussian_point_2d<double> p2d(1, 1, s2);

  vnl_double_2x2 c = p2d.get_covariant_matrix();
  TEST("cov[0][0]==1", c[0][0], 1.0);
  TEST("cov[1][1]==3", c[1][1], 3.0);
  TEST("cov[0][1]==0", c[0][1], 0.0);
  TEST("cov[1][0]==0", c[1][0], 0.0);

  bugl_gaussian_point_3d<double> p1;
  vnl_double_3x3 s3(0.0); s3[0][0] = 1; s3[1][1] = 3; s3[2][2] = 5;
  bugl_gaussian_point_3d<double> p3d(100, 100, 100, s3);

  vnl_double_3x3 t = p3d.get_covariant_matrix();
  TEST("cov3d[0][0]==1", t[0][0], 1.0);
  TEST("cov3d[1][1]==3", t[1][1], 3.0);
  TEST("cov3d[2][2]==5", t[2][2], 5.0);

  // test smart pointer
  bugl_normal_point_2d_sptr p2d_ptr = new bugl_normal_point_2d(1, 1, s2);
  TEST("p2d.x()==1", p2d_ptr->x(), 1.0);
  TEST("p2d.y()==1", p2d_ptr->y(), 1.0);
  bugl_normal_point_3d_sptr p3d_ptr = new bugl_normal_point_3d(1, 1, 1, s3);
  TEST("p3d.x()==1", p3d_ptr->x(), 1.0);
  TEST("p3d.y()==1", p3d_ptr->y(), 1.0);
  TEST("p3d.z()==1", p3d_ptr->z(), 1.0);

  // test set point
  vgl_point_3d<double> pt(5.0,5.0,5.0);
  p3d_ptr->set_point(pt);

  TEST("p3d.x()==5", p3d_ptr->x(), 5.0);
  TEST("p3d.y()==5", p3d_ptr->y(), 5.0);
  TEST("p3d.z()==5", p3d_ptr->z(), 5.0);
}

TESTLIB_DEFINE_MAIN(test_bugl);
