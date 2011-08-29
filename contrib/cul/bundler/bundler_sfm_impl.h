#ifndef BUNDLER_SFM_IMPL_H
#define BUNDLER_SFM_IMPL_H
//:
// \file

#include <bundler/bundler_settings.h>
#include <bundler/bundler_sfm.h>
#include <bundler/bundler_inters.h>

#include <vnl/vnl_double_3x3.h>

//:
// The default implementation of the function to create the initial
// reconstruction. Takes in the track set, and returns the initial
// reconstruction.
class bundler_sfm_impl_create_initial_recon
  : public bundler_sfm_create_initial_recon
{
 private:
  bundler_settings_create_initial_recon settings;

 public:
  bundler_sfm_impl_create_initial_recon()
    : settings(bundler_settings_create_initial_recon()) {}

  bundler_sfm_impl_create_initial_recon (
      bundler_settings_create_initial_recon s) : settings(s) {}

  void operator() (
      bundler_inters_track_set &track_set,
      bundler_inters_reconstruction &reconstruction);

 private:
  double get_homography_inlier_percentage(
      const bundler_inters_match_set &match);

  void get_homography(
      const vcl_vector<vgl_point_2d<double> > &rhs,
      const vcl_vector<vgl_point_2d<double> > &lhs,
      vnl_double_3x3 &homography);
};


//:
// The default implementation of the select next image phase.
// Constructors same as above.
class bundler_sfm_default_select_next_images
  : public bundler_sfm_select_next_images
{
 private:
  bundler_settings_select_next_images settings;

 public:
  bundler_sfm_default_select_next_images()
    : settings(bundler_settings_select_next_images()) {}

  bundler_sfm_default_select_next_images (
      bundler_settings_select_next_images s) : settings(s) {}

  //: Takes in reconstruction and track_set, fills to_add as a return val
  // Returns true if an image was found that observes the minimum number
  // of points
  bool operator() (
      bundler_inters_reconstruction &reconstruction,
      bundler_inters_track_set &track_set,
      vcl_vector<bundler_inters_feature_set_sptr> &to_add);
};


//:
// The default implementation of the add next image stage.
class bundler_sfm_default_add_next_images
  : public bundler_sfm_add_next_images
{
 private:
  bundler_settings_add_next_images settings;

 public:
  bundler_sfm_default_add_next_images()
    : settings(bundler_settings_add_next_images()) {}

  bundler_sfm_default_add_next_images (
      bundler_settings_add_next_images s) : settings(s) {}

  //: Adds to_the reconstruction
  void operator() (
      bundler_inters_reconstruction reconstruction,
      vcl_vector<bundler_inters_camera> &added_cameras,
      const vcl_vector<bundler_inters_feature_set_sptr> &to_add);
};


//:
// The default implementation of the add new points stage.
class bundler_sfm_default_add_new_points
  : public bundler_sfm_add_new_points
{
 private:
  bundler_settings_add_new_points settings;

 public:
  bundler_sfm_default_add_new_points()
    : settings(bundler_settings_add_new_points()) {}

  bundler_sfm_default_add_new_points (
      bundler_settings_add_new_points s) : settings(s) {}

  void operator() (
      bundler_inters_reconstruction &reconstruction,
      bundler_inters_track_set &track_set,
      const vcl_vector<bundler_inters_camera> &added_cameras);
};


//:
// The default implementation of bundle adjustment.
class bundler_sfm_default_bundle_adjust
  : public bundler_sfm_bundle_adjust
{
 private:
  bundler_settings_bundle_adjust settings;

 public:
  bundler_sfm_default_bundle_adjust()
    : settings(bundler_settings_bundle_adjust()) {}

  bundler_sfm_default_bundle_adjust (
      bundler_settings_bundle_adjust s) : settings(s) {}

  //: Adjusts the reconstruction using nonlinear least squares
  void operator() (
      bundler_inters_reconstruction reconstruction);
};

#endif // BUNDLER_SFM_IMPL_H
