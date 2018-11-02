#include <iostream>
#include <cmath>
#include <limits>
#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_object.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_2d.h>
#include <vul/vul_file.h>

static void test_volm_conf_object()
{
  // define a location along with some land type id
  vgl_point_2d<double> pt(12.0f, 31.0f);
  double height = 12.31f;
  unsigned char land_id = 18;

  float theta = (std::atan2(pt.y(), pt.x())<0) ? (float)std::atan2(pt.y(), pt.x()) + vnl_math::twopi : (float)std::atan2(pt.y(), pt.x());
  float theta_in_deg = theta / vnl_math::pi_over_180;
  auto dist = (float)std::sqrt(pt.x()*pt.x() + pt.y()*pt.y());

  volm_conf_object_sptr obj_sptr = new volm_conf_object(pt, height, land_id);
  std::cout << "given location point: (" << pt.x() << ',' << pt.y() << ") with land id: " << (int)land_id << std::endl;
  obj_sptr->print(std::cout);
  float epsilon = 1E-5;
  std::cout << "epsilon = " << epsilon << std::endl;
  std::cout << "size of an volm_conf_object: " << sizeof(*obj_sptr) << " bytes" << std::endl;
  std::cout << "size of its member: " << sizeof(obj_sptr->theta()) + sizeof(obj_sptr->dist()) + sizeof(obj_sptr->land()) + sizeof(obj_sptr->height()) << " bytes" << std::endl;
  TEST_NEAR("Test angle value", obj_sptr->theta(), theta, epsilon);
  TEST_NEAR("Test angle value(in degree)", obj_sptr->theta_in_deg(), theta_in_deg, epsilon);
  TEST_NEAR("Test distance value", obj_sptr->dist(), dist, epsilon);
  TEST_NEAR("Test height value", obj_sptr->height(), height, epsilon);
  TEST_NEAR("Test location x value", obj_sptr->x(), (float)pt.x(), epsilon);
  TEST_NEAR("Test location y value", obj_sptr->y(), (float)pt.y(), epsilon);
  vgl_vector_2d<float> diff = obj_sptr->loc() - vgl_point_2d<float>(pt.x(), pt.y());
  TEST_NEAR("Test location pt value", diff.length(), 0.0f, epsilon);

  // Test binary IO
  vsl_b_ofstream os("./volm_conf_object.bin");
  vsl_b_write(os, obj_sptr);
  os.close();
  vsl_b_ifstream is("./volm_conf_object.bin");
  volm_conf_object_sptr obj_in = new volm_conf_object();
  vsl_b_read(is, obj_in);
  TEST("Test binary IO", obj_sptr->is_same(obj_in), true);
  return;
}

TESTMAIN( test_volm_conf_object );
