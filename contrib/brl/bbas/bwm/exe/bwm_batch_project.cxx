#include <bwm/bwm_observer_cam.h>

#include <vcl_vector.h>
#include <vcl_string.h>

#include <vgl/vgl_polygon.h>

#include <vpgl/vpgl_rational_camera.h>

int main(int argc, char** argv)
{
  //bwm_observer_cam obs;
  // read the list of object (.ply) files
  vcl_vector<vcl_string> paths;
  vpgl_rational_camera<double> cam;

  vcl_vector<vgl_polygon<double> > poly_2d_list;
  bwm_observer_cam::project_meshes(paths, &cam, poly_2d_list);

}
