#include "testlib/testlib_test.h"

#include <vsph/vsph_view_sphere.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_spherical_triangle.h>
#include "vnl/vnl_math.h"
#include "vgl/vgl_distance.h"

static void test_spherical_triangle()
{
  // define the spherical coordinate system
 vgl_point_3d<double> origin(370.4965 ,371.332, 17.811);
 double radius = 2000.0;
 vsph_spherical_coord sph_cs(origin, radius);
 vsph_sph_point_3d v0(2000.0, 2.2647769216958906, 0.98279372324732883);
 vsph_sph_point_3d v1(2000.0, 2.2319848934238333, 1.1508565259394712);
 vsph_sph_point_3d v2(2000.0, 2.3755885820717468,  1.0881057065037711);
 vsph_spherical_triangle sph_tri(v0, v1, v2, sph_cs);

 vsph_sph_point_3d v01(2000.0, 0.86715737549016125, -2.3561944901923453);
 vsph_sph_point_3d v11(2000.0, 0.74376579121895314, -2.4689679890861180);
 vsph_sph_point_3d v21(2000.0, 0.87681573189390261, -2.5535900500422257);
 vsph_spherical_triangle sph_tri1(v01, v11, v21, sph_cs);

 vsph_sph_point_3d p(2000.0,  0.83171219569572830, -2.0226056328986735);
 vsph_sph_point_3d p1(2000.0, 0.85371026482968049,  -2.4035943139579694);

 bool cont = sph_tri.contains(p, sph_cs);
 bool cont1 = sph_tri1.contains(p1, sph_cs);


 TEST("spherical triangle contains", cont, false);
}

TESTMAIN(test_spherical_triangle);
