#include <testlib/testlib_test.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_camera_space.h>
#include <volm/volm_camera_space_sptr.h>
#include <volm/volm_io.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>

static void test_query()
{
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
  //vsol_polygon_2d_sptr gp = new vsol_polygon_2d(verts);
  //depth_scene->add_ground(gp, 0.0, 0.0, 0, "beach", 6);
  // add an object
  vsol_point_2d_sptr pb0 = new vsol_point_2d(640.0, 400.0);
  vsol_point_2d_sptr pb1 = new vsol_point_2d(940.0, 600.0);
  vsol_point_2d_sptr pb2 = new vsol_point_2d(340.0, 200.0);
  std::vector<vsol_point_2d_sptr> verts_bd;
  verts_bd.push_back(pb0);  verts_bd.push_back(pb1);  verts_bd.push_back(pb2);
  vsol_polygon_2d_sptr bp = new vsol_polygon_2d(verts_bd);
  vgl_vector_3d<double> np(1.0, 1.0, 0.0);
  depth_scene->add_region(bp, np, 100.0, 1000.0, "hotel", depth_map_region::FRONT_PARALLEL, 1, 15);
  std::string dms_bin_file = "./depth_map_scene.bin";
  vsl_b_ofstream ofs_dms(dms_bin_file);
  depth_scene->b_write(ofs_dms);
  ofs_dms.close();

  // create a camera space
  double head_mid=180.0, head_radius=10.0, head_inc=5.0;
  double tilt_mid=90.0, tilt_radius=0.0, tilt_inc=2.0;
  double roll_mid=0.0,  roll_radius=0.0,  roll_inc=0.0;
  //double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  double top_fov_vals[] = {20.0, 24.0};
  std::vector<double> fovs(top_fov_vals, top_fov_vals + 5);
  double altitude = 1.6;
  volm_camera_space_sptr csp = new volm_camera_space(fovs, altitude, ni, nj,
                                                     head_mid, head_radius, head_inc,
                                                     tilt_mid, tilt_radius, tilt_inc,
                                                     roll_mid, roll_radius, roll_inc);
  if (depth_scene->ground_plane().size() > 0)  // enforce ground plane constraint if user specified a ground plane
  {
    camera_space_iterator cit = csp->begin();
    for ( ; cit != csp->end(); ++cit) {
      unsigned current = csp->cam_index();
      vpgl_perspective_camera<double> cam = csp->camera(); // camera at current state of iterator
      bool success = true;
      for (unsigned i = 0; success && i < depth_scene->ground_plane().size(); i++)
        success = depth_scene->ground_plane()[i]->region_ground_2d_to_3d(cam);
      if (success) // add this camera
        csp->add_camera_index(current);
    }
  }
  else
    csp->generate_full_camera_index_space();

  // create depth interval
  float vmin = 2.0f;
  float dmax = 3000.0f;
  float solid_angle = 2.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle, vmin, dmax);

  // create spherical shell container
  float cap_angle = 180;
  float point_angle = 5;
  double radius = 1;
  float top_angle = 70;
  float bottom_angle = 70;
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(radius, cap_angle, point_angle, top_angle, bottom_angle);

  // create volm_query
  volm_query_sptr query = new volm_query(csp, dms_bin_file, sph_shell, sph);

  // query check
  // sky
  std::vector<std::vector<unsigned> >& sky_id = query->sky_id();
  std::vector<unsigned>& sky_offset = query->sky_offset();
  TEST("size of sky id equals numbers of n_cam", sky_id.size(), query->get_cam_num());
  TEST("size of sky offset equals number of n_cam+1", sky_offset.size(), 1+query->get_cam_num());
  depth_map_scene_sptr dm = query->depth_scene();
  std::cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << std::endl;
  if (dm->sky().size()) {
    std::cout << " -------------- SKYs --------------" << std::endl;
    for (unsigned i = 0; i < dm->sky().size(); i++) {
      std::cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << ", land_id = " << dm->sky()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dm->sky()[i]->land_id())
               << ", land_fallback_category = ";
      volm_fallback_label::print_id(dm->sky()[i]->land_id());
      std::cout << ", land_fallback_weight = " ;
      volm_fallback_label::print_wgt(dm->sky()[i]->land_id());
      std::cout << std::endl;
    }
  }

  // ground
  if (!dm->ground_plane().empty()) {
    std::vector<std::vector<unsigned> >&      grd_id = query->ground_id();
    std::vector<std::vector<unsigned char> >& grd_dist = query->ground_dist();
    std::vector<unsigned>&                   grd_offset = query->ground_offset();
    std::vector<std::vector<std::vector<unsigned char> > >& grd_land = query->ground_land_id();
    TEST("size of grd_id vector equals n_cam",   grd_id.size(),   query->get_cam_num());
    TEST("size of grd_dist vector equals n_cam", grd_dist.size(), query->get_cam_num());
    TEST("size of grd_land vector equals n_cam", grd_land.size(), query->get_cam_num());
    TEST("size of grd_offset equals n_cam+1", grd_offset.size(), 1+query->get_cam_num());

    if (dm->ground_plane().size()) {
      std::cout << " -------------- GROUND PLANE --------------" << std::endl;
      for (unsigned i = 0; i < dm->ground_plane().size(); i++) {
        std::cout << "\t name = " << dm->ground_plane()[i]->name()
                 << ", depth = " << dm->ground_plane()[i]->min_depth()
                 << ", orient = " << dm->ground_plane()[i]->orient_type()
                 << ", land_id = " << dm->ground_plane()[i]->land_id()
                 << ", land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
                 << ", land_fallback = ";
        volm_fallback_label::print_id(dm->ground_plane()[i]->land_id());
        std::cout << ", land_fallback_wgt = ";
        volm_fallback_label::print_wgt(dm->ground_plane()[i]->land_id());
        std::cout << std::endl;
      }
      std::vector<unsigned char> grd_single_ray = grd_land[0][0];
      std::cout << " For a single ray on ground region: land_fallback has size " << grd_single_ray.size() << " = ";
      for (unsigned char & vit : grd_single_ray) {
        std::cout << volm_label_table::land_string(vit) << ", ";
      }
      std::cout << std::endl;
    }
  }


  // object
  std::vector<std::vector<std::vector<unsigned> > >& obj_id = query->dist_id();
  std::vector<unsigned>& obj_offset = query->dist_offset();
  std::vector<unsigned char>& obj_min = query->min_obj_dist();
  std::vector<unsigned char>& obj_orient = query->obj_orient();
  std::vector<std::vector<unsigned char> >& obj_land = query->obj_land_id();
  std::vector<std::vector<float> >& obj_land_wgt = query->obj_land_wgt();
  std::vector<depth_map_region_sptr> drs = query->depth_regions();

  TEST("size of object id vector equals n_cam", obj_id.size(), query->get_cam_num());
  TEST("size of object id per camera equals n_cam", (obj_id[0]).size(), drs.size());
  TEST("size of object offset equals 1+n_cam*n_obj", obj_offset.size(), 1+drs.size()*query->get_cam_num());
  TEST("size of land_id vector equals n_obj", obj_land.size(), drs.size());
  TEST("size of land_id_wgt equals  n_obj", obj_land_wgt.size(), drs.size());
  TEST("size of min_dist vector equals n_obj", obj_min.size(), drs.size());
  TEST("size of orient vector equals n_obj", obj_orient.size(), drs.size());
  TEST("size of land_fallback_label equals 4", obj_land[0].size(), 4);
  TEST("size of land_fallback_wgt equals 4", obj_land_wgt[0].size(), 4);

  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); i++) {
      std::cout << "\t " <<  drs[i]->name()
               << " region,\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << ",\t fallback_category = ";
      volm_fallback_label::print_id(drs[i]->land_id());
      std::cout << " (";
      for (unsigned char jj : obj_land[i])
        std::cout << volm_label_table::land_string(jj) << ", ";
      std::cout << " ),\t fallback_wgt = ";
      volm_fallback_label::print_wgt(drs[i]->land_id());
      std::cout << " (";
      for (float jj : obj_land_wgt[i])
        std::cout << jj << ' ';
      std::cout << ')' << std::endl;
    }
  }
}


TESTMAIN(test_query);
