#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vul/vul_timer.h>
#include <bbas/volm/volm_spherical_container.h>

static void test_spherical_container()
{
  double vmin = 10;
  double dmax = 60000;
  double angle = 2;
  volm_spherical_container sph(angle,vmin,dmax);
  vcl_cout << " solid angle = " << angle << "\t, finnest resolution = " << vmin << ", dmax = " << dmax  << ", number of voxel = " << (sph.get_voxels()).size() << vcl_endl;

}


TESTMAIN(test_spherical_container);
