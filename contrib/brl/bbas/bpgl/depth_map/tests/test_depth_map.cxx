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
#include <vsl/vsl_binary_io.h>
#include <vpl/vpl.h>

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

  vsol_point_2d_sptr p0v= new vsol_point_2d(0.0, 360.0);
  vsol_point_2d_sptr p1v= new vsol_point_2d(640.0, 360.0);
  vsol_point_2d_sptr p2v= new vsol_point_2d(640.0, 0.0);
  vsol_point_2d_sptr p3v= new vsol_point_2d(0.0, 0.0);
  vcl_vector<vsol_point_2d_sptr> vertsv;
  vertsv.push_back(p0v);   vertsv.push_back(p1v);
  vertsv.push_back(p2v);   vertsv.push_back(p3v);

  vsol_polygon_2d_sptr vp = new vsol_polygon_2d(vertsv);

  double min_depth = 10000, max_depth = 30000;
  double depth = 10000;
  vcl_string name =  "vert_perp";
  depth_map_scene dms(ni, nj, cam, gpr, 0, vcl_vector<depth_map_region_sptr>());
  dms.add_ortho_perp_region(vp, min_depth, max_depth, name);
  bool success = dms.set_depth(depth, name);

  vsl_b_ofstream os("./temp.bin");
  vsl_b_write(os, gpr);
  os.close();
  vsl_b_ifstream is("./temp.bin");
  depth_map_region_sptr r_in;
  vsl_b_read(is, r_in);
  if (r_in) {
    bool good = r_in->name() == gpr->name();
    good = good && r_in->min_depth() == gpr->min_depth();
    good = good && r_in->region_2d()->size() == gpr->region_2d()->size();
    TEST("binary read write - depth_map_region", good, true);
  }
  else {
    TEST("binary_read_write - depth_map_region", true, false);
  }
  vpl_unlink("./temp.bin");
  depth_map_scene_sptr ssptr = new depth_map_scene(dms);
  vsl_b_ofstream sos("./temps.bin");
  vsl_b_write(sos, ssptr);
  sos.close();
  vsl_b_ifstream sis("./temps.bin");
  depth_map_scene_sptr s_in;
  vsl_b_read(sis, s_in);
  vpl_unlink("./temps.bin");
  if (s_in) {
    bool good = s_in->ni() == ssptr->ni();
    good = good && s_in->nj() == ssptr->nj();
    good = good && s_in->ground_plane()->region_2d()->size() == gpr->region_2d()->size();
    TEST("binary read write - depth_map_scene", good, true);
  }
  else {
    TEST("binary_read_write - depth_map_scene", true, false);
  }
  // test ortho perp scene planes, sky region
  vsol_point_2d_sptr p0s= new vsol_point_2d(640.0, 360.0);
  vsol_point_2d_sptr p1s= new vsol_point_2d(1280.0, 360.0);
  vsol_point_2d_sptr p2s= new vsol_point_2d(1280.0, 0.0);
  vsol_point_2d_sptr p3s= new vsol_point_2d(640.0, 0.0);
  vcl_vector<vsol_point_2d_sptr> vertss;
  vertss.push_back(p0s);   vertss.push_back(p1s);
  vertss.push_back(p2s);   vertss.push_back(p3s);

  vsol_polygon_2d_sptr sky = new vsol_polygon_2d(vertss);
  dms.set_sky(sky);

#if 0
  vcl_string spath = "e:/mundy/VisionSystems/Finder/VolumetricQuery/p1a_res06_dirtroad_031_scene_dummy.vsl";
  vsl_b_ifstream tis(spath.c_str());
  depth_map_scene scin;
  scin.b_read(tis);
  scin.set_depth(22000.0, "left_mount");
  scin.set_depth(22000.0, "m1");
  scin.set_depth(22000.0, "m2");
  depth_map_region_sptr test_gp = scin.ground_plane();
  scin.set_ground_plane_max_depth(10000.0);
  vil_image_view<float> depth_view = scin.depth_map();
  vil_save(depth_view, "e:/mundy/VisionSystems/Finder/VolumetricQuery/depth_map_dummy.tiff");
#endif
}


TESTMAIN(test_depth_map);
