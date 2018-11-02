#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <bwm/bwm_observer_cam.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_3d_corr.h>
#include <bwm/bwm_3d_corr_sptr.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_ortho_procrustes.h>

// the resulting similarity maps from the coordinate frame of pts1
// to the coordinate frame of pts0
static bool compute_similarity(vnl_matrix<double> const& pts0,
                               vnl_matrix<double> const& pts1,
                               vgl_rotation_3d<double>& R,
                               vnl_vector_fixed<double, 3>& t,
                               double& scale)
{
  vpgl_ortho_procrustes op(pts0, pts1);
  R = op.R();
  t = op.t();
  scale = op.s();
  if (! op.compute_ok()) return false;
  std::cout << "Ortho procrustes error "
           << std::sqrt(op.residual_mean_sq_error()) << '\n';
  return true;
}

// the input camera is in the coordinate system of pts0. The output camera
// is the camera the coordinate system of pts 0 mapped to the
// coordinate system of pts1, that is,
//
//  x1 =  K[R0|t0](Hs Hs^-1) X1, where Hs is the similarity transform.
//
// Thus, the similarity transform is applied to the camera as,
// (s = scale)
//                        _     _  _      _
//                       |s 0 0 0||        |
//  K[R' | t'] = K[R0|t0]|0 s 0 0||  Rs  ts|
//                       |0 0 s 0||        |
//                       |0 0 0 1|| 0 0 0 1|
//                        -      - -      -
// It follows that R' = R0*Rs and t' = t0/s + R0*ts
//
static vpgl_perspective_camera<double>
transform_camera(vpgl_perspective_camera<double> const& cam,
                 vgl_rotation_3d<double> const& Rs,
                 vnl_vector_fixed<double, 3> const& ts,
                 const double scale)
{
  vnl_matrix_fixed<double,3,3> Rms = Rs.as_matrix();
  //Get input camera components
  //note, the homogeneous calibration matrix is unaffected by the scale
  vpgl_calibration_matrix<double> K = cam.get_calibration();
  vnl_matrix_fixed<double, 3, 3> R = cam.get_rotation().as_matrix();
  vgl_vector_3d<double> tv = cam.get_translation();
  vnl_vector_fixed<double, 3> t(tv.x(), tv.y(), tv.z());
  //compose rotations
  vnl_matrix_fixed<double, 3, 3> Rt = R*Rms;
  vgl_rotation_3d<double> Rtr(Rt);
  //compute new translation
  vnl_vector_fixed<double, 3> tt = (1.0/scale)*t + R*ts;
  vgl_vector_3d<double> ttg(tt[0], tt[1], tt[2]);
  //construct transformed camera
  vpgl_perspective_camera<double> camt(K, Rtr, ttg);
  return camt;
}

// this executable finds a similarity transform, given a set of corresponding
// 3-d points. The similiarity transform is then applied to a directory of
// perspective cameras to produce cameras in the new coordinate system

int main(int argc, char** argv)
{
  //Get Inputs
  vul_arg<std::string> corrs_path   ("-corrs", "corr input file",  "");
  vul_arg<std::string> gps_file ("-gps", "gps text file (x y z in local coords per line)", "");
  vul_arg<std::string> xform_file("-xform","xform file sRT ( 3x4) file ","");
  vul_arg<std::string> input_cam_dir ("-in_cam_dir","directory to get cams","");
  vul_arg<std::string> output_cam_dir ("-out_cam_dir","directory to store cams", "");

  if (argc != 7) {
    std::cout << "usage: bwm_3d_site_transform <-corrs <corr file>, -gps <gps file> > -in_cam_dir <dir> -out_cam_dir <dir>\n";
    std::cout << "Use either -corrs to specify correspondences, or -gps to specify a text file with gps coordinates of the camera centers in local coordinates" << std::endl;
    return -1;
  }

  vul_arg_parse(argc, argv);

  // verify input camera dir
  if (!vul_file::is_directory(input_cam_dir().c_str()))
  {
    std::cout<<"Input Camera directory does not exist"<<std::endl;
    return -1;
  }

  // verify output camera dir
  if (!vul_file::is_directory(output_cam_dir().c_str()))
  {
    std::cout<<"Output Camera directory does not exist"<<std::endl;
    return -1;
  }

  vnl_matrix<double> pts0, pts1;
  vgl_rotation_3d<double> R;
  vnl_vector_fixed<double, 3> t;
  double scale;
  if (corrs_path() != "") {
    std::vector<bwm_3d_corr_sptr> corrs;
    bwm_observer_mgr::load_3d_corrs(corrs_path(), corrs);
    // assume correspondences between two sites only
    unsigned n = corrs.size();
    pts0.set_size(3,n);
    pts1.set_size(3,n);
    for (unsigned i = 0; i<n; ++i) {
      std::cout << *(corrs[i]);
      std::vector<vgl_point_3d<double> > match_pts = corrs[i]->matching_pts();
      pts0[0][i] = match_pts[0].x();  pts1[0][i] = match_pts[1].x();
      pts0[1][i] = match_pts[0].y();  pts1[1][i] = match_pts[1].y();
      pts0[2][i] = match_pts[0].z();  pts1[2][i] = match_pts[1].z();
    }
    if (!compute_similarity(pts0, pts1, R, t, scale)) {
        std::cout << "similarity computation failed\n";
        return -1;
    }
  }
  else if(gps_file() != "") {
    std::cout << "Using GPS file to register cameras." << std::endl;
    // create list of camera centers
    std::vector<vgl_point_3d<double> > cam_centers_bundler;
    std::vector<vgl_point_3d<double> > cam_centers_gps;
    // get directory listing and sort
    std::vector<std::string> filenames;
    std::string in_dir = input_cam_dir() + "/*.txt";
    for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
      std::string fname = fn();
      filenames.push_back(fname);
    }
    std::sort(filenames.begin(), filenames.end());

    std::string gps_fname = gps_file();
    std::ifstream gps_ifs(gps_fname.c_str());
    if (!gps_ifs.good()) {
      std::cerr << "ERROR: error opening gps file " << gps_fname << std::endl;
      return -1;
    }
    unsigned int n_cams = filenames.size();
    for (unsigned int i=0; i<n_cams; ++i) {
      double gps_x, gps_y, gps_z;
      gps_ifs >> gps_x >> gps_y >> gps_z;
      vgl_point_3d<double> gps_pt(gps_x, gps_y, gps_z);
      cam_centers_gps.push_back(gps_pt);

      std::ifstream cam_ifs(filenames[i].c_str());
      vpgl_perspective_camera<double> cam;
      cam_ifs >> cam;
      cam_ifs.close();
      cam_centers_bundler.push_back(cam.get_camera_center());
    }
    gps_ifs.close();
    // save points to a matrix
    pts0.set_size(3,n_cams);
    pts1.set_size(3,n_cams);
    for (unsigned i = 0; i<n_cams; ++i) {
      pts0[0][i] = cam_centers_bundler[i].x();  pts1[0][i] = cam_centers_gps[i].x();
      pts0[1][i] = cam_centers_bundler[i].y();  pts1[1][i] = cam_centers_gps[i].y();
      pts0[2][i] = cam_centers_bundler[i].z();  pts1[2][i] = cam_centers_gps[i].z();
    }
    if (!compute_similarity(pts0, pts1, R, t, scale)) {
        std::cout << "similarity computation failed\n";
        return -1;
    }
  }
  else if(xform_file() != "")
  {
      std::ifstream ifile( xform_file().c_str() ) ;
      if(!ifile)
      {
          std::cout<<"Error: Cannot open" <<xform_file()<<std::endl;
          return -1;
      }
      ifile >> scale ;
      vnl_matrix<double> mat(4,4);
      ifile >> mat;

      vnl_matrix<double> matr(3,3);
      mat.extract(matr);
      matr = matr/scale;

      matr = (matr.transpose());
      vgl_rotation_3d<double> r1(matr);R =r1;

      t[0] = mat(0,3);t[1] = mat(1,3);t[2] = mat(2,3);
      t = - matr*t ;

      scale = 1/scale;
  }
  else
  {
      std::cout<<"No Way to transform the cameras found "<<std::endl;
      return -1;
  }


  std::cout << "scale = " << scale << "\nR = " << R << "\nt = " << t << '\n';
  //transform the cameras
  std::string in_dir = input_cam_dir() + "/*.txt";
  for (vul_file_iterator fn = in_dir.c_str(); fn; ++fn) {
    std::string f = fn();
    std::ifstream is(f.c_str());
    vpgl_perspective_camera<double> cam;
    is >> cam;
    is.close();
    std::string fname = vul_file::strip_directory(f.c_str());
    std::cout << fname << '\n';
    vpgl_perspective_camera<double> tcam =
      transform_camera(cam, R, t, scale);
    std::cout<<"CC : "<<tcam.camera_center()<<std::endl;
    std::string out_dir = output_cam_dir() + "/";
    std::string out_file = out_dir + fname;
    std::ofstream os(out_file.c_str());
    os << tcam;
    os.close();
  }
  return 0;
}
