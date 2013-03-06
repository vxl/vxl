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

static void test_query()
{
#if 1
  // input files -- check the generate binary query
  vcl_string cam_bin_file = "Z:/projects/FINDER/test1_result/local_output/test_query_binary_gt_pa_5/p1a_test1_40/camera_space.bin";
  vcl_string dms_bin_file = "Z:/projects/FINDER/test1/p1a_test1_40/p1a_test1_40.vsl";
  vcl_string sph_shell_file = "Z:/projects/FINDER/index/sph_shell_vsph_ca_180_pa_5_ta_75_ba_75.bin";
  vcl_string query_bin = "Z:/projects/FINDER/test1_result/local_output/test_query_binary_gt_pa_5/p1a_test1_40/p1a_test1_40_query.bin";

  // parameter for depth_map_interval
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 2.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  // load the spherical_shell_container
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  vsl_b_ifstream sph_ifs(sph_shell_file);
  sph_shell->b_read(sph_ifs);
  sph_ifs.close();


  // load cam_space
  vsl_b_ifstream cam_ifs(cam_bin_file);
  volm_camera_space_sptr csp_in = new volm_camera_space;
  csp_in->b_read(cam_ifs);

  volm_query query_test(query_bin, csp_in, dms_bin_file, sph_shell, sph);

  volm_query_sptr query = &query_test;
  // query check
  // sky
  vcl_vector<vcl_vector<unsigned> >& sky_id = query->sky_id();
  vcl_vector<unsigned>& sky_offset = query->sky_offset();
  vcl_cout << "sky_id.size() = " << sky_id.size() << vcl_endl;
  TEST("size of sky id vector equals numbers of camera", sky_id.size(), query->get_cam_num());
  TEST("size of sky offset vector equals number of cameras + 1", sky_offset.size(), 1+query->get_cam_num());
  depth_map_scene_sptr dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++) {
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << ", land_id = " << dm->sky()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dm->sky()[i]->land_id())
               << ", land_fallback_category = ";
      volm_fallback_label::print_id(dm->sky()[i]->land_id());
      vcl_cout << ", land_fallback_weight = " ;
      volm_fallback_label::print_wgt(dm->sky()[i]->land_id());
      vcl_cout << vcl_endl;
    }
  }

  // ground
  if (!dm->ground_plane().empty()) {
    vcl_vector<vcl_vector<unsigned> >&      grd_id = query->ground_id();
    vcl_vector<vcl_vector<unsigned char> >& grd_dist = query->ground_dist();
    vcl_vector<unsigned>&                   grd_offset = query->ground_offset();
    vcl_vector<vcl_vector<vcl_vector<unsigned char> > >& grd_land = query->ground_land_id();
    TEST("size of grd_id vector equals n_cam",   grd_id.size(),   query->get_cam_num());
    TEST("size of grd_dist vector equals n_cam", grd_dist.size(), query->get_cam_num());
    TEST("size of grd_land vector euqals n_cam", grd_land.size(), query->get_cam_num());
    TEST("size of grd_offset euqals n_cam+1", grd_offset.size(), 1+query->get_cam_num());

    if (dm->ground_plane().size()) {
      vcl_cout << " -------------- GROUND PLANE --------------" << vcl_endl;
      for (unsigned i = 0; i < dm->ground_plane().size(); i++) {
        vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
                 << ", depth = " << dm->ground_plane()[i]->min_depth()
                 << ", orient = " << dm->ground_plane()[i]->orient_type()
                 << ", land_id = " << dm->ground_plane()[i]->land_id()
                 << ", land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
                 << ", land_fallback = ";
        volm_fallback_label::print_id(dm->ground_plane()[i]->land_id());
        vcl_cout << ", land_fallback_wgt = ";
        volm_fallback_label::print_wgt(dm->ground_plane()[i]->land_id());
        vcl_cout << vcl_endl;
      }
      // test a ray
      vcl_vector<unsigned char> grd_single_ray = grd_land[0][0];
      vcl_cout << " For a single ray on ground region: land_fallback has size " << grd_single_ray.size() << " = ";
      for (vcl_vector<unsigned char>::iterator vit = grd_single_ray.begin(); vit != grd_single_ray.end(); ++vit) {
        vcl_cout << volm_label_table::land_string(*vit) << ", ";
      }
      vcl_cout << vcl_endl;
    }
  }


  // object
  vcl_vector<vcl_vector<vcl_vector<unsigned> > >& obj_id = query->dist_id();
  vcl_vector<unsigned>& obj_offset = query->dist_offset();
  vcl_vector<unsigned char>& obj_min = query->min_obj_dist();
  vcl_vector<unsigned char>& obj_orient = query->obj_orient();
  vcl_vector<vcl_vector<unsigned char> >& obj_land = query->obj_land_id();
  vcl_vector<vcl_vector<float> >& obj_land_wgt = query->obj_land_wgt();
  vcl_vector<depth_map_region_sptr> drs = query->depth_regions();

  TEST("size of object id vector should equal to n_cam", obj_id.size(), query->get_cam_num());
  TEST("size of object id vector per camera should equal n_cam", (obj_id[0]).size(), drs.size());
  TEST("size of object offset vector should equal to 1+n_cam*n_obj", obj_offset.size(), 1+drs.size()*query->get_cam_num());
  TEST("size of land_id vector should equal to n_obj", obj_land.size(), drs.size());
  TEST("size of land_id_wgt should equal to n_obj", obj_land_wgt.size(), drs.size());
  TEST("size of min_dist vector should equal to n_obj", obj_min.size(), drs.size());
  TEST("size of orient vector should equal to n_obj", obj_orient.size(), drs.size());
  TEST("size of land_fallback_label should equal 4", obj_land[0].size(), 4);
  TEST("size of land_fallback_wgt should equal 4", obj_land_wgt[0].size(), 4);

  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); i++) {
      vcl_cout << "\t " <<  drs[i]->name()
               << " region,\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << ",\t fallback_category = ";
      volm_fallback_label::print_id(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj =0; jj < obj_land[i].size(); jj++)
        vcl_cout << volm_label_table::land_string(obj_land[i][jj]) << ", ";
      vcl_cout << " ),\t fallback_wgt = ";
      volm_fallback_label::print_wgt(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj = 0; jj < obj_land_wgt[i].size(); jj++)
        vcl_cout << obj_land_wgt[i][jj] << ' ';
      vcl_cout << ')' << vcl_endl;
    }
  }
#endif

#if 0
  // input files
  vcl_string cam_bin_file = "Z:/projects/FINDER/test1_result/local_output/test_weight_gt_pa_5/p1a_test1_36/camera_space.bin";
  vcl_string dms_bin_file = "Z:/projects/FINDER/test1/p1a_test1_36/p1a_test1_36.vsl";
  vcl_string sph_shell_file = "Z:/projects/FINDER/index/sph_shell_vsph_ca_180_pa_5_ta_75_ba_75.bin";

  // parameter for depth_map_interval
  float vmin = 2.0f;         // min voxel resolution
  float dmax = 3000.0f;      // maximum depth
  float solid_angle = 5.0f;
  volm_spherical_container_sptr sph = new volm_spherical_container(solid_angle,vmin,dmax);

  // load the spherical_shell_container
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container;
  vsl_b_ifstream sph_ifs(sph_shell_file);
  sph_shell->b_read(sph_ifs);
  sph_ifs.close();


  // load cam_space
  vsl_b_ifstream cam_ifs(cam_bin_file);
  volm_camera_space_sptr csp_in = new volm_camera_space;
  csp_in->b_read(cam_ifs);

#if 0 // nested if !!
  double head_mid=0.0, head_radius=180.0, head_inc=90.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0};
  vcl_vector<double> fovs(top_fov_vals, top_fov_vals + 2);
  double altitude = 1.6;
  unsigned ni = 4000, nj = 3000;
  volm_camera_space_sptr csp_in = new volm_camera_space(fovs, altitude, ni, nj, head_mid, head_radius, head_inc, tilt_mid, tilt_radius, tilt_inc, roll_mid, roll_radius, roll_inc);
  csp_in->generate_full_camera_index_space();  // don't bother with removing cams that don't satisfy ground plane constraint for testing purposes
#endif // 0

  // create volm_query
  volm_query_sptr query = new volm_query(csp_in, dms_bin_file, sph_shell, sph);

  // screen output for query information
  unsigned total_size = query->obj_based_query_size_byte();
  vcl_cout << " For spherical surface, point angle = " << sph_shell->point_angle()
           << " degree, top_angle = " << sph_shell->top_angle()
           << " degree, bottom_angle = " << sph_shell->bottom_angle()
           << " degree, generated query has " << query->get_query_size() << " rays, "
           << query->get_cam_num() << " cameras:\n"
           << " The query with " << query->get_cam_num() << " has " << (float)total_size/1024 << " Kbyte in total"
           << vcl_endl;

  // query check
  // sky
  vcl_vector<vcl_vector<unsigned> >& sky_id = query->sky_id();
  vcl_vector<unsigned>& sky_offset = query->sky_offset();
  TEST("size of sky id vector equals numbers of camera", sky_id.size(), query->get_cam_num());
  TEST("size of sky offset vector equals number of cameras + 1", sky_offset.size(), 1+query->get_cam_num());
  depth_map_scene_sptr dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++) {
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query->sky_orient()
               << ", land_id = " << dm->sky()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dm->sky()[i]->land_id())
               << ", land_fallback_category = ";
      volm_fallback_label::print_id(dm->sky()[i]->land_id());
      vcl_cout << ", land_fallback_weight = " ;
      volm_fallback_label::print_wgt(dm->sky()[i]->land_id());
      vcl_cout << vcl_endl;
    }
  }

  // ground
  if (!dm->ground_plane().empty()) {
    vcl_vector<vcl_vector<unsigned> >&      grd_id = query->ground_id();
    vcl_vector<vcl_vector<unsigned char> >& grd_dist = query->ground_dist();
    vcl_vector<unsigned>&                   grd_offset = query->ground_offset();
    vcl_vector<vcl_vector<vcl_vector<unsigned char> > >& grd_land = query->ground_land_id();
    TEST("size of grd_id vector equals n_cam",   grd_id.size(),   query->get_cam_num());
    TEST("size of grd_dist vector equals n_cam", grd_dist.size(), query->get_cam_num());
    TEST("size of grd_land vector euqals n_cam", grd_land.size(), query->get_cam_num());
    TEST("size of grd_offset euqals n_cam+1", grd_offset.size(), 1+query->get_cam_num());

    if (dm->ground_plane().size()) {
      vcl_cout << " -------------- GROUND PLANE --------------" << vcl_endl;
      for (unsigned i = 0; i < dm->ground_plane().size(); i++) {
        vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
                 << ", depth = " << dm->ground_plane()[i]->min_depth()
                 << ", orient = " << dm->ground_plane()[i]->orient_type()
                 << ", land_id = " << dm->ground_plane()[i]->land_id()
                 << ", land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
                 << ", land_fallback = ";
        volm_fallback_label::print_id(dm->ground_plane()[i]->land_id());
        vcl_cout << ", land_fallback_wgt = ";
        volm_fallback_label::print_wgt(dm->ground_plane()[i]->land_id());
        vcl_cout << vcl_endl;
      }
      // test a ray
      vcl_vector<unsigned char> grd_single_ray = grd_land[0][0];
      vcl_cout << " For a single ray on ground region: land_fallback has size " << grd_single_ray.size() << " = ";
      for (vcl_vector<unsigned char>::iterator vit = grd_single_ray.begin(); vit != grd_single_ray.end(); ++vit) {
        vcl_cout << volm_label_table::land_string(*vit) << ", ";
      }
      vcl_cout << vcl_endl;
    }
  }

  // object
  vcl_vector<vcl_vector<vcl_vector<unsigned> > >& obj_id = query->dist_id();
  vcl_vector<unsigned>& obj_offset = query->dist_offset();
  vcl_vector<unsigned char>& obj_min = query->min_obj_dist();
  vcl_vector<unsigned char>& obj_orient = query->obj_orient();
  vcl_vector<vcl_vector<unsigned char> >& obj_land = query->obj_land_id();
  vcl_vector<vcl_vector<float> >& obj_land_wgt = query->obj_land_wgt();
  vcl_vector<depth_map_region_sptr> drs = query->depth_regions();

  TEST("size of object id vector should equal to n_cam", obj_id.size(), query->get_cam_num());
  TEST("size of object id vector per camera should equal n_cam", (obj_id[0]).size(), drs.size());
  TEST("size of object offset vector should equal to 1+n_cam*n_obj", obj_offset.size(), 1+drs.size()*query->get_cam_num());
  TEST("size of land_id vector should equal to n_obj", obj_land.size(), drs.size());
  TEST("size of land_id_wgt should equal to n_obj", obj_land_wgt.size(), drs.size());
  TEST("size of min_dist vector should equal to n_obj", obj_min.size(), drs.size());
  TEST("size of orient vector should equal to n_obj", obj_orient.size(), drs.size());
  TEST("size of land_fallback_label should equal 4", obj_land[0].size(), 4);
  TEST("size of land_fallback_wgt should equal 4", obj_land_wgt[0].size(), 4);

  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); i++) {
      vcl_cout << "\t " <<  drs[i]->name()
               << " region,\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << ",\t fallback_category = ";
      volm_fallback_label::print_id(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj =0; jj < obj_land[i].size(); jj++)
        vcl_cout << volm_label_table::land_string(obj_land[i][jj]) << ", ";
      vcl_cout << " ),\t fallback_wgt = ";
      volm_fallback_label::print_wgt(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj = 0; jj < obj_land_wgt[i].size(); jj++)
        vcl_cout << obj_land_wgt[i][jj] << ' ';
      vcl_cout << ')' << vcl_endl;
    }
  }

  vsl_b_ofstream ofs("./test_query.bin");
  query->write_data(ofs);
  ofs.close();

  volm_query query_test("./test_query.bin", csp_in, dms_bin_file, sph_shell, sph);
  TEST("binary i/o", *query == query_test, true);
#endif // 0


#if 0
  // query check
  // sky
  sky_id = query_test.sky_id();
  sky_offset = query_test.sky_offset();
  TEST("size of sky id vector equals numbers of camera", sky_id.size(), query_test.get_cam_num());
  TEST("size of sky offset vector equals number of cameras + 1", sky_offset.size(), 1+query_test.get_cam_num());
  dm = query_test.depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
    vcl_cout << " -------------- SKYs --------------" << vcl_endl;
    for (unsigned i = 0; i < dm->sky().size(); i++) {
      vcl_cout << "\t name = " << (dm->sky()[i]->name())
               << ", depth = " << 254
               << ", orient = " << (int)query_test.sky_orient()
               << ", land_id = " << dm->sky()[i]->land_id()
               << ", land_name = " << volm_label_table::land_string(dm->sky()[i]->land_id())
               << ", land_fallback_category = ";
      volm_fallback_label::print_id(dm->sky()[i]->land_id());
      vcl_cout << ", land_fallback_weight = " ;
      volm_fallback_label::print_wgt(dm->sky()[i]->land_id());
      vcl_cout << vcl_endl;
    }
  }

  // ground
  if (!dm->ground_plane().empty()) {
    vcl_vector<vcl_vector<unsigned> >&      grd_id = query_test.ground_id();
    vcl_vector<vcl_vector<unsigned char> >& grd_dist = query_test.ground_dist();
    vcl_vector<unsigned>&                   grd_offset = query_test.ground_offset();
    vcl_vector<vcl_vector<vcl_vector<unsigned char> > >& grd_land = query_test.ground_land_id();
    TEST("size of grd_id vector equals n_cam",   grd_id.size(),   query_test.get_cam_num());
    TEST("size of grd_dist vector equals n_cam", grd_dist.size(), query_test.get_cam_num());
    TEST("size of grd_land vector euqals n_cam", grd_land.size(), query_test.get_cam_num());
    TEST("size of grd_offset euqals n_cam+1", grd_offset.size(), 1+query_test.get_cam_num());

    if (dm->ground_plane().size()) {
      vcl_cout << " -------------- GROUND PLANE --------------" << vcl_endl;
      for (unsigned i = 0; i < dm->ground_plane().size(); i++) {
        vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
                 << ", depth = " << dm->ground_plane()[i]->min_depth()
                 << ", orient = " << dm->ground_plane()[i]->orient_type()
                 << ", land_id = " << dm->ground_plane()[i]->land_id()
                 << ", land_name = " << volm_label_table::land_string(dm->ground_plane()[i]->land_id())
                 << ", land_fallback = ";
        volm_fallback_label::print_id(dm->ground_plane()[i]->land_id());
        vcl_cout << ", land_fallback_wgt = ";
        volm_fallback_label::print_wgt(dm->ground_plane()[i]->land_id());
        vcl_cout << vcl_endl;
      }
      // test a ray
      vcl_vector<unsigned char> grd_single_ray = grd_land[0][0];
      vcl_cout << " For a single ray on ground region: land_fallback has size " << grd_single_ray.size() << " = ";
      for (vcl_vector<unsigned char>::iterator vit = grd_single_ray.begin(); vit != grd_single_ray.end(); ++vit) {
        vcl_cout << volm_label_table::land_string(*vit) << ", ";
      }
      vcl_cout << vcl_endl;
    }
  }

  // object
  obj_id = query_test.dist_id();
  obj_offset = query_test.dist_offset();
  obj_min = query_test.min_obj_dist();
  obj_orient = query_test.obj_orient();
  obj_land = query_test.obj_land_id();
  obj_land_wgt = query_test.obj_land_wgt();
  drs = query_test.depth_regions();

  TEST("size of object id vector should equal to n_cam", obj_id.size(), query_test.get_cam_num());
  TEST("size of object id vector per camera should equal n_cam", (obj_id[0]).size(), drs.size());
  TEST("size of object offset vector should equal to 1+n_cam*n_obj", obj_offset.size(), 1+drs.size()*query_test.get_cam_num());
  TEST("size of land_id vector should equal to n_obj", obj_land.size(), drs.size());
  TEST("size of land_id_wgt should equal to n_obj", obj_land_wgt.size(), drs.size());
  TEST("size of min_dist vector should equal to n_obj", obj_min.size(), drs.size());
  TEST("size of orient vector should equal to n_obj", obj_orient.size(), drs.size());
  TEST("size of land_fallback_label should equal 4", obj_land[0].size(), 4);
  TEST("size of land_fallback_wgt should equal 4", obj_land_wgt[0].size(), 4);

  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); i++) {
      vcl_cout << "\t " <<  drs[i]->name()
               << " region,\t min_depth = " << drs[i]->min_depth()
               << ",\t max_depth = " << drs[i]->max_depth()
               << ",\t order = " << drs[i]->order()
               << ",\t orient = " << drs[i]->orient_type()
               << ",\t land_id = " << drs[i]->land_id()
               << ",\t land_name = " << volm_label_table::land_string( drs[i]->land_id() )
               << ",\t fallback_category = ";
      volm_fallback_label::print_id(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj =0; jj < obj_land[i].size(); jj++)
        vcl_cout << volm_label_table::land_string(obj_land[i][jj]) << ", ";
      vcl_cout << " ),\t fallback_wgt = ";
      volm_fallback_label::print_wgt(drs[i]->land_id());
      vcl_cout << " (";
      for (unsigned jj = 0; jj < obj_land_wgt[i].size(); jj++)
        vcl_cout << obj_land_wgt[i][jj] << ' ';
      vcl_cout << ')' << vcl_endl;
    }
  }
#endif

#if 0
  vcl_string depth_scene_path = "c:/Users/mundy/VisionSystems/Finder/VolumetricQuery/Queries/p1a_res06_dirtroad_depthscene_v2.vsl";
  // create the query
  volm_query_sptr query = new volm_query(depth_scene_path, "desert", sph, sph_shell, 1.6);

  // query infomation
  vcl_cout << " for spherical surface, point angle = " << point_angle
           << " degree, top_angle = " << top_angle
           << ", bottom_angle = " << bottom_angle
           << ", number of rays = " << query->get_query_size()
           << "\n The query has " << query->get_cam_num() << " cameras:"
           << vcl_endl;
  // the depth_map_scene stored in query (can be used to fetch all attributes and polygons)
  depth_map_scene_sptr dm = query->depth_scene();
  vcl_cout << " The " << dm->ni() << " x " << dm->nj() << " query image has following defined depth region" << vcl_endl;
  if (dm->sky().size()) {
  vcl_cout << " -------------- SKY --------------" << vcl_endl;
  for (unsigned i = 0; i < dm->sky().size(); ++i) {
    vcl_cout << "\t name = " << (dm->sky()[i]->name())
             << ", depth = " << 254
             << ", orient = " << dm->sky()[i]->orient_type()
             << ", NLCD_id = " << dm->sky()[i]->nlcd_id()
             << " ---> " << (int)volm_nlcd_table::land_id[dm->sky()[i]->nlcd_id()]
             << vcl_endl;
    }
  }
  if (dm->ground_plane().size()) {
    vcl_cout << " -------------- GROUND_PLANE ------" << vcl_endl;
    for (unsigned i = 0; i < dm->ground_plane().size(); ++i) {
      vcl_cout << "\t name = " << dm->ground_plane()[i]->name()
               << ", depth = " << dm->ground_plane()[i]->min_depth()
               << ", orient = " << dm->ground_plane()[i]->orient_type()
               << ", NLCD_id = " << dm->ground_plane()[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dm->ground_plane()[i]->nlcd_id()]
               << vcl_endl;
    }
  }
  if (dm->scene_regions().size()) {
    vcl_cout << " --------------- OBJECTS ------------------" << vcl_endl;
    for (unsigned i = 0; i < dm->scene_regions().size(); ++i) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()
               << " region,\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << ",\t orient = " << (dm->scene_regions())[i]->orient_type()
               << ",\t NLCD_id = " << (dm->scene_regions())[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[dm->scene_regions()[i]->nlcd_id()]
               << vcl_endl;
    }
  }

#if 0
  // attributes of all non_sky/non_ground objects
  vcl_vector<depth_map_region_sptr> drs = query->depth_regions();
  vcl_cout << " The depth regions map inside query follows on order" << vcl_endl;
  if (drs.size()) {
    for (unsigned i = 0; i < drs.size(); ++i) {
      vcl_cout << "\t " << drs[i]->name()
               << " region,\t\t order = " << drs[i]->order()
               << ",\t min_dist = " << drs[i]->min_depth()
               << ",\t min_dist_interval = " << (int)sph->get_depth_interval(drs[i]->min_depth())
               << ",\t max_dist = " << drs[i]->max_depth()
               << ",\t max_dist_interval = " << (int)sph->get_depth_interval(drs[i]->max_depth())
               << ",\t orientation = " << drs[i]->orient_type()
               << ",\t NLCD = " << drs[i]->nlcd_id()
               << " ---> " << (int)volm_nlcd_table::land_id[drs[i]->nlcd_id()]
               << vcl_endl;
    }
  }
#endif

  TEST("number of rays for current query", query->get_query_size(), 29440); // for 2 degree resolution
#endif//NEED FILES TO RUN!! JLM
}


TESTMAIN(test_query);
