#ifndef VPGL_BUNDLER_SFM_IMPL_H
#define VPGL_BUNDLER_SFM_IMPL_H
//:
// \file

#include <vpgl/algo/vpgl_bundler_settings.h>
#include <vpgl/algo/vpgl_bundler_sfm.h>
#include <vpgl/algo/vpgl_bundler_inters.h>

#include <vnl/vnl_double_3x3.h>

//:
// The default implementation of the function to create the initial
// reconstruction. Takes in the track set, and returns the initial
// reconstruction.
class vpgl_bundler_sfm_impl_create_initial_recon
  : public vpgl_bundler_sfm_create_initial_recon
{
 private:
  vpgl_bundler_settings_create_initial_recon settings;

 public:
  vpgl_bundler_sfm_impl_create_initial_recon()
    : settings(vpgl_bundler_settings_create_initial_recon()) {}

  vpgl_bundler_sfm_impl_create_initial_recon (
      vpgl_bundler_settings_create_initial_recon s) : settings(s) {}

  void operator() (
      vpgl_bundler_inters_track_set &track_set,
      vpgl_bundler_inters_reconstruction &reconstruction);

 private:
  double get_homography_inlier_percentage(
      const vpgl_bundler_inters_match_set &match);

  void get_homography(
      const vcl_vector<vgl_point_2d<double> > &rhs,
      const vcl_vector<vgl_point_2d<double> > &lhs,
      vnl_double_3x3 &homography);
};


//:
// The default implementation of the select next image phase.
// Constructors same as above.
class vpgl_bundler_sfm_default_select_next_images
  : public vpgl_bundler_sfm_select_next_images
{
 private:
  vpgl_bundler_settings_select_next_images settings;

 public:
  vpgl_bundler_sfm_default_select_next_images()
    : settings(vpgl_bundler_settings_select_next_images()) {}

  vpgl_bundler_sfm_default_select_next_images (
      vpgl_bundler_settings_select_next_images s) : settings(s) {}

  //: Takes in reconstruction and track_set, fills to_add as a return val
  void operator() (
      vpgl_bundler_inters_reconstruction &reconstruction,
      vpgl_bundler_inters_track_set &track_set,

      vcl_vector<vpgl_bundler_inters_feature_set_sptr> &to_add);
};


//:
// The default implementation of the add next image stage.
class vpgl_bundler_sfm_default_add_next_images
  : public vpgl_bundler_sfm_add_next_images
{
 private:
  vpgl_bundler_settings_add_next_images settings;

 public:
  vpgl_bundler_sfm_default_add_next_images()
    : settings(vpgl_bundler_settings_add_next_images()) {}

  vpgl_bundler_sfm_default_add_next_images (
      vpgl_bundler_settings_add_next_images s) : settings(s) {}

  //: Adds to_the reconstruction
  void operator() (
      vpgl_bundler_inters_reconstruction reconstruction,
      const vcl_vector<vpgl_bundler_inters_feature_set> &to_add);
};


//:
// The default implementation of the add new points stage.
class vpgl_bundler_sfm_default_add_new_points
  : public vpgl_bundler_sfm_add_new_points
{
 private:
  vpgl_bundler_settings_add_new_points settings;

 public:
  vpgl_bundler_sfm_default_add_new_points()
    : settings(vpgl_bundler_settings_add_new_points()) {}

  vpgl_bundler_sfm_default_add_new_points (
      vpgl_bundler_settings_add_new_points s) : settings(s) {}

  void operator() (
      vpgl_bundler_inters_reconstruction &reconstruction,
      vpgl_bundler_inters_track_set &track_set);
};


//:
// The default implementation of bundle adjustment.
class vpgl_bundler_sfm_default_bundle_adjust
  : public vpgl_bundler_sfm_bundle_adjust
{
 private:
  vpgl_bundler_settings_bundle_adjust settings;

 public:
  vpgl_bundler_sfm_default_bundle_adjust()
    : settings(vpgl_bundler_settings_bundle_adjust()) {}

  vpgl_bundler_sfm_default_bundle_adjust (
      vpgl_bundler_settings_bundle_adjust s) : settings(s) {}

  //: Adjusts the reconstruction using nonlinear least squares
  void operator() (
      vpgl_bundler_inters_reconstruction reconstruction);
};

#endif // VPGL_BUNDLER_SFM_IMPL_H
