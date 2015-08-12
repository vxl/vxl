//:
// \file
// \brief compute a fixed similarity transform (only R and t) take carries meta-data cameras to actual cameras given a number of survey points and image correspondences in each frame
//
// \author Ozge C. Ozcanli
// \date   January 11, 2014
//
// \verbatim
//  Modifications
//
// \endverbatim
#include <bwm/bwm_observer_cam.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_3d_corr.h>
#include <bwm/bwm_3d_corr_sptr.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vul/vul_timer.h>
#include <vgl/vgl_point_3d.h>

#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_cam_istream.h>

#include <vpgl/algo/vpgl_camera_transform.h>


// this executable takes a set of site files as input with a set of 2d image to image correspondences
// and a gps file with a 3d point that is marked in the site files
// the order of the 3d gps points in the txt file has to be same as the site files that will be given by the glob
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<vcl_string> site_files   ("-site_files", "a txt file with the name of site xml files at each line",  "");  // we need a site file for each 3d point in the gps file, the orders should be the same
  vul_arg<vcl_string> site_sfm   ("-tmp", "",  ""); 
  vul_arg<vcl_string> gps_file ("-gps", "gps text file (x y z in local coords per line)", "");  
  vul_arg<vcl_string> output_cam_dir ("-out_cam_dir","directory to store cams", "");

  vcl_cout << " argc: " << argc << vcl_endl;
  for (unsigned i = 0; i < argc; i++)
    vcl_cout << i << " : " << argv[i] << vcl_endl;

  //if (argc != 7) {
  if (argc != 9) {
    vcl_cout << "usage: bwm_3d_fixed_transform -site_files <a txt file with the name of site xml files at each line> -gps <3d point file> -in_cam_prefix <uses glob: prefix*_cam.txt> -out_cam_dir <transform each input cam by the same fixed R|t and write into this folder>\n";
    return -1;
  }

  vul_timer timer;
  timer.mark();

  vul_arg_parse(argc, argv);
  // read the site file names
  vcl_ifstream ifs(site_files().c_str());
  vcl_vector<vcl_string> site_names;
  while (!ifs.eof()) {
    char buf[10000];
    ifs.getline(buf, 10000);
    vcl_stringstream namestr(buf);
    vcl_string name; namestr >> name;
    if (name.size() > 2)
      site_names.push_back(name);
  }
  vcl_vector<bwm_video_corr_sptr> corrs;
  vcl_string cam_path = ""; 
  for (unsigned i = 0; i < site_names.size(); i++)
  {
    vcl_string file = site_names[i];
    vcl_cout << file << vcl_endl;

    bwm_video_corr_processor cp;
    cp.set_verbose(true);
    if (!cp.open_video_site(file.c_str(), true))
      return false;
    vcl_vector<bwm_video_corr_sptr> corrs_it = cp.correspondences();
    vcl_cout << "there are: " << corrs_it.size() << " corrs in the file: "  << file << "\n";
    for (unsigned j = 0; j < corrs_it.size(); j++) {
      corrs.push_back(corrs_it[j]);
    }
    if (cam_path.compare("") != 0) {
      if (cam_path.compare(cp.camera_path()) != 0) {
        vcl_cerr << " Input site files have inconsistent camera paths!\n Exiting!\n";
        return -1;
      }
    } else
      cam_path = cp.camera_path();
  }
  
  unsigned n_pts = corrs.size();
  vcl_cout << "there are: " << n_pts << " corrs total in all the site files,expect to find this many 3d points in the gps file\n";

  vcl_ifstream gps_ifs(gps_file().c_str());
  if (!gps_ifs.good()) {
    vcl_cerr << "ERROR: error opening gps file " << gps_file() << vcl_endl;
    return -1;
  }
  
  vcl_vector<vnl_vector_fixed<double, 4> > pts_3d;
  for (unsigned int i=0; i<n_pts; ++i) {
    double gps_x, gps_y, gps_z;
    gps_ifs >> gps_x >> gps_y >> gps_z;
    vnl_vector_fixed<double, 4> gps_pt(gps_x, gps_y, gps_z,1.0);
    pts_3d.push_back(gps_pt);
    if (gps_ifs.eof())
      break;
  }
  gps_ifs.close();
  if (pts_3d.size() != n_pts) {
    vcl_cerr << "input gps file and the number of correspondences in the input site files are not consistent! Exiting!\n";
    return -1;
  }
  vcl_cout << " read " << pts_3d.size() << " 3d points, start computation..\n";
  
  vcl_cout << "cam_path from site files: " << cam_path << vcl_endl;
  vcl_vector<vpgl_perspective_camera<double> > cams;
  vcl_vector<vcl_string> out_names;
  unsigned cam_id = 0; 
  bwm_video_cam_istream_sptr cstr = new bwm_video_cam_istream(cam_path);
  do {
    //vpgl_perspective_camera<double>* cam = cstr->read_camera();
    cstr->advance();
    vpgl_perspective_camera<double>* cam = cstr->current_camera();
    if (cam) {
      vcl_string name = cstr->current_cam_name();
      cams.push_back(*cam);
      vcl_string fname = vul_file::strip_directory(name.c_str());
      //vcl_cout << "cam_id: " << cam_id++ << " name: " << fname << ":\n" << *cam << vcl_endl;
      vcl_string out_dir = output_cam_dir() + "/";
      vcl_string out_file = out_dir + fname;
      out_names.push_back(out_file);
    }
    else
      break;
  } while (true);
  
  vcl_cout << "found: " << cams.size() << " cameras!\n";
  /*
  bwm_video_corr_processor cp_temp;
  cp_temp.set_verbose(true);
  if (!cp_temp.open_video_site(site_sfm().c_str(), true))
    return false;
  vcl_string cam_path_temp = cp_temp.camera_path();

  vcl_vector<vpgl_perspective_camera<double> > cams_temp;
  cam_id = 0; 
  bwm_video_cam_istream_sptr cstr_temp = new bwm_video_cam_istream(cam_path_temp);
  do {
    cstr_temp->advance();
    vpgl_perspective_camera<double>* cam = cstr_temp->current_camera();
    if (cam) {
      cams_temp.push_back(*cam);
    }
    else
      break;
  } while (true);
  
  vcl_cout << "found: " << cams_temp.size() << " temp cameras!\n";

  // now make the centers of input cams the centers of these temp cams  (sfm cams)
  for (unsigned i = 0; i < cams_temp.size(); i++) {
    vpgl_calibration_matrix<double> K = cams[i].get_calibration();
    vgl_rotation_3d<double> R = cams[i].get_rotation();
    vgl_point_3d<double> C = cams[i].get_camera_center();
    vgl_point_3d<double> C_temp = cams_temp[i].get_camera_center();
    vpgl_perspective_camera<double> cnew(K, C_temp, R);
    cams[i] = cnew;
  }*/

  
  // for each 3d point, there will be a vector of 2-d correspondences and cameras 
  vcl_vector< vcl_vector< vcl_pair<vnl_vector_fixed<double, 2>, unsigned> > > img_pt_corrs;  // so size img_pt_corrs = 3d_pts.size()

  for (unsigned i = 0; i < corrs.size(); i++)
  {
    vcl_vector< vcl_pair<vnl_vector_fixed<double, 2>, unsigned> > pt_cams;

    bwm_video_corr_sptr corr = corrs[i];
    vcl_map<unsigned, vgl_point_2d<double> > matches = corr->matches();
    for (vcl_map<unsigned, vgl_point_2d<double> >::iterator iter = matches.begin(); iter != matches.end(); iter++) {
      vnl_vector_fixed<double, 2> pt(iter->second.x(), iter->second.y());
      vcl_pair<vnl_vector_fixed<double, 2>, unsigned> pair(pt, iter->first);
      pt_cams.push_back(pair);
    }

    img_pt_corrs.push_back(pt_cams);
  }
  
  vcl_vector<vpgl_perspective_camera<double>  > output_cams;
  /*if (!vpgl_camera_transform::compute_fixed_transformation(cams, img_pt_corrs, pts_3d, output_cams)) {
    vcl_cerr << " In bwm_3d_fixed_transform() - problems computing the transform!\n";
    return -1;
  }*/
  if (!vpgl_camera_transform::compute_fixed_transformation_sample(cams, img_pt_corrs, pts_3d, output_cams)) {
    vcl_cerr << " In bwm_3d_fixed_transform() - problems computing the transform!\n";
    return -1;
  }
  
  /*if (!vpgl_camera_transform::compute_initial_transformation_R(cams, img_pt_corrs, pts_3d, output_cams)) {
    vcl_cerr << " In bwm_3d_fixed_transform() - problems computing the transform!\n";
    return -1;
  }*/
  
  if (output_cams.size() != out_names.size()) {
    vcl_cerr << " In bwm_3d_fixed_transform() - inconsistent output cam vector sizes!\n";
    return -1;
  }

  for (unsigned i = 0; i < output_cams.size(); i++) {
    vcl_ofstream os(out_names[i].c_str());
    os << output_cams[i];
    os.close();
  }

  double t = timer.all();
  vcl_cout << "Finished! Time: " << t << " ms: " << t/1000 << " secs: " << t/(1000*60) << " mins." << vcl_endl;
  
  return 0;
}
