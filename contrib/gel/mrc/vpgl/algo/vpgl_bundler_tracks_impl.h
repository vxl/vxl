#ifndef VPGL_BUNDLER_TRACKS_IMPL
#define VPGL_BUNDLER_TRACKS_IMPL

#include <vcl_vector.h>
#include <vil/vil_image_resource.h>
#include <vpgl/algo/vpgl_bundler_tracks.h>
#include <vpgl/algo/vpgl_bundler_settings.h>

/*Provides a default implementation of the feature detect routines. These
 * can all be overrided by creating a new class that inherits from the 
 * relevant stage. These defaults are the most general case.*/

/*This is the default of the features detect stage. It uses SIFT to find
 * all of the features in the image set.*/
class vpgl_bundler_tracks_impl_detect_sift: 
    public vpgl_bundler_tracks_detect{

protected:
    vpgl_bundler_settings_sift settings;

public:
    /*A default constructor that uses default settings*/
    vpgl_bundler_tracks_impl_detect_sift() : 
        settings(vpgl_bundler_settings_sift()){ }

    /*A constructor that allows the user to override thresholds and
     * such.*/
    vpgl_bundler_tracks_impl_detect_sift(
        vpgl_bundler_settings_sift s) : settings(s){ }

    /*Performs the feature matching.*/
    vpgl_bundler_inters_feature_set_sptr operator ()(
        const vil_image_resource_sptr &image,
        const double exif_focal_len);
};



/*An functor that provides the default matching algorithm. It outputs a 
 * set of pairs of images that should be matched. Constructors are the 
 * same as above.*/
class vpgl_bundler_tracks_impl_propose_matches_all: 
    public vpgl_bundler_tracks_propose_matches{

protected:
    vpgl_bundler_settings_propose_matches_all settings;

public:
    vpgl_bundler_tracks_impl_propose_matches_all() : 
        settings(vpgl_bundler_settings_propose_matches_all()){ }

    vpgl_bundler_tracks_impl_propose_matches_all(
        vpgl_bundler_settings_propose_matches_all s) : settings(s){ }

    /*Performs the match list creation.*/
    void operator ()(
        const vcl_vector<vpgl_bundler_inters_feature_set_sptr> &features,
        vcl_vector<vpgl_bundler_inters_feature_set_pair> &proposed_matches);
};



/*A functor that takes in a pair of images to be matched,
 * finds all corresponding features between the two images.*/
class vpgl_bundler_tracks_impl_match_ann : 
    public vpgl_bundler_tracks_match {

protected:
    vpgl_bundler_settings_match_ann settings;

public:
    vpgl_bundler_tracks_impl_match_ann() : 
        settings(vpgl_bundler_settings_match_ann()) { }

    vpgl_bundler_tracks_impl_match_ann(
            vpgl_bundler_settings_match_ann s) : settings(s){ }

    /*Performs the feature matching.*/
    void operator ()(
        const vpgl_bundler_inters_feature_set_pair &to_match,
        vpgl_bundler_inters_match_set &matches);
}; 


/*A functor that takes refines a set of matches features
 * according to some thresholds.*/
class vpgl_bundler_tracks_impl_refine_epipolar : 
    public vpgl_bundler_tracks_refine {

protected:
    vpgl_bundler_settings_refine_epipolar settings;

public:
    vpgl_bundler_tracks_impl_refine_epipolar() : 
        settings(vpgl_bundler_settings_refine_epipolar()) { }

    vpgl_bundler_tracks_impl_refine_epipolar(
            vpgl_bundler_settings_refine_epipolar s) : settings(s){ }

    /*Performs the feature matching.*/
    void operator ()(vpgl_bundler_inters_match_set &matches);
}; 


/*An functor that takes in all matched features between pairs 
 * of images, and chains them together into tracks*/
class vpgl_bundler_tracks_default_chain_matches : 
    public vpgl_bundler_tracks_chain_matches { 

protected:
    vpgl_bundler_settings_chain_matches settings;

public:
    vpgl_bundler_tracks_default_chain_matches() : 
        settings(vpgl_bundler_settings_chain_matches()){ }

    vpgl_bundler_tracks_default_chain_matches(
        vpgl_bundler_settings_chain_matches s) : settings(s){ }

    /*Performs the track chaining.*/
    void operator ()(
        const vcl_vector<vpgl_bundler_inters_match_set> &match_set,
        vcl_vector<vpgl_bundler_inters_track> &tracks);
};


#endif /*VPGL_BUNDLER_TRACKS_IMPL*/
