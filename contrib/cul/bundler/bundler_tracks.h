#ifndef BUNDLER_TRACKS_H
#define BUNDLER_TRACKS_H
//:
// \file

#include <vcl_vector.h>
#include <vil/vil_image_resource.h>

#include <bundler/bundler_settings.h>
#include <bundler/bundler_inters.h>

//:
// An abstract functor that takes in an image, and returns features of
// that image.
class bundler_tracks_detect
{
 public:
  //: Performs the feature matching
  virtual bundler_inters_image_sptr operator() (
      const vil_image_resource_sptr &image,
      const double exif_focal_len) = 0;
};


//:
// An abstract functor that outputs a set of pairs of images that should
// be matched.
class bundler_tracks_propose_matches
{
 public:
  //: Performs the match list creation
  virtual void operator() (
      const vcl_vector<bundler_inters_image_sptr> &features,
      vcl_vector<bundler_inters_image_pair> &matches) = 0;
};


//:
// An abstract functor that takes in a pair of images to be matched,
// finds all corresponding features between the two images
class bundler_tracks_match
{
 public:
  //: Performs the feature matching
  virtual void operator() (
      const bundler_inters_image_pair &to_match,
      bundler_inters_match_set &matches) = 0;
}; 


//:
// An abstract functor that takes in corresponding features between 
// two images, and refines them according to some thresholds.
class bundler_tracks_refine
{
 public:
  //: Performs the feature matching
  virtual void operator() (bundler_inters_match_set &matches) = 0;
}; 


//:
// An abstract functor that takes in all matched features between pairs 
// of images, and chains them together into tracks
class bundler_tracks_chain_matches
{
 public:
  //: Performs the track chaining
  virtual void operator() (
      vcl_vector<bundler_inters_match_set> &match_sets,
      vcl_vector<bundler_inters_image_sptr> &images,
      vcl_vector<bundler_inters_track_sptr> &tracks) = 0;
};

#endif // BUNDLER_TRACKS_H
