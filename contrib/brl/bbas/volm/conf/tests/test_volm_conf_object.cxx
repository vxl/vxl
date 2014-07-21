#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_object.h>
#include <vcl_cmath.h>
#include <vnl/vnl_math.h>
#include <vcl_limits.h>
#include <vgl/vgl_point_2d.h>

static void test_volm_conf_object()
{
  // define a location along with some land type id
  vgl_point_2d<double> pt(12.0f, 31.0f);
  unsigned char land_id = 18;

  float theta = (vcl_atan2(pt.y(), pt.x())<0) ? (float)vcl_atan2(pt.y(), pt.x()) + vnl_math::twopi : (float)vcl_atan2(pt.y(), pt.x());
  float theta_in_deg = theta / vnl_math::pi_over_180;
  float dist = (float)vcl_sqrt(pt.x()*pt.x() + pt.y()*pt.y());

  volm_conf_object_sptr obj_sptr = new volm_conf_object(pt, land_id);
  vcl_cout << "given location point: (" << pt.x() << ',' << pt.y() << ") with land id: " << (int)land_id << vcl_endl;
  obj_sptr->print(vcl_cout);
  float epsilon = 1E-5;
  vcl_cout << "epsilon = " << epsilon << vcl_endl;
  TEST_NEAR("Test angle value", obj_sptr->theta(), theta, 1e-5f);
  TEST_NEAR("Test angle value(in degree)", obj_sptr->theta_in_deg(), theta_in_deg, epsilon);
  TEST_NEAR("Test distance value", obj_sptr->dist(), dist, epsilon);
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


}

TESTMAIN( test_volm_conf_object );