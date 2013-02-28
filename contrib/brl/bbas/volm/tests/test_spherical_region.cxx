#include <testlib/testlib_test.h>

#include <vsph/vsph_sph_box_2d.h>
#include <volm/volm_spherical_region.h>
#include <vnl/vnl_math.h>

static void test_spherical_region()
{
      vsph_sph_box_2d box(true);
      box.set(vnl_math::pi/4, 3*vnl_math::pi/4, 
              vnl_math::pi/2, 3*vnl_math::pi/2,
              vnl_math::pi, true);

      volm_spherical_region sph_region(box);

      sph_region.set_attribute(spherical_region_attributes::ORIENTATION,1);
      sph_region.set_attribute(spherical_region_attributes::NLCD,44);


      sph_region.print(vcl_cout);
      bool good = sph_region.is_attribute(spherical_region_attributes::ORIENTATION);

      TEST("Attribute Orietnation Present", good, true);

      good = sph_region.is_attribute(spherical_region_attributes::DEPTH_ORDER);
      TEST("Attribute Depth Order Not Present", good, false);

}


TESTMAIN(test_spherical_region);
