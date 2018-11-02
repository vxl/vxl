#include <iostream>
#include <testlib/testlib_test.h>

#include <bvpl/bvpl_direction_to_color_map.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

// Test changes
static void test_direction_to_color_map()
{
  double theta_res = vnl_math::pi_over_4;
  double phi_res = vnl_math::pi_over_4;

  std::vector<vgl_point_3d<double> >  samples;
  samples.emplace_back(0.0,0.0,1.0);
  samples.emplace_back(0.0,0.0,-1.0);

  // theta=pi/4,pi/2,3pi/4
  for (double phi=vnl_math::pi_over_4;phi <= 3*vnl_math::pi_over_4;phi+=phi_res)
  {
    for (double theta=0.0;theta<vnl_math::twopi-theta_res*0.5;theta+=theta_res)
    {
      samples.emplace_back(std::cos(theta) * std::sin(phi),std::sin(theta) * std::sin(phi),std::cos(phi));
    }
  }
  bvpl_direction_to_color_map color_map(samples,"random");
  color_map.make_svg_color_map("cmap.svg");
}

TESTMAIN( test_direction_to_color_map );
