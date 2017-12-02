#include <testlib/testlib_test.h>
#include <boxm/util/boxm_rational_camera_utils.h>
#include <vgl/vgl_plane_3d.h>
#include <vpgl/vpgl_local_rational_camera.h>


static void test_boxm_rational_camera_utils()
{
  std::string filename="camera0.txt";

  vpgl_local_rational_camera<double>* rcam=read_local_rational_camera<double>( "./" + filename);
  vgl_plane_3d<double> top(0,0,1,-200);
  vgl_plane_3d<double> bottom(0,0,1,0);

  unsigned ni=800,
           nj=800;
  double u, v;

  rcam->project(495,431,200,u,v);
  vgl_plane_3d<double>  plane_parallel=boxm_rational_camera_utils::boxm_find_parallel_image_plane(rcam,top,bottom,ni,nj);
  // TODO: do something with plane_parallel !
}

TESTMAIN(test_boxm_rational_camera_utils);
