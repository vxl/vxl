#include <testlib/testlib_test.h>

#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_2d.h>
#include <vsph/vsph_sph_box_2d.h>
#include <vsph/vsph_utils.h>
#include <vnl/vnl_math.h>
#include <vsl/vsl_binary_io.h>
#include <vpl/vpl.h>


static void test_unit_sphere()
{
  double point_angle = 30.0;//coarse for fast test
  double min_theta = 70.0;
  double max_theta = 120.0;
  vsph_unit_sphere_sptr usph =
    new vsph_unit_sphere(point_angle, min_theta, max_theta);
  unsigned n = usph->size();
  TEST("Number of vertices", n , 112);
  vsl_b_ofstream os("./temp.bin");
  vsl_b_write(os, usph);
  os.close();
  vsl_b_ifstream is("./temp.bin");
  vsph_unit_sphere_sptr usph_in;
  vsl_b_read(is, usph_in);
  bool good = usph_in != nullptr;
  if(good){
    double point_angle_in = usph_in->point_angle();
    double min_theta_in = usph_in->min_theta();
    double max_theta_in = usph_in->max_theta();
    unsigned n_in = usph_in->size();
    good = good && point_angle_in == point_angle;
    good = good && min_theta_in == min_theta;
    good = good && max_theta_in == max_theta;
    good = good && n_in == n;
  }
  TEST("binary read write - vsph_unit_sphere", good, true);
  vpl_unlink("./temp.bin");
}
TESTMAIN(test_unit_sphere);
