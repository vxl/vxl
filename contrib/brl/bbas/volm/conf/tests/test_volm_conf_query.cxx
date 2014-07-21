#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_query.h>
#include <vul/vul_file.h>

static void test_volm_conf_query()
{
  // load the depth_map_scene
  vcl_string depth_scene_file = "d:/work/find/conf_matcher_expt/p1a_test1_40/p1a_test1_40.vsl";
  if (!vul_file::exists(depth_scene_file)) {
    vcl_cout << "can not find file: " << depth_scene_file << vcl_endl;
    return;
  }
  depth_map_scene_sptr dms = new depth_map_scene;
  vsl_b_ifstream is(depth_scene_file.c_str());
  dms->b_read(is);
  is.close();

  unsigned ni = dms->ni();
  unsigned nj = dms->nj();
  vcl_cout << "query image size: " << ni << "x" << nj << vcl_endl;
  vcl_cout << "input depth map scene has following labeled object:" << vcl_endl;
  unsigned num_of_ref_objects = 0;
  if (dms->sky().size()) {
    vcl_cout << "----- sky -----" << vcl_endl;
    for (unsigned i = 0; i < dms->sky().size(); i++) {
      vcl_cout << "name = " << dms->sky()[i]->name() << ", is_ref = " << dms->sky()[i]->is_ref() << ", land = sky, orientation = invalid, min_dist = 1E6" << vcl_endl;
    }
  }
  if (dms->ground_plane().size()) {
    vcl_cout << "----- ground -----" << vcl_endl;
    for (unsigned i = 0; i < dms->ground_plane().size(); i++) {
      vcl_cout << "name = " << dms->ground_plane()[i]->name() << ", is_ref = " << dms->ground_plane()[i]->is_ref()
               << ", land = " << volm_osm_category_io::volm_land_table[dms->ground_plane()[i]->land_id()].name_ << ", orient = " << dms->ground_plane()[i]->orient_type()
               << ", min_dist = " << dms->ground_plane()[i]->min_depth() << vcl_endl;
      if (dms->ground_plane()[i]->is_ref())
        num_of_ref_objects++;
    }
  }
  if (dms->scene_regions().size()) {
    vcl_cout << "----- label object -----" << vcl_endl;
    for (unsigned i = 0; i < dms->scene_regions().size(); i++) {
      vcl_cout << "name = " << dms->scene_regions()[i]->name() << ", is_ref = " << dms->scene_regions()[i]->is_ref()
               << ", land = " << volm_osm_category_io::volm_land_table[dms->scene_regions()[i]->land_id()].name_ << ", orient = " << dms->scene_regions()[i]->orient_type()
               << ", min_dist = " << dms->scene_regions()[i]->min_depth() << vcl_endl;
      if (dms->scene_regions()[i]->is_ref())
        num_of_ref_objects++;
    }
  }
  // create a camera space
  double head_mid=67.0,   head_radius=3.0,  head_inc=2.0;
  double tilt_mid=87.7,   tilt_radius=3.0,  tilt_inc=2.0;
  double roll_mid=-0.74,  roll_radius=0.0,  roll_inc=0.0;
  //double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  double top_fov_vals[] = {5.3, 7.3, 3.3};
  vcl_vector<double> fovs(top_fov_vals, top_fov_vals + 3);

  for (vcl_vector<double>::iterator vit = fovs.begin();  vit != fovs.end();  ++vit)
    vcl_cout << *vit << ", ";
  vcl_cout << vcl_endl;

  double altitude = 3.0;
  volm_camera_space_sptr csp = new volm_camera_space(fovs, altitude, ni, nj,
                                                     head_mid, head_radius, head_inc,
                                                     tilt_mid, tilt_radius, tilt_inc,
                                                     roll_mid, roll_radius, roll_inc);

  if (dms->ground_plane().size() > 0)  // enforce ground plane constraint if user specified a ground plane
  {
    camera_space_iterator cit = csp->begin();
    for ( ; cit != csp->end(); ++cit) {
      unsigned current = csp->cam_index();
      vpgl_perspective_camera<double> cam = csp->camera(); // camera at current state of iterator
      bool success = true;
      for (unsigned i = 0; success && i < dms->ground_plane().size(); i++)
        success = dms->ground_plane()[i]->region_ground_2d_to_3d(cam);
      if (success) // add this camera
        csp->add_camera_index(current);
    }
  }
  else
    csp->generate_full_camera_index_space();

  // construct a volm_conf_queue
  volm_conf_query_sptr query = new volm_conf_query(csp, dms);

  vcl_cout << "configurational query reference object list: ";
  vcl_vector<vcl_string> ref_object_names = query->ref_obj_name();
  for (vcl_vector<vcl_string>::iterator vit = ref_object_names.begin(); vit != ref_object_names.end(); ++vit)
    vcl_cout << (*vit) << ", ";
  vcl_cout << vcl_endl;

  vcl_cout << "configuration query camera list: " << vcl_endl;
  vcl_vector<vcl_string> cam_string = query->camera_strings();
  for (vcl_vector<vcl_string>::iterator vit = cam_string.begin();  vit != cam_string.end();  ++vit)
    vcl_cout << "\t" << (*vit) << vcl_endl;
  vcl_cout << query->cameras().size() << " perspective cameras are created." << vcl_endl;
  TEST("configurational query cameras", query->cameras().size(), query->camera_angles().size());
  TEST("configurational query reference objects", query->ref_obj_name().size(), num_of_ref_objects);


  vcl_vector<vpgl_perspective_camera<double> > cam = query->cameras();
  for (unsigned i = 0; i < cam.size(); i++) {
    vcl_cout << "camera " << i << " has center " << cam[i].camera_center() << vcl_endl;
  }
}

TESTMAIN( test_volm_conf_query );