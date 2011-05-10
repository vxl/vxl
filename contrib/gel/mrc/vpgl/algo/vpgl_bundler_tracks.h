#ifndef VPGL_BUNDLER_TRACKS_H
#define VPGL_BUNDLER_TRACKS_H

#include <vcl_vector.h>
#include <vil/vil_image_resource.h>

#include <vpgl/algo/vpgl_bundler_settings.h>
#include <vpgl/algo/vpgl_bundler_inters.h>


/*An abstract functor that takes in an image, and returns features of
 * that image.*/
class vpgl_bundler_tracks_detect{
public:
    /*Performs the feature matching.*/
    virtual vpgl_bundler_inters_feature_set_sptr operator ()(
        const vil_image_resource_sptr &image,
        const double exif_focal_len) = 0;
};


/*An abstract functor that outputs a set of pairs of images that should
 * be matched. */
class vpgl_bundler_tracks_propose_matches{
public:
    /*Performs the match list creation.*/
    virtual void operator ()(
        const vcl_vector<vpgl_bundler_inters_feature_set_sptr> &features,
        vcl_vector<vpgl_bundler_inters_feature_set_pair> &matches) = 0;
};


/*An abstract functor that takes in a pair of images to be matched,
 * finds all corresponding features between the two images*/
class vpgl_bundler_tracks_match{
public:
    /*Performs the feature matching.*/
    virtual void operator ()(
        const vpgl_bundler_inters_feature_set_pair &to_match,
        vpgl_bundler_inters_match_set &matches) = 0;
}; 


/*An abstract functor that takes in corresponding features between 
 * two images, and refines them according to some thresholds.*/
class vpgl_bundler_tracks_refine{
public:
    /*Performs the feature matching.*/
    virtual void operator ()(vpgl_bundler_inters_match_set &matches) = 0;
}; 


/*An abstract functor that takes in all matched features between pairs 
 * of images, and chains them together into tracks*/
class vpgl_bundler_tracks_chain_matches{
public:
    /*Performs the track chaining.*/
    virtual void operator ()(
        const vcl_vector<vpgl_bundler_inters_match_set> &match_sets,
        vcl_vector<vpgl_bundler_inters_track> &tracks) = 0;
};

#endif /*VPGL_BUNDLER_TRACKS_H*/
