#ifndef VPGL_BUNDLER_SFM_H
#define VPGL_BUNDLER_SFM_H

#include <vpgl/algo/vpgl_bundler_inters.h>

#include <vcl_vector.h>

/*An abstract functor. Takes in the track set, and returns the initial
 * reconstruction.*/
class vpgl_bundler_sfm_create_initial_recon {
public:
    virtual void operator()(
        vpgl_bundler_inters_track_set &track_set,
        vpgl_bundler_inters_reconstruction &reconstruction) = 0;
};


/*Takes in the current reconstruction and the image set, and selects the 
 * next images to be added to the reconstruction.*/
class vpgl_bundler_sfm_select_next_images {
public:
    /*Takes in reconstruction and track_set, fills to_add as a return val.*/
    virtual void operator()(
        vpgl_bundler_inters_reconstruction &reconstruction,
        vpgl_bundler_inters_track_set &track_set,

        vcl_vector<vpgl_bundler_inters_feature_set_sptr> &to_add) = 0;
};


/*Takes in the current reconstruction and an image, and adds the image to
 * the reconstruction. This calculates the pose of the images' camera.*/
class vpgl_bundler_sfm_add_next_images {
public:
    virtual void operator()(
        vpgl_bundler_inters_reconstruction reconstruction,
        const vcl_vector<vpgl_bundler_inters_feature_set> &to_add) = 0;
};

/*Adds points to the reconstruction using the new cameras calculated in the 
 * add_next_images stage.*/
class vpgl_bundler_sfm_add_new_points {
public:
    virtual void operator()(
        vpgl_bundler_inters_reconstruction &reconstruction,
        vpgl_bundler_inters_track_set &track_set) = 0;
};

/*Performs bundle adjustment on the reconstruction.*/
class vpgl_bundler_sfm_bundle_adjust {
public:
    virtual void operator()(
        vpgl_bundler_inters_reconstruction reconstruction) = 0;
};

#endif /*VPGL_BUNDLER_SFM_H*/
