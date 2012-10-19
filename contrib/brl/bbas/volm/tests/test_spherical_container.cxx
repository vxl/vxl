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
  float vmin = 10;
  float dmax = 60000;
  float angle = 4;
  volm_spherical_container sph(angle,vmin,dmax);
  vcl_cout << " solid angle = " << angle << "\t, finnest resolution = " << vmin << ", dmax = " << dmax  << ", number of voxel = " << (sph.get_voxels()).size() << vcl_endl;
  TEST("number of depth intervals.. ", sph.get_depth_offset_map().size(), 144); // change if using an angle different than 4
  unsigned int offset, end_offset;
  double depth;
  sph.last_res(vmin, offset, end_offset, depth);
  TEST("# voxels of last layer with res min_res.. ", end_offset-offset, 20888); 
  sph.first_res(vmin*2, offset, end_offset, depth);
  TEST("# voxels of first layer with res min_res*2.. ", end_offset-offset, 5768); 
  
  vcl_map<double, unsigned char>& depth_interval_map = sph.get_depth_interval_map();
  TEST("number of depth intervals.. ", depth_interval_map.size(), 144);
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.end();
  iter--;
  TEST("last interval.. ", iter->second, (unsigned char)143);
}


TESTMAIN(test_spherical_container);
