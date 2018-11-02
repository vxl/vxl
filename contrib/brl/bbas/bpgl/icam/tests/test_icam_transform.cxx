#include <iostream>
#include <string>
#include <vector>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/algo/vgl_rotation_3d.h>

#include <vil/vil_image_view.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vil/vil_load.h>
#include <vil/vil_convert.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>

#include <icam/icam_depth_transform.h>
#include <icam/icam_sample.h>


static void test_icam_transform()
{
  START("icam transform test");
  // a constant depth at 100
  vil_image_view<double> depth(5,5);
  depth.fill(100.0);
  vnl_matrix_fixed<double, 3,3> K(0.0);
  K[0][0]=100;   K[1][1]=100; K[0][2] = 2.5; K[1][2]=2.5; K[2][2]=1.0;

  vnl_vector_fixed<double,3> x(1,0,0), v45(vnl_math::sqrt1_2, vnl_math::sqrt1_2, 0.0);
  vgl_rotation_3d<double> R(x, v45);

  vgl_vector_3d<double> t(1.5, 2.5, 4.0);
  icam_depth_transform dt(K, depth, R, t);
  double from_u = 3.0, from_v = 1.5, to_u, to_v;
  dt.transform(from_u, from_v, to_u, to_v);
  TEST_NEAR("icam_transform simple", to_u, 4.9621732532978, 1e-12);
  TEST_NEAR("icam_transform simple", to_v, 4.5638907328248, 1e-12);
  vgl_point_2d<double> from(3.0, 1.5), to;
  dt.transform(from, to);
  TEST_NEAR("icam_transform point", to.x(), 4.9621732532978, 1e-12);
  TEST_NEAR("icam_transform point", to.y(), 4.5638907328248, 1e-12);
}

TESTMAIN( test_icam_transform );
