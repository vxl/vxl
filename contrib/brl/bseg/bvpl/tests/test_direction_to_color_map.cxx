#include <testlib/testlib_test.h>

#include <bvpl/bvpl_direction_to_color_map.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_iostream.h>

// Test changes
static void test_direction_to_color_map()
{
  double theta_res = vnl_math::pi_over_4;
  double phi_res = vnl_math::pi_over_4;

  vcl_vector<vgl_point_3d<double> >  samples;
  samples.push_back(vgl_point_3d<double>(0.0,0.0,1.0));
  samples.push_back(vgl_point_3d<double>(0.0,0.0,-1.0));

  // theta=pi/4,pi/2,3pi/4
  for (double phi=vnl_math::pi_over_4;phi <= 3*vnl_math::pi_over_4;phi+=phi_res)
  {
    for (double theta=0.0;theta<vnl_math::twopi-theta_res*0.5;theta+=theta_res)
    {
      samples.push_back(vgl_point_3d<double>(vcl_cos(theta) * vcl_sin(phi),vcl_sin(theta) * vcl_sin(phi),vcl_cos(phi)));
    }
  }
  bvpl_direction_to_color_map color_map(samples,"random");
  color_map.make_svg_color_map("cmap.svg");
}

TESTMAIN( test_direction_to_color_map );
