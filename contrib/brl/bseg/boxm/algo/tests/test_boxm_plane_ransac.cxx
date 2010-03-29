#include <testlib/testlib_test.h>
#include <boxm/algo/boxm_plane_ransac.h>

static void test_boxm_plane_ransac()
{
  vcl_vector<vgl_plane_3d<double> > planes;
  planes.push_back(vgl_plane_3d<double>(1,0,0,0));
  planes.push_back(vgl_plane_3d<double>(0,1,0,0));
  planes.push_back(vgl_plane_3d<double>(0,0,1,0));
  planes.push_back(vgl_plane_3d<double>(1,1,0,0));
  planes.push_back(vgl_plane_3d<double>(1,0,1,0));
  planes.push_back(vgl_plane_3d<double>(0,1,1,0));
  vcl_vector<unsigned> indices;
  boxm_plane_ransac(planes, indices, 1);
  vcl_cout << indices.size() << '\n';
}

TESTMAIN(test_boxm_plane_ransac);
