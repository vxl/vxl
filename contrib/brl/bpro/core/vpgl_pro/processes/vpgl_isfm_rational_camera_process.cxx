
#include <iostream>
#include <sstream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take a list of rational cameras and a list of 2D image correspondences
//         some of the correspondences may be very poor/wrong but a majority is of good quality (i.e. corresponds to the same 3D point)
//         Use a RANSAC scheme to find offsets for each camera using "inlier" correspondences
//
//         This code was developed under contract #FA8650-13-C-1613 and is approved for public release.
//         The case number for the approval is 88ABW-2014-1143.
//
// \verbatim
//  Modifications
//   Yi Dong May 20, 2015 fix bugs due to invalid camera translation
//   Yi Dong May 21, 2015 modify the track file such that the seed camera file path are loaded from input track file
// \endverbatim
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
#include <vil/vil_load.h>
#include <vgl/vgl_distance.h>
//:
//  Take a list of rational cameras and a list of 2D image correspondences of the same 3D point location,
//  find that 3D location,
//  project that point back to images and correct each camera by adjusting its 2D image offset so that they all project the 3D location to the same 2D location
bool vpgl_isfm_rational_camera_process_cons(bprb_func_process& pro)
{
    std::vector<std::string> input_types;
    input_types.emplace_back("vcl_string");  // track file
    input_types.emplace_back("vcl_string");  // output folder to write the corrected cams
    input_types.emplace_back("float"); // radius in pixels for the disagreement among inliers, e.g. 2 pixels
    std::vector<std::string> output_types;
    output_types.emplace_back("vpgl_camera_double_sptr");
    output_types.emplace_back("float"); // projection error
    output_types.emplace_back("float"); // % inlier
    return pro.set_input_types(input_types)
        && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_isfm_rational_camera_process(bprb_func_process& pro)
{
    if (!pro.verify_inputs()) {
      std::cout << pro.name() << ": Wroing inputs" << std::endl;
      return false;
    }
    // get the inputs
    std::string trackfile = pro.get_input<std::string>(0);
    std::string nonseedcamdir = pro.get_input<std::string>(1);
    auto pix_rad = pro.get_input<float>(2);
#if 0
    //: load seed cams
    vul_file_iterator iter(seedcamdir + "/*corrected*rpb");
    std::vector<vpgl_local_rational_camera<double> * > lcams;
    while (iter())
    {
        vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(seedcamdir + "/" + iter.filename());
        if (!ratcam) {
            std::cerr << "Failed to load local rational camera from file" << iter.filename() << '\n';
            return false;
        }
        lcams.push_back(ratcam);
        ++iter;
    }
    vul_file_iterator iter2(nonseedcamdir + "/*corrected*rpb");
    while (iter2())
    {
        vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(nonseedcamdir + "/" + iter2.filename());
        if (!ratcam) {
            std::cerr << "Failed to load local rational camera from file" << iter2.filename() << '\n';
            return false;
        }
        lcams.push_back(ratcam);
        ++iter2;
    }
#endif
    //: load the tracks
    std::ifstream ifs(trackfile.c_str());
    if (!ifs) {
        std::cerr << " cannot open file: " << trackfile << '\n';
        return false;
    }
    // load the uncorrected camera file, the number of tracks and the number of seed cameras
    unsigned int n;
    unsigned n_seeds;
    std::string uncorrectedcam;
    ifs >> uncorrectedcam >> n >> n_seeds;
    vpgl_local_rational_camera<double> *cam = read_local_rational_camera<double>(uncorrectedcam);
    if (!cam) {
        std::cerr << "Failed to load local rational camera from file" << uncorrectedcam << '\n';
        return false;
    }
    std::cout << "will read: " << n << " tracks " << trackfile << std::endl;
    // load the seed cameras
    std::vector<std::string> seed_cam_files;
    std::vector<vpgl_local_rational_camera<double> * > lcams;
    for (unsigned i = 0; i < n_seeds; i++) {
      unsigned id;
      std::string seed_file;
      ifs >> id >> seed_file;
      seed_cam_files.push_back(seed_file);
    }
    for (unsigned i = 0; i < n_seeds; i++) {
      vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(seed_cam_files[i]);
      if (!ratcam) {
        std::cerr << pro.name() << ": Failed to load local seed rational camera " << i << " from file: " << seed_cam_files[i] << '\n';
        return false;
      }
      lcams.push_back(ratcam);
    }
    std::cout << "will load: " << n_seeds << " seed cameras " << std::endl;
    // load all tracks
    std::vector<std::vector<vgl_point_2d<double> > > tracks;
    std::vector<std::vector<int> > trackimgids;
    std::vector<vgl_point_2d<double> > currpts;
    for (unsigned int i = 0; i < n; i++)
    {
        int numfeatures = 0;
        ifs >> numfeatures;
        int id; float u, v;
        ifs >> id >> u >> v;
        currpts.emplace_back(u, v);
        std::vector<int> ids;
        std::vector<vgl_point_2d<double> > pts;
        for (unsigned int j = 1; j < numfeatures; j++)
        {
            int id; float u, v;
            ifs >> id >> u >> v;
            pts.emplace_back(u, v);
            ids.push_back(id);
        }
        trackimgids.push_back(ids);
        tracks.push_back(pts);
    }
    // compute translations
    std::vector<vgl_vector_2d<double> > cam_trans;
    std::vector<vgl_point_3d<double> > reconstructed_points;
    std::vector<vgl_point_2d<double> > img_points;
    for (unsigned i = 0; i < n; i++) {
        std::vector<vgl_vector_2d<double> > cam_trans_i;
        std::vector<vgl_point_2d<double> > corrs;
        std::vector<float> weights;
        std::vector<vpgl_rational_camera<double>  > cams;
        cams.push_back(*cam);
        weights.push_back(0.0);  // the back-projection from un-corrected camera is ignored due to zero weight
        corrs.push_back(currpts[i]);
        for (unsigned k = 0; k < tracks[i].size(); k++)
        {
            cams.push_back(*lcams[trackimgids[i][k]]);
            weights.push_back(1.0);
            corrs.push_back(tracks[i][k]);
        }
        vgl_point_3d<double> intersection;
        if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, weights, corrs, cam_trans_i, intersection))
            continue;
        img_points.push_back(currpts[i]);
        cam_trans.push_back(cam_trans_i[0]);
        reconstructed_points.push_back(intersection);
    }

    // remove unrealistic trans
    std::vector<vgl_vector_2d<double> > cam_trans_new;
    for (auto & cam_tran : cam_trans)
      if (cam_tran.x() >= -1000) {
        cam_trans_new.push_back(cam_tran);
      }
    if (cam_trans_new.empty()) {
      std::cerr << pro.name() << ": can not find any valid translations, exit without correction!\n";
      return false;
    }

    // find the inliers from valid camera translations
    std::vector<unsigned> inlier_cnts(cam_trans_new.size(), 0);
    std::vector<std::vector<unsigned> > inliers;
    std::vector<std::vector<unsigned> > inlier_track_ids;
    for (unsigned i = 0; i < cam_trans_new.size(); i++) {  // for each correction find how many inliers are there for it
        std::vector<unsigned> inliers_i;
        std::vector<unsigned> inlier_track_id;
        // first push itself
        inliers_i.push_back(i);
        inlier_cnts[i]++;
        for (unsigned j = 0; j < cam_trans_new.size(); j++) {
            if (i == j || cam_trans_new[i].x() < -1000) continue;
            double dif = (cam_trans_new[i] - cam_trans_new[j]).length();
            if (dif < pix_rad) {
                inlier_cnts[i]++;
                inliers_i.push_back(j);
            }
        }
        inliers.push_back(inliers_i);
        inlier_track_ids.push_back(inlier_track_id);
    }
    unsigned max = 0; unsigned max_i = 0;
    for (unsigned i = 0; i < cam_trans_new.size(); i++) {
        if (max < inlier_cnts[i]) {
            max = inlier_cnts[i];
            max_i = i;
        }
    }
    // use the average correspondence with the most number of inliers to correct camera
    vgl_vector_2d<double> sum(0.0, 0.0);
    for (unsigned int k : inliers[max_i])
        sum += cam_trans_new[k];
    sum /= (double)inliers[max_i].size();

    std::cout << "camera: " << uncorrectedcam
             << " out of " << n << " correspondences, inlier cnts using pixel radius: " << pix_rad << ", "
             << " number of valid translation: " << cam_trans_new.size()
             << " inliers size: " << inliers[max_i].size() << "(" << max << ")"
             << " average offset: " << sum
             << std::endl;

    // evaluate the correction error and inlier percentage
    double error = 0.0;
    vpgl_rational_camera<double>  rcam = (*cam);
    for (unsigned int k : inliers[max_i]) {
        vgl_point_2d<double> uvp = rcam.project(reconstructed_points[k]);
        error += ((img_points[k] - uvp) - sum).sqr_length();
    }
    error = std::sqrt(error) / (double)inliers[max_i].size();
    double inlierpercent = (double)max / (double)cam_trans_new.size() * 100;

    double u, v;
    cam->image_offset(u, v);
    cam->set_image_offset(u + sum.x(), v + sum.y());
    std::cout << "Final Translation is " << sum << std::endl;

    pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
    pro.set_output_val<float>(1, error);
    pro.set_output_val<float>(2, inlierpercent);
    return true;
}
