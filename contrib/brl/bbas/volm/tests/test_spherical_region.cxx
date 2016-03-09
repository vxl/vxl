#include <testlib/testlib_test.h>

#include <vsph/vsph_sph_box_2d.h>
#include <volm/volm_spherical_region.h>
#include <vnl/vnl_math.h>

static void test_spherical_region()
{
  vsph_sph_box_2d box(true);
  box.set(0.25*vnl_math::pi, 0.75*vnl_math::pi,
          0.5*vnl_math::pi, 1.5*vnl_math::pi,
          vnl_math::pi, true);

  volm_spherical_region sph_region(box);

  sph_region.set_attribute(ORIENTATION,1);
  sph_region.set_attribute(NLCD,44);


  sph_region.print(std::cout);
  bool good = sph_region.is_attribute(ORIENTATION);

  TEST("Attribute Orietnation Present", good, true);

  good = sph_region.is_attribute(DEPTH_ORDER);
  TEST("Attribute Depth Order Not Present", good, false);
}


TESTMAIN(test_spherical_region);
