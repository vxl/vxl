#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

static void test_region_query()
{
  // create the depth interval using volm_spherical_container

  // parameter for coast
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 2.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  // create a depth_map_scene
  depth_map_scene_sptr depth_scene = new depth_map_scene;
  unsigned ni = 1280;
  unsigned nj = 720;
  depth_scene->set_image_size(ni, nj);
  // add a sky object
  vsol_point_2d_sptr ps0 = new vsol_point_2d(0.0, 200.0);
  vsol_point_2d_sptr ps1 = new vsol_point_2d(1280.0, 200.0);
  vsol_point_2d_sptr ps2 = new vsol_point_2d(1280.0, 0.0);
  vsol_point_2d_sptr ps3 = new vsol_point_2d(0.0, 0.0);
  std::vector<vsol_point_2d_sptr> verts_sky;
  verts_sky.push_back(ps0);  verts_sky.push_back(ps1);
  verts_sky.push_back(ps2);  verts_sky.push_back(ps3);
  vsol_polygon_2d_sptr sp = new vsol_polygon_2d(verts_sky);
  depth_scene->add_sky(sp, 255, "sky");
  // add a ground plane object
  vsol_point_2d_sptr p0= new vsol_point_2d(0.0, 720.0);
  vsol_point_2d_sptr p1= new vsol_point_2d(1280.0, 720.0);
  vsol_point_2d_sptr p2= new vsol_point_2d(1280.0, 500.0);
  vsol_point_2d_sptr p3= new vsol_point_2d(0.0, 500.0);
  std::vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);
  verts.push_back(p2);   verts.push_back(p3);
  // add an object
  vsol_point_2d_sptr pb0 = new vsol_point_2d(640.0, 400.0);
  vsol_point_2d_sptr pb1 = new vsol_point_2d(940.0, 600.0);
  vsol_point_2d_sptr pb2 = new vsol_point_2d(340.0, 200.0);
  std::vector<vsol_point_2d_sptr> verts_bd;
  verts_bd.push_back(pb0);  verts_bd.push_back(pb1);  verts_bd.push_back(pb2);
  vsol_polygon_2d_sptr bp = new vsol_polygon_2d(verts_bd);
  vgl_vector_3d<double> np(1.0, 1.0, 0.0);
  depth_scene->add_region(bp, np, 100.0, 1000.0, "hotel", depth_map_region::FRONT_PARALLEL, 1, 15);

  double head_mid=0.0, head_radius=180.0, head_inc=45.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  std::vector<double> fovs(top_fov_vals, top_fov_vals + 9);
  double altitude = 1.6;
  volm_camera_space_sptr cs_ptr =
    new volm_camera_space(fovs, altitude, ni, nj,
                          head_mid, head_radius, head_inc,
                          tilt_mid, tilt_radius, tilt_inc,
                          roll_mid, roll_radius, roll_inc);
  volm_spherical_region_query srq(depth_scene, cs_ptr, sph);

  srq.print(std::cout);
}

TESTMAIN(test_region_query);
