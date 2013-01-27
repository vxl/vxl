#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>
#include <bbas/volm/volm_spherical_container.h>

static void test_spherical_container()
{
  float vmin = 10.0f;
  float dmax = 60000.0f;
  float angle = 4.0f;
#if 0
  float vmin = 3.0f;
  float dmax = 2000.0f;
  float angle = 1.1f;
#endif

  volm_spherical_container sph(angle,vmin,dmax);
  vcl_cout << " solid angle = " << angle << "\t, finnest resolution = " << vmin << ", dmax = " << dmax  << ", number of voxel = " << (sph.get_voxels()).size() << '\n'
           << "number of depth intervals.. " << sph.get_depth_offset_map().size() << vcl_endl;

  TEST("number of depth intervals.. ", sph.get_depth_offset_map().size(), 144); // change if using an angle different than 4
#if 0
  unsigned int offset, end_offset;
  double depth;
  sph.last_res(vmin, offset, end_offset, depth);
  TEST("# voxels of last layer with res min_res.. ", end_offset-offset, 20888);
  sph.first_res(vmin*2, offset, end_offset, depth);
  TEST("# voxels of first layer with res min_res*2.. ", end_offset-offset, 5768);
  vcl_vector<unsigned int> ids;
  for (unsigned i = offset; i < end_offset; i++)
    ids.push_back(i);
  vcl_cout << " drawing template for vmin*2 layer, there are : " << ids.size() << " ids..\n";
  //sph.draw_template_painted("./2vmin_layer.vrml", 0, ids, 1.0f, 0.0f, 0.0f, 0.8f);
  unsigned off = sph.get_depth_offset_map()[depth];
  TEST("depth of layer with res min_res*2.. ", off, offset);
#endif // 0
  vcl_map<double, unsigned char>& depth_interval_map = sph.get_depth_interval_map();
  vcl_cout << " interval map size: " << depth_interval_map.size();
  TEST("number of depth intervals.. ", depth_interval_map.size(), 144);
  vcl_map<double, unsigned char>::iterator iter = depth_interval_map.end();
  iter--;
  vcl_cout << " last interval: " << (unsigned)iter->second;
  TEST("last interval.. ", iter->second, (unsigned char)143);
  int cnt = 0;
  for (vcl_map<double, unsigned char>::iterator iter = depth_interval_map.begin(); iter != depth_interval_map.end(); iter++, cnt++) {
    vcl_cout << "depth: " << iter->first << " interval: " << iter->second << vcl_endl;
    if (cnt > 5)
      break;
  }
  cnt=0;
  for (vcl_map<double, unsigned char>::iterator iter = depth_interval_map.end(); cnt < 5; cnt++) {
    iter--;
    vcl_cout << "depth: " << iter->first << " interval: " << (int)iter->second << vcl_endl;
  }

  TEST("depth interval for -1..", sph.get_depth_interval(-1), 1);
  TEST("depth interval for 0..", sph.get_depth_interval(0), 1);
  TEST("depth interval for 1..", sph.get_depth_interval(1), 1);
  TEST("depth interval for 9..", sph.get_depth_interval(9), 1);
  TEST("depth interval for 10..", sph.get_depth_interval(10), 2);
  TEST("depth interval for 20..", sph.get_depth_interval(20), 3);
  TEST("depth interval for 51500..", sph.get_depth_interval(51500), 141);
  TEST("depth interval for 58879..", sph.get_depth_interval(58879), 143);
  TEST("depth interval for 58880..", sph.get_depth_interval(58880), 144);
  TEST("depth interval for dmax..", sph.get_depth_interval(dmax), 144);
}


TESTMAIN(test_spherical_container);
