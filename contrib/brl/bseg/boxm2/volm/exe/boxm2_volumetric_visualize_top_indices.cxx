//:
// \file
// \brief executable to visualize the index depth image for the top 30 matches result
// \author Yi Dong
// \date November 20, 2012

#include <volm/volm_io.h>
#include <volm/volm_tile.h>
#include <vul/vul_arg.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <volm/volm_spherical_container.h>
#include <volm/volm_spherical_container_sptr.h>
#include <volm/volm_spherical_shell_container.h>
#include <volm/volm_spherical_shell_container_sptr.h>
#include <volm/volm_query.h>
#include <volm/volm_query_sptr.h>
#include <volm/volm_loc_hyp.h>
#include <boxm2/volm/boxm2_volm_wr3db_index.h>
#include <boxm2/volm/boxm2_volm_wr3db_index_sptr.h>
#include <boxm2/volm/boxm2_volm_matcher.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vcl_set.h>


int main(int argc,  char** argv)
{
  vul_arg<vcl_string> cam_file("-cam", "cam kml filename", "");
  vul_arg<vcl_string> label_file("-label", "xml file with labeled polygons", "");
  vul_arg<bool> use_default("-def", "use default value for query", false);
  vul_arg<float> buffer_capacity("-buff", "index buffer capacity (GB)",1.0f);
  vul_arg<vcl_string> top_matches_filename("-top", "top_match.txt file", "");  // composer will read the score files from this folder for this job
  vul_arg<vcl_string> rat_folder("-rat", "rationale folder", "");      // composer will write top 30 to this folder

  vul_arg_parse(argc, argv);

  if (cam_file().compare("") == 0 || top_matches_filename().compare("") == 0 || label_file().compare("") == 0 || rat_folder().compare("") == 0 ) {
    vcl_cerr << "EXE_ARGUMENT_ERROR!\n";
    vul_arg_display_usage_and_exit();
    return volm_io::EXE_ARGUMENT_ERROR;
  }

  vcl_cout << "argc: " << argc << '\n'
           << " check input" << '\n'
           << " input camera file = " << cam_file() << '\n'
           << " input labemme file = " << label_file() << '\n'
           << " input whether to use default = " << use_default() << '\n'
           << " buffer_capacity = " << buffer_capacity() << '\n'
           << " top_matches = " << top_matches_filename() << '\n'
           << " output folder = " << rat_folder() << vcl_endl;

  // read the top 30 list
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > > top_matches;
  vcl_multiset<vcl_pair<float, volm_rationale>, std::greater<vcl_pair<float, volm_rationale> > >::iterator top_matches_iter;
  volm_rationale::read_top_matches(top_matches, top_matches_filename());

  // create the query
  top_matches_iter = top_matches.begin();
  volm_rationale r = top_matches_iter->second;
  // read the params of index 0, assume the container params are the same for all these indices
  boxm2_volm_wr3db_index_params q_params;
  unsigned long ei;
  if (!q_params.read_params_file(r.index_file) || !boxm2_volm_wr3db_index_params::read_size_file(r.index_file, ei) ) {
    vcl_cerr << "ERROR: cannot read params file for " << r.index_file << '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << " solid_angle = " << q_params.solid_angle << '\n'
           << " vmin = " << q_params.vmin << '\n'
           << " dmax = " << q_params.dmax << '\n'
           << " cap_angle = " << q_params.cap_angle << '\n'
           << " point_angle = " << q_params.point_angle << '\n'
           << " top_angle = " << q_params.top_angle << '\n'
           << " bottom_angle = " << q_params.bottom_angle << '\n'
           << " total number of indices = " << ei << vcl_endl;
  // create container and depth values
  volm_spherical_container_sptr sph = new volm_spherical_container(q_params.solid_angle,q_params.vmin,q_params.dmax);
  volm_spherical_shell_container_sptr sph_shell = new volm_spherical_shell_container(1.0, q_params.cap_angle, q_params.point_angle, q_params.top_angle, q_params.bottom_angle);
  vcl_cout << " index layer size: " << sph_shell->get_container_size() << vcl_endl;
  // check cam_file and label_file
  depth_map_scene_sptr dm = new depth_map_scene;
  vcl_string img_category;
  if (!volm_io::read_labelme(label_file(), dm, img_category)) {
    vcl_cerr << "ERROR: cannot read label xml file\n";
    return volm_io::LABELME_FILE_IO_ERROR;
  }
  double heading, heading_dev, tilt, tilt_dev, roll, roll_dev;
  double top_fov, top_fov_dev, altitude, lat, lon;
  if (!volm_io::read_camera(cam_file(), dm->ni(), dm->nj(), heading, heading_dev, tilt, tilt_dev, roll, roll_dev, top_fov, top_fov_dev, altitude, lat, lon)) {
    vcl_cerr << "ERROR:: cannot read camera kml file from " << cam_file() << '\n';
    return volm_io::EXE_ARGUMENT_ERROR;
  }
  vcl_cout << " img_category = " << img_category << vcl_endl;
  if (dm->sky().size()) {
    for (unsigned i = 0; i < dm->sky().size(); i++)
      vcl_cout << "\t " << (dm->sky()[i])->name() << " region "
               << ",\t min_depth = " << 254
               << ",\t max_depth = " << 255
               << ",\t order = " << (dm->sky()[i])->order()
               << vcl_endl;
  }
  if (dm->ground_plane().size())
    for (unsigned i = 0; i < dm->ground_plane().size(); i++)
      vcl_cout << "\t " << (dm->ground_plane()[i])->name() << " region "
               << ",\t min_depth = " << (dm->ground_plane()[i])->min_depth()
               << ",\t max_dpeth = " << (dm->ground_plane()[i])->max_depth()
               << ",\t order = " << (dm->ground_plane()[i])->order()
               << vcl_endl;

  if (dm->scene_regions().size()) {
    for (unsigned i = 0; i < dm->scene_regions().size(); i++) {
      vcl_cout << "\t " <<  (dm->scene_regions())[i]->name()  << " region "
               << ",\t min_depth = " << (dm->scene_regions())[i]->min_depth()
               << ",\t max_depth = " << (dm->scene_regions())[i]->max_depth()
               << ",\t order = " << (dm->scene_regions())[i]->order()
               << vcl_endl;
     }
  }
  vcl_cout << " cam params\n\t heading: " << heading << " dev: " << heading_dev
           << "\n\t tilt: " << tilt << " dev: " << tilt_dev
           << "\n\t roll: " << roll << " dev: " << roll_dev
           << "\n\t top_fov: " << top_fov << " dev: " << top_fov_dev
           << "\n\t alt: " << altitude << vcl_endl;
  volm_query_sptr query = new volm_query(cam_file(), label_file(), sph, sph_shell, use_default());
  vcl_cout << " generated query has size " << query->get_query_size() << '\n'
           << " The query has " << query->get_cam_num() << " cameras: " << '\n'
           << " Generated query_size for 1 camera is " << query->get_query_size() << " byte, "
           << " gives total query size = " << query->get_cam_num() << " x " << query->get_query_size()
           << " = " << (double)query->get_cam_num()*(double)query->get_query_size()/(1024*1024*1024) << " GB"
           << " query image size: ni = " << (query->depth_scene())->ni() << " nj = " << (query->depth_scene())->ni()
           << vcl_endl;

  unsigned ni = (query->depth_scene())->ni();
  unsigned nj = (query->depth_scene())->nj();

  // save top 30 index images
  // loop over top 30 list
  unsigned count = 0;
  for (top_matches_iter = top_matches.begin(); top_matches_iter != top_matches.end(); top_matches_iter++) {
    volm_rationale r = top_matches_iter->second;
    vcl_string cam_postfix = query->get_cam_string(r.cam_id);
    vcl_cout << " top " << count << " score = " << top_matches_iter->first
             << " index_id = " << r.index_id
             << " camera_id = " << r.cam_id << " cam_param = " << cam_postfix
             << " index_file = " << r.index_file << vcl_endl;
    boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index((unsigned)(sph_shell->get_container_size()), buffer_capacity());
    ind->initialize_read(r.index_file);
    vcl_vector<unsigned char> values(query->get_query_size());
    for (unsigned i = 0; i <= r.index_id; i++)
      ind->get_next(values);
    // print it out
#if 0
    vcl_cout << " ---------------- top " << count << " ---------------------------" << '\n'
             << "index for top " << count << vcl_endl;
    for (unsigned i = 0; i < values.size(); i++) {
      vcl_cout << " top " << count << " -- index[" << i << "] = " << (int)values[i] << vcl_endl;
    }
    vcl_vector<unsigned char> query_layer = query->min_dist()[r.cam_id];
    vcl_cout << "query for top " << count << vcl_endl;
    for (unsigned k = 0; k < query_layer.size(); k++) {
      vcl_cout << " top " << count << " -- query[" << k << "] = " << (int)query_layer[k] << vcl_endl;
    }
    vcl_cout << " ****************************************************************" << vcl_endl;
#endif
    // create an rgb image instance
    vil_image_view<vil_rgb<vxl_byte> > img(ni, nj);
    // initialize the image
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        img(i,j).r = (unsigned char)120;
        img(i,j).g = (unsigned char)120;
        img(i,j).b = (unsigned char)120;
    }
    query->depth_rgb_image(values, r.cam_id, img);
    //save the image
    vcl_stringstream str;
    str << rat_folder() + '/' << "index_top_" << count++ << cam_postfix << ".png";
    // save the images
    vil_save(img,(str.str()).c_str());
    ind->finalize();
  }

  // Save the ground truth image for each index
  boxm2_volm_wr3db_index_sptr ind = new boxm2_volm_wr3db_index((unsigned)(sph_shell->get_container_size()), buffer_capacity());
  ind->initialize_read(r.index_file);
  vcl_vector<unsigned char> values(query->get_query_size());
  for (unsigned idx = 0; idx < 11; idx++){
    ind->get_next(values);
#if 0
    for (unsigned i = 0; i < values.size(); i++) {
      vcl_cout << " index " << idx << " -- index[" << i << "] = " << (int)values[i] << vcl_endl;
    }
    vcl_cout << " ****************************************************************" << vcl_endl;
#endif
    // create an rgb image instance
    vil_image_view<vil_rgb<vxl_byte> > img(ni, nj);
    // initialize the image
    for (unsigned i = 0; i < ni; i++)
      for (unsigned j = 0; j < nj; j++) {
        img(i,j).r = (unsigned char)120;
        img(i,j).g = (unsigned char)120;
        img(i,j).b = (unsigned char)120;
    }
    query->depth_rgb_image(values, 0, img);
    //save the image
    vcl_string cam_postfix = query->get_cam_string(0);
    vcl_stringstream str;
    str << rat_folder() + "/" << "index_gt_" << idx << cam_postfix << ".png";
    // save the images
    vil_save(img,(str.str()).c_str());
  }
  ind->finalize();
  // create an rgb image instance
  vil_image_view<vil_rgb<vxl_byte> > query_gt_img(ni, nj);
    // initialize the image
      for (unsigned i = 0; i < ni; i++)
        for (unsigned j = 0; j < nj; j++) {
          query_gt_img(i,j).r = (unsigned char)120;
          query_gt_img(i,j).g = (unsigned char)120;
          query_gt_img(i,j).b = (unsigned char)120;
      }
  vcl_vector<unsigned char> query_layer = query->min_dist()[0];
  query->depth_rgb_image(query_layer, 0, query_gt_img);

  //save the image
  vcl_string cam_postfix = query->get_cam_string(0);
  vcl_stringstream str;
  str << rat_folder() + "/" << "query_gt_" << cam_postfix << ".png";
  // save the images
  vil_save(query_gt_img,(str.str()).c_str());

  return volm_io::SUCCESS;
}
