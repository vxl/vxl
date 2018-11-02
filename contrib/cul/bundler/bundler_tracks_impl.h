// This is cul/bundler/bundler_tracks_impl.h
#ifndef BUNDLER_TRACKS_IMPL
#define BUNDLER_TRACKS_IMPL
//:
// \file
// Provides a default implementation of the feature detect routines. These
// can all be overridden by creating a new class that inherits from the
// relevant stage. These defaults are the most general case.

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_resource.h>
#include <bundler/bundler_tracks.h>
#include <bundler/bundler_settings.h>

//:
// This is the default of the features detect stage.
// It uses SIFT to find all of the features in the image set.
class bundler_tracks_impl_detect_sift
  : public bundler_tracks_detect
{
 protected:
  bundler_settings_sift settings;

 public:
  //: A default constructor that uses default settings
  bundler_tracks_impl_detect_sift()
    : settings(bundler_settings_sift()) {}

  //: A constructor that allows the user to override thresholds and such
  bundler_tracks_impl_detect_sift(
      bundler_settings_sift s) : settings(s) {}

  //: Performs the feature matching
  bundler_inters_image_sptr operator() (
      const vil_image_resource_sptr &image,
      const double exif_focal_len) override;
};


//: A functor that provides the default matching algorithm.
// It outputs a  set of pairs of images that should be matched.
// Constructors are the  same as above.
class bundler_tracks_impl_propose_matches_all
  : public bundler_tracks_propose_matches
{
 protected:
  bundler_settings_propose_matches_all settings;

 public:
  bundler_tracks_impl_propose_matches_all()
    : settings(bundler_settings_propose_matches_all()) {}

  bundler_tracks_impl_propose_matches_all(
      bundler_settings_propose_matches_all s) : settings(s) {}

    //: Performs the match list creation
  void operator() (
      const std::vector<bundler_inters_image_sptr> &features,
      std::vector<bundler_inters_image_pair> &proposed_matches) override;
};


//:
// A functor that takes in a pair of images to be matched,
// finds all corresponding features between the two images.
class bundler_tracks_impl_match_ann
  : public bundler_tracks_match
{
 protected:
  bundler_settings_match_ann settings;

 public:
  bundler_tracks_impl_match_ann()
    : settings(bundler_settings_match_ann()) {}

  bundler_tracks_impl_match_ann(
      bundler_settings_match_ann s) : settings(s) {}

  //: Performs the feature matching
  void operator() (
    const bundler_inters_image_pair &to_match,
    bundler_inters_match_set &matches) override;
};


//:
// A functor that takes refines a set of matches features
// according to some thresholds.
class bundler_tracks_impl_refine_epipolar
  : public bundler_tracks_refine
{
 protected:
  bundler_settings_refine_epipolar settings;

 public:
  bundler_tracks_impl_refine_epipolar()
    : settings(bundler_settings_refine_epipolar()) {}

  bundler_tracks_impl_refine_epipolar(
      bundler_settings_refine_epipolar s) : settings(s) {}

  // Performs the feature matching.
  void operator() (bundler_inters_match_set &matches) override;
};


//:
// An functor that takes in all matched features between pairs
// of images, and chains them together into tracks
class bundler_tracks_impl_chain_matches
  : public bundler_tracks_chain_matches
{
 protected:
  bundler_settings_chain_matches settings;

 public:
  bundler_tracks_impl_chain_matches()
    : settings(bundler_settings_chain_matches()) {}

  bundler_tracks_impl_chain_matches(
      bundler_settings_chain_matches s) : settings(s) {}

  //: Performs the track chaining
  void operator() (
      std::vector<bundler_inters_match_set> &match_set,
      std::vector<bundler_inters_image_sptr> &images,
      std::vector<bundler_inters_track_sptr> &tracks) override;
};


#endif // BUNDLER_TRACKS_IMPL
