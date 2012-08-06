#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <depth_map/depth_map_scene.h>
#include <depth_map/depth_map_region.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vcl_limits.h>
#include <vil/vil_save.h>
static void test_depth_map()
{
  // construct the camera looking along Y with 1.6m height off the ground
  // focal length = 1126.
  unsigned ni = 1280, nj = 720;
  double nid = static_cast<double>(ni), njd = static_cast<double>(nj);
  double right_fov = 29.605;
  double top_fov = 17.725;
  double altitude = 1.6;
  double heading = 0.0;
  double tilt = 90.0;
  double roll = 0.0;
  vpgl_perspective_camera<double> cam = 
    bpgl_camera_utils::camera_from_kml(nid, njd, right_fov, top_fov,
                                       altitude, heading, tilt, roll);
  //construct a ground plane region
  vsol_point_2d_sptr p0= new vsol_point_2d(0.0, 720.0);
  vsol_point_2d_sptr p1= new vsol_point_2d(1280.0, 720.0);
  vsol_point_2d_sptr p2= new vsol_point_2d(1280.0, 361.0);
  vsol_point_2d_sptr p3= new vsol_point_2d(0.0, 361.0);
  vcl_vector<vsol_point_2d_sptr> verts;
  verts.push_back(p0);   verts.push_back(p1);
  verts.push_back(p2);   verts.push_back(p3);

  vsol_polygon_2d_sptr gp = new vsol_polygon_2d(verts);
  vgl_plane_3d<double> plane(0.0, 0.0, 1.0, 0.0);
  depth_map_region_sptr gpr = 
    new depth_map_region(gp, plane, "test_region",
                         depth_map_region::GROUND_PLANE);
  gpr->set_ground_plane_max_depth(10000.0, cam, 3.0);
#if 1
  vsol_point_2d_sptr p0v= new vsol_point_2d(0.0, 360.0);
  vsol_point_2d_sptr p1v= new vsol_point_2d(640.0, 360.0);
  vsol_point_2d_sptr p2v= new vsol_point_2d(640.0, 0.0);
  vsol_point_2d_sptr p3v= new vsol_point_2d(0.0, 0.0);
  vcl_vector<vsol_point_2d_sptr> vertsv;
  vertsv.push_back(p0v);   vertsv.push_back(p1v);
  vertsv.push_back(p2v);   vertsv.push_back(p3v);

  vsol_polygon_2d_sptr vp = new vsol_polygon_2d(vertsv);
  vgl_plane_3d<double> vplane(0.0, 1.0, 0.0, 0.0);
  double min_depth = 10000, max_depth = 30000;
  depth_map_region_sptr vr = 
    new depth_map_region(vp, vplane, min_depth, max_depth,
						 "vertical_region",
                         depth_map_region::VERTICAL);
  double depth = 10000;
  vr->set_region_3d(depth,cam);
  vcl_vector<depth_map_region_sptr> regions;
  regions.push_back(vr);

  depth_map_scene dms(ni, nj, cam, gpr, regions);

  // create depth map image
  vil_image_view<double> depth_view = dms.depth_map();
  vil_save(depth_view, "e:/mundy/VisionSystems/Finder/VolumetricQuery/test_depth_map.tiff");

#endif

}


TESTMAIN(test_depth_map);
