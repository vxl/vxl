// This is brl/bbas/volm/pro/processes/volm_correct_rational_cameras_process.cxx
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take a list of rational cameras and a list of 2D image correspondences
//         some of the correspondences may be very poor/wrong but a majority is of good quality (i.e. corresponds to the same 3D point)
//         Use a RANSAC scheme to find offsets for each camera using "inlier" correspondences
//
//
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
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

vpgl_rational_camera<double>* load_cam_from_nitf(std::string& nitf_image_path) {
  vil_image_resource_sptr image = vil_load_image_resource(nitf_image_path.c_str());
  if (!image)
  {
    std::cout << "NITF image load failed from the file: " << nitf_image_path << '\n';
    return nullptr;
  }

  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);

  if (prefix != "nitf")
  {
    std::cout << "source image is not NITF in load_cam_from_nitf\n";
    return nullptr;
  }

  //cast to an nitf2_image
  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  auto *nitf_cam=new vpgl_nitf_rational_camera(nitf_image, true);

  return dynamic_cast<vpgl_rational_camera<double>* >(nitf_cam);
}

//:
//  Take a list of rational cameras and a list of 2D image correspondences of the same 3D point location,
//  find that 3D location,
//  project that point back to images and correct each camera by adjusting its 2D image offset so that they all project the 3D location to the same 2D location
bool volm_correct_rational_cameras_ransac_process_cons(bprb_func_process& pro)
{
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // a file that lists the name to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
                                    // .
                                    // .
                                    // .
  input_types.emplace_back("vcl_string");  // output folder to write the corrected cams
  input_types.emplace_back("float"); // radius in pixels for the disagreement among inliers, e.g. 2 pixels
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_correct_rational_cameras_ransac_process(bprb_func_process& pro)
{
    if (pro.n_inputs() < 3) {
        std::cout << "volm_correct_rational_cameras_ransac_process: The number of inputs should be 3" << std::endl;
        return false;
    }

    // get the inputs
    std::string input_cams = pro.get_input<std::string>(0);
    std::string output_path = pro.get_input<std::string>(1);
    auto pix_rad = pro.get_input<float>(2);


    std::ifstream ifs(input_cams.c_str());
    if (!ifs) {
        std::cerr << " cannot open file: " << input_cams << '\n';
        return false;
    }
    unsigned int n; ifs >> n;
    std::cout << "will read: " << n << " correspondences for each frame from " << input_cams << std::endl;

    std::vector<std::string> cam_names;
    std::vector<std::string> cam_sat_names;
    std::vector<std::vector<vgl_point_2d<double> > > corrs;
    std::vector<std::string> out_cam_names;

    while (!ifs.eof())
    {
        std::string cam_name;
        ifs >> cam_name;
        if (cam_name.size() < 2) break;
        std::cout << "reading cam: " << cam_name << std::endl;

        //std::string out_cam_name = vul_file::strip_extension(cam_name) + "_corrected.rpb";
        std::string out_cam_name = output_path + "/" + vul_file::strip_extension(vul_file::strip_directory(cam_name)) + "_corrected.rpb";
        std::cout << "out cam name: " << out_cam_name << std::endl;

        cam_names.push_back(cam_name);
        out_cam_names.push_back(out_cam_name);

        std::vector<vgl_point_2d<double> > corrs_frame;
        for (unsigned int ii = 0; ii < n; ++ii) {
            double i, j;
            ifs >> i; ifs >> j;
            vgl_point_2d<double> cor(i, j);
            corrs_frame.push_back(cor);
        }
        corrs.push_back(corrs_frame);
    }
    ifs.close();

    if (!n) {
        std::cerr << "In vpgl_correct_rational_cameras_process2 - 0 correspondences in file: " << input_cams << "! returning without correcting any cams!\n";
        return false;
    }

    std::vector<float> cam_weights;
    std::vector<vpgl_rational_camera<double> > cams;
    std::vector<vpgl_local_rational_camera<double> > lcams;
    std::vector<std::string> new_out_names;
    std::vector<std::string> new_sat_names;
    std::vector<std::vector<vgl_point_2d<double> > > new_corrs;

    unsigned cnt_exists = 0;

    // now determine which cameras already exist
    for (unsigned i = 0; i < out_cam_names.size(); i++) {

        if (vul_file::exists(out_cam_names[i])) {
            cnt_exists++;

            // use the corrected camera
            vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(out_cam_names[i]);
            if (!ratcam) {
                std::cerr << "Failed to load rational camera from file" << out_cam_names[i] << '\n';
                return false;
            }
            cams.push_back(*ratcam);
            lcams.push_back(*ratcam);
            cam_weights.push_back(1.0f);  // first pass 1 for everything
            new_out_names.push_back(out_cam_names[i]);
            //new_sat_names.push_back(cam_sat_names[i]);
            new_corrs.push_back(corrs[i]);
        }
        else {  // does not exist, then load the original camera and correct it

            vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(cam_names[i]);
            if (!ratcam) {
                std::cerr << "Failed to load rational camera from file" << cam_names[i] << '\n';
                return false;
            }
            cams.push_back(*ratcam);
            lcams.push_back(*ratcam);
            cam_weights.push_back(0.0f);
            new_out_names.push_back(out_cam_names[i]);
            //new_sat_names.push_back(cam_sat_names[i]);
            new_corrs.push_back(corrs[i]);
        }
    }

    // distribute the weights
    if (cnt_exists == 0)
      cam_weights.assign(cam_weights.size(), 1.0f / cams.size());
    else if (cnt_exists == out_cam_names.size()) {
      std::cerr << pro.name() << ": all cameras have been corrected, exit without any corrections!\n";
      return false;
    }
    else {
      if (cnt_exists < 2) { // enforce to have at least two existing
          std::cerr << " If there a pre-existing cams, then there should be at least two!\n";
          return false;
      }
    }

    if (cams.size() == 1) {
      std::cerr << pro.name() << ": only 1 cameras are found, exit without any corrections!\n";
      return false;
    }
    //if (cams.size() == 1) {
    //    if (new_sat_names[0] == "GeoEye-1") {
    //        cams[0].save(out_cam_names[0]);
    //        return true;
    //    }
    //    else
    //        return false;
    //}

    std::cout << " cam assigned weights: \n";
    for (unsigned i = 0; i < cams.size(); i++) {
        std::cout << new_out_names[i] << " weight: " << cam_weights[i] << '\n';
    }

    std::cout << "cams size: " << cams.size() << " corrs size: " << new_corrs.size() << std::endl;
    std::cout.flush();



    // adjust using each correspondence and save the offsets
    std::vector<std::vector<vgl_vector_2d<double> > > cam_trans;
    std::vector<unsigned> corrs_ids;
    for (unsigned i = 0; i < n; i++) {
        std::vector<vgl_point_2d<double> > corrs_i;
        corrs_i.reserve(new_corrs.size());
for (auto & new_corr : new_corrs) {
            corrs_i.push_back(new_corr[i]);
        }
        //std::cout << "Executing adjust image offsets\n";
        std::vector<vgl_vector_2d<double> > cam_trans_i;
        vgl_point_3d<double> intersection;

        if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, cam_weights, corrs_i, cam_trans_i, intersection))
            continue;
        //std::cout << "after adjustment 2d correspondence: " << i <<  " gives a 3D intersection point: " << intersection << std::endl;
        cam_trans.push_back(cam_trans_i);
        corrs_ids.push_back(i);
    }
    std::cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections:";
    for (unsigned int corrs_id : corrs_ids) {
      std::cout << " " << corrs_id;
    }
    std::cout << "\n";
    if (!cam_trans.size()) {
        std::cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections! exit without any corrections!\n";
        return false;
    }

    // find the inliers
    std::vector<unsigned> inlier_cnts(cam_trans.size(), 0);
    std::vector<std::vector<unsigned> > inliers;
    for (unsigned i = 0; i < cam_trans.size(); i++) {  // for each correction find how many inliers are there for it
        //std::cout << "offset_u: " << cam_trans[i][0].x() << " v: " << cam_trans[i][0].y() << std::endl;
        std::vector<unsigned> inliers_i;
        inliers_i.push_back(corrs_ids[i]); // first push itself
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
                inliers_i.push_back(corrs_ids[j]);
            }
        }
        inliers.push_back(inliers_i);
    }
    std::cout << "out of " << n << " correspondences, inlier cnts using pixel radius: " << pix_rad << " : " << std::endl;
    unsigned max = 0; unsigned max_i = 0;
    for (unsigned i = 0; i < cam_trans.size(); i++) {
        std::cout << " " << inlier_cnts[i];
        if (max < inlier_cnts[i]) {
            max = inlier_cnts[i];
            max_i = i;
        }
    }
    // use the correspondence with the most number of inliers to correct the cameras
    for (unsigned k = 0; k < cams.size(); k++) {
        double u_off, v_off;
        cams[k].image_offset(u_off, v_off);
        cams[k].set_image_offset(u_off + cam_trans[max_i][k].x(), v_off + cam_trans[max_i][k].y());
        lcams[k].set_image_offset(u_off + cam_trans[max_i][k].x(), v_off + cam_trans[max_i][k].y());
    }

    // refine the cameras using all the inliers of this correspondence
    std::vector<std::vector<vgl_point_2d<double> > > corrs_inliers;

    for (auto & new_corr : new_corrs) {
        std::vector<vgl_point_2d<double> > vec;
        for (unsigned int j : inliers[max_i])
            vec.push_back(new_corr[j]);
        corrs_inliers.push_back(vec);
    }

    std::vector<vgl_vector_2d<double> > cam_trans_inliers;
    std::vector<vgl_point_3d<double> > intersections;
    if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, cam_trans_inliers, intersections))
    {
        std::cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
        return false;
    }

    for (unsigned i = 0; i < cams.size(); i++) {
        double u_off, v_off;
        cams[i].image_offset(u_off, v_off);
        cams[i].set_image_offset(u_off + cam_trans_inliers[i].x(), v_off + cam_trans_inliers[i].y());
        lcams[i].set_image_offset(u_off + cam_trans_inliers[i].x(), v_off + cam_trans_inliers[i].y());
        lcams[i].save(out_cam_names[i]);
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
  std::vector<std::string> input_types;
  input_types.emplace_back("volm_satellite_resources_sptr");  // fetch the full path of the satellite image from the resource file
  input_types.emplace_back("vcl_string");  // a file that lists the name to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
                                    // .
                                    // .
                                    // .
  input_types.emplace_back("vcl_string");  // output folder to write the corrected cams
  input_types.emplace_back("float"); // radius in pixels for the disagreement among inliers, e.g. 2 pixels
  input_types.emplace_back("int");  // option to enforce having 2 existing cameras
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool volm_correct_rational_cameras_ransac_process2(bprb_func_process& pro)
{
  if (pro.n_inputs() < 5) {
    std::cout << "volm_correct_rational_cameras_ransac_process: The number of inputs should be 4" << std::endl;
    return false;
  }

  // get the inputs
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(0);
  std::string input_cams = pro.get_input<std::string>(1);
  std::string output_path  = pro.get_input<std::string>(2);
  auto pix_rad = pro.get_input<float>(3);
  bool enforce_existing = pro.get_input<int>(4) == 1 ? true : false;
  if (enforce_existing)
    std::cout << "!!!!!!! enforce to have at least 2 existing images!!\n";
  else
    std::cout << "!!!!!!! DO NOT enforce to have at least 2 existing images!!\n";

  std::ifstream ifs(input_cams.c_str());
  if (!ifs) {
    std::cerr<< " cannot open file: " << input_cams << '\n';
    return false;
  }
  unsigned int n; ifs >> n;
  std::cout << "will read: " << n << " correspondences for each frame from "<< input_cams << std::endl;

  std::vector<std::string> cam_names;
  std::vector<std::string> cam_sat_names;
  std::vector<std::vector<vgl_point_2d<double> > > corrs;
  std::vector<std::string> out_cam_names;

  while (!ifs.eof())
  {
    std::string cam_name;
    ifs >> cam_name;
    if (cam_name.size() < 2) break;
    std::cout << "reading cam: " << cam_name << std::endl;

    std::string out_cam_name = output_path + cam_name + "_corrected.rpb";
    std::cout << "out cam name: " << out_cam_name << std::endl;

    // locate the cam in the resources
    //std::string img_path = res->full_path(cam_name);
    std::pair<std::string, std::string> img_path = res->full_path(cam_name);
    if (img_path.first.compare("") == 0) {
      std::cerr << "Cannot locate: " << cam_name << " in satellite resources! exiting!\n";
      return false;
    }
    std::cout << img_path.first << std::endl;

    cam_names.push_back(img_path.first);
    cam_sat_names.push_back(img_path.second);
    out_cam_names.push_back(out_cam_name);

    std::vector<vgl_point_2d<double> > corrs_frame;
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
    std::cerr << "In vpgl_correct_rational_cameras_process2 - 0 correspondences in file: " << input_cams << "! returning without correcting any cams!\n";
    return false;
  }

  std::vector<float> cam_weights;
  std::vector<vpgl_rational_camera<double> > cams;
  std::vector<std::string> new_out_names;
  std::vector<std::string> new_sat_names;
  std::vector<std::vector<vgl_point_2d<double> > > new_corrs;

  unsigned cnt_exists = 0;

  // now determine which cameras already exist
  for (unsigned i = 0; i < out_cam_names.size(); i++) {

    if (vul_file::exists(out_cam_names[i])) {
      cnt_exists++;

      // use the corrected camera
      vpgl_rational_camera<double> *ratcam = read_rational_camera<double>(out_cam_names[i]);
      if ( !ratcam ) {
        std::cerr << "Failed to load rational camera from file" << out_cam_names[i] << '\n';
        return false;
      }
      cams.push_back(*ratcam);
      cam_weights.push_back(1.0f);  // first pass 1 for everything
      new_out_names.push_back(out_cam_names[i]);
      new_sat_names.push_back(cam_sat_names[i]);
      new_corrs.push_back(corrs[i]);
    } else {  // does not exist, then load the original camera and correct it

      vpgl_rational_camera<double> *ratcam = load_cam_from_nitf(cam_names[i]);
      if ( !ratcam ) {
        std::cerr << "Failed to load rational camera from file" << cam_names[i] << '\n';
        return false;
      }
      cams.push_back(*ratcam);
      cam_weights.push_back(0.0f);
      new_out_names.push_back(out_cam_names[i]);
      new_sat_names.push_back(cam_sat_names[i]);
      new_corrs.push_back(corrs[i]);
    }
  }
  if (enforce_existing && cnt_exists < 2) {
    std::cerr << " Enforcing condition to have 2 pre-existing corrected cameras! EXITING since there is: " << cnt_exists << " cameras.\n";
    return false;
  }

  // distribute the weights
  if (cnt_exists == 0)
    cam_weights.assign(cam_weights.size(), 1.0f/cams.size());
  else {
    if (cnt_exists < 2) { // enforce to have at least two existing
      std::cerr << " If there a pre-existing cams, then there should be at least two!\n";
      return false;
    }
  }

  //if (cams.size() == 1) {
  //  if (new_sat_names[0] == "GeoEye-1") {
  //    cams[0].save(out_cam_names[0]);
  //    return true;
  //  } else
  //    return false;
  //}

  std::cout << " cam assigned weights: \n";
  for (unsigned i = 0; i < cams.size(); i++) {
    std::cout << new_out_names[i] << " weight: " << cam_weights[i] << '\n';
  }

  std::cout << "cams size: " << cams.size() << " corrs size: " << new_corrs.size() << std::endl;
  std::cout.flush();



  // adjust using each correspondence and save the offsets
  std::vector<std::vector<vgl_vector_2d<double> > > cam_trans;
  std::vector<unsigned> corrs_ids;
  for (unsigned i = 0; i < n; i++) {
    std::vector<vgl_point_2d<double> > corrs_i;
    corrs_i.reserve(new_corrs.size());
for (auto & new_corr : new_corrs) {
      corrs_i.push_back(new_corr[i]);
    }

    //std::cout << "Executing adjust image offsets\n";
    std::vector<vgl_vector_2d<double> > cam_trans_i;
    vgl_point_3d<double> intersection;
    //if (!vpgl_rational_adjust_onept::adjust(cams, corrs_i, cam_trans_i, intersection))
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, cam_weights, corrs_i, cam_trans_i, intersection)) {
      std::cout << "correspondence adjustment failed for correspondence: " << std::endl;
      for (auto & i : corrs_i) {
        std::cout << "[" << i.x() << "," << i.y() << '\t';
      }
      std::cout << '\n';
      continue;
    }
    std::cout << i << " --> correspondence: ";
    for (auto & ii : corrs_i) {
        std::cout << "[" << ii.x() << "," << ii.y() << "]\t";
    }
    std::cout << " --> project to 3D intersection point: [" << std::setprecision(12) << intersection.y()
                                                         << "," << std::setprecision(12) << intersection.x()
                                                         << "," << std::setprecision(12) << intersection.z()
                                                         << "], giving offset: ";
    std::cout << " --> camera translation: ";
    for (auto & ii : cam_trans_i) {
      std::cout << "[" << ii.x() << "," << ii.y() << "]\t";
    }
    std::cout << '\n';
    cam_trans.push_back(cam_trans_i);
    corrs_ids.push_back(i);
  }
  std::cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections:";
  for (unsigned int corrs_id : corrs_ids) {
      std::cout << " " << corrs_id;
    }
    std::cout << "\n";
  if (!cam_trans.size()) {
    std::cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections! exit without any corrections!\n";
    return false;
  }

  // find the inliers
  std::vector<unsigned> inlier_cnts(cam_trans.size(), 0);
  std::vector<std::vector<unsigned> > inliers;
  for (unsigned i = 0; i < cam_trans.size(); i++) {  // for each correction find how many inliers are there for it
    //std::cout << "offset_u: " << cam_trans[i][0].x() << " v: " << cam_trans[i][0].y() << std::endl;
    std::vector<unsigned> inliers_i;
    inliers_i.push_back(corrs_ids[i]); // first push itself
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
        inliers_i.push_back(corrs_ids[j]);
      }
    }
    inliers.push_back(inliers_i);
  }
  std::cout << "out of " << n << " correspondences, inlier cnts using pixel radius: " << pix_rad << " : " << std::endl;
  unsigned max = 0; unsigned max_i = 0;
  for (unsigned i = 0; i < cam_trans.size(); i++) {
    std::cout << i << " --> " << inlier_cnts[i] << std::endl;
    if (max < inlier_cnts[i]) {
      max = inlier_cnts[i];
      max_i = i;
    }
  }
  std::cout << '\n';
  std::cout << std::endl << " using translations of 1 pt with the most inliers: " << max_i << "\n";
  for (unsigned k = 0; k < cams.size(); k++) {
    std::cout << "offset_u: " << cam_trans[max_i][k].x() << " offset_v: " << cam_trans[max_i][k].y() << '\n';
  }
  std::cout << "correspondence that provides inliers:" << std::endl;
  for (unsigned int j : inliers[max_i])
    std::cout << j << ' ';
  std::cout << '\n';

  // use the correspondence with the most number of inliers to correct the cameras
  for (unsigned k = 0; k < cams.size(); k++) {
    double u_off,v_off;
    cams[k].image_offset(u_off,v_off);
    cams[k].set_image_offset(u_off + cam_trans[max_i][k].x(), v_off + cam_trans[max_i][k].y());
    //cams[k].save(out_cam_names[k]);
  }

  // refine the cameras using all the inliers of this correspondence
  std::vector<std::vector<vgl_point_2d<double> > > corrs_inliers;
  for (auto & new_corr : new_corrs) {
    std::vector<vgl_point_2d<double> > vec;
    for (unsigned int j : inliers[max_i])
      vec.push_back(new_corr[j]);
    corrs_inliers.push_back(vec);
  }

  std::vector<vgl_vector_2d<double> > cam_trans_inliers;
  std::vector<vgl_point_3d<double> > intersections;
  //std::vector<float> cam_weights_equal(cams.size(), 1.0f/cams.size());
  //if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights_equal, corrs_inliers, cam_trans_inliers, intersections))
  if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, cam_trans_inliers, intersections))
  {
    std::cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  std::cout << " after refinement: \n";
  for (auto & intersection : intersections)
    std::cout << "after adjustment 3D intersection point: " << std::setprecision(12) << intersection.y() << "," << std::setprecision(12) << intersection.x()
                                                           << "," << std::setprecision(12) << intersection.z()
                                                           << std::endl;

  for (auto & cam_trans_inlier : cam_trans_inliers)   // for each correction find how many inliers are there for it
    std::cout << "offset_u: " << cam_trans_inlier.x() << " v: " << cam_trans_inlier.y() << std::endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    double u_off,v_off;
    cams[i].image_offset(u_off,v_off);
    cams[i].set_image_offset(u_off + cam_trans_inliers[i].x(), v_off + cam_trans_inliers[i].y());
    cams[i].save(out_cam_names[i]);
  }

  return true;
}
