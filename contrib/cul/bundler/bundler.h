#ifndef BUNDLER_H
#define BUNDLER_H
// TODO:
//       visibility matrix.
//       check angle for adding new points.
//       focal length estimation.
//       the lens distortion stuff.
//
//       round of bundle adjustment after adding image, new image fixed.
//       deal with color images.
//
//       more complete test suite
//       arguments in tests.


// Generic includes.
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_sparse_matrix.h>

// We use vil_image_resources since they are not in memory, and we might
// have a very large collection of images.
#include <vil/vil_image_resource.h>

// Include the classes used to represent cameras and points.
#include <vpgl/vpgl_perspective_camera.h>
#include <vgl/vgl_point_3d.h>

// Include the classes for the stages.
#include <bundler/bundler_tracks.h>
#include <bundler/bundler_sfm.h>

// Include the default implementations.
#include <bundler/bundler_tracks_impl.h>
#include <bundler/bundler_sfm_impl.h>

// Include the settings and intermediate types
#include <bundler/bundler_settings.h>
#include <bundler/bundler_inters.h>

struct bundler_routines;

// The default driver for the bundler routines. Uses the functions packaged
// in routines to calculate the pose of each of the cameras and a set
// of 3D points from the images included in imageset. These values
// are placed in cameras and points, respectively. Also includes a matrix
// which holds which points are available from which cameras.
// The focal_length should hold information extracted from the EXIF tags for
// each image. It either holds the focal length in pixels, or
// BUNDLER_NO_FOCAL_LEN for images that either don't have an exif tag
// or whose exif tag is missing the focal length.
bool bundler_driver(
    const bundler_routines &routines,
    const std::vector<vil_image_resource_sptr> &imageset,
    const std::vector<double> &exif_tags,

    std::vector<vpgl_perspective_camera<double> > &cameras,
    std::vector<vgl_point_3d<double> > &points,
    vnl_sparse_matrix<bool> &visibility_graph);


// Creates a file according to the PLY specifications, containing
// the point data held in points.
bool bundler_write_ply_file(
    const char *filename,
    const std::vector<vgl_point_3d<double> > &points);

bool bundler_write_bundle_file(
    const char* filename,
    const std::vector<vpgl_perspective_camera<double> > &cameras,
    const std::vector<vgl_point_3d<double> > &points);

// A collection of routines that provide the feature phase of
// the bundler pipeline. There are three constructors, one for the default,
// which will use the routines for the standard feature matching as well as
// default settings, another which allows the user to specify thresholds,
// and the last which takes in pointers to the stages used. If the default
// stages are used (the first or second constructor), the stages are freed
// in the destructor, otherwise, the stages are left well enough alone.
class bundler_tracks
{
    private:
        bool manage_pointers;

    public:
        bundler_tracks_detect *detect_features;
        bundler_tracks_propose_matches *propose_matches;
        bundler_tracks_match *match;
        bundler_tracks_refine *refine;
        bundler_tracks_chain_matches *chain_matches;

        // A constructor that uses the default pipeline stages as well
        // as default settings for each stage.
        bundler_tracks();

        // A constructor that uses the default pipeline stages, but
        // allows the user to override the settings for each stage.
        // The settings include thresholds, number of iterations, and
        // similar details.
        bundler_tracks(bundler_settings_tracks s);

        // Constructor that allows an override of each of the stages
        // of the feature pipeline. These pointers are managed
        // externally! As in, if you allocate them, you must
        // deallocate them.
        bundler_tracks(
            bundler_tracks_detect *d,
            bundler_tracks_propose_matches *ml,
            bundler_tracks_match *ma,
            bundler_tracks_refine *re,
            bundler_tracks_chain_matches *cm):

            manage_pointers(false),
            detect_features(d), propose_matches(ml),
            match(ma), refine(re), chain_matches(cm) { }

        // The destructor that frees the allocations made in the
        // constructor, if there are any.
        virtual ~bundler_tracks() {
            if (manage_pointers) {
                delete detect_features;
                delete propose_matches;
                delete match;
                delete refine;
                delete chain_matches;
            }
        }

        // A convenience method that runs the entire feature matching
        // pipeline. Alternatively, you can directly call functors
        // yourself.
        virtual void run_feature_stage(
            const std::vector<vil_image_resource_sptr> &imageset,
            const std::vector<double> &focal_lengths,
            bundler_inters_reconstruction &empty_recon) const;
};


// A collections of routines that provides the structure from motion (sfm)
// phase of the bundler pipeline. The constructors are similar to the above.
class bundler_sfm
{
    private:
        bool manage_pointers;

    public:
        bundler_sfm_create_initial_recon *create_initial_recon;

        bundler_sfm_select_next_images *select_next_images;

        bundler_sfm_add_next_images *add_next_images;

        bundler_sfm_add_new_points *add_new_points;

        bundler_sfm_bundle_adjust *bundle_adjust;

        // A constructor that uses the default pipeline stages as well as
        // default settings for each stage
        bundler_sfm();

        // A constructor that uses the default pipeline stages, but
        // allows the user to override the settings for each stage.
        // The settings include thresholds, number of iterations, and
        // similar details.
        bundler_sfm(bundler_settings_sfm s);

        // Constructor that allows an override of each of the stages
        // of the sfm pipeline. These pointers are managed
        // externally! As in, if you allocate them, you must
        // deallocate them.
        bundler_sfm(
            bundler_sfm_create_initial_recon *cir,
            bundler_sfm_select_next_images *sni,
            bundler_sfm_add_next_images *ani,
            bundler_sfm_add_new_points *anp,
            bundler_sfm_bundle_adjust *ba) :
                create_initial_recon(cir), select_next_images(sni),
                add_next_images(ani), add_new_points(anp),
                bundle_adjust(ba) {}

        // The destructor that frees the allocations made in the
        // constructor, if there are any.
        virtual ~bundler_sfm() {
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
        virtual bool run_sfm_stage(
            bundler_inters_reconstruction &empty_recon,
            std::vector<vpgl_perspective_camera<double> > &cameras,
            std::vector<vgl_point_3d<double> > &points,
            vnl_sparse_matrix<bool> visibility_graph) const;
};


// A wrapper struct for the functions bundler_driver will use.
struct bundler_routines
{
    bundler_tracks features_phase;
    bundler_sfm sfm_phase;

    // Uses default everything, stages and settings
    bundler_routines() = default;

    // Uses the default stages, but allows the user to specify settings
    // for the thresholds.
    bundler_routines(bundler_settings s):
        features_phase(s.feature_settings),
        sfm_phase(s.sfm_settings) {}

    // Allows the user to specify a set of feature and sfm routines
    // to use.
    bundler_routines(bundler_tracks f, bundler_sfm s) {
        features_phase = f;
        sfm_phase = s;
    }
};


#endif // BUNDLER_H
