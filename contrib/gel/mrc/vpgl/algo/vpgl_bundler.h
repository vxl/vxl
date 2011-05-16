#ifndef VPGL_BUNDLER_H
#define VPGL_BUNDLER_H

// Generic includes.
#include <vcl_vector.h>
#include <vnl/vnl_sparse_matrix.h>

// We use vil_image_resources since they are not in memory, and we might
// have a very large collection of images.
#include <vil/vil_image_resource.h>

// Include the classes used to represent cameras and points.
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_point_3d.h>

// Include the classes for the stages.
#include <vpgl/algo/vpgl_bundler_tracks.h>
#include <vpgl/algo/vpgl_bundler_sfm.h>

// Include the default implementations.
#include <vpgl/algo/vpgl_bundler_tracks_impl.h>
#include <vpgl/algo/vpgl_bundler_sfm_impl.h>

// Include the settings and intermediate types
#include <vpgl/algo/vpgl_bundler_settings.h>
#include <vpgl/algo/vpgl_bundler_inters.h>

struct vpgl_bundler_routines;

// The default driver for the bundler routines. Uses the functions packaged
// in routines to calculate the pose of each of the cameras and a set
// of 3D points from the images included in imageset. These values
// are placed in cameras and points, respectively. Also includes a matrix
// which holds which points are available from which cameras.
// The exif_tags should hold information extracted from the EXIF tags for
// each image. It either holds the focal length in pixels, or
// VPGL_BUNDLER_NO_FOCAL_LEN for images that either don't have an exif tag
// or whose exif tag is missing the focal length.
void vpgl_bundler_driver(
    const vpgl_bundler_routines &routines,
    const vcl_vector<vil_image_resource_sptr> &imageset,
    const vcl_vector<double> exif_tags,

    vcl_vector<vpgl_perspective_camera<double> > &cameras,
    vcl_vector<vgl_point_3d<double> > &points,
    vnl_sparse_matrix<bool> visibility_graph);


// A collection of routines that provide the feature phase of
// the bundler pipeline. There are three constructors, one for the default,
// which will use the routines for the standard feature matching as well as
// default settings, another which allows the user to specify thresholds,
// and the last which takes in pointers to the stages used. If the default
// stages are used (the first or second constructor), the stages are freed
// in the destructor, otherwise, the stages are left well enough alone.
class vpgl_bundler_tracks
{
    private:
        bool manage_pointers;

    public:
        vpgl_bundler_tracks_detect *detect_features;
        vpgl_bundler_tracks_propose_matches *propose_matches;
        vpgl_bundler_tracks_match *match;
        vpgl_bundler_tracks_refine *refine;
        vpgl_bundler_tracks_chain_matches *chain_matches;

        // A constructor that uses the default pipeline stages as well
        // as default settings for each stage.
        vpgl_bundler_tracks();

        // A constructor that uses the default pipeline stages, but
        // allows the user to override the settings for each stage.
        // The settings include thresholds, number of iterations, and
        // similar details.
        vpgl_bundler_tracks(vpgl_bundler_settings_tracks s);

        // Constructor that allows an override of each of the stages
        // of the feature pipeline. These pointers are managed
        // externally! As in, if you allocate them, you must
        // deallocate them.
        vpgl_bundler_tracks(
            vpgl_bundler_tracks_detect *d,
            vpgl_bundler_tracks_propose_matches *ml,
            vpgl_bundler_tracks_match *ma,
            vpgl_bundler_tracks_refine *re,
            vpgl_bundler_tracks_chain_matches *cm):

            manage_pointers(false),
            detect_features(d), propose_matches(ml),
            match(ma), refine(re), chain_matches(cm) { }

        // The destructor that frees the allocations made in the
        // constructor, if there are any.
        ~vpgl_bundler_tracks() {
            if (manage_pointers) {
                delete detect_features;
                delete propose_matches;
                delete match;
                delete refine;
                delete chain_matches;
            }
        }

        // A convienence method that runs the entire feature matching
        // pipeline. Alternatively, you can directly call functors
        // yourself.
        virtual void run_feature_stage(
            const vcl_vector<vil_image_resource_sptr> &imageset,
            const vcl_vector<double> exif_tags,

            vpgl_bundler_inters_track_set &track_set);
};


// A collections of routines that provides the structure from motion (sfm)
// phase of the bundler pipeline. The constructors are similar to the above.
class vpgl_bundler_sfm
{
    private:
        bool manage_pointers;

    public:
        vpgl_bundler_sfm_create_initial_recon *create_initial_recon;

        vpgl_bundler_sfm_select_next_images *select_next_images;

        vpgl_bundler_sfm_add_next_images *add_next_images;

        vpgl_bundler_sfm_add_new_points *add_new_points;

        vpgl_bundler_sfm_bundle_adjust *bundle_adjust;

        // A constructor that uses the default pipeline stages as well as
        // default settings for each stage
        vpgl_bundler_sfm();

        // A constructor that uses the default pipeline stages, but
        // allows the user to override the settings for each stage.
        // The settings include thresholds, number of iterations, and
        // similar details.
        vpgl_bundler_sfm(vpgl_bundler_settings_sfm s);

        // Constructor that allows an override of each of the stages
        // of the sfm pipeline. These pointers are managed
        // externally! As in, if you allocate them, you must
        // deallocate them.
        vpgl_bundler_sfm(
            vpgl_bundler_sfm_create_initial_recon *cir,
            vpgl_bundler_sfm_select_next_images *sni,
            vpgl_bundler_sfm_add_next_images *ani,
            vpgl_bundler_sfm_add_new_points *anp,
            vpgl_bundler_sfm_bundle_adjust *ba) :
                create_initial_recon(cir), select_next_images(sni),
                add_next_images(ani), add_new_points(anp),
                bundle_adjust(ba) {}

        // The destructor that frees the allocations made in the
        // constructor, if there are any.
        ~vpgl_bundler_sfm() {
            if (manage_pointers) {
                delete create_initial_recon;
                delete select_next_images;
                delete add_next_images;
                delete add_new_points;
                delete bundle_adjust;
            }
        }

        // A convenience method that runs the entire sfm matching
        // pipeline. Alternatively, you can directly call functors
        // yourself.
        virtual void run_sfm_stage(vpgl_bundler_inters_track_set &track_set,
                                   vcl_vector<vpgl_perspective_camera<double> > &cameras,
                                   vcl_vector<vgl_point_3d<double> > &points,
                                   vnl_sparse_matrix<bool> visibility_graph);
};


// A wrapper struct for the functions vpgl_bundler_driver will use.
struct vpgl_bundler_routines
{
    vpgl_bundler_tracks features_phase;
    vpgl_bundler_sfm sfm_phase;

    // Uses default everything, stages and settings
    vpgl_bundler_routines() {}

    // Uses the default stages, but allows the user to specify settings
    // for the thresholds.
    vpgl_bundler_routines(vpgl_bundler_settings s):
        features_phase(s.feature_settings),
        sfm_phase(s.sfm_settings) {}

    // Allows the user to specify a set of feature and sfm routines
    // to use.
    vpgl_bundler_routines(vpgl_bundler_tracks f,
                          vpgl_bundler_sfm s) {
        features_phase = f;
        sfm_phase = s;
    }
};


#endif // VPGL_BUNDLER_H
