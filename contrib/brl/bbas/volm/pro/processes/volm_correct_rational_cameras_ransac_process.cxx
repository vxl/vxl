// This is brl/bbas/volm/pro/processes/volm_correct_rational_cameras_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take a list of rational cameras and a list of 2D image correspondences
//         some of the correspondences may be very poor/wrong but a majority is of good quality (i.e. corresponds to the same 3D point)
//         Use a RANSAC scheme to find offsets for each camera using "inlier" correspondences
//
//
#include <bprb/bprb_parameters.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vcl_fstream.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vul/vul_file.h>
#include <vul/vul_awk.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#include <vul/vul_file_iterator.h>
#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vgl/vgl_distance.h>
#include <vpgl/algo/vpgl_rational_adjust_multipt.h>

vpgl_rational_camera<double>* load_cam_from_nitf(vcl_string& nitf_image_path) {
  vil_image_resource_sptr image = vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    vcl_cout << "NITF image load failed from the file: " << nitf_image_path << '\n';
    return 0;
  }

  vcl_string format = image->file_format();
  vcl_string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    vcl_cout << "source image is not NITF in load_cam_from_nitf\n";
    return 0;
  }

  //cast to an nitf2_image
  vil_nitf2_image *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  vpgl_nitf_rational_camera *nitf_cam=new vpgl_nitf_rational_camera(nitf_image, true);

  return dynamic_cast<vpgl_rational_camera<double>* >(nitf_cam);
}

//:
//  Take a list of rational cameras and a list of 2D image correspondences of the same 3D point location,
//  find that 3D location,
//  project that point back to images and correct each camera by adjusting its 2D image offset so that they all project the 3D location to the same 2D location
bool volm_correct_rational_cameras_ransac_process_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("volm_satellite_resources_sptr");  // fetch the full path of the satellite image from the resource file
  input_types.push_back("vcl_string");  // a file that lists the name to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
                                    // .
                                    // .
                                    // .
  input_types.push_back("vcl_string");  // output folder to write the corrected cams
  input_types.push_back("float"); // radius in pixels for the disagreement among inliers, e.g. 2 pixels
  vcl_vector<vcl_string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_correct_rational_cameras_ransac_process(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cout << "volm_correct_rational_cameras_ransac_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }

  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string input_cams = pro.get_input<vcl_string>(1);
  vcl_string output_path  = pro.get_input<vcl_string>(2);
  float pix_rad = pro.get_input<float>(3);

  vcl_ifstream ifs(input_cams.c_str());
  if (!ifs) {
    vcl_cerr<< " cannot open file: " << input_cams << '\n';
    return false;
  }
  unsigned int n; ifs >> n;
  vcl_cout << "will read: " << n << " correspondences for each frame from "<< input_cams << vcl_endl;

  vcl_vector<vpgl_rational_camera<double> > cams;
  //vcl_vector<float> cam_ce_reliability;
  vcl_vector<vcl_string> cam_ce_reliability;
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs;

  vcl_vector<vcl_string> out_cam_names;
  while (!ifs.eof()) 
  {
  //vul_awk awk(ifs);
  //for (; awk; ++awk)
  //{
    //vcl_stringstream line(awk.line());
    //char buffer[10000];
    //ifs.getline(buffer, 10000);
    //vcl_stringstream line(buffer);
    vcl_string cam_name;
    ifs >> cam_name;
    if (cam_name.size() < 2) break;
    vcl_cout << "reading cam: " << cam_name << vcl_endl;
    
    vcl_string out_cam_name = output_path + cam_name + "_corrected.rpb";
    vcl_cout << "out cam name: " << out_cam_name << vcl_endl;
    
    // locate the cam in the resources
    //vcl_string img_path = res->full_path(cam_name);
    vcl_pair<vcl_string, vcl_string> img_path = res->full_path(cam_name);
    if (img_path.first.compare("") == 0) {
      vcl_cerr << "Cannot locate: " << cam_name << " in satellite resources! exiting!\n";
      return false;
    }
    vcl_cout << img_path.first << vcl_endl;
    vpgl_rational_camera<double> *ratcam = load_cam_from_nitf(img_path.first);
    if ( !ratcam ) {
      vcl_cerr << "Failed to load rational camera from file" << img_path.first << '\n';
      return false;
    }
    cams.push_back(*ratcam);
    cam_ce_reliability.push_back(img_path.second);
    out_cam_names.push_back(out_cam_name);
    
    vcl_vector<vgl_point_2d<double> > corrs_frame;
    for (unsigned int ii = 0; ii < n; ++ii) {
      double i, j;
      ifs >> i; ifs >> j;
      vgl_point_2d<double> cor(i,j);
      corrs_frame.push_back(cor);
    }
    corrs.push_back(corrs_frame);
  }
  //float main_weight = 0.99f;
  float main_weight = 1.0f;
  vcl_vector<float> cam_weights(cams.size(), (1.0f-main_weight)/(cams.size()-1));
  //float sum = 0.0f;
  //for (unsigned i = 0; i < cam_ce_reliability.size(); i++) 
  //  sum += cam_ce_reliability[i];
  bool found_geo_eye = false;
  for (unsigned i = 0; i < cam_ce_reliability.size(); i++) {
    if (cam_ce_reliability[i].compare("GeoEye-1") == 0) {
      cam_weights[i] = main_weight;
      found_geo_eye = true;
      break;
    }
    //cam_weights.push_back(cam_ce_reliability[i]/sum);
  }
  if (!found_geo_eye)
    cam_weights.assign(cam_weights.size(), 1.0f/cams.size());
  
  vcl_cout << " cam reliabilities and assigned weights: \n";
  for (unsigned i = 0; i < cams.size(); i++) {
    vcl_cout << out_cam_names[i] << ": rel " << cam_ce_reliability[i] << " weight: " << cam_weights[i] << '\n';
  }

  ifs.close();
  vcl_cout << "cams size: " << cams.size() << " corrs size: " << corrs.size() << vcl_endl;
  
  // adjust using each correspondence and save the offsets
  vcl_vector<vcl_vector<vgl_vector_2d<double> > > cam_trans;

  for (unsigned i = 0; i < n; i++) {
    vcl_vector<vgl_point_2d<double> > corrs_i;
    for (unsigned jj = 0; jj < corrs.size(); jj++) {
      corrs_i.push_back(corrs[jj][i]);
    }

    //vcl_cout << "Executing adjust image offsets\n";
    vcl_vector<vgl_vector_2d<double> > cam_trans_i;
    vgl_point_3d<double> intersection;
    //if (!vpgl_rational_adjust_onept::adjust(cams, corrs_i, cam_trans_i, intersection))
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, cam_weights, corrs_i, cam_trans_i, intersection))
      continue;
    //vcl_cout << "after adjustment 3D intersection point: " << intersection << vcl_endl;
    cam_trans.push_back(cam_trans_i);
  }
  vcl_cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections!\n";

  // find the inliers
  vcl_vector<unsigned> inlier_cnts(cam_trans.size(), 0);
  vcl_vector<vcl_vector<unsigned> > inliers;
  for (unsigned i = 0; i < cam_trans.size(); i++) {  // for each correction find how many inliers are there for it
    vcl_cout << "offset_u: " << cam_trans[i][0].x() << " v: " << cam_trans[i][0].y() << vcl_endl;
    vcl_vector<unsigned> inliers_i;
    inliers_i.push_back(i); // first push itself
    for (unsigned j = 0; j < cam_trans.size(); j++) {
      if (i == j) continue;
      double dif = 0;
      for (unsigned k = 0; k < cam_trans[i].size(); k++) {
        vgl_point_2d<double> trans1(cam_trans[i][k].x(), cam_trans[i][k].y());
        vgl_point_2d<double> trans2(cam_trans[j][k].x(), cam_trans[j][k].y());
        dif += vgl_distance(trans1, trans2);
      }
      dif /= cam_trans[i].size();
      if (dif < pix_rad) {
        inlier_cnts[i]++;
        inliers_i.push_back(j);
      }
    }
    inliers.push_back(inliers_i);
  }
  vcl_cout << "out of " << n << " correspondences, inlier cnts using pixel radius: " << pix_rad << " : " << vcl_endl;
  unsigned max = 0; unsigned max_i = 0;
  for (unsigned i = 0; i < cam_trans.size(); i++) {
    vcl_cout << " " << inlier_cnts[i];
    if (max < inlier_cnts[i]) {
      max = inlier_cnts[i];
      max_i = i;
    }
  }
  vcl_cout << vcl_endl << " using translations of 1 pt with the most inliers: \n";
  for (unsigned k = 0; k < cams.size(); k++) {
    vcl_cout << "offset_u: " << cam_trans[max_i][k].x() << " offset_v: " << cam_trans[max_i][k].y() << '\n';
  }
  

  // use the correspondence with the most number of inliers to correct the cameras
  for (unsigned k = 0; k < cams.size(); k++) {
    double u_off,v_off;
    cams[k].image_offset(u_off,v_off);
    cams[k].set_image_offset(u_off + cam_trans[max_i][k].x(), v_off + cam_trans[max_i][k].y());
    cams[k].save(out_cam_names[k]);
  }

  // refine the cameras using all the inliers of this correspondence
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs_inliers;
  
  for (unsigned k = 0; k < corrs.size(); k++) {
    vcl_vector<vgl_point_2d<double> > vec;
    for (unsigned j = 0; j < inliers[max_i].size(); j++)
      vec.push_back(corrs[k][inliers[max_i][j]]);
    corrs_inliers.push_back(vec);
  }

  vcl_vector<vgl_vector_2d<double> > cam_trans_inliers;
  vcl_vector<vgl_point_3d<double> > intersections;
  //vcl_vector<float> cam_weights_equal(cams.size(), 1.0f/cams.size());
  //if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights_equal, corrs_inliers, cam_trans_inliers, intersections))
  if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, cam_trans_inliers, intersections))
  {
    vcl_cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  vcl_cout << " after refinement: \n";
  for (unsigned i = 0; i < intersections.size(); i++)
    vcl_cout << "after adjustment 3D intersection point: " << intersections[i] << vcl_endl;

  for (unsigned i = 0; i < cam_trans_inliers.size(); i++)   // for each correction find how many inliers are there for it
    vcl_cout << "offset_u: " << cam_trans_inliers[i].x() << " v: " << cam_trans_inliers[i].y() << vcl_endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    double u_off,v_off;
    cams[i].image_offset(u_off,v_off);
    cams[i].set_image_offset(u_off + cam_trans_inliers[i].x(), v_off + cam_trans_inliers[i].y());
    cams[i].save(out_cam_names[i]);
  }

  return true;
}


//:
//  Take a list of rational cameras and a list of 2D image correspondences of the same 3D point location,
//  find that 3D location,
//  project that point back to images and correct each camera by adjusting its 2D image offset so that they all project the 3D location to the same 2D location
// this process is the same as above but it checks the availability of the output camera first. if none of the cameras are available, it weights them all equal and corrects them
//  if one of the cameras is already available, it corrects the rest with respect to that (makes its weight 1.0)
//  if two or more cameras are already available, it picks the first available camera, weights it 1 and ignores the other existing cameras
//  if all cameras are available, quits without doing anything
bool volm_correct_rational_cameras_ransac_process2_cons(bprb_func_process& pro)
{
  vcl_vector<vcl_string> input_types;
  input_types.push_back("volm_satellite_resources_sptr");  // fetch the full path of the satellite image from the resource file
  input_types.push_back("vcl_string");  // a file that lists the name to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
                                    // .
                                    // .
                                    // .
  input_types.push_back("vcl_string");  // output folder to write the corrected cams
  input_types.push_back("float"); // radius in pixels for the disagreement among inliers, e.g. 2 pixels
  vcl_vector<vcl_string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_correct_rational_cameras_ransac_process2(bprb_func_process& pro)
{
  if (pro.n_inputs() < 4) {
    vcl_cout << "volm_correct_rational_cameras_ransac_process: The number of inputs should be 4" << vcl_endl;
    return false;
  }

  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  vcl_string input_cams = pro.get_input<vcl_string>(1);
  vcl_string output_path  = pro.get_input<vcl_string>(2);
  float pix_rad = pro.get_input<float>(3);

  vcl_ifstream ifs(input_cams.c_str());
  if (!ifs) {
    vcl_cerr<< " cannot open file: " << input_cams << '\n';
    return false;
  }
  unsigned int n; ifs >> n;
  vcl_cout << "will read: " << n << " correspondences for each frame from "<< input_cams << vcl_endl;

  vcl_vector<vcl_string> cam_names;
  vcl_vector<vcl_string> cam_sat_names;
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs;
  vcl_vector<vcl_string> out_cam_names;

  while (!ifs.eof()) 
  {
    vcl_string cam_name;
    ifs >> cam_name;
    if (cam_name.size() < 2) break;
    vcl_cout << "reading cam: " << cam_name << vcl_endl;
    
    vcl_string out_cam_name = output_path + cam_name + "_corrected.rpb";
    vcl_cout << "out cam name: " << out_cam_name << vcl_endl;
    
    // locate the cam in the resources
    //vcl_string img_path = res->full_path(cam_name);
    vcl_pair<vcl_string, vcl_string> img_path = res->full_path(cam_name);
    if (img_path.first.compare("") == 0) {
      vcl_cerr << "Cannot locate: " << cam_name << " in satellite resources! exiting!\n";
      return false;
    }
    vcl_cout << img_path.first << vcl_endl;
  
    cam_names.push_back(img_path.first);  
    cam_sat_names.push_back(img_path.second);
    out_cam_names.push_back(out_cam_name);
    
    vcl_vector<vgl_point_2d<double> > corrs_frame;
    for (unsigned int ii = 0; ii < n; ++ii) {
      double i, j;
      ifs >> i; ifs >> j;
      vgl_point_2d<double> cor(i,j);
      corrs_frame.push_back(cor);
    }
    corrs.push_back(corrs_frame);
  }
  ifs.close();
  
  if (!n) {
    vcl_cerr << "In vpgl_correct_rational_cameras_process2 - 0 correspondences in file: " << input_cams << "! returning without correcting any cams!\n";
    return false;
  }

  bool first = true;
  bool already_exists = false;
  vcl_vector<float> cam_weights;
  vcl_vector<vpgl_rational_camera<double> > cams;
  vcl_vector<vcl_string> new_out_names;
  vcl_vector<vcl_string> new_sat_names;
  vcl_vector<vcl_vector<vgl_point_2d<double> > > new_corrs;
  // now determine which cameras already exist
  for (unsigned i = 0; i < out_cam_names.size(); i++) {

    if (vul_file::exists(out_cam_names[i])) {
      already_exists = true;
      if (first) {
        first = false;
        // use the corrected camera
        vpgl_rational_camera<double> *ratcam = read_rational_camera<double>(out_cam_names[i]);
        if ( !ratcam ) {
          vcl_cerr << "Failed to load rational camera from file" << out_cam_names[i] << '\n';
          return false;
        }
        cams.push_back(*ratcam);
        // since this is the first make its weight 1
        cam_weights.push_back(1.0f);
        new_out_names.push_back(out_cam_names[i]);
        new_sat_names.push_back(cam_sat_names[i]);
        new_corrs.push_back(corrs[i]);
      } else {  // exists but not first! just skip it
        vcl_cout << "\t\t skipping cam: " << cam_names[i] << " already exists!\n";
        continue;
      }
    } else {  // does not exist, then load the original camera and correct it
      vpgl_rational_camera<double> *ratcam = load_cam_from_nitf(cam_names[i]);
      if ( !ratcam ) {
        vcl_cerr << "Failed to load rational camera from file" << cam_names[i] << '\n';
        return false;
      }
      cams.push_back(*ratcam);
      cam_weights.push_back(0.0f);
      new_out_names.push_back(out_cam_names[i]);
      new_sat_names.push_back(cam_sat_names[i]);
      new_corrs.push_back(corrs[i]);
    }
  }

  if (cams.size() == 1) {
    if (new_sat_names[0] == "GeoEye-1") {
      cams[0].save(out_cam_names[0]);
      return true;
    } else 
      return false;
  }
  
  // check if we have any that already existed, if not, then all of them need to be assigned equal weight
  if (!already_exists) {
    int geoeye1_cnt = 0;
    unsigned g_id = 0;
    for (unsigned i = 0; i < cams.size(); i++) {
      if (new_sat_names[i] == "GeoEye-1") {
        geoeye1_cnt++;
        g_id = i;
      }
    }
    
    if (geoeye1_cnt != cams.size()) {
      cam_weights.assign(cam_weights.size(), 0.0f);
      cam_weights[g_id] = 1.0f;
    } else
      cam_weights.assign(cam_weights.size(), 1.0f/cams.size());
  }
  
  vcl_cout << " cam assigned weights: \n";
  for (unsigned i = 0; i < cams.size(); i++) {
    vcl_cout << new_out_names[i] << " weight: " << cam_weights[i] << '\n';
  }

  vcl_cout << "cams size: " << cams.size() << " corrs size: " << new_corrs.size() << vcl_endl;
  
  // adjust using each correspondence and save the offsets
  vcl_vector<vcl_vector<vgl_vector_2d<double> > > cam_trans;

  for (unsigned i = 0; i < n; i++) {
    vcl_vector<vgl_point_2d<double> > corrs_i;
    for (unsigned jj = 0; jj < new_corrs.size(); jj++) {
      corrs_i.push_back(new_corrs[jj][i]);
    }

    //vcl_cout << "Executing adjust image offsets\n";
    vcl_vector<vgl_vector_2d<double> > cam_trans_i;
    vgl_point_3d<double> intersection;
    //if (!vpgl_rational_adjust_onept::adjust(cams, corrs_i, cam_trans_i, intersection))
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, cam_weights, corrs_i, cam_trans_i, intersection))
      continue;
    //vcl_cout << "after adjustment 3D intersection point: " << intersection << vcl_endl;
    cam_trans.push_back(cam_trans_i);
  }
  vcl_cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections!\n";
  if (!cam_trans.size()) {
    vcl_cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections! exit without any corrections!\n";
    return true;
  }
  
  // find the inliers
  vcl_vector<unsigned> inlier_cnts(cam_trans.size(), 0);
  vcl_vector<vcl_vector<unsigned> > inliers;
  for (unsigned i = 0; i < cam_trans.size(); i++) {  // for each correction find how many inliers are there for it
    vcl_cout << "offset_u: " << cam_trans[i][0].x() << " v: " << cam_trans[i][0].y() << vcl_endl;
    vcl_vector<unsigned> inliers_i;
    inliers_i.push_back(i); // first push itself
    for (unsigned j = 0; j < cam_trans.size(); j++) {
      if (i == j) continue;
      double dif = 0;
      for (unsigned k = 0; k < cam_trans[i].size(); k++) {
        vgl_point_2d<double> trans1(cam_trans[i][k].x(), cam_trans[i][k].y());
        vgl_point_2d<double> trans2(cam_trans[j][k].x(), cam_trans[j][k].y());
        dif += vgl_distance(trans1, trans2);
      }
      dif /= cam_trans[i].size();
      if (dif < pix_rad) {
        inlier_cnts[i]++;
        inliers_i.push_back(j);
      }
    }
    inliers.push_back(inliers_i);
  }
  vcl_cout << "out of " << n << " correspondences, inlier cnts using pixel radius: " << pix_rad << " : " << vcl_endl;
  unsigned max = 0; unsigned max_i = 0;
  for (unsigned i = 0; i < cam_trans.size(); i++) {
    vcl_cout << " " << inlier_cnts[i];
    if (max < inlier_cnts[i]) {
      max = inlier_cnts[i];
      max_i = i;
    }
  }
  vcl_cout << vcl_endl << " using translations of 1 pt with the most inliers: \n";
  for (unsigned k = 0; k < cams.size(); k++) {
    vcl_cout << "offset_u: " << cam_trans[max_i][k].x() << " offset_v: " << cam_trans[max_i][k].y() << '\n';
  }
  
  // use the correspondence with the most number of inliers to correct the cameras
  for (unsigned k = 0; k < cams.size(); k++) {
    double u_off,v_off;
    cams[k].image_offset(u_off,v_off);
    cams[k].set_image_offset(u_off + cam_trans[max_i][k].x(), v_off + cam_trans[max_i][k].y());
    cams[k].save(out_cam_names[k]);
  }

  // refine the cameras using all the inliers of this correspondence
  vcl_vector<vcl_vector<vgl_point_2d<double> > > corrs_inliers;
  
  for (unsigned k = 0; k < new_corrs.size(); k++) {
    vcl_vector<vgl_point_2d<double> > vec;
    for (unsigned j = 0; j < inliers[max_i].size(); j++)
      vec.push_back(new_corrs[k][inliers[max_i][j]]);
    corrs_inliers.push_back(vec);
  }

  vcl_vector<vgl_vector_2d<double> > cam_trans_inliers;
  vcl_vector<vgl_point_3d<double> > intersections;
  //vcl_vector<float> cam_weights_equal(cams.size(), 1.0f/cams.size());
  //if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights_equal, corrs_inliers, cam_trans_inliers, intersections))
  if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, cam_trans_inliers, intersections))
  {
    vcl_cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  vcl_cout << " after refinement: \n";
  for (unsigned i = 0; i < intersections.size(); i++)
    vcl_cout << "after adjustment 3D intersection point: " << intersections[i] << vcl_endl;

  for (unsigned i = 0; i < cam_trans_inliers.size(); i++)   // for each correction find how many inliers are there for it
    vcl_cout << "offset_u: " << cam_trans_inliers[i].x() << " v: " << cam_trans_inliers[i].y() << vcl_endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    double u_off,v_off;
    cams[i].image_offset(u_off,v_off);
    cams[i].set_image_offset(u_off + cam_trans_inliers[i].x(), v_off + cam_trans_inliers[i].y());
    cams[i].save(out_cam_names[i]);
  }
  
  return true;
}


