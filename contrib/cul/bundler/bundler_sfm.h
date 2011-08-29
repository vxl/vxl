#ifndef BUNDLER_SFM_H
#define BUNDLER_SFM_H
//:
// \file

#include <bundler/bundler_inters.h>
#include <vcl_vector.h>

//: An abstract functor. Takes in the track set, and returns the initial reconstruction.
class bundler_sfm_create_initial_recon
{
 public:
  virtual void operator() (
      bundler_inters_track_set &track_set,
      bundler_inters_reconstruction &reconstruction) = 0;
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
      bundler_inters_track_set &track_set,

      vcl_vector<bundler_inters_feature_set_sptr> &to_add) = 0;
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
      bundler_inters_reconstruction reconstruction,
      vcl_vector<bundler_inters_camera> &added_cameras,
      const vcl_vector<bundler_inters_feature_set_sptr> &to_add) = 0;
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
      bundler_inters_track_set &track_set,
      const vcl_vector<bundler_inters_camera> &added) = 0;
};


//:
// Performs bundle adjustment on the reconstruction.
class bundler_sfm_bundle_adjust
{
 public:
  virtual void operator() (
      bundler_inters_reconstruction reconstruction) = 0;
};

#endif // BUNDLER_SFM_H
