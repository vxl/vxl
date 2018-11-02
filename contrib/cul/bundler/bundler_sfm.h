#ifndef BUNDLER_SFM_H
#define BUNDLER_SFM_H
//:
// \file

#include <iostream>
#include <vector>
#include <bundler/bundler_inters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//:
// An abstract functor. Takes in the track set, and fills the
// initial reconstruction. Returns true if it was successful,
// false otherwise.
class bundler_sfm_create_initial_recon
{
 public:
  virtual bool operator() (
      bundler_inters_reconstruction &reconstruction) = 0;
  virtual ~bundler_sfm_create_initial_recon() = default;
};


//:
// Takes in the current reconstruction and the image set, and selects the
// next images to be added to the reconstruction.
class bundler_sfm_select_next_images
{
 public:
  //: Takes in reconstruction and track_set, fills to_add as a return val.
  // Returns true if at least one image was found that fits whatever the
  // criteria for a good next image is. If this returns false, the
  // reconstruction should be considered to be complete.
  virtual bool operator() (
      bundler_inters_reconstruction &reconstruction,

      std::vector<bundler_inters_image_sptr> &to_add) = 0;
  virtual ~bundler_sfm_select_next_images() = default;
};


//:
// Takes in the current reconstruction and an image, and adds the image to
// the reconstruction. This calculates the pose of the images' camera.
// added_cameras is filled with references to the created cameras, for
// the next stage.
class bundler_sfm_add_next_images
{
 public:
  virtual void operator() (
      const std::vector<bundler_inters_image_sptr> &to_add,

      bundler_inters_reconstruction &reconstruction,
      std::vector<bundler_inters_image_sptr> &added_images) = 0;
  virtual ~bundler_sfm_add_next_images() = default;
};


//:
// Adds points to the reconstruction using the new cameras calculated in the
// add_next_images stage. to_add are the cameras that were added in the
// previous stage.
class bundler_sfm_add_new_points
{
 public:
  virtual void operator() (
      bundler_inters_reconstruction &reconstruction,
      const std::vector<bundler_inters_image_sptr> &added) = 0;
  virtual ~bundler_sfm_add_new_points() = default;
};


//:
// Performs bundle adjustment on the reconstruction.
class bundler_sfm_bundle_adjust
{
 public:
  virtual void operator() (
      bundler_inters_reconstruction &reconstruction) = 0;
  virtual ~bundler_sfm_bundle_adjust() = default;
};

#endif // BUNDLER_SFM_H
