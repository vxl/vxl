#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <depth_map/depth_map_scene.h>
#include <depth_map/depth_map_region.h>
#include <depth_map/depth_map_region_sptr.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_3d.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vil/vil_save.h>
#include <vsl/vsl_binary_io.h>
#include <vpl/vpl.h>
#include <vcl_limits.h>

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
 // double tilt = 78.62;
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
                         depth_map_region::HORIZONTAL);
  gpr->set_ground_plane_max_depth(10000.0, cam, 3.0);
  // test ground plane homography
  vgl_h_matrix_2d<double> H;
  bool good_H = gpr->img_to_region_plane(cam, H);
  TEST("ground plane homography", good_H, true);
  vsol_polygon_2d_sptr r2d = gpr->region_2d();
  vsol_polygon_3d_sptr r3d = gpr->region_3d();
  vgl_plane_3d<double> pl3d = r3d->plane();
  vcl_cout << "Plane: " << pl3d << '\n';
  unsigned nv = r2d->size();
  double erh = 0.0;
  for (unsigned i = 0; i<nv; ++i) {
    vsol_point_3d_sptr p3d = r3d->vertex(i);
    vgl_point_2d<double> pimg = r2d->vertex(i)->get_p();
    vgl_point_2d<double> pmp_2d = H(vgl_homg_point_2d<double>(pimg.x(), pimg.y()));
    vgl_point_3d<double> rmap_3d = plane.world_coords(pmp_2d);
    vcl_cout << pimg << ' ' << *p3d << ' ' << pmp_2d << ' ' << rmap_3d << '\n';
    erh += vcl_fabs(p3d->x()-rmap_3d.x()) +
      vcl_fabs(p3d->y()-rmap_3d.y()) +
      vcl_fabs(p3d->z()-rmap_3d.z());
  }

  TEST_NEAR("planar homography", erh, 0.0, 1e-4);
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
  vcl_string image_path = "dummy_path";
  depth_map_scene dms(ni, nj, image_path, cam, gpr, 0, vcl_vector<depth_map_region_sptr>());
  dms.add_ortho_perp_region(vp, min_depth, max_depth, name);
  /* bool success = */ dms.set_depth(depth, name);

  vsl_b_ofstream os("./temp.bin");
  vsl_b_write(os, gpr);
  os.close();
  vsl_b_ifstream is("./temp.bin");
  depth_map_region_sptr r_in;
  vsl_b_read(is, r_in);
  bool good = r_in
           && r_in->name() == gpr->name()
           && r_in->min_depth() == gpr->min_depth()
           && r_in->region_2d()->size() == gpr->region_2d()->size();
  TEST("binary read write - depth_map_region", good, true);

  vpl_unlink("./temp.bin");
  depth_map_scene_sptr ssptr = new depth_map_scene(dms);
  vsl_b_ofstream sos("./temps.bin");
  vsl_b_write(sos, ssptr);
  sos.close();
  vsl_b_ifstream sis("./temps.bin");
  depth_map_scene_sptr s_in;
  vsl_b_read(sis, s_in);
  vpl_unlink("./temps.bin");
  good = s_in
      && s_in->ni() == ssptr->ni()
      && s_in->nj() == ssptr->nj()
      && s_in->ground_plane()[0]->region_2d()->size() == gpr->region_2d()->size()
      && s_in->image_path() == image_path;
  TEST("binary read write - depth_map_scene", good, true);

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
  //  vcl_string spath = "e:/mundy/VisionSystems/Finder/VolumetricQuery/Queries/p1a_res03_coloredmounds_depthscene_v2.vsl";
  //    vcl_string spath = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/Queries/p1a_res06_dirtroad_depthscene_v2.vsl";
  // vcl_string spath = "e:/mundy/VisionSystems/Finder/VolumetricQuery/Queries/p1a_res17_beachgrass_depthmap_scene.vsl";
vcl_string spath = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/feb_test_queries/tile6_drainage/p1a_test20/p1a_res20_groundtruth_labelme.vsl";
  vsl_b_ifstream tis(spath.c_str()); 
  depth_map_scene scin;
  scin.b_read(tis);
  tis.close();
#endif
#if 0 
// needed to convert old depthmap scenes to new region types
  vcl_vector<depth_map_region_sptr> regs = scin.scene_regions();
  for(vcl_vector<depth_map_region_sptr>::iterator rit = regs.begin(); rit !=regs.end();++rit)
    if((*rit)->orient_type() == depth_map_region::HORIZONTAL)
      (*rit)->set_orient_type( depth_map_region::VERTICAL);
  vcl_vector<depth_map_region_sptr> gps = scin.ground_plane();
  for(vcl_vector<depth_map_region_sptr>::iterator rit = gps.begin(); rit !=gps.end();++rit)
    (*rit)->set_orient_type( depth_map_region::GROUND_PLANE);

  vcl_vector<depth_map_region_sptr> sks = scin.sky();
  for(vcl_vector<depth_map_region_sptr>::iterator rit = sks.begin(); rit !=sks.end();++rit)
    (*rit)->set_orient_type( depth_map_region::INFINT);

  vsl_b_ofstream tos(spath.c_str());
  scin.b_write(tos);

  scene_depth_iterator dend = scin.end();
  scene_depth_iterator sit = scin.begin();
  for (; sit != dend; ++sit)
    scin.print_depth_states();

  vil_image_view<float> dv = sit->depth_map(0);
  vil_save(dv, "e:/mundy/VisionSystems/Finder/VolumetricQuery/depth_map_with_iterator_v2.tiff");

#endif
}


TESTMAIN(test_depth_map);
