#include <bugl/bugl_gaussian_point_2d.h>
#include <bugl/bugl_gaussian_point_3d.h>
#include <bugl/bugl_normal_point_2d.h>
#include <bugl/bugl_normal_point_2d_sptr.h>
#include <bugl/bugl_normal_point_3d.h>
#include <bugl/bugl_normal_point_3d_sptr.h>
#include <vnl/vnl_double_2x2.h>
#include <vnl/vnl_double_3x3.h>

int main()
{
  // test constructor
 vnl_double_2x2 s2(0.0);
  s2[0][0] = 1; s2[1][1] = 3;
  bugl_gaussian_point_2d<double> p2d(1, 1, s2);

  bugl_gaussian_point_3d<double> p1;
  vnl_double_3x3 s3(0.0);
  s3[0][0] = 1; s3[1][1] = 3; s3[2][2] = 5;
  bugl_gaussian_point_3d<double> p3d(100, 100, 100, s3);

  vnl_double_3x3 t = p3d.get_covariant_matrix();

  // test smart pointer
  bugl_normal_point_2d_sptr p2d_ptr = new bugl_normal_point_2d(1, 1, s2);
  bugl_normal_point_3d_sptr p3d_ptr = new bugl_normal_point_3d(1, 1, 1, s3);
  return 0;
}
