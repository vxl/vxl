#include <testlib/testlib_test.h>
#include <volm/conf/volm_conf_query.h>
#include <vul/vul_file.h>
#include <vsph/vsph_spherical_coord.h>
#include <vsph/vsph_sph_point_3d.h>
#include <bpgl/bpgl_camera_utils.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

double line_coord(vgl_line_2d<double> const& line, double const& x)
{
  if (line.b() == 0)
    return 0.0;
  else
    return -line.a()/line.b()*x - line.c()/line.b();
}

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
  double head_mid=67.0,   head_radius=0.0,  head_inc=2.0;
  double tilt_mid=87.70,   tilt_radius=0.0,  tilt_inc=2.0;
  double roll_mid=-0.74,  roll_radius=0.0,  roll_inc=0.0;
  //double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  double top_fov_vals[] = {5.3};
  vcl_vector<double> fovs;
  fovs.push_back(top_fov_vals[0]);


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

  vcl_cout << "configurational query camera list: " << vcl_endl;
  vcl_vector<vcl_string> cam_string = query->camera_strings();
  for (vcl_vector<vcl_string>::iterator vit = cam_string.begin();  vit != cam_string.end();  ++vit)
    vcl_cout << "\t" << (*vit) << vcl_endl;
  vcl_cout << query->cameras().size() << " perspective cameras are created." << vcl_endl;
  TEST("configurational query cameras", query->cameras().size(), query->camera_angles().size());
  TEST("configurational query reference objects", query->ref_obj_name().size(), num_of_ref_objects);

  vcl_cout << "configurational query has following reference object: " << vcl_endl;
  vcl_vector<vcl_string> query_ref_obj = query->ref_obj_name();
  for (vcl_vector<vcl_string>::iterator vit = query_ref_obj.begin();  vit != query_ref_obj.end();  ++vit)
    vcl_cout << " " << (*vit);
  vcl_cout << vcl_endl;

  vcl_cout << "configurational query has following configuration object: " << vcl_endl;
  vcl_vector<vcl_map<vcl_string, volm_conf_object_sptr> > conf_objs = query->conf_objects();
  for (unsigned i = 0; i < conf_objs.size(); i++) {
    vcl_cout << "\t camera: " << cam_string[i] << vcl_endl;
    for (vcl_map<vcl_string, volm_conf_object_sptr>::iterator mit = conf_objs[i].begin();  mit != conf_objs[i].end(); ++mit) {
      vcl_cout << "\t\t obj name: " << mit->first << "\t\t" ;  mit->second->print(vcl_cout);
    }
  }



  // visualize the configurational query
  vcl_string out_folder = "d:/work/find/conf_matcher_expt/p1a_test1_40/";
  vcl_string img_file = "d:/work/find/conf_matcher_expt/p1a_test1_40/p1a_test1_40.jpg";
  query->visualize_ref_objs(img_file, out_folder);
  query->generate_top_views(out_folder);

  
#if 0
  vcl_vector<vpgl_perspective_camera<double> > cam = query->cameras();
  // get the horizon line
  // obtain camera center
  vgl_homg_point_3d<double> cam_center = cam[0].camera_center();
  // obtain the horizon line
  vgl_line_2d<double> h_line = bpgl_camera_utils::horizon(cam[0]);
  vcl_cout << "\t camera center: " << cam_center << ", horizontal line: " << h_line << vcl_endl;

  double i, j;
  i = 1500;  j = 2000;
  vgl_ray_3d<double> l_ray = cam[0].backproject_ray(i, j);
  vsph_spherical_coord sph_coord(vgl_point_3d<double>(0.0,0.0,0.0), 1.0);
  vsph_sph_point_3d sp;
  vgl_point_3d<double> cp(l_ray.direction().x(), l_ray.direction().y(), l_ray.direction().z());
  sph_coord.spherical_coord(cp, sp);
  double dist = vcl_tan(vnl_math::pi - sp.theta_)*altitude;
  double jj = line_coord(h_line, i);
  if (j < jj)
    vcl_cout << "pixel: " << i << 'x' << j << " is above horizon " << jj << vcl_endl;
  vcl_cout << "pixel: " << i << 'x' << j << ", world_point: " << cp << ", world_point sph: " << sp
           << ", dist: " << dist << ", phi: " << sp.phi_/vnl_math::pi_over_180 << vcl_endl;
  volm_conf_object_sptr obj = new volm_conf_object(sp.phi_, dist, 1);
  obj->print(vcl_cout);

  i = 0;  j = 2500;
  l_ray = cam[0].backproject_ray(i, j);
  cp.set(l_ray.direction().x(), l_ray.direction().y(), l_ray.direction().z());
  sph_coord.spherical_coord(cp, sp);
  dist = vcl_tan(vnl_math::pi - sp.theta_)*altitude;
  jj = line_coord(h_line, i);
  if (j < jj)
    vcl_cout << "pixel: " << i << 'x' << j << " is above horizon " << jj << vcl_endl;
  vcl_cout << "pixel: " << i << 'x' << j << ", world_point: " << cp << ", world_point sph: " << sp
           << ", dist: " << dist << ", phi: " << sp.phi_/vnl_math::pi_over_180 << vcl_endl;


  i = 1000; j = 2500;
  l_ray = cam[0].backproject_ray(i, j);
  cp.set(l_ray.direction().x(), l_ray.direction().y(), l_ray.direction().z());
  sph_coord.spherical_coord(cp, sp);
  dist = vcl_tan(vnl_math::pi - sp.theta_)*altitude;
  jj = line_coord(h_line, i);
  if (j < jj)
    vcl_cout << "pixel: " << i << 'x' << j << " is above horizon " << jj << vcl_endl;
  vcl_cout << "pixel: " << i << 'x' << j << ", world_point: " << cp << ", world_point sph: " << sp
           << ", dist: " << dist << ", phi: " << sp.phi_/vnl_math::pi_over_180 << vcl_endl;

  i = 2000; j = 2500;
  l_ray = cam[0].backproject_ray(i, j);
  cp.set(l_ray.direction().x(), l_ray.direction().y(), l_ray.direction().z());
  sph_coord.spherical_coord(cp, sp);
  dist = vcl_tan(vnl_math::pi - sp.theta_)*altitude;
  jj = line_coord(h_line, i);
  if (j < jj)
    vcl_cout << "pixel: " << i << 'x' << j << " is above horizon " << jj << vcl_endl;
  vcl_cout << "pixel: " << i << 'x' << j << ", world_point: " << cp << ", world_point sph: " << sp
           << ", dist: " << dist << ", phi: " << sp.phi_/vnl_math::pi_over_180 << vcl_endl;


  i = 3999; j = 2500;
  l_ray = cam[0].backproject_ray(i, j);
  cp.set(l_ray.direction().x(), l_ray.direction().y(), l_ray.direction().z());
  sph_coord.spherical_coord(cp, sp);
  dist = vcl_tan(vnl_math::pi - sp.theta_)*altitude;
  jj = line_coord(h_line, i);
  if (j < jj)
    vcl_cout << "pixel: " << i << 'x' << j << " is above horizon " << jj << vcl_endl;
  vcl_cout << "pixel: " << i << 'x' << j << ", world_point: " << cp << ", world_point sph: " << sp
           << ", dist: " << dist << ", phi: " << sp.phi_/vnl_math::pi_over_180 << vcl_endl;

  //// test the horizonal line in image coordinates
  //hp_3d.set(0.0,0.0,1.0,0.0);
  //hp_2d = cam[0].project(hp_3d);
  //vcl_cout << "p3da " << hp_3d << " p2da " << hp_2d << '\n';
  //vcl_cout << "p3da " << hp_3d << ", img pixels: " << hp_2d.x()/hp_2d.w() << "x" << hp_2d.y()/hp_2d.w() << vcl_endl;

  //vgl_line_2d<double> h_line = bpgl_camera_utils::horizon(cam[0]);
  //vcl_cout << "h_line: " << h_line << vcl_endl;


  //// plot the horizon on the image
  //vcl_string img_file = "V:/p1a_related/test1/p1a_test1_40/p1a_test1_40.jpg";
  //vil_image_view<vil_rgb<vxl_byte> > img = vil_load(img_file.c_str());
  //unsigned nii = img.ni();
  //unsigned njj = img.nj();

  //for (unsigned i = 0; i < nii; i++) {
  //  unsigned j = vcl_floor(-h_line.a()/h_line.b()*i - h_line.c()/h_line.b());
  //  if (j < njj)
  //    img(i,j) = vil_rgb<vxl_byte>(0,0,0);
  //}
  //vcl_string save_file = "v:/p1a_related/test1/p1a_test1_40/p1a_test1_40_h_line.jpg";
  //vil_save(img, save_file.c_str());
#endif

  return;
}

TESTMAIN( test_volm_conf_query );