//:
// \file
// \brief compute a covariance matrix for each pair in a sequence of images using their existing cameras and a number of survey points and image correspondences in each frame
//
// \author Ozge C. Ozcanli
// \date   Feb 11, 2014
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
#include <vnl/vnl_trace.h>
#include <vnl/algo/vnl_real_eigensystem.h>

#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_cam_istream.h>

#include <vpgl/algo/vpgl_camera_transform.h>


// this executable takes a set of site files as input with a set of 2d image to image correspondences
int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<vcl_string> site_files   ("-site_files", "a txt file with the name of site xml files at each line",  "");  // we need a site file for each image to image correspondence
                                                                                                                     // input cameras will be read from these site files

  vcl_cout << " argc: " << argc << vcl_endl;
  for (unsigned i = 0; i < argc; i++)
    vcl_cout << i << " : " << argv[i] << vcl_endl;

  if (argc != 3) {
    vcl_cout << "usage: bwm_3d_fixed_transform -site_files <a txt file with the name of site xml files at each line> \n";
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
  vcl_cout << "there are: " << n_pts << " corrs total in all the site files\n";

  vcl_cout << "cam_path from site files: " << cam_path << vcl_endl;
  vcl_vector<vpgl_perspective_camera<double> > cams;
  //vcl_vector<vcl_string> out_names;
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
      //vcl_string out_dir = output_cam_dir() + "/";
      //vcl_string out_file = out_dir + fname;
      //out_names.push_back(out_file);
    }
    else
      break;
  } while (true);
  
  vcl_cout << "found: " << cams.size() << " cameras!\n";
  
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

  vcl_vector< vcl_vector < vcl_pair< vcl_pair<vnl_vector_fixed<double, 3>, vnl_matrix_fixed<double, 3, 3> >, unsigned> > > img_pt_norm_corrs;
  vnl_matrix_fixed<double, 3, 3> input_correspondence_cov;
  input_correspondence_cov.fill(0.0);
  input_correspondence_cov[0][0] = 1.0;   // 1 pixel
  input_correspondence_cov[1][1] = 1.0;
  input_correspondence_cov[2][2] = 1.0;
  vpgl_camera_transform::K_normalize_img_pts(cams, input_correspondence_cov, img_pt_corrs, img_pt_norm_corrs);
  
  // compute (h,R) - base line vector and relative orientation matrix for each pair of cameras
  vgl_rotation_3d<double> R = vpgl_persp_cam_relative_orientation(cams[0], cams[1]);
  vgl_vector_3d<double> h = vpgl_persp_cam_base_line_vector(cams[0], cams[1]);

  vcl_cout << "cam0 center: " << cams[0].get_camera_center() << "\n";
  vcl_cout << "cam1 center: " << cams[1].get_camera_center() << "\n";
  vcl_cout << "base line vector: " << h << "\n";
  vcl_cout << "relative orientation: " << R.as_matrix() << "\n";
  
  vgl_vector_3d<double> p0 = cams[0].principal_axis();
  vgl_vector_3d<double> p1 = cams[1].principal_axis();

  vnl_vector_fixed<double, 3> p0v(p0.x(), p0.y(), p0.z());
  vnl_vector_fixed<double, 3> p1v(p1.x(), p1.y(), p1.z());

  vcl_cout << "R*p1: " << R.as_matrix()*p1v << vcl_endl;
  vcl_cout << "should be same as p0: " << p0 << vcl_endl;
  vcl_cout << "R_inv*p0: " << R.inverse().as_matrix()*p0v << vcl_endl;
  vcl_cout << "should be same as p1: " << p1 << vcl_endl;

  // use all pairs and get the max as the rotation variance
  double max_angle = 0.0;
  for (unsigned i = 1; i < cams.size(); i++) {
    vnl_matrix_fixed<double, 3, 3> rot_variance;
    vpgl_camera_transform::compute_covariance(i-1, i, cams, img_pt_norm_corrs, rot_variance);
    vcl_cout << " computed rotation covariance: " << rot_variance << vcl_endl;
    // compute the magnitude of the angle of rotation
    vnl_real_eigensystem sys(rot_variance);
    double largest = 0.0;
    for (unsigned k = 0; k < 3; k++) {
      vcl_cout << " eigenvalues: " << sys.D[k].real() << vcl_endl;
      if (largest < sys.D[k].real())
        largest = sys.D[k].real();
    }
    vcl_cout << " the angle of perturbation: " << vcl_sqrt(largest) << vcl_endl;
     if (largest > max_angle)  
       max_angle = largest;
  }  
  vcl_cout << "the max angle of perturbation: " << max_angle << vcl_endl;
    
  double t = timer.all();
  vcl_cout << "Finished! Time: " << t << " ms: " << t/1000 << " secs: " << t/(1000*60) << " mins." << vcl_endl;
  
  return 0;
}
