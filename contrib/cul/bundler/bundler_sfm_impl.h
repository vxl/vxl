#ifndef BUNDLER_SFM_IMPL_H
#define BUNDLER_SFM_IMPL_H
//:
// \file

#include <bundler/bundler_settings.h>
#include <bundler/bundler_sfm.h>
#include <bundler/bundler_inters.h>

//: The default implementation of the function to create the initial reconstruction.
// Takes in the track set, and \returns the initial reconstruction.
class bundler_sfm_impl_create_initial_recon
  : public bundler_sfm_create_initial_recon
{
  bundler_settings_create_initial_recon settings;
  bool can_be_initial_recon(const bundler_inters_match_set &a);

 public:
  bundler_sfm_impl_create_initial_recon()
    : settings(bundler_settings_create_initial_recon()) {}

  bundler_sfm_impl_create_initial_recon (
      bundler_settings_create_initial_recon s) : settings(s) {}

  bool operator() (bundler_inters_reconstruction &reconstruction) override;
};


//: The default implementation of the select next image phase.
// Selects the image that observes the larget number of tracks
// whose 3D locations have already been estimated.
class bundler_sfm_impl_select_next_images
  : public bundler_sfm_select_next_images
{
  bundler_settings_select_next_images settings;

 public:
  bundler_sfm_impl_select_next_images()
    : settings(bundler_settings_select_next_images()) {}

  bundler_sfm_impl_select_next_images (
      bundler_settings_select_next_images s) : settings(s) {}

  //: Takes in reconstruction and track_set, fills to_add as a return val
  // \returns true if an image was found that observes the minimum number of points
  bool operator() (
      bundler_inters_reconstruction &reconstruction,

      std::vector<bundler_inters_image_sptr> &to_add) override;
};


//: The default implementation of the add next image stage.
class bundler_sfm_impl_add_next_images
  : public bundler_sfm_add_next_images
{
  bundler_settings_add_next_images settings;

 public:
  bundler_sfm_impl_add_next_images()
    : settings(bundler_settings_add_next_images()) {}

  bundler_sfm_impl_add_next_images (
      bundler_settings_add_next_images s) : settings(s) {}

  //: Adds to_the reconstruction
  void operator() (
      const std::vector<bundler_inters_image_sptr> &to_add,

      bundler_inters_reconstruction &reconstruction,
      std::vector<bundler_inters_image_sptr> &added_cameras) override;
};


//: The default implementation of the add new points stage.
class bundler_sfm_impl_add_new_points
  : public bundler_sfm_add_new_points
{
  bundler_settings_add_new_points settings;

 public:
  bundler_sfm_impl_add_new_points()
    : settings(bundler_settings_add_new_points()) {}

  bundler_sfm_impl_add_new_points (
      bundler_settings_add_new_points s) : settings(s) {}

  void operator() (
      bundler_inters_reconstruction &reconstruction,
      const std::vector<bundler_inters_image_sptr> &added_cameras) override;
};


//: The default implementation of bundle adjustment.
class bundler_sfm_impl_bundle_adjust
  : public bundler_sfm_bundle_adjust
{
  bundler_settings_bundle_adjust settings;

 public:
  bundler_sfm_impl_bundle_adjust()
    : settings(bundler_settings_bundle_adjust()) {}

  bundler_sfm_impl_bundle_adjust (
      bundler_settings_bundle_adjust s) : settings(s) {}

  //: Adjusts the reconstruction using nonlinear least squares
  void operator() (
      bundler_inters_reconstruction &reconstruction) override;
};

#endif // BUNDLER_SFM_IMPL_H
