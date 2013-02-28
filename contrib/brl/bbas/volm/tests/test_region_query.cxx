#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <vsph/vsph_unit_sphere.h>
#include <vsph/vsph_sph_point_3d.h>
#include <depth_map/depth_map_scene.h>
#include <depth_map/depth_map_region.h>
#include <volm/volm_spherical_query_region.h>
#include <volm/volm_spherical_region_query.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_camera_space.h>

static void test_region_query()
{
  // create the depth interval using volm_spherical_container

  // parameter for coast
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 2.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  vcl_string depth_scene_path = "Z://projects//FINDER//test1//p1a_test1_40//p1a_test1_40.vsl";

  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream dis(depth_scene_path.c_str());
  if (!dis)
    return;
  dms->b_read(dis);
  dis.close();
  double head_mid=0.0, head_radius=180.0, head_inc=45.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  vcl_vector<double> fovs(top_fov_vals, top_fov_vals + 9);
  double altitude = 1.6;
  unsigned ni = dms->ni(), nj = dms->nj();
  volm_camera_space_sptr cs_ptr =
    new volm_camera_space(fovs, altitude, ni, nj,
                          head_mid, head_radius, head_inc,
                          tilt_mid, tilt_radius, tilt_inc,
                          roll_mid, roll_radius, roll_inc);
  volm_spherical_region_query srq(dms, cs_ptr, sph);

  srq.print(vcl_cout);


}

TESTMAIN(test_region_query);
