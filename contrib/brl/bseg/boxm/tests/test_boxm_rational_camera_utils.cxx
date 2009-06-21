#include <testlib/testlib_test.h>
#include <boxm/boxm_rational_camera_utils.h>
#include <vgl/vgl_point_2d.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpl/vpl.h>


MAIN( test_boxm_rational_camera_utils )
{
  START ("BOXM RATIONAL CAMERA UTILS");
  vcl_string filename="d:/vj/projects/vxl/contrib/brl/bseg/boxm/tests/camera0.txt";
  vpgl_local_rational_camera<double>* rcam=read_local_rational_camera<double>(filename);
  vgl_plane_3d<double> top(0,0,1,-200);
  vgl_plane_3d<double> bottom(0,0,1,0);

  unsigned ni=800;
  unsigned nj=800;

  double u; double v;

  rcam->project(495,431,200,u,v);

  vgl_plane_3d<double>  plane_parallel=boxm_find_parallel_image_plane(rcam,top,bottom,ni,nj);
  SUMMARY();

}
