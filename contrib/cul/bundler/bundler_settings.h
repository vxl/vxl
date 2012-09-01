#ifndef BUNDLER_SETTINGS
#define BUNDLER_SETTINGS

//-----------------Settings structs for the features stages---------------//

//Settings for the feature detection.
struct bundler_settings_sift
{
    float keypoint_curve_ratio; //Used in bapl_keypoint_extractor.
    int sift_octave_size; //Used in bapl_dense_sift
    int sift_num_octaves; //Used in bapl_dense_sift

    bundler_settings_sift() :
        keypoint_curve_ratio(10.0f), sift_octave_size(6),
        sift_num_octaves(1) {}
};

//Settings for the propose stage. This stage doesn't actually use any
// settings, but we've included it for completeness.
typedef char bundler_settings_propose_matches_all;

//Settings for the match stage. The single setting here is the maximum
// allowable distance ratio for a match to be considered valid.
struct bundler_settings_match_ann
{
    double min_dists_ratio;

    bundler_settings_match_ann():
        min_dists_ratio(.6) { }
};

//Settings for the refine stage. The settings have to do with
// RANSAC and the f-matrix estimation.
struct bundler_settings_refine_epipolar
{
    //The desired probability of getting a good f matrix
    double probability_good_f_mat;

    //The maximum distance between the generated point and the matched
    // point for the point to be considered an outlier.
    double outlier_threshold;

    //The maximum expected porportion of the outliers in the set. This, as
    // well as the desired probability, is used to get the number of
    // iterations to run.
    double max_outlier_frac;

    //The minimum number of inliers in order for two images to be matched.
    int min_inliers;

    bundler_settings_refine_epipolar():
        probability_good_f_mat(.95), outlier_threshold(9.0),
        max_outlier_frac(.5), min_inliers(16) { }
};

// Same deal as _settings_propose_match_all
typedef char bundler_settings_chain_matches;

struct bundler_settings_tracks
{
    bundler_settings_sift detect_settings;
    bundler_settings_propose_matches_all propose_matches_settings;
    bundler_settings_match_ann match_settings;
    bundler_settings_refine_epipolar refine_settings;
    bundler_settings_chain_matches chain_matches_settings;
};


//----------------Settings structs for the sfm stages.-------------------//
struct bundler_settings_create_initial_recon
{
    //Settings for estimating homographies using RANSAC
    int min_number_of_matches_homography;
    int number_ransac_rounds_homography;
    double inlier_threshold_homography;

    //Pretty much the same settings for estimating essential matrices
    int number_ransac_rounds_e_matrix;
    double inlier_threshold_e_matrix;

    bundler_settings_create_initial_recon() :
        min_number_of_matches_homography(50),
        number_ransac_rounds_homography(128),
        inlier_threshold_homography(6.0),
        number_ransac_rounds_e_matrix(2048),
        inlier_threshold_e_matrix(4.0){ }
};

struct bundler_settings_select_next_images
{
    //The minimum number of points already added that a camera must observe.
    // If no camera meets this description, the reconstruction is
    // considered complete.
    int min_number_observed_points;

    bundler_settings_select_next_images():
        min_number_observed_points(10) { }
};

struct bundler_settings_add_next_images
{
    // The standard two ransac parameters for the
    // projection matrix estimation.
    int number_ransac_rounds;

    // The actual threshold is
    // inlier_size_multiplier * max(img width, img height)
    double inlier_size_multiplier;

    bundler_settings_add_next_images() :
        number_ransac_rounds(40960),
        inlier_size_multiplier(6.0){ }
};

struct bundler_settings_add_new_points
{
    int min_observing_images;
    double min_ray_angle;

    bundler_settings_add_new_points() :
        min_observing_images(2),
        min_ray_angle(2.0) { }
};

struct bundler_settings_bundle_adjust
{
    int number_of_iterations;

    bundler_settings_bundle_adjust():
        number_of_iterations(10000) { }
};

struct bundler_settings_sfm
{
    bundler_settings_create_initial_recon initial_recon_settings;
    bundler_settings_select_next_images select_next_images_settings;
    bundler_settings_add_next_images add_next_images_settings;
    bundler_settings_add_new_points add_new_points_settings;
    bundler_settings_bundle_adjust bundle_adjust_settings;
};

//-----------------------------------------------------------------------//
//Highest-level settings struct
struct bundler_settings
{
    bundler_settings_tracks feature_settings;
    bundler_settings_sfm sfm_settings;
};

#endif //BUNDLER_SETTINGS
